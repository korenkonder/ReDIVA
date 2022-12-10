/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <string>
#include <vector>
#include "../../KKdLib/default.hpp"
#include "../../CRE/Glitter/glitter.hpp"
#include "../../CRE/data.hpp"
#include "../task_window.hpp"

class TaskDataTestGlitterParticle : public app::TaskWindow {
public:
    uint64_t hash;
    Glitter::SceneCounter scene_counter;
    float_t frame;
    bool auto_and_repeat;
    bool reload;
    bool pv_mode;
    bool show_grid;
    bool rebuild_geff;

    std::vector<std::string> geff;
    size_t geff_index;

    std::vector<data_struct_file> files;
    size_t file_index;
    bool load_file;

    bool input_play;
    bool input_stop;
    double_t delta_frame;

    bool stage_test;

    TaskDataTestGlitterParticle();
    virtual ~TaskDataTestGlitterParticle() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Window() override;

    void LoadFile(const char* file);
};

extern TaskDataTestGlitterParticle* task_data_test_glitter_particle;

extern void task_data_test_glitter_particle_init();
extern void task_data_test_glitter_particle_free();
