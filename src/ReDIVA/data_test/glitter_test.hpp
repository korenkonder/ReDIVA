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
#include "../dw.hpp"

class DataTestGlitterParticleDw;

class TaskDataTestGlitterParticle : public app::Task {
public:
    uint64_t hash;
    Glitter::SceneCounter scene_counter;
    DataTestGlitterParticleDw* dw;
    float_t frame;
    bool auto_and_repeat;
    bool reload;
    bool pv_mode;
    bool show_grid;
    bool rebuild_geff;

    TaskDataTestGlitterParticle();
    virtual ~TaskDataTestGlitterParticle() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    void LoadFile(const char* file);
    void SceneFree();
    void SetPause(bool value);
    void SetReload();
};

class DataTestGlitterParticleDw : public dw::Shell {
public:
    TaskDataTestGlitterParticle* task;
    dw::ListBox* list;
    dw::ListBox* geff;
    dw::Button* pause;
    dw::Button* auto_and_repeat;
    dw::Button* pv_mode;
    dw::Button* play;
    dw::Button* stop;
    dw::Slider* emission;
    dw::Button* show_grid;
    dw::Button* stage;
    dw::Button* chara;

    DataTestGlitterParticleDw(TaskDataTestGlitterParticle* task);
    virtual ~DataTestGlitterParticleDw() override;

    void Init();

    static void AutoAndRepeatCallback(dw::Widget* data);
    static void CharaCallback(dw::Widget* data);
    static void EmissionCallback(dw::Widget* data);
    static void ListCallback(dw::Widget* data);
    static void PauseCallback(dw::Widget* data);
    static void PlayCallback(dw::Widget* data);
    static void PvModeCallback(dw::Widget* data);
    static void ShowGridCallback(dw::Widget* data);
    static void StageCallback(dw::Widget* data);
    static void StopCallback(dw::Widget* data);
};

extern TaskDataTestGlitterParticle* task_data_test_glitter_particle;

extern void task_data_test_glitter_particle_init();
extern void task_data_test_glitter_particle_free();
