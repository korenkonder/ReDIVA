/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include <map>
#include <string>
#include <vector>
#include "../KKdLib/default.hpp"
#include "../KKdLib/vec.hpp"
#include "rob/rob.hpp"
#include "light_param.hpp"
#include "task.hpp"

namespace pv_param {
    struct bloom {
        int32_t id;
        vec3 color;
        vec3 brightpass;
        float_t range;

        bloom();
    };

    struct dof {
        int32_t id;
        float_t focus;
        float_t focus_range;
        float_t fuzzing_range;
        float_t ratio;
        float_t quality;

        dof();
    };

    struct chara_alpha {
        float_t frame;
        float_t duration;
        float_t prev_alpha;
        float_t alpha;
        int32_t type;

        chara_alpha();
    };

    struct color_correction {
        int32_t id;
        float_t hue;
        float_t saturation;
        float_t lightness;
        float_t exposure;
        vec3 gamma;
        float_t contrast;

        color_correction();
    };

    struct post_process_data_struct {
        dof dof_default;
        color_correction cc_default;
        bloom bloom_default;
        std::string dof_file;
        std::string cc_file;
        std::string bloom_file;
        std::vector<dof> dof;
        std::vector<color_correction> cc;
        std::vector<bloom> bloom;

        post_process_data_struct();
        ~post_process_data_struct();
    };

    struct file_data_struct {
        std::vector<dof> dof_default;
        std::vector<color_correction> cc_default;
        std::vector<bloom> bloom_default;
        std::map<std::string, std::vector<dof>> dof;
        std::map<std::string, std::vector<color_correction>> cc;
        std::map<std::string, std::vector<bloom>> bloom;

        file_data_struct();
        ~file_data_struct();
    };

    struct light_data_struct {
        light_param_light_data light_default;
        std::string chara_light_file;
        std::string stage_light_file;
        std::map<int32_t, light_param_light_data> chara_light;
        std::map<int32_t, light_param_light_data> stage_light;

        light_data_struct();
        ~light_data_struct();
    };
}

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
            pv_param::bloom data;
            pv_param::bloom data_prev;

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
            pv_param::color_correction data;
            pv_param::color_correction data_prev;

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
            pv_param::chara_alpha data[ROB_CHARA_COUNT];

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
            pv_param::dof data;
            pv_param::dof data_prev;

            Data();
        } data;

        PostProcessCtrlDof();
        ~PostProcessCtrlDof();

        virtual void Reset() override;
        virtual void Set() override;

        void SetData(pv_param::dof* dof, float_t duration);
    };

    class PostProcessTask : public app::Task {
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
