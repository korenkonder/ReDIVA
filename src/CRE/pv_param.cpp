/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "pv_param.hpp"
#include "../KKdLib/io/file_stream.hpp"
#include "../KKdLib/str_utils.hpp"
#include "app_system_detail.hpp"
#include "render.hpp"
#include "render_context.hpp"

extern render_context* rctx_ptr;

namespace pv_param {
    struct file_data_struct {
        std::vector<dof> dof_default;
        std::vector<color_correction> cc_default;
        std::vector<bloom> bloom_default;
        std::map<std::string, std::vector<dof>> dof;
        std::map<std::string, std::vector<color_correction>> cc;
        std::map<std::string, std::vector<bloom>> bloom;

        file_data_struct();
        ~file_data_struct();

        std::vector<pv_param::bloom>& get_bloom_data(std::string& file);
        std::vector<pv_param::color_correction>& get_color_correction_data(std::string& file);
        std::vector<pv_param::dof>& get_dof_data(std::string& file);
        bool load_bloom_file(std::string& file);
        bool load_color_correction_file(std::string& file);
        bool load_dof_file(std::string& file);
        void unload_bloom_file(std::string& file);
        void unload_color_correction_file(std::string& file);
        void unload_dof_file(std::string& file);
    };

    struct light_data_struct {
        light_param_light_data light_default;
        std::string chara_light_file;
        std::string stage_light_file;
        std::map<int32_t, light_param_light_data> chara_light;
        std::map<int32_t, light_param_light_data> stage_light;

        light_data_struct();
        ~light_data_struct();

        void clear_data();
        bool load_file(std::string& file, std::map<int32_t, light_param_light_data>& map);
        bool load_files(std::string& path);
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

        void clear_data();
        bool load_files(std::string& path);
        void set_dof(::dof& d);
    };

    file_data_struct file_data;
    light_data_struct light_data;
    post_process_data_struct post_process_data;

    static char* get_next_item(char*& str);
    static bool load_text_file(void* data, const char* path, const char* file, uint32_t hash);

    bloom::bloom() {
        id = -1;
        color = 0.0f;
        brightpass = 0.0f;
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
        gamma = 1.0f;
        contrast = 1.0f;
    }

    void light_data_clear_data() {
        light_data.clear_data();
    }

    light_param_light_data& light_data_get_chara_light_data(int32_t id) {
        auto elem = light_data.chara_light.find(id);
        if (elem != light_data.chara_light.end())
            return elem->second;

        return light_data.light_default;
    }

    light_param_light_data& light_data_get_stage_light_data(int32_t id) {
        auto elem = light_data.stage_light.find(id);
        if (elem != light_data.stage_light.end())
            return elem->second;

        return light_data.light_default;
    }

    bool light_data_load_files(int32_t pv_id, std::string&& mdata_dir) {
        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "pv%03d", pv_id);

        std::string path;
        path.assign("rom/pv_param/");
        path.append(buf);

        if (mdata_dir.size()) {
            std::string temp_path;
            temp_path.assign(mdata_dir);
            temp_path.append(path);
            if (data_list[DATA_AFT].check_directory_exists(temp_path.c_str()))
                path.assign(temp_path);
        }

        if (data_list[DATA_AFT].check_directory_exists(path.c_str()))
            return light_data.load_files(path);
        return false;
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

    bool post_process_data_load_files(int32_t pv_id, std::string&& mdata_dir) {
        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "pv%03d", pv_id);

        std::string path;
        path.assign("rom/pv_param/");
        path.append(buf);

        if (mdata_dir.size()) {
            std::string temp_path;
            temp_path.assign(mdata_dir);
            temp_path.append(path);
            if (data_list[DATA_AFT].check_directory_exists(temp_path.c_str()))
                path.assign(temp_path);
        }

