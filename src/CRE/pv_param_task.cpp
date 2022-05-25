/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_param_task.hpp"
#include "post_process.hpp"
#include "render_context.hpp"

extern render_context* rctx_ptr;

pv_param_bloom::pv_param_bloom() {
    id = -1;
    color = vec3_null;
    brightpass = vec3_null;
    range = 0.0f;
}

pv_param_dof::pv_param_dof() {
    id = -1;
    focus = 0.0f;
    focus_range = 100.0f;
    fuzzing_range = 2.0f;
    ratio = 0.0f;
    quality = 1.0f;
}

pv_param_chara_alpha::pv_param_chara_alpha() {
    frame = -1.0f;
    duration = 1.0f;
    prev_alpha = 1.0f;
    alpha = 1.0f;
    type = 0;
}

pv_param_color_correction::pv_param_color_correction() {
    id = -1;
    hue = 1.0f;
    saturation = 1.0f;
    lightness = 1.0f;
    exposure = 1.0f;
    gamma = vec3_identity;
    contrast = 1.0f;
}

pv_param_post_process_data_struct::pv_param_post_process_data_struct() {

}

pv_param_post_process_data_struct::~pv_param_post_process_data_struct() {

}

pv_param_file_data_struct::pv_param_file_data_struct() {

}

pv_param_file_data_struct::~pv_param_file_data_struct() {

}

pv_param_light_data_struct::pv_param_light_data_struct() {

}

pv_param_light_data_struct::~pv_param_light_data_struct() {

}

namespace pv_param_task {
    PostProcessTask post_process_task;

    PostProcessCtrl::PostProcessCtrl() {
        frame = -1.0f;
        duration = -1.0f;
    }

    PostProcessCtrl::~PostProcessCtrl() {

    }

    PostProcessCtrlBloom::Data::Data() {

    }

    PostProcessCtrlBloom::PostProcessCtrlBloom() {

    }

    PostProcessCtrlBloom::~PostProcessCtrlBloom() {

    }

    void PostProcessCtrlBloom::Reset() {
        frame = -1.0f;
    }

    void PostProcessCtrlBloom::Set() {
        if (frame < 0.0f)
            return;

        vec3 value;
        if (fabsf(duration) <= 0.000001f)
            value = data.data.color;
        else if (duration < 0.0f)
            value = data.data_prev.color;
        else
            vec3_lerp_scalar(data.data_prev.color, data.data.color, value, frame / duration);

        post_process_blur_set_intensity(rctx_ptr->post_process.blur, &value);

        frame += 1.0f;
        if (frame > duration) {
            frame = -1.0f;
            duration = -1.0f;
            data.data_prev = data.data;
        }
    }

    PostProcessCtrlCC::Data::Data() {

    }

    PostProcessCtrlCC::PostProcessCtrlCC() {

    }

    PostProcessCtrlCC::~PostProcessCtrlCC() {

    }

    void PostProcessCtrlCC::Reset() {
        frame = -1.0f;
    }

    void PostProcessCtrlCC::Set() {
        if (frame < 0.0f)
            return;

        float_t saturation;
        float_t exposure;
        vec3 gamma;
        float_t contrast ;
        if (fabsf(duration) <= 0.000001f) {
            saturation = data.data.saturation;
            exposure = data.data.exposure;
            gamma = data.data.gamma;
            contrast = data.data.contrast;
        }
        else if (duration < 0.0f) {
            saturation = data.data_prev.saturation;
            exposure = data.data_prev.exposure;
            gamma = data.data_prev.gamma;
            contrast = data.data_prev.contrast;
        }
        else {
            float_t t = frame / duration;
            saturation = lerp(data.data_prev.saturation, data.data.saturation, t);
            exposure = lerp(data.data_prev.exposure, data.data.exposure, t);
            vec3_lerp_scalar(data.data_prev.gamma, data.data.gamma, gamma, t);
            contrast = lerp(data.data_prev.contrast, data.data.contrast, t);
        }

        post_process_tone_map* tone_map = rctx_ptr->post_process.tone_map;
        post_process_tone_map_set_saturate_coeff(tone_map, saturation);
        post_process_tone_map_set_exposure(tone_map, exposure * 2.0f);

        vec3 tone_trans_start;
        vec3 tone_trans_end;
        CalcToneTrans(contrast + gamma.x, tone_trans_start.x, tone_trans_end.x);
        CalcToneTrans(contrast + gamma.y, tone_trans_start.y, tone_trans_end.y);
        CalcToneTrans(contrast + gamma.z, tone_trans_start.z, tone_trans_end.z);

        post_process_tone_map_set_tone_trans_start(tone_map, &tone_trans_start);
        post_process_tone_map_set_tone_trans_end(tone_map, &tone_trans_end);

        frame += 1.0f;
        if (frame > duration) {
            frame = -1.0f;
            duration = -1.0f;
            data.data_prev = data.data;
        }
    }

    void PostProcessCtrlCC::CalcToneTrans(float_t value, float_t& tone_trans_start, float_t& tone_trans_end) {
        float_t pow = max(value, 0.125f) * 0.5f;
        if (fabsf(value - 2.0f) < 0.000001f) {
            tone_trans_start = 0.0f;
            tone_trans_end = 1.0f;
        }
        else if (value < 2.0f) {
            tone_trans_end = 1.0f;
            tone_trans_start = powf(0.075f, pow);
        }
        else if (value > 2.0f) {
            tone_trans_start = 0.0f;
            tone_trans_end = powf(0.95f, pow);
        }
    }

    PostProcessCtrlCharaAlpha::Data::Data() {

    }

