/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "process_edit_dsc.hpp"
#include "../../KKdLib/io/file_stream.hpp"
#include "../../KKdLib/dsc.hpp"
#include "../../KKdLib/pv_exp.hpp"

#if PROCESS_EDIT
static void process_edit_dsc(const char* path, const char* file,
    int32_t chara_id, dsc& d, pv_exp& exp, prj::stack_allocator* exp_alloc) {
    char buf[0x200];
    strcpy_s(buf, sizeof(buf), path);
    strcat_s(buf, sizeof(buf), file);
    strcat_s(buf, sizeof(buf), ".dsc");

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
    int32_t src_set_chara_func_id = src_get_func_id("SET_CHARA");
    int32_t src_pv_branch_mode_func_id = src_get_func_id("PV_BRANCH_MODE");
    int32_t src_edit_eye_anim_func_id = src_get_func_id("EDIT_EYE_ANIM");
    int32_t src_edit_expression_func_id = src_get_func_id("EDIT_EXPRESSION");
    int32_t src_edit_mouth_anim_func_id = src_get_func_id("EDIT_MOUTH_ANIM");
    int32_t src_edit_eyelid_anim_func_id = src_get_func_id("EDIT_EYELID_ANIM");

    dsc_get_func_id get_func_id = dsc_type_get_dsc_get_func_id(d.type);
    dsc_get_func_length get_func_length = d.get_dsc_get_func_length();
    dsc_get_func_name get_func_name = dsc_type_get_dsc_get_func_name(d.type);

    int32_t time_func_id = get_func_id("TIME");
    int32_t mouth_anim_func_id = get_func_id("MOUTH_ANIM");
    int32_t pv_branch_mode_func_id = get_func_id("PV_BRANCH_MODE");

    const char* time_func_name = get_func_name(time_func_id);
    const char* mouth_anim_func_name = get_func_name(mouth_anim_func_id);
    const char* pv_branch_mode_func_name = get_func_name(pv_branch_mode_func_id);

    int32_t time_func_length = get_func_length(time_func_id);
    int32_t mouth_anim_func_length = get_func_length(mouth_anim_func_id);
    int32_t pv_branch_mode_func_length = get_func_length(pv_branch_mode_func_id);

    std::vector<pv_exp_data> face_data;
    std::vector<pv_exp_data> face_cl_data;

    int32_t time = -1;
    int32_t pv_branch_mode = -1;
    int32_t mouth_anim_time = -1;
    int32_t mouth_anim_pv_branch_mode = -1;
    int32_t set_chara = 0;
    for (::dsc_data& i : d_src.data) {
        if (i.func == src_time_func_id)
            time = ((int32_t*)d_src.get_func_data(&i))[0];
        else if (i.func == src_set_chara_func_id)
            set_chara = ((int32_t*)d_src.get_func_data(&i))[0];
        else if (i.func == src_pv_branch_mode_func_id)
            pv_branch_mode = ((int32_t*)d_src.get_func_data(&i))[0];
        else if (set_chara != chara_id)
            continue;
        else if (i.func == src_edit_expression_func_id) {
            int32_t* data = d_src.get_func_data(&i);

            float_t frame = (float_t)(int32_t)((int64_t)time * 60 / 100000);
            int32_t expression_id = data[0];
            float_t blend_duration = (float_t)data[1] * 0.001f * 60.0f;

            pv_exp_data d;
            d.frame = frame;
            d.type = 0;
            d.id = expression_id;
            d.value = 1.0f;
            d.trans = blend_duration;
            face_data.push_back(d);
        }
        else if (i.func == src_edit_mouth_anim_func_id) {
            if (mouth_anim_pv_branch_mode != pv_branch_mode) {
                mouth_anim_pv_branch_mode = pv_branch_mode;

                int32_t* pv_branch_mode_func_data = (int32_t*)d.add_func(
                    pv_branch_mode_func_name, pv_branch_mode_func_id, pv_branch_mode_func_length);
                pv_branch_mode_func_data[0] = pv_branch_mode;
            }

            if (mouth_anim_time != time) {
                mouth_anim_time = time;

                int32_t* time_func_data = (int32_t*)d.add_func(
                    time_func_name, time_func_id, time_func_length);
                time_func_data[0] = time;
            }

            int32_t* data = d_src.get_func_data(&i);

            int32_t* mouth_anim_func_data = (int32_t*)d.add_func(
                time_func_name, mouth_anim_func_id, mouth_anim_func_length);
            mouth_anim_func_data[0] = chara_id;
            mouth_anim_func_data[1] = 0;
            mouth_anim_func_data[2] = data[0];
            mouth_anim_func_data[3] = data[1];
            mouth_anim_func_data[4] = 1000;
        }
        else if (i.func == src_edit_eyelid_anim_func_id) {
            int32_t* data = d_src.get_func_data(&i);

            float_t frame = (float_t)(int32_t)((int64_t)time * 60 / 100000);
            int32_t v342 = data[0];
            float_t blend_duration = (float_t)data[1] * 0.001f * 60.0f;
            float_t value = (float_t)data[2] * 0.001f;

            int32_t v346;
            switch (v342) {
            case 0:
                v346 = 2;
                break;
            case 1:
            default:
                v346 = 2;
                break;
            case 2:
                v346 = 1;
                break;
            case 3:
                v346 = 3;
                break;
            }

            pv_exp_data d;
            d.frame = frame;
            d.type = 1;
            d.id = v346;
            d.value = value;
            d.trans = blend_duration;
            face_cl_data.push_back(d);
        }
        else
            printf("Unsupported: %s\n", i.name);
    }

    face_data.push_back(pv_exp_data_null);
    face_cl_data.push_back(pv_exp_data_null);

    sprintf_s(buf, sizeof(buf), "PV3400_SMS_P%d_00", chara_id + 1);

    pv_exp_mot* exp_mot = &exp.motion_data[chara_id];
    exp_mot->face_data = exp_alloc->allocate<pv_exp_data>(face_data.data(), face_data.size());
    exp_mot->face_cl_data = exp_alloc->allocate<pv_exp_data>(face_cl_data.data(), face_cl_data.size());
    exp_mot->name = exp_alloc->allocate<char>(buf, utf8_length(buf) + 1);
}

