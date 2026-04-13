/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "a3da_to_dft_dsc.hpp"
#include "../../KKdLib/io/file_stream.hpp"
#include "../../KKdLib/post_process_table/dof.hpp"
#include "../../KKdLib/a3da.hpp"
#include "../../KKdLib/dsc.hpp"
#include "../../CRE/auth_3d.hpp"

struct dft_dsc_data {
    int32_t frames;
    dof dof;
    std::vector<int32_t> dof_index;

    dft_dsc_data();
    ~dft_dsc_data();
};

dft_dsc_data::dft_dsc_data() : frames() {

}

dft_dsc_data::~dft_dsc_data() {

}

static bool a3da_to_dof_data(const char* a3da_path, dft_dsc_data& data) {
    a3da cam_a3da_file;
    a3da::load_file(&cam_a3da_file, "DOF\\auth_3d\\", a3da_path, 0);

    if (!cam_a3da_file.dof.has_dof) {
        data.frames = 0;
        return false;
    }

    auth_3d cam_a3da;
    cam_a3da.load(&cam_a3da_file, 0, 0);

    data.frames = (int32_t)cam_a3da.play_control.size;

    data.dof.data.reserve(data.frames + 1ULL);
    data.dof_index.reserve(data.frames);

    data.dof.data.push_back({ (dof_flags)0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f });

    auth_3d_camera_root* cr = &cam_a3da.camera_root[0];

    for (int32_t i = 0; i < data.frames; i++) {
        cam_a3da.dof.model_transform.interpolate((float_t)i);
        bool enable = fabsf(cam_a3da.dof.model_transform.rotation_value.z) > 0.000001f;
        if (!enable) {
            data.dof_index.push_back(0);
            continue;
        }

        float_t focus = vec3::distance(cam_a3da.dof.model_transform.translation_value, cr->view_point_value);

        dof_data d;
        d.flags = (dof_flags)(DOF_QUALITY | DOF_RATIO | DOF_FUZZING_RANGE | DOF_FOCUS_RANGE | DOF_FOCUS);
        d.focus = focus;
        d.focus_range = cam_a3da.dof.model_transform.scale_value.x;
        d.fuzzing_range = cam_a3da.dof.model_transform.rotation_value.x;
        d.ratio = cam_a3da.dof.model_transform.rotation_value.y;
        d.quality = 1.0f;

        bool found = false;
        for (dof_data& j : data.dof.data)
            if (!memcmp(&d, &j, sizeof(dof_data))) {
                data.dof_index.push_back((int32_t)(&j - data.dof.data.data()));
                found = true;
                break;
            }

        if (found)
            continue;

        data.dof_index.push_back((int32_t)data.dof.data.size());
        data.dof.data.push_back(d);
    }
    return true;
}

