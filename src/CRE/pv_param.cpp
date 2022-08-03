/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_param.hpp"
#include "post_process.hpp"
#include "render_context.hpp"
#include "../KKdLib/str_utils.hpp"

extern render_context* rctx_ptr;

namespace pv_param {
    static bool load_text_file(void* data, const char* path, const char* file, uint32_t hash);

    file_data_struct file_data;
    post_process_data_struct post_process_data;

    bloom::bloom() {
        id = -1;
        color = vec3_null;
        brightpass = vec3_null;
        range = 0.0f;
    }

    dof::dof() {
        id = -1;
        focus = 0.0f;
        focus_range = 100.0f;
        fuzzing_range = 2.0f;
        ratio = 0.0f;
        quality = 1.0f;
        chara_id = -1;
    }

    chara_alpha::chara_alpha() {
        frame = -1.0f;
        duration = 1.0f;
        prev_alpha = 1.0f;
        alpha = 1.0f;
        type = 0;
    }

    color_correction::color_correction() {
        id = -1;
        hue = 1.0f;
        saturation = 1.0f;
        lightness = 1.0f;
        exposure = 1.0f;
        gamma = vec3_identity;
        contrast = 1.0f;
    }

    file_data_struct::file_data_struct() {

    }

    file_data_struct::~file_data_struct() {

    }

    std::vector<pv_param::bloom>& file_data_struct::get_bloom_data(std::string& file) {
        if (!file.size())
            return bloom_default;

        auto elem = bloom.find(file);
        if (elem != bloom.end())
            return elem->second;

        return bloom_default;
    }

    std::vector<pv_param::color_correction>& file_data_struct::get_color_correction_data(std::string& file) {
        if (!file.size())
            return cc_default;

        auto elem = cc.find(file);
        if (elem != cc.end())
            return elem->second;

        return cc_default;
    }

    std::vector<pv_param::dof>& file_data_struct::get_dof_data(std::string& file) {
        if (!file.size())
            return dof_default;

        auto elem = dof.find(file);
        if (elem != dof.end())
            return elem->second;

        return dof_default;
    }

    bool file_data_struct::load_bloom_file(std::string& file) {
        std::string data;
        if (!data_list[DATA_AFT].load_file(&data, file.c_str(), load_text_file))
            return false;

        char* buf;
        char** lines;
        size_t count;
        if (!str_utils_text_file_parse(data.c_str(), data.size(), &buf, &lines, &count))
            return false;

        if (count <= 1 || str_utils_compare(lines[0],
            "ID,ColorR,ColorG,ColorB,BrightpassR,BrightpassG,BrightpassB,Range")) {
            free(buf);
            free(lines);
            return false;
        }

        lines++;
        count--;

        std::vector<pv_param::bloom> bloom;
        bloom.reserve(count);

        for (size_t i = 0; i < count; i++) {
            pv_param::bloom bloom_data;
            if (sscanf_s(lines[i], "%d,%g,%g,%g,%g,%g,%g,%g",
                &bloom_data.id, &bloom_data.color.x, &bloom_data.color.y, &bloom_data.color.z,
                &bloom_data.brightpass.x, &bloom_data.brightpass.y, &bloom_data.brightpass.z, &bloom_data.range) == 8)
                bloom.push_back(bloom_data);
        }

        this->bloom.insert_or_assign(file, bloom);

        lines--;

        free(buf);
        free(lines);
        return true;
    }

    bool file_data_struct::load_color_correction_file(std::string& file) {
        std::string data;
        if (!data_list[DATA_AFT].load_file(&data, file.c_str(), load_text_file))
            return false;

        char* buf;
        char** lines;
        size_t count;
        if (!str_utils_text_file_parse(data.c_str(), data.size(), &buf, &lines, &count))
            return false;

        if (count <= 1 || str_utils_compare(lines[0],
            "ID,Hue,Saturation,Lightness,Exposure,GammaR,GammaG,GammaB,Contrast")) {
            free(buf);
            free(lines);
            return false;
        }

        lines++;
        count--;

        std::vector<pv_param::color_correction> cc;
        cc.reserve(count);

        for (size_t i = 0; i < count; i++) {
            pv_param::color_correction cc_data;
            if (sscanf_s(lines[i], "%d,%g,%g,%g,%g,%g,%g,%g,%g",
                &cc_data.id, &cc_data.hue, &cc_data.saturation, &cc_data.lightness,
                &cc_data.exposure, &cc_data.gamma.x, &cc_data.gamma.y, &cc_data.gamma.z, &cc_data.contrast) == 9)
                cc.push_back(cc_data);
        }

        this->cc.insert_or_assign(file, cc);

        lines--;

        free(buf);
        free(lines);
        return true;
    }