        if (data_list[DATA_AFT].check_directory_exists(path.c_str()))
            return post_process_data.load_files(path);
        return false;
    }

    void post_process_data_set_dof(::dof& d) {
        post_process_data.set_dof(d);
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
        if (!str_utils_text_file_parse(data.c_str(), data.size(), buf, lines, count))
            return false;

        if (count <= 1 || str_utils_compare(get_next_item(lines[0]), "ID")) {
            free_def(buf);
            free_def(lines);
            return false;
        }

        lines++;
        count--;

        std::vector<pv_param::bloom> bloom;
        bloom.reserve(count);

        for (size_t i = 0; i < count; i++) {
            pv_param::bloom bloom_data;
            bloom_data.id = atoi(get_next_item(lines[i]));
            bloom_data.color.x = (float_t)atof(get_next_item(lines[i]));
            bloom_data.color.y = (float_t)atof(get_next_item(lines[i]));
            bloom_data.color.z = (float_t)atof(get_next_item(lines[i]));
            bloom_data.brightpass.x = (float_t)atof(get_next_item(lines[i]));
            bloom_data.brightpass.y = (float_t)atof(get_next_item(lines[i]));
            bloom_data.brightpass.z = (float_t)atof(get_next_item(lines[i]));
            bloom_data.range = (float_t)atof(get_next_item(lines[i]));
            bloom.push_back(bloom_data);
        }

        this->bloom.insert_or_assign(file, bloom);

        lines--;

        free_def(buf);
        free_def(lines);
        return true;
    }

    bool file_data_struct::load_color_correction_file(std::string& file) {
        std::string data;
        if (!data_list[DATA_AFT].load_file(&data, file.c_str(), load_text_file))
            return false;

        char* buf;
        char** lines;
        size_t count;
        if (!str_utils_text_file_parse(data.c_str(), data.size(), buf, lines, count))
            return false;

        if (count <= 1 || str_utils_compare(get_next_item(lines[0]), "ID")) {
            free_def(buf);
            free_def(lines);
            return false;
        }

        lines++;
        count--;

        std::vector<pv_param::color_correction> cc;
        cc.reserve(count);

        for (size_t i = 0; i < count; i++) {
            pv_param::color_correction cc_data;
            cc_data.id = atoi(get_next_item(lines[i]));
            cc_data.hue = (float_t)atof(get_next_item(lines[i]));
            cc_data.saturation = (float_t)atof(get_next_item(lines[i]));
            cc_data.lightness = (float_t)atof(get_next_item(lines[i]));
            cc_data.exposure = (float_t)atof(get_next_item(lines[i]));
            cc_data.gamma.x = (float_t)atof(get_next_item(lines[i]));
            cc_data.gamma.y = (float_t)atof(get_next_item(lines[i]));
            cc_data.gamma.z = (float_t)atof(get_next_item(lines[i]));
            cc_data.contrast = (float_t)atof(get_next_item(lines[i]));
            cc.push_back(cc_data);
        }

        this->cc.insert_or_assign(file, cc);

        lines--;

        free_def(buf);
        free_def(lines);
        return true;
    }

    bool file_data_struct::load_dof_file(std::string& file) {
        std::string data;
        if (!data_list[DATA_AFT].load_file(&data, file.c_str(), load_text_file))
            return false;

        char* buf;
        char** lines;
        size_t count;
        if (!str_utils_text_file_parse(data.c_str(), data.size(), buf, lines, count))
            return false;

        if (count <= 1 || str_utils_compare(get_next_item(lines[0]), "ID")) {
            free_def(buf);
            free_def(lines);
            return false;
        }

        lines++;
        count--;

        std::vector<pv_param::dof> dof;
        dof.reserve(count);

        for (size_t i = 0; i < count; i++) {
            pv_param::dof dof_data;
            dof_data.id = atoi(get_next_item(lines[i]));
            dof_data.focus = (float_t)atof(get_next_item(lines[i]));
            dof_data.focus_range = (float_t)atof(get_next_item(lines[i]));
            dof_data.fuzzing_range = (float_t)atof(get_next_item(lines[i]));
            dof_data.ratio = (float_t)atof(get_next_item(lines[i]));
            dof_data.quality = (float_t)atof(get_next_item(lines[i]));
            dof.push_back(dof_data);
        }

        this->dof.insert_or_assign(file, dof);

        lines--;

        free_def(buf);
        free_def(lines);
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

    void light_data_struct::clear_data() {
        chara_light_file.clear();
        chara_light_file.shrink_to_fit();
        stage_light_file.clear();
        stage_light_file.shrink_to_fit();
        chara_light.clear();
        stage_light.clear();
    }

    bool light_data_struct::load_file(std::string& file, std::map<int32_t, light_param_light_data>& map) {
        std::string data;
        if (!data_list[DATA_AFT].load_file(&data, file.c_str(), load_text_file))
            return false;

        char* buf;
        char** lines;
        size_t count;
        if (!str_utils_text_file_parse(data.c_str(), data.size(), buf, lines, count))
            return false;

        if (count <= 1 || str_utils_compare(get_next_item(lines[0]), "Type")) {
            free_def(buf);
            free_def(lines);
            return false;
        }

        lines++;
        count--;

        for (size_t i = 0; i < count; i++) {
            light_param_light_data light;

            char* str = get_next_item(lines[i]);
            if (!str_utils_compare(str, "PARALLEL"))
                light.type = LIGHT_PARALLEL;
            else if (!str_utils_compare(str, "POINT"))
                light.type = LIGHT_POINT;
            else if (!str_utils_compare(str, "SPOT"))
                light.type = LIGHT_SPOT;
            else
                light.type = LIGHT_OFF;
            light.has_type = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.ambient.x) == 1)
                light.has_ambient = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.ambient.y) == 1)
                light.has_ambient = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.ambient.z) == 1)
                light.has_ambient = true;

            light.ambient.w = 1.0f;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.diffuse.x) == 1)
                light.has_diffuse = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.diffuse.y) == 1)
                light.has_diffuse = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.diffuse.z) == 1)
                light.has_diffuse = true;

            light.diffuse.w = 1.0f;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.specular.x) == 1)
                light.has_specular = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.specular.y) == 1)
                light.has_specular = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.specular.z) == 1)
                light.has_specular = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.specular.w) == 1)
                light.has_specular = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.position.x) == 1)
                light.has_position = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.position.y) == 1)
                light.has_position = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.position.z) == 1)
                light.has_position = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.tone_curve.start_point) == 1)
                light.has_tone_curve = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.tone_curve.end_point) == 1)
                light.has_tone_curve = true;

            if (sscanf_s(get_next_item(lines[i]), "%g", &light.tone_curve.coefficient) == 1)
                light.has_tone_curve = true;

            int32_t id;
            if (sscanf_s(get_next_item(lines[i]), "%d", &id) != 1)
                id = -1;

            map.insert_or_assign(id, light);
        }

        lines--;

        free_def(buf);
        free_def(lines);
        return true;
    }

    bool light_data_struct::load_files(std::string& path) {
        data_struct* aft_data = &data_list[DATA_AFT];

        if (!aft_data->check_directory_exists(path.c_str()))
            return true;

        if (aft_data->check_file_exists(path.c_str(), "/chara_light.txt")) {
            chara_light_file.assign(path);
            chara_light_file.append("/chara_light.txt");
        }
        else
            chara_light_file.clear();

        if (aft_data->check_file_exists(path.c_str(), "/stage_light.txt")) {
            stage_light_file.assign(path);
            stage_light_file.append("/stage_light.txt");
        }
        else
            stage_light_file.clear();

        load_file(chara_light_file, chara_light);
        load_file(stage_light_file, stage_light);
        return true;
    }

    post_process_data_struct::post_process_data_struct() {

    }

    post_process_data_struct::~post_process_data_struct() {

    }

    void post_process_data_struct::clear_data() {
        if (dof_file.size()) {
            file_data.unload_dof_file(dof_file);
            dof_file.clear();
            dof_file.shrink_to_fit();
        }

        if (dof.size()) {
            dof.clear();
            dof.shrink_to_fit();
        }

        if (cc_file.size()) {
            file_data.unload_color_correction_file(cc_file);
            cc_file.clear();
            cc_file.shrink_to_fit();
        }

        if (cc.size()) {
            cc.clear();
            cc.shrink_to_fit();
        }

        if (bloom_file.size()) {
            file_data.unload_bloom_file(bloom_file);
            bloom_file.clear();
            bloom_file.shrink_to_fit();
        }

        if (bloom.size()) {
            bloom.clear();
            bloom.shrink_to_fit();
        }
    }

    bool post_process_data_struct::load_files(std::string& path) {
        data_struct* aft_data = &data_list[DATA_AFT];

        if (!aft_data->check_directory_exists(path.c_str()))
            return false;

        if (aft_data->check_file_exists(path.c_str(), "/dof.txt")) {
            dof_file.assign(path);
            dof_file.append("/dof.txt");
        }
        else
            dof_file.clear();

        if (aft_data->check_file_exists(path.c_str(), "/cc.txt")) {
            cc_file.assign(path);
            cc_file.append("/cc.txt");
        }
        else
            cc_file.clear();

        if (aft_data->check_file_exists(path.c_str(), "/bloom.txt")) {
            bloom_file.assign(path);
            bloom_file.append("/bloom.txt");
        }
        else
            bloom_file.clear();

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

    static char* get_next_item(char*& str) {
        char* orig_str = str;
        while (*str)
            if (*str == ',') {
                *str++ = 0;
                break;
            }
            else
                str++;
        return orig_str;
    }

    static bool load_text_file(void* data, const char* path, const char* file, uint32_t hash) {
        std::string s;
        s.assign(path);
        s.append(file);

        file_stream fs;
        fs.open(s.c_str(), "rb");
        if (fs.check_not_null()) {
            *(std::string*)data = fs.read_string(fs.get_length());
            fs.close();
            return true;
        }
        return false;
    }
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
        post_process_task_set_chara_item_alpha_callback callback[ROB_CHARA_COUNT];
        void* callback_data[ROB_CHARA_COUNT];

        PostProcessCtrlCharaItemAlpha();
        ~PostProcessCtrlCharaItemAlpha();

        virtual void Reset() override;
        virtual void Set() override;
    };

    class PostProcessCtrlDof : public PostProcessCtrl {
    public:
        struct Data {
            pv_param::dof data;
            pv_param::dof data_prev;

            Data();
        } data;

        post_process_task_set_dof_callback callback;
        void* callback_data;

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

        virtual bool init() override;
        virtual bool ctrl() override;
        virtual bool dest() override;
    };

    PostProcessTask post_process_task;

    bool post_process_task_add_task() {
        return app::TaskWork::add_task(&post_process_task, "PV POST PROCESS TASK");
    }

    void post_process_task_set_bloom_data(
        pv_param::bloom& data, float_t duration) {
        PostProcessCtrlBloom& bloom = post_process_task.bloom;
        bloom.frame = 0.0f;
        bloom.duration = duration;
        bloom.data.data = data;
    }

    void post_process_task_set_color_correction_data(
        pv_param::color_correction& data, float_t duration) {
        PostProcessCtrlCC& cc = post_process_task.cc;
        cc.frame = 0.0f;
        cc.duration = duration;
        cc.data.data = data;
    }

    void post_process_task_set_dof_data(
        pv_param::dof& data, float_t duration) {
        PostProcessCtrlDof& dof = post_process_task.dof;
        dof.frame = 0.0f;
        dof.duration = duration;
        dof.data.data = data;
        dof.callback = 0;
        dof.callback_data = 0;
    }

    extern void post_process_task_set_dof_data(
        pv_param::dof& data, float_t duration,
        post_process_task_set_dof_callback callback, void* callback_data) {
        PostProcessCtrlDof& dof = post_process_task.dof;
        dof.frame = 0.0f;
        dof.duration = duration;
        dof.data.data = data;
        dof.callback = callback;
        dof.callback_data = callback_data;
    }

    void post_process_task_set_chara_alpha(
        int32_t chara_id, int32_t type, float_t alpha, float_t duration) {
        PostProcessCtrlCharaAlpha& chara_alpha = post_process_task.chara_alpha;
        pv_param::chara_alpha& chara_alpha_data = chara_alpha.data.data[chara_id];
        chara_alpha_data.type = type;
        chara_alpha_data.frame = 0.0f;
        chara_alpha_data.alpha = alpha;
        chara_alpha_data.duration = duration;
    }

    void post_process_task_set_chara_item_alpha(
        int32_t chara_id, int32_t type, float_t alpha, float_t duration,
        post_process_task_set_chara_item_alpha_callback callback, void* callback_data) {
        PostProcessCtrlCharaItemAlpha& chara_item_alpha = post_process_task.chara_item_alpha;
        pv_param::chara_alpha& chara_item_alpha_data = chara_item_alpha.data.data[chara_id];
        chara_item_alpha_data.type = type;
        chara_item_alpha_data.frame = 0.0f;
        chara_item_alpha_data.alpha = alpha;
        chara_item_alpha_data.duration = duration;

        chara_item_alpha.callback[chara_id] = callback;
        chara_item_alpha.callback_data[chara_id] = callback_data;
    }

    bool post_process_task_del_task() {
        return post_process_task.del();
    }

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
            value = vec3::lerp(data.data_prev.color, data.data.color, frame / duration);

        rctx_ptr->render.set_intensity(value);

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
            saturation = lerp_def(data.data_prev.saturation, data.data.saturation, t);
            exposure = lerp_def(data.data_prev.exposure, data.data.exposure, t);
            gamma = vec3::lerp(data.data_prev.gamma, data.data.gamma, t);
            contrast = lerp_def(data.data_prev.contrast, data.data.contrast, t);
        }

        rctx_ptr->render.set_saturate_coeff(saturation, 1, false);
        rctx_ptr->render.set_exposure(exposure * 2.0f);

        vec3 tone_trans_start;
        vec3 tone_trans_end;
        CalcToneTrans(contrast + gamma.x, tone_trans_start.x, tone_trans_end.x);
        CalcToneTrans(contrast + gamma.y, tone_trans_start.y, tone_trans_end.y);
        CalcToneTrans(contrast + gamma.z, tone_trans_start.z, tone_trans_end.z);

        rctx_ptr->render.set_tone_trans(tone_trans_start, tone_trans_end, 1);

        frame += get_delta_frame();
        if (frame > duration) {
            frame = -1.0f;
            duration = -1.0f;
            data.data_prev = data.data;
        }
    }

    void PostProcessCtrlCC::CalcToneTrans(float_t value, float_t& tone_trans_start, float_t& tone_trans_end) {
        float_t pow = max_def(value, 0.125f) * 0.5f;
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
            if (fabsf(i.duration) <= 0.000001f)
                value = i.alpha;
            else if (i.duration <= 0.0)
                value = i.prev_alpha;
            else
                value = lerp_def(i.prev_alpha, i.alpha, i.frame / i.duration);

            mdl::ObjFlags obj_flags;
            switch (i.type) {
            case 0:
            default:
                obj_flags = mdl::OBJ_ALPHA_ORDER_POST_GLITTER;
                break;
            case 1:
                obj_flags = mdl::OBJ_ALPHA_ORDER_POST_TRANSLUCENT;
                break;
            case 2:
                obj_flags = mdl::OBJ_ALPHA_ORDER_POST_OPAQUE;
                break;
            }

            rob_chara_array_set_alpha_obj_flags(index, value, obj_flags);

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

        for (post_process_task_set_chara_item_alpha_callback& i : callback)
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
            if (fabsf(i.duration) <= 0.000001f)
                value = i.alpha;
            else if (i.duration <= 0.0)
                value = i.prev_alpha;
            else
                value = lerp_def(i.prev_alpha, i.alpha, i.frame / i.duration);

            if (callback)
                callback[index](callback_data[index], index, i.type, value);

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

    PostProcessCtrlDof::PostProcessCtrlDof() : callback(), callback_data() {

    }

    PostProcessCtrlDof::~PostProcessCtrlDof() {

    }

    void PostProcessCtrlDof::Reset() {
        frame = -1.0f;
    }

    static void get_autofocus_data(float_t distance, float_t fov, bool& enable,
        float_t& focus, float_t& focus_range, float_t& fuzzing_range, float_t& ratio) {
        float_t v5 = tanf(fov * DEG_TO_RAD_FLOAT * 0.5f);
        float_t v6 = distance * 1000.0f;
        float_t v7 = (36.0f / (v5 * 2.0f)) * (36.0f / (v5 * 2.0f));

        float_t _focus_range = (v6 * 0.36608f * v6) / (v7 - v6 * 0.36608f) * 0.001f;
        if (_focus_range >= 0.0f) {
            float_t range;
            float_t _ratio;
            float_t v11 = (v7 * 20000.0f) / (v6 * 220000.0f + v6 * 11.0f * v6);
            if (v11 >= 0.3328f) {
                range = (v6 * 3.6608f * v6) / (v7 - v6 * 3.6608f) * 0.001f;
                _ratio = 1.0f;
            }
            else {
                range = 20.0f;
                _ratio = v11 * 3.0048077f;
            }

            range = max_def(range, 0.2f);
            _focus_range = max_def(_focus_range, 0.2f);
            float_t _fuzzing_range = max_def(range - _focus_range, 0.01f);

            enable = true;
            focus = distance;
            focus_range = _focus_range;
            fuzzing_range = _fuzzing_range;
            ratio = _ratio;
        }
        else
            enable = false;
    }

    void PostProcessCtrlDof::Set() {
        bool autofocus = data.data.chara_id != -1;
        if (autofocus) {
            if (!rctx_ptr->render.get_dof_update())
                return;

            vec3 trans = 0.0f;
            if (callback)
                trans = callback(callback_data, data.data.chara_id);
            else {
                rob_chara* rob_chr = rob_chara_array_get(data.data.chara_id);
                if (rob_chr)
                    mat4_get_translation(rob_chr->get_bone_data_mat(MOTION_BONE_CL_KAO), &trans);
            }

            camera* cam = rctx_ptr->camera;

            bool enable = true;
            vec3 view_point;
            cam->get_view_point(view_point);
            get_autofocus_data(vec3::distance(trans, view_point), cam->get_fov(),
                enable, data.data.focus, data.data.focus_range,
                data.data.fuzzing_range, data.data.ratio);
            rctx_ptr->render.set_dof_enable(enable);
        }
        else if (frame < 0.0f)
            return;

        float_t focus;
        float_t focus_range;
        float_t fuzzing_range;
        float_t ratio;
        if (fabsf(duration) <= 0.000001f || autofocus) {
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
            focus = lerp_def(data.data_prev.focus, data.data.focus, t);
            focus_range = lerp_def(data.data_prev.focus_range, data.data.focus_range, t);
            fuzzing_range = lerp_def(data.data_prev.fuzzing_range, data.data.fuzzing_range, t);
            ratio = lerp_def(data.data_prev.ratio, data.data.ratio, t);
        }

        rctx_ptr->render.set_dof_data(focus, focus_range, fuzzing_range, ratio);

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

        if (fabsf(duration) > 0.000001f)
            rctx_ptr->render.get_dof_data(data.data_prev.focus,
                data.data_prev.focus_range, data.data_prev.fuzzing_range, data.data_prev.ratio);
        else
            data.data_prev = data.data;
    }

    PostProcessTask::PostProcessTask() {

    }

    PostProcessTask::~PostProcessTask() {

    }

    bool PostProcessTask::init() {
        dof.Reset();
        cc.Reset();
        bloom.Reset();
        chara_alpha.Reset();
        chara_item_alpha.Reset();
        return true;
    }

    bool PostProcessTask::ctrl() {
        dof.Set();
        cc.Set();
        bloom.Set();
        chara_alpha.Set();
        chara_item_alpha.Set();
        return false;
    }

    bool PostProcessTask::dest() {
        dof.Reset();
        cc.Reset();
        bloom.Reset();
        chara_alpha.Reset();
        chara_item_alpha.Reset();
        return true;
    }
}