void a3da_to_dft_dsc(int32_t pv_id) {
    char buf[0x200];

    sprintf_s(buf, sizeof(buf), "DOF\\script\\pv_%03d_hard.dsc", pv_id);
    file_stream s_src_dsc;
    s_src_dsc.open(buf, "rb");

    size_t src_dsc_length = s_src_dsc.get_length();
    void* src_dsc_data = force_malloc(src_dsc_length);
    s_src_dsc.read(src_dsc_data, src_dsc_length);
    s_src_dsc.close();

    dsc d_src;
    d_src.parse(src_dsc_data, src_dsc_length, DSC_FT);
    if (src_dsc_data)
        free(src_dsc_data);
    d_src.rebuild();

    dsc_get_func_id src_get_func_id = dsc_type_get_dsc_get_func_id(d_src.type);

    int32_t src_time_func_id = src_get_func_id("TIME");
    int32_t src_pv_branch_mode_func_id = src_get_func_id("PV_BRANCH_MODE");
    int32_t src_data_camera_func_id = src_get_func_id("DATA_CAMERA");

    int32_t time = -1;
    int32_t pv_branch_mode = -1;
    int32_t pv_branch_mode_2nd_camera_time = 9999999;
    for (dsc_data& i : d_src.data) {
        if (i.func == src_time_func_id) {
            time = ((int32_t*)d_src.get_func_data(&i))[0];
            continue;
        }
        else if (i.func == src_pv_branch_mode_func_id)
            pv_branch_mode = ((int32_t*)d_src.get_func_data(&i))[0];
        else if (i.func == src_data_camera_func_id) {
            int32_t camera = ((int32_t*)d_src.get_func_data(&i))[0];
            if (camera && pv_branch_mode == 2 && pv_branch_mode_2nd_camera_time == 9999999) {
                pv_branch_mode_2nd_camera_time = time;
                break;
            }
        }
    }

    sprintf_s(buf, sizeof(buf), "CAMPV%03d_BASE.a3da", pv_id);

    dft_dsc_data dft_data;
    bool base_cam = a3da_to_dof_data(buf, dft_data);

    size_t base_index_end = dft_data.dof_index.size();
    int32_t base_frames = dft_data.frames;

    sprintf_s(buf, sizeof(buf), "CAMPV%03d_PARTS02.a3da", pv_id);

    bool parts02_cam = a3da_to_dof_data(buf, dft_data);

    size_t parts02_index_end = dft_data.dof_index.size();
    int32_t parts02_frames = dft_data.frames;

    dsc d = dsc(DSC_FT, 0x15122517);

    dsc_get_func_id get_func_id = dsc_type_get_dsc_get_func_id(d.type);
    dsc_get_func_length get_func_length = d.get_dsc_get_func_length();
    dsc_get_func_name get_func_name = dsc_type_get_dsc_get_func_name(d.type);

    int32_t time_func_id = get_func_id("TIME");
    int32_t pv_branch_mode_func_id = get_func_id("PV_BRANCH_MODE");
    int32_t dof_func_id = get_func_id("DOF");

    const char* time_func_name = get_func_name(time_func_id);
    const char* pv_branch_mode_func_name = get_func_name(pv_branch_mode_func_id);
    const char* dof_func_name = get_func_name(dof_func_id);

    int32_t time_func_length = get_func_length(time_func_id);
    int32_t pv_branch_mode_func_length = get_func_length(pv_branch_mode_func_id);
    int32_t dof_func_length = get_func_length(dof_func_id);

    *d.add_func(pv_branch_mode_func_name,
        pv_branch_mode_func_id, pv_branch_mode_func_length) = 0;

    int32_t last_dof_index = -1;
    for (int32_t i = 0; i < base_frames; i++) {
        if (last_dof_index == dft_data.dof_index[i])
            continue;

        int32_t time = (int32_t)((double_t)i * (100000.0 / 60.0)); // 00000

        int32_t* time_func_data = (int32_t*)d.add_func(time_func_name,
            time_func_id, time_func_length);
        time_func_data[0] = time;

        if (time >= pv_branch_mode_2nd_camera_time) {
            int32_t* pv_branch_mode_data = (int32_t*)d.add_func(pv_branch_mode_func_name,
                pv_branch_mode_func_id, pv_branch_mode_func_length);
            pv_branch_mode_data[0] = 1;
        }

        int32_t* dof_func_data = (int32_t*)d.add_func(dof_func_name,
            dof_func_id, dof_func_length);
        dof_func_data[0] = dft_data.dof_index[i] ? 0x01 : 0x00;
        dof_func_data[1] = dft_data.dof_index[i];
        dof_func_data[2] = 0x00;

        last_dof_index = dft_data.dof_index[i];
    }

    last_dof_index = -1;
    for (int32_t i = 0; i < parts02_frames; i++) {
        if (last_dof_index == dft_data.dof_index[i + base_index_end])
            continue;

        int32_t time = (int32_t)((double_t)i * (100000.0 / 60.0)); // 00000
        time += pv_branch_mode_2nd_camera_time;

        if (time < pv_branch_mode_2nd_camera_time)
            continue;

        int32_t* time_func_data = (int32_t*)d.add_func(time_func_name,
            time_func_id, time_func_length);
        time_func_data[0] = time;

        int32_t* pv_branch_mode_data = (int32_t*)d.add_func(pv_branch_mode_func_name,
            pv_branch_mode_func_id, pv_branch_mode_func_length);
        pv_branch_mode_data[0] = 2;

        int32_t* dof_func_data = (int32_t*)d.add_func(dof_func_name,
            dof_func_id, dof_func_length);
        dof_func_data[0] = dft_data.dof_index[i + base_index_end] ? 0x01 : 0x00;
        dof_func_data[1] = dft_data.dof_index[i + base_index_end];
        dof_func_data[2] = 0x00;

        last_dof_index = dft_data.dof_index[i + base_index_end];
    }

    d.rebuild();

    void* dsc_data = 0;
    size_t dsc_length = 0;
    d.unparse(&dsc_data, &dsc_length);

    sprintf_s(buf, sizeof(buf), "DOF\\pv_script\\pv_%03d_dof.dsc", pv_id);
    file_stream s_dsc;
    s_dsc.open(buf, "wb");
    s_dsc.write(dsc_data, dsc_length);
    s_dsc.close();

    free_def(dsc_data);

    sprintf_s(buf, sizeof(buf), "DOF\\post_process_table\\pv%03d.dft", pv_id);
    dft_data.dof.write(buf);
}