    bool file_data_struct::load_dof_file(std::string& file) {
        std::string data;
        if (!data_list[DATA_AFT].load_file(&data, file.c_str(), load_text_file))
            return false;

        char* buf;
        char** lines;
        size_t count;
        if (!str_utils_text_file_parse(data.c_str(), data.size(), &buf, &lines, &count))
            return false;

        if (count <= 1 || str_utils_compare(lines[0],
            "ID,Focus,FocusRange,FuzzingRange,Ratio,Quality")) {
            free(buf);
            free(lines);
            return false;
        }

        lines++;
        count--;

        std::vector<pv_param::dof> dof;
        dof.reserve(count);

        for (size_t i = 0; i < count; i++) {
            pv_param::dof dof_data;
            if (sscanf_s(lines[i], "%d,%g,%g,%g,%g,%g",
                &dof_data.id, &dof_data.focus, &dof_data.focus_range,
                &dof_data.fuzzing_range, &dof_data.ratio, &dof_data.quality) == 6)
                dof.push_back(dof_data);
        }

        this->dof.insert_or_assign(file, dof);

        lines--;

        free(buf);
        free(lines);
        return true;
    }

    void file_data_struct::unload_bloom_file(std::string& file) {
        auto elem = bloom.find(file);
        if (elem != bloom.end())
            bloom.erase(elem);
    }

    void file_data_struct::unload_color_correction_file(std::string& file) {
        auto elem = cc.find(file);
        if (elem != cc.end())
            cc.erase(elem);
    }

    void file_data_struct::unload_dof_file(std::string& file) {
        auto elem = dof.find(file);
        if (elem != dof.end())
            dof.erase(elem);
    }

    light_data_struct::light_data_struct() {

    }

    light_data_struct::~light_data_struct() {

    }

    post_process_data_struct::post_process_data_struct() {

    }

    post_process_data_struct::~post_process_data_struct() {

    }

    void post_process_data_struct::clear_data() {
        if (dof_file.size()) {
            file_data.unload_dof_file(dof_file);
            dof_file.clear();
        }

        if (dof.size())
            dof.clear();

        if (cc_file.size()) {
            file_data.unload_color_correction_file(cc_file);
            cc_file.clear();
        }

        if (cc.size())
            cc.clear();

        if (bloom_file.size()) {
            file_data.unload_bloom_file(bloom_file);
            bloom_file.clear();
        }

        if (bloom.size())
            bloom.clear();
    }

    bool post_process_data_struct::load_files(std::string& path) {
        data_struct* aft_data = &data_list[DATA_AFT];

        if (!aft_data->check_directory_exists(path.c_str()))
            return false;

        if (aft_data->check_file_exists(path.c_str(), "dof.txt"))
            dof_file = path + "dof.txt";
        else {
            dof_file.clear();
            dof_file.shrink_to_fit();
        }

        if (aft_data->check_file_exists(path.c_str(), "cc.txt"))
            cc_file = path + "cc.txt";
        else {
            cc_file.clear();
            cc_file.shrink_to_fit();
        }

        if (aft_data->check_file_exists(path.c_str(), "bloom.txt"))
            bloom_file = path + "bloom.txt";
        else {
            bloom_file.clear();
            bloom_file.shrink_to_fit();
        }

        if ((!dof_file.size() || file_data.load_dof_file(dof_file))
            && (!cc_file.size() || file_data.load_color_correction_file(cc_file))
            && (!bloom_file.size() || file_data.load_bloom_file(bloom_file))) {
            dof = file_data.get_dof_data(dof_file);
            cc = file_data.get_color_correction_data(cc_file);
            bloom = file_data.get_bloom_data(bloom_file);
            return true;
        }
        return false;
    }