    PostProcessCtrlCharaAlpha::PostProcessCtrlCharaAlpha() {

    }

    PostProcessCtrlCharaAlpha::~PostProcessCtrlCharaAlpha() {

    }

    void PostProcessCtrlCharaAlpha::Reset() {
        for (pv_param_chara_alpha& i : data.data)
            i = {};
    }

    void PostProcessCtrlCharaAlpha::Set() {
        int32_t index = 0;
        for (pv_param_chara_alpha& i : data.data) {
            if (i.frame < 0.0f) {
                index++;
                continue;
            }

            float_t value;
            if (fabs(i.duration) <= 0.000001)
                value = i.alpha;
            else if (i.duration <= 0.0)
                value = i.prev_alpha;
            else
                value = lerp(i.prev_alpha, i.alpha, i.frame / i.duration);

            draw_task_flags draw_task_flags;
            switch (i.type) {
            case 0:
            default:
                draw_task_flags = DRAW_TASK_10000;
                break;
            case 1:
                draw_task_flags = DRAW_TASK_20000;
                break;
            case 2:
                draw_task_flags = DRAW_TASK_40000;
                break;
            }

            rob_chara_array_set_alpha_draw_task_flags(index, value, draw_task_flags);

            i.frame += 1.0f;
            if (i.frame > i.duration) {
                i.frame = -1.0f;
                i.prev_alpha = i.alpha;
            }
            index++;
        }
    }

    PostProcessCtrlCharaItemAlpha::PostProcessCtrlCharaItemAlpha() {

    }

    PostProcessCtrlCharaItemAlpha::~PostProcessCtrlCharaItemAlpha() {

    }

    void PostProcessCtrlCharaItemAlpha::Set() {
        int32_t index = 0;
        for (pv_param_chara_alpha& i : data.data) {
            if (i.frame < 0.0f) {
                index++;
                continue;
            }

            float_t value;
            if (fabs(i.duration) <= 0.000001)
                value = i.alpha;
            else if (i.duration <= 0.0)
                value = i.prev_alpha;
            else
                value = lerp(i.prev_alpha, i.alpha, i.frame / i.duration);

            /*pv_game* v5 = pv_game_data_get();
            if (v5)
                sub_140115EC0(v5, index, i.type, value);*/

            i.frame += 1.0f;
            if (i.frame > i.duration) {
                i.frame = -1.0f;
                i.prev_alpha = i.alpha;
            }
            index++;
        }
    }

    PostProcessCtrlDof::Data::Data() {

    }

    PostProcessCtrlDof::PostProcessCtrlDof() {

    }

    PostProcessCtrlDof::~PostProcessCtrlDof() {

    }

    void PostProcessCtrlDof::Reset() {
        frame = -1.0f;
    }

    void PostProcessCtrlDof::Set() {
        if (frame < 0.0f)
            return;


        float_t focus;
        float_t focus_range;
        float_t fuzzing_range;
        float_t ratio;
        if (fabsf(duration) <= 0.000001f) {
            focus = data.data.focus;
            focus_range = data.data.focus_range;
            fuzzing_range = data.data.fuzzing_range;
            ratio = data.data.ratio;
        }
        else if (duration < 0.0f) {
            focus = data.data_prev.focus;
            focus_range = data.data_prev.focus_range;
            fuzzing_range = data.data_prev.fuzzing_range;
            ratio = data.data_prev.ratio;
        }
        else {
            float_t t = frame / duration;
            focus = lerp(data.data_prev.focus, data.data.focus, t);
            focus_range = lerp(data.data_prev.focus_range, data.data.focus_range, t);
            fuzzing_range = lerp(data.data_prev.fuzzing_range, data.data.fuzzing_range, t);
            ratio = lerp(data.data_prev.ratio, data.data.ratio, t);
        }

        post_process_dof* dof = rctx_ptr->post_process.dof;
        dof_pv pv;
        dof->get_dof_pv(&pv);
        pv.f2.focus = focus;
        pv.f2.focus_range = focus_range;
        pv.f2.fuzzing_range = fuzzing_range;
        pv.f2.ratio = ratio;
        dof->set_dof_pv(&pv);

        frame += 1.0f;
        if (frame > duration) {
            frame = -1.0f;
            duration = -1.0f;
            data.data_prev = data.data;
        }
    }

    void PostProcessCtrlDof::SetData(pv_param_dof* dof, float_t duration) {
        frame = 0.0f;
        this->duration = duration;
        data.data = *dof;

        if (fabsf(duration) > 0.000001f) {
            dof_pv pv;
            rctx_ptr->post_process.dof->get_dof_pv(&pv);
            data.data_prev.focus = pv.f2.focus;
            data.data_prev.focus_range = pv.f2.focus_range;
            data.data_prev.fuzzing_range = pv.f2.fuzzing_range;
            data.data_prev.ratio = pv.f2.ratio;
        }
        else
            data.data_prev = data.data;
    }

    PostProcessTask::PostProcessTask() {

    }
    
    PostProcessTask::~PostProcessTask() {

    }
    
    bool PostProcessTask::Init() {
        dof.Reset();
        cc.Reset();
        bloom.Reset();
        chara_alpha.Reset();
        chara_item_alpha.Reset();
        return true;
    }
    
    bool PostProcessTask::Ctrl() {
        dof.Set();
        cc.Set();
        bloom.Set();
        chara_alpha.Set();
        chara_item_alpha.Set();
        return false;
    }
    
    bool PostProcessTask::Dest() {
        dof.Reset();
        cc.Reset();
        bloom.Reset();
        chara_alpha.Reset();
        chara_item_alpha.Reset();
        return true;
    }
}