void process_edit_dsc() {
    const char* path = "misc\\zamza\\";
    const int32_t chara_count = 5;

    char buf[0x200];

    strcpy_s(buf, sizeof(buf), path);
    strcat_s(buf, sizeof(buf), "pv_3400_base.dsc");

    file_stream s_base_dsc;
    s_base_dsc.open(buf, "rb");

    size_t src_dsc_length = s_base_dsc.get_length();
    void* src_dsc_data = force_malloc(src_dsc_length);
    s_base_dsc.read(src_dsc_data, src_dsc_length);
    s_base_dsc.close();

    dsc d_base;
    d_base.parse(src_dsc_data, src_dsc_length, DSC_FT);
    if (src_dsc_data)
        free(src_dsc_data);
    d_base.rebuild();

    dsc d = dsc(DSC_FT, 0x15122517);

    d.merge(1, d_base);

    d.rebuild();

    prj::stack_allocator* exp_alloc = new prj::stack_allocator;

    pv_exp exp;
    exp.motion_data = exp_alloc->allocate<pv_exp_mot>(chara_count);
    exp.motion_num = chara_count;

    for (int32_t i = 0; i < chara_count; i++) {
        sprintf_s(buf, sizeof(buf), "pv_3400_p%d", i + 1);
        process_edit_dsc(path, buf, i, d, exp, exp_alloc);
    }

    exp.ready = true;

    void* exp_data = 0;
    size_t exp_length = 0;
    exp.pack_file(&exp_data, &exp_length);

    delete exp_alloc;

    strcpy_s(buf, sizeof(buf), path);
    strcat_s(buf, sizeof(buf), "exp_PV3400.bin");

    file_stream s_exp;
    s_exp.open(buf, "wb");
    s_exp.write(exp_data, exp_length);
    s_exp.close();

    d.rebuild();

    void* dsc_data = 0;
    size_t dsc_length = 0;
    d.unparse(&dsc_data, &dsc_length);

    strcpy_s(buf, sizeof(buf), path);
    strcat_s(buf, sizeof(buf), "pv_3400.dsc");

    file_stream s_dsc;
    s_dsc.open(buf, "wb");
    s_dsc.write(dsc_data, dsc_length);
    s_dsc.close();

    if (dsc_data)
        free(dsc_data);
}
#endif