    void post_process_data_struct::set_dof(::dof& d) {
        if (!d.ready)
            return;

        if (dof_file.size()) {
            file_data.unload_dof_file(dof_file);
            dof_file.clear();
        }

        dof.resize(0);
        dof.reserve(d.data.size());

        int32_t id = 0;
        for (dof_data& i : d.data) {
            pv_param::dof d;
            d.id = id++;
            if (i.flags & DOF_FOCUS)
                d.focus = i.focus;
            if (i.flags & DOF_FOCUS_RANGE)
                d.focus_range = i.focus_range;
            if (i.flags & DOF_FUZZING_RANGE)
                d.fuzzing_range = i.fuzzing_range;
            if (i.flags & DOF_RATIO)
                d.ratio = i.ratio;
            if (i.flags & DOF_QUALITY)
                d.quality = i.quality;
            if (i.flags & DOF_AUTO_FOCUS)
                d.chara_id = i.chara_id;
            dof.push_back(d);
        }
    }

    void post_process_data_clear_data() {
        post_process_data.clear_data();
    }

    pv_param::bloom& post_process_data_get_bloom_data(int32_t id) {
        for (pv_param::bloom& i : post_process_data.bloom)
            if (i.id == id)
                return i;

        return post_process_data.bloom_default;
    }

    pv_param::color_correction& post_process_data_get_color_correction_data(int32_t id) {
        for (pv_param::color_correction& i : post_process_data.cc)
            if (i.id == id)
                return i;

        return post_process_data.cc_default;
    }

    pv_param::dof& post_process_data_get_dof_data(int32_t id) {
        for (pv_param::dof& i : post_process_data.dof)
            if (i.id == id)
                return i;

        return post_process_data.dof_default;
    }

    bool post_process_data_load_files(int32_t pv_id) {
        std::string path = "rom/pv_param/";

        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "pv%03d/", pv_id);

        path += buf;

