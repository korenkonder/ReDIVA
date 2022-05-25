/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <map>
#include <string>
#include <vector>
#include "../KKdLib/default.h"
#include "../KKdLib/vec.h"
#include "light_param.hpp"
#include "rob.hpp"
#include "task.hpp"

struct pv_param_bloom {
    int32_t id;
    vec3 color;
    vec3 brightpass;
    float_t range;

    pv_param_bloom();
};

struct pv_param_dof {
    int32_t id;
    float_t focus;
    float_t focus_range;
    float_t fuzzing_range;
    float_t ratio;
    float_t quality;

    pv_param_dof();
};

struct pv_param_chara_alpha {
    float_t frame;
    float_t duration;
    float_t prev_alpha;
    float_t alpha;
    int32_t type;

    pv_param_chara_alpha();
};

struct pv_param_color_correction {
    int32_t id;
    float_t hue;
    float_t saturation;
    float_t lightness;
    float_t exposure;
    vec3 gamma;
    float_t contrast;

    pv_param_color_correction();
};

struct pv_param_post_process_data_struct {
    pv_param_dof dof_default;
    pv_param_color_correction cc_default;
    pv_param_bloom bloom_default;
    std::string dof_file;
    std::string cc_file;
    std::string bloom_file;
    std::vector<pv_param_dof> dof;
    std::vector<pv_param_color_correction> cc;
    std::vector<pv_param_bloom> bloom;

    pv_param_post_process_data_struct();
    ~pv_param_post_process_data_struct();
};

struct pv_param_file_data_struct {
    std::vector<pv_param_dof> dof_default;
    std::vector<pv_param_color_correction> cc_default;
    std::vector<pv_param_bloom> bloom_default;
    std::map<std::string, std::vector<pv_param_dof>> dof;
    std::map<std::string, std::vector<pv_param_color_correction>> cc;
    std::map<std::string, std::vector<pv_param_bloom>> bloom;

    pv_param_file_data_struct();
    ~pv_param_file_data_struct();
};

struct pv_param_light_data_struct {
    light_param_light_data light_default;
    std::string chara_light_file;
    std::string stage_light_file;
    std::map<int32_t, light_param_light_data> chara_light;
    std::map<int32_t, light_param_light_data> stage_light;

    pv_param_light_data_struct();
    ~pv_param_light_data_struct();
};

namespace pv_param_task {
    class PostProcessCtrl {
    public:
        float_t frame;
        float_t duration;

        PostProcessCtrl();
        ~PostProcessCtrl();

        virtual void Reset() = 0;
        virtual void Set() = 0;
    };

    class PostProcessCtrlBloom : public PostProcessCtrl {
    public:
        struct Data {
            pv_param_bloom data;
            pv_param_bloom data_prev;

            Data();
        } data;

        PostProcessCtrlBloom();
        ~PostProcessCtrlBloom();

        virtual void Reset() override;
        virtual void Set() override;
    };

    class PostProcessCtrlCC : public PostProcessCtrl {
    public:
        struct Data {
            pv_param_color_correction data;
            pv_param_color_correction data_prev;

            Data();
        } data;

        PostProcessCtrlCC();
        ~PostProcessCtrlCC();

        virtual void Reset() override;
        virtual void Set() override;

        void CalcToneTrans(float_t value, float_t& tone_trans_start, float_t& tone_trans_end);
    };

    class PostProcessCtrlCharaAlpha : public PostProcessCtrl {
    public:
        struct Data {
            pv_param_chara_alpha data[ROB_CHARA_COUNT];

            Data();
        } data;

        PostProcessCtrlCharaAlpha();
        ~PostProcessCtrlCharaAlpha();

        virtual void Reset() override;
        virtual void Set() override;
    };

    class PostProcessCtrlCharaItemAlpha : public PostProcessCtrlCharaAlpha {
    public:
        PostProcessCtrlCharaItemAlpha();
        ~PostProcessCtrlCharaItemAlpha();

        virtual void Set() override;
    };

    class PostProcessCtrlDof : public PostProcessCtrl {
    public:
        struct Data {
            pv_param_dof data;
            pv_param_dof data_prev;

            Data();
        } data;

        PostProcessCtrlDof();
        ~PostProcessCtrlDof();

        virtual void Reset() override;
        virtual void Set() override;

        void SetData(pv_param_dof* dof, float_t duration);
    };

    class PostProcessTask : public Task {
    public:
        PostProcessCtrlDof dof;
        PostProcessCtrlCC cc;
        PostProcessCtrlBloom bloom;
        PostProcessCtrlCharaAlpha chara_alpha;
        PostProcessCtrlCharaItemAlpha chara_item_alpha;

        PostProcessTask();
        virtual ~PostProcessTask() override;

        virtual bool Init() override;
        virtual bool Ctrl() override;
        virtual bool Dest() override;
    };

    extern PostProcessTask post_process_task;
}