        if (data_list[DATA_AFT].check_directory_exists(path.c_str()))
            return post_process_data.load_files(path);
        return false;
    }

    bool post_process_data_load_files(int32_t pv_id, std::string& mdata_dir) {
        std::string path = "rom/pv_param/";

        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "pv%03d/", pv_id);

        path += buf;

        if (mdata_dir.size()) {
            std::string temp_path = mdata_dir + path;
            if (data_list[DATA_AFT].check_directory_exists(temp_path.c_str()))
                path = temp_path;
        }

        if (data_list[DATA_AFT].check_directory_exists(path.c_str()))
            return post_process_data.load_files(path);
        return false;
    }

    void post_process_data_set_dof(::dof& d) {
        post_process_data.set_dof(d);
    }

    static bool load_text_file(void* data, const char* path, const char* file, uint32_t hash) {
        stream s;
        s.open((std::string(path) + file).c_str(), "rb");
        if (!s.io.stream)
            return false;

        *(std::string*)data = s.read_string(s.length);
        s.close();
        return true;
    }
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

        rctx_ptr->post_process.blur->set_intensity(value);

        frame += get_delta_frame();
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
        tone_map->set_saturate_coeff(saturation);
        tone_map->set_exposure(exposure * 2.0f);

        vec3 tone_trans_start;
        vec3 tone_trans_end;
        CalcToneTrans(contrast + gamma.x, tone_trans_start.x, tone_trans_end.x);
        CalcToneTrans(contrast + gamma.y, tone_trans_start.y, tone_trans_end.y);
        CalcToneTrans(contrast + gamma.z, tone_trans_start.z, tone_trans_end.z);

        tone_map->set_tone_trans(tone_trans_start, tone_trans_end);

        frame += get_delta_frame();
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
        for (pv_param::chara_alpha& i : data.data)
            i = {};
    }

    void PostProcessCtrlCharaAlpha::Set() {
        int32_t index = 0;
        for (pv_param::chara_alpha& i : data.data) {
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
                draw_task_flags = DRAW_TASK_ALPHA_ORDER_1;
                break;
            case 1:
                draw_task_flags = DRAW_TASK_ALPHA_ORDER_2;
                break;
            case 2:
                draw_task_flags = DRAW_TASK_ALPHA_ORDER_3;
                break;
            }

            rob_chara_array_set_alpha_draw_task_flags(index, value, draw_task_flags);

            i.frame += get_delta_frame();
            if (i.frame > i.duration) {
                i.frame = -1.0f;
                i.prev_alpha = i.alpha;
            }
            index++;
        }
    }

    PostProcessCtrlCharaItemAlpha::PostProcessCtrlCharaItemAlpha() : callback(), callback_data() {

    }

    PostProcessCtrlCharaItemAlpha::~PostProcessCtrlCharaItemAlpha() {

    }

    void PostProcessCtrlCharaItemAlpha::Reset() {
        for (pv_param::chara_alpha& i : data.data)
            i = {};

        for (PostProcessCtrlCharaItemAlpha::Callback& i : callback)
            i = 0;

        for (void*& i : callback_data)
            i = 0;
    }

    void PostProcessCtrlCharaItemAlpha::Set() {
        int32_t index = 0;
        for (pv_param::chara_alpha& i : data.data) {
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

            if (callback)
                callback[index](callback_data[index], index, i.type, value);
            /*pv_game* v5 = pv_game_data_get();
            if (v5)
                sub_140115EC0(v5, index, i.type, value);*/

            i.frame += get_delta_frame();
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

#define DOF_FIX 1

    void PostProcessCtrlDof::Set() {
        float_t focus;
        float_t focus_range;
        float_t fuzzing_range;
        float_t ratio;
#if DOF_FIX
        float_t auto_focus;
        float_t auto_focus_range;
#endif
        bool autofocus = data.data.chara_id != -1;
#if DOF_FIX
        if (true) {
#else
        if (autofocus) {
#endif
            vec3 trans = vec3_null;
            rob_chara* rob_chr = rob_chara_array_get(autofocus ? data.data.chara_id : 0);
            if (rob_chr) {
                motion_blend_mot* mot = rob_chr->bone_data->motion_loaded.front();
                ::bone_data* bone_data = mot->bone_data.bones.data();

                mat4_get_translation(bone_data[MOTION_BONE_CL_KAO].node->mat, &trans);
            }

            camera* cam = rctx_ptr->camera;

            vec3 view_point;
            cam->get_view_point(view_point);
            float_t fov = (float_t)cam->get_fov();

            vec3_distance(trans, view_point, focus);

            float_t v5 = tanf(fov * DEG_TO_RAD_FLOAT * 0.5f);
            float_t v6 = focus * 1000.0f;
            float_t v7 = 36.0f / (v5 * 2.0f);
            v7 *= v7;
            float_t v8 = v6 * 0.36608f * v6 / (v7 - v6 * 0.36608f) * 0.001f;
            if (v8 >= 0.0f) {
                float_t v9 = v7 - (v6 * 3.6608f);
                float_t v10 = 20.0f;
                float_t v11 = (v7 * 20000.0f) / (v6 * 220000.0f + v6 * 11.0f * v6);
                focus_range = (v8 + v8) * 0.5f;
                float_t v13 = (v6 * 3.6608f * v6) / v9 * 0.001f;
                if (v11 >= 0.3328f)
                    v10 = v13;
                if (v10 <= 0.2f)
                    v10 = 0.2f;
                if (focus_range <= 0.2f)
                    focus_range = 0.2f;
                if (v11 >= 0.3328f)
                    ratio = 1.0f;
                else
                    ratio = v11 * 3.004807711f;

                fuzzing_range = v10 - focus_range;
                if (fuzzing_range <= 0.01f)
                    fuzzing_range = 0.01f;
            }
            else {
                focus = 0.0f;
                focus_range = 0.0f;
                fuzzing_range = 0.0f;
                ratio = 0.0f;
            }

            if (autofocus) {
                data.data.focus = focus;
                data.data.focus_range = focus_range;
                data.data.fuzzing_range = fuzzing_range;
                data.data.ratio = ratio;
            }

#if DOF_FIX
            auto_focus = focus;
            auto_focus_range = focus_range;
        }
#else
        }
        else if (frame < 0.0f)
            return;
#endif

        if (fabsf(duration) <= 0.000001f || autofocus && duration < 0.0f) {
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

#if DOF_FIX
        bool ret = true;
        if (ratio > 0.0f) {
            if (focus == 0.0f && auto_focus >= 0.0f) {
                focus = auto_focus;
                ret = false;
            }
            if (focus_range == 0.0f && auto_focus_range >= 0.0f) {
                focus_range = auto_focus_range;
                ret = false;
            }
        }

        if (!autofocus && ret && frame < 0.0f)
            return;
#endif

        post_process_dof* dof = rctx_ptr->post_process.dof;
        dof_pv pv;
        dof->get_dof_pv(&pv);
        pv.f2.focus = focus;
        pv.f2.focus_range = focus_range;
        pv.f2.fuzzing_range = fuzzing_range;
        pv.f2.ratio = ratio;
        dof->set_dof_pv(&pv);

        frame += get_delta_frame();
        if (frame > duration) {
            frame = -1.0f;
            duration = -1.0f;
            data.data_prev = data.data;
        }
    }

    void PostProcessCtrlDof::SetData(pv_param::dof* dof, float_t duration) {
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
