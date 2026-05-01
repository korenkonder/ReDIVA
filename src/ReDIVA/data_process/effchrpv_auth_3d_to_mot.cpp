/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effchrpv_auth_3d_to_mot.hpp"
#include "../../KKdLib/farc.hpp"
#include "../../KKdLib/pvpp.hpp"
#include "../../KKdLib/waitable_timer.hpp"
#include "../../CRE/rob/rob.hpp"
#include "../../CRE/auth_3d.hpp"
#include "../../CRE/data.hpp"

struct auth_3d_to_mot_keys {
    std::vector<float_t> x;
    std::vector<float_t> y;
    std::vector<float_t> z;

    auth_3d_to_mot_keys();
    ~auth_3d_to_mot_keys();
};

struct auth_3d_to_mot_data {
    auth_3d_id id;
    int32_t gblctr;
    int32_t n_hara;
    int32_t n_hara_y;
    int32_t j_hara_wj;
    int32_t n_kosi;
    int32_t j_mune_wj;
    int32_t n_mune_kl;
    int32_t j_mune_b_wj;
    int32_t j_kubi_wj;
    int32_t n_kao;
    int32_t j_kao_wj;
    int32_t j_eye_r_wj;
    int32_t j_eye_l_wj;
    int32_t n_waki_l;
    int32_t j_waki_l_wj;
    int32_t n_kata_l;
    int32_t j_kata_l_wj;
    int32_t j_ude_l_wj;
    int32_t j_te_l_wj;
    int32_t n_waki_r;
    int32_t j_waki_r_wj;
    int32_t n_kata_r;
    int32_t j_kata_r_wj;
    int32_t j_ude_r_wj;
    int32_t j_te_r_wj;
    int32_t j_kosi_wj;
    int32_t n_momo_l;
    int32_t j_momo_l_wj;
    int32_t j_sune_l_wj;
    int32_t j_asi_l_wj;
    int32_t n_momo_r;
    int32_t j_momo_r_wj;
    int32_t j_sune_r_wj;
    int32_t j_asi_r_wj;
    std::map<BONE_BLK, auth_3d_to_mot_keys> bone_keys;
    std::map<BONE_BLK, auth_3d_to_mot_keys> sec_bone_keys;

    auth_3d_to_mot_data(auth_3d_id id);
    ~auth_3d_to_mot_data();

    void get_bone_indices(const auth_3d_object_hrc* oh);
};

struct mot_data_bake;

struct mot_data_bake_data {
    int32_t performer;
    auth_3d_to_mot_data* data;
    std::atomic_uint32_t state;
    mot_data_bake* bake;

    mot_data_bake_data() : performer(), data(), bake() {

    }
};

struct mot_data_bake {
    int32_t pv_id;

    int32_t threads_count;

    const motion_set_info* set_info;
    std::thread* thread;
    mot_data_bake_data* mot_data;
    int32_t performer;
    std::mutex* alloc_mutex;
    prj::shared_ptr<prj::stack_allocator>* alloc;
    ::mot_set* mot_set;

    mot_data_bake();

    void write();

    static bool load_file(void* data, const char* path, const char* file, uint32_t hash);
};

struct effchrpv_auth_3d_to_mot {
    int32_t pv_id;
    const ROB_ID* rob_ids;

    std::unordered_map<std::string, string_hash> auth_3d_mot_names;
    std::map<uint32_t, auth_3d_id> auth_3d_mot_ids;

    std::vector<int32_t> rob_mot_ids;
    std::map<int32_t, auth_3d_to_mot_data> auth_3d_rob_mot_ids;

    mot_data_bake bake;

    mat4 n_hara_cp_src;
    mat4 n_hara_src;
    mat4 n_mune_b_src;
    mat4 n_kao_src;
    mat4 n_momo_l_src;
    mat4 n_momo_r_src;

    mat4 n_hara_cp_dst;
    mat4 n_hara_dst;
    mat4 cl_mune_dst;
    mat4 n_mune_b_dst;
    mat4 n_kao_dst;
    mat4 cl_kao_dst;
    mat4 cl_momo_l_dst;
    mat4 cl_momo_r_dst;

    effchrpv_auth_3d_to_mot();
    ~effchrpv_auth_3d_to_mot();

    void add_chara_effect_auth_3d(uint32_t hash, int32_t id);
    void add_file_name(const std::string& file, const std::string& category);
    void disp();
    void get_body_anim(int32_t frame, bool add_keys);
    void get_hand_anim(int32_t frame);
    void load(int32_t frame);
    void modify_play_param(pvpp* play_param);
    void post_modify_play_param();
    void write_mot_set();
};

effchrpv_auth_3d_to_mot* effchr_a2m;

extern render_context* rctx_ptr;

void effchrpv_auth_3d_to_mot_init(int32_t pv_id, const ROB_ID* rob_ids) {
    if (!effchr_a2m)
        effchr_a2m = new effchrpv_auth_3d_to_mot;

    effchr_a2m->pv_id = pv_id;
    effchr_a2m->rob_ids = rob_ids;

    effchr_a2m->bake.pv_id = pv_id;
}

void effchrpv_auth_3d_to_mot_add_chara_effect_auth_3d(uint32_t hash, int32_t id) {
    effchr_a2m->add_chara_effect_auth_3d(hash, id);
}

void effchrpv_auth_3d_to_mot_add_file_name(const std::string& file, const std::string& category) {
    effchr_a2m->add_file_name(file, category);
}

void effchrpv_auth_3d_to_mot_disp() {
    effchr_a2m->disp();
}

void effchrpv_auth_3d_to_mot_get_body_anim(int32_t frame, bool add_keys) {
    effchr_a2m->get_body_anim(frame, add_keys);
}

void effchrpv_auth_3d_to_mot_get_hand_anim(int32_t frame) {
    effchr_a2m->get_hand_anim(frame);
}

void effchrpv_auth_3d_to_mot_load(int32_t frame) {
    effchr_a2m->load(frame);
}

void effchrpv_auth_3d_to_mot_modify_play_param(void* play_param) {
    effchr_a2m->modify_play_param((pvpp*)play_param);
}

void effchrpv_auth_3d_to_mot_post_modify_play_param() {
    effchr_a2m->post_modify_play_param();
}

void effchrpv_auth_3d_to_mot_write_mot_set() {
    effchr_a2m->write_mot_set();
}

void effchrpv_auth_3d_to_mot_free() {
    if (effchr_a2m)
        delete effchr_a2m;
    effchr_a2m = 0;
}

auth_3d_to_mot_keys::auth_3d_to_mot_keys() {

}

auth_3d_to_mot_keys::~auth_3d_to_mot_keys() {

}

auth_3d_to_mot_data::auth_3d_to_mot_data(auth_3d_id id) : id(id), gblctr(-1), n_hara(-1), n_hara_y(-1),
j_hara_wj(-1), n_kosi(-1), j_mune_wj(-1), n_mune_kl(-1), j_mune_b_wj(-1), j_kubi_wj(-1),
n_kao(-1), j_kao_wj(-1), j_eye_r_wj(-1), j_eye_l_wj(-1), n_waki_l(-1), j_waki_l_wj(-1),
n_kata_l(-1), j_kata_l_wj(-1), j_ude_l_wj(-1), j_te_l_wj(-1), n_waki_r(-1),
j_waki_r_wj(-1), n_kata_r(-1), j_kata_r_wj(-1), j_ude_r_wj(-1), j_te_r_wj(-1),
j_kosi_wj(-1), n_momo_l(-1), j_momo_l_wj(-1), j_sune_l_wj(-1), j_asi_l_wj(-1),
n_momo_r(-1), j_momo_r_wj(-1), j_sune_r_wj(-1), j_asi_r_wj(-1) {
}

auth_3d_to_mot_data::~auth_3d_to_mot_data() {

}

static int32_t get_bone_index(const auth_3d_object_hrc* oh, const char* name) {
    uint32_t name_hash = hash_utf8_murmurhash(name);
    for (const auth_3d_object_node& i : oh->node)
        if (hash_string_murmurhash(i.name) == name_hash)
            return (int32_t)(&i - oh->node.data());
    return -1;
}

void auth_3d_to_mot_data::get_bone_indices(const auth_3d_object_hrc* oh) {
    gblctr = get_bone_index(oh, "gblctr");
    n_hara = get_bone_index(oh, "n_hara");
    n_hara_y = get_bone_index(oh, "n_hara_y");
    j_hara_wj = get_bone_index(oh, "j_hara_wj");
    n_kosi = get_bone_index(oh, "n_kosi");
    j_mune_wj = get_bone_index(oh, "j_mune_wj");
    n_mune_kl = get_bone_index(oh, "n_mune_kl");
    j_mune_b_wj = get_bone_index(oh, "j_mune_b_wj");
    j_kubi_wj = get_bone_index(oh, "j_kubi_wj");
    n_kao = get_bone_index(oh, "n_kao");
    j_kao_wj = get_bone_index(oh, "j_kao_wj");
    j_eye_r_wj = get_bone_index(oh, "j_eye_r_wj");
    j_eye_l_wj = get_bone_index(oh, "j_eye_l_wj");
    n_waki_l = get_bone_index(oh, "n_waki_l");
    j_waki_l_wj = get_bone_index(oh, "j_waki_l_wj");
    n_kata_l = get_bone_index(oh, "n_kata_l");
    j_kata_l_wj = get_bone_index(oh, "j_kata_l_wj");
    j_ude_l_wj = get_bone_index(oh, "j_ude_l_wj");
    j_te_l_wj = get_bone_index(oh, "j_te_l_wj");
    n_waki_r = get_bone_index(oh, "n_waki_r");
    j_waki_r_wj = get_bone_index(oh, "j_waki_r_wj");
    n_kata_r = get_bone_index(oh, "n_kata_r");
    j_kata_r_wj = get_bone_index(oh, "j_kata_r_wj");
    j_ude_r_wj = get_bone_index(oh, "j_ude_r_wj");
    j_te_r_wj = get_bone_index(oh, "j_te_r_wj");
    j_kosi_wj = get_bone_index(oh, "j_kosi_wj");
    n_momo_l = get_bone_index(oh, "n_momo_l");
    j_momo_l_wj = get_bone_index(oh, "j_momo_l_wj");
    j_sune_l_wj = get_bone_index(oh, "j_sune_l_wj");
    j_asi_l_wj = get_bone_index(oh, "j_asi_l_wj");
    n_momo_r = get_bone_index(oh, "n_momo_r");
    j_momo_r_wj = get_bone_index(oh, "j_momo_r_wj");
    j_sune_r_wj = get_bone_index(oh, "j_sune_r_wj");
    j_asi_r_wj = get_bone_index(oh, "j_asi_r_wj");
}

mot_data_bake::mot_data_bake() : pv_id(), threads_count(4), set_info(),
thread(), mot_data(), performer(), alloc_mutex(), alloc(), mot_set() {

}

void mot_data_bake::write() {
    if (!set_info)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    {
        std::string mot_file;
        mot_file.append("mot_");
        mot_file.append(set_info->name);
        mot_file.append(".bin");

        farc f;

        f.add_file(mot_file.c_str());
        farc_file* ff = &f.files.back();
        mot_set->pack_file(&ff->data, &ff->size);
        ff->compressed = true;

        std::string mot_farc;
        mot_farc.append(sprintf_s_string("pv%03d\\mot_", pv_id));
        mot_farc.append(set_info->name);
        f.write(mot_farc.c_str(), FARC_FArC, FARC_NONE, true, false);
    }

    delete[] thread;
    delete[] mot_data;
    delete alloc_mutex;

    delete alloc;

    thread = 0;
    mot_data = 0;
    alloc_mutex = 0;
    mot_set = 0;
    alloc = 0;
    set_info = 0;
}

bool mot_data_bake::load_file(void* data, const char* path, const char* file, uint32_t hash) {
    mot_data_bake* bake = (mot_data_bake*)data;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", bake->pv_id);
    bake->set_info = aft_mot_db->get_motion_set_by_name(buf);
    if (!bake->set_info)
        return true;

    bake->alloc = new prj::shared_ptr<prj::stack_allocator>(new prj::stack_allocator);
    bake->alloc_mutex = 0;
    bake->thread = 0;
    bake->mot_data = 0;
    bake->performer = 1;

    bake->mot_set = (*bake->alloc)->allocate<::mot_set>();
    {
        std::string mot_file;
        mot_file.append("mot_");
        mot_file.append(bake->set_info->name);
        mot_file.append(".bin");

        farc f;
        farc::load_file(&f, path, file, hash);

        farc_file* ff = f.read_file(mot_file.c_str());
        if (!ff) {
            delete bake->alloc;
            bake->alloc = 0;
            bake->mot_set = 0;
            return true;
        }

        bake->mot_set->unpack_file(*bake->alloc, ff->data, ff->size, false);
    }

    if (!bake->mot_set->ready) {
        delete bake->alloc;
        bake->alloc = 0;
        bake->mot_set = 0;
        return true;
    }

    bake->alloc_mutex = new std::mutex;
    bake->thread = new std::thread[bake->threads_count];
    bake->mot_data = new mot_data_bake_data[bake->threads_count];
    for (int32_t i = 0; i < bake->threads_count; i++)
        bake->mot_data[i].bake = bake;
    return true;
}

effchrpv_auth_3d_to_mot::effchrpv_auth_3d_to_mot() : pv_id(), rob_ids() {

}

effchrpv_auth_3d_to_mot::~effchrpv_auth_3d_to_mot() {

}

void effchrpv_auth_3d_to_mot::add_chara_effect_auth_3d(uint32_t hash, int32_t id) {
    auth_3d_mot_ids.insert({ hash, id });
}

void effchrpv_auth_3d_to_mot::add_file_name(const std::string& file, const std::string& category) {
    auth_3d_mot_names.insert({ file, category });
}

void effchrpv_auth_3d_to_mot::disp() {
    /*for (auto& i : auth_3d_rob_mot_ids) {
        rob_chara* rob_chr = rob_chara_array_get(rob_ids[i.first]);
        auth_3d_to_mot_data& a2m = i.second;
        auth_3d* auth = a2m.id.get_auth_3d();
        auth_3d_object_hrc* oh = &auth->object_hrc[0];

        mat4 mat;
        mat = oh->node[a2m.j_mune_wj].model_transform.mat;
        mat4_scale_rot(&mat, 0.1f, &mat);
        debug_put_line_axis(mat);

        mat = oh->node[a2m.j_kao_wj].model_transform.mat;
        mat4_scale_rot(&mat, 0.1f, &mat);
        debug_put_line_axis(mat);
    }*/
}

static void set_bone_key_set_global_data(
    std::map<BONE_BLK, auth_3d_to_mot_keys>& bone_keys,
    std::map<BONE_BLK, auth_3d_to_mot_keys>& second_bone_keys, bool add_keys,
    motion_blend_mot* mot, FcurveKey* fck, const vec3* data, const int32_t count = 1) {
    if (add_keys) {
        auto elem = bone_keys.find(BLK_MAX);
        if (elem == bone_keys.end())
            elem = bone_keys.insert({ BLK_MAX, {} }).first;

        auth_3d_to_mot_keys& keys = elem->second;
        keys.x.push_back(data[0].x);
        keys.y.push_back(data[0].y);
        keys.z.push_back(data[0].z);

        if (count == 2) {
            auto elem = second_bone_keys.find(BLK_MAX);
            if (elem == second_bone_keys.end())
                elem = second_bone_keys.insert({ BLK_MAX, {} }).first;

            auth_3d_to_mot_keys& keys = elem->second;
            keys.x.push_back(data[1].x);
            keys.y.push_back(data[1].y);
            keys.z.push_back(data[1].z);
        }
    }

    fck += mot->bone_data.bone_key_set_count;
    for (int32_t i = 0; i < count; i++) {
        if (fck[0].kind == FCURVE_KEY_KIND_STATIC_DATA && fck[0].val)
            *(float_t*)&fck[0].val[0] = data[i].x;
        if (fck[1].kind == FCURVE_KEY_KIND_STATIC_DATA && fck[1].val)
            *(float_t*)&fck[1].val[0] = data[i].y;
        if (fck[2].kind == FCURVE_KEY_KIND_STATIC_DATA && fck[2].val)
            *(float_t*)&fck[2].val[0] = data[i].z;
        fck += 3;
    }

    mot->bone_data.gblctr_pos = data[0];
    mot->bone_data.gblctr_rot = data[1];

    mot->apply_global_transform();
}

static void set_bone_key_set_data(RobBlock* block_top,
    std::map<BONE_BLK, auth_3d_to_mot_keys>& bone_keys,
    std::map<BONE_BLK, auth_3d_to_mot_keys>& second_bone_keys, bool add_keys,
    BONE_BLK blk, int32_t& curr_block_id, int32_t motion_body_type,
    FcurveKey* fck, const vec3* data, const int32_t count = 1) {
    if (add_keys) {
        auto elem = bone_keys.find(blk);
        if (elem == bone_keys.end())
            elem = bone_keys.insert({ blk, {} }).first;

        auth_3d_to_mot_keys& keys = elem->second;
        keys.x.push_back(data[0].x);
        keys.y.push_back(data[0].y);
        keys.z.push_back(data[0].z);

        if (count == 2) {
            auto elem = second_bone_keys.find(blk);
            if (elem == second_bone_keys.end())
                elem = second_bone_keys.insert({ blk, {} }).first;

            auth_3d_to_mot_keys& keys = elem->second;
            keys.x.push_back(data[1].x);
            keys.y.push_back(data[1].y);
            keys.z.push_back(data[1].z);
        }
    }

    fck += block_top[blk].key_set_offset;
    block_top[blk].frame = -FLT_MAX;
    for (int32_t i = 0; i < count; i++) {
        if (fck[0].kind == FCURVE_KEY_KIND_STATIC_DATA && fck[0].val)
            *(float_t*)&fck[0].val[0] = data[i].x;
        if (fck[1].kind == FCURVE_KEY_KIND_STATIC_DATA && fck[1].val)
            *(float_t*)&fck[1].val[0] = data[i].y;
        if (fck[2].kind == FCURVE_KEY_KIND_STATIC_DATA && fck[2].val)
            *(float_t*)&fck[2].val[0] = data[i].z;
        fck += 3;
    }

    block_top[blk].set_global_leaf_sub(data, BONE_KIND_CMN, true, false);

    while (curr_block_id <= blk)
        block_top[curr_block_id++].get_mat(motion_body_type);
}

static void rotate_euler(const mat4& src, const mat4& dst, const vec3& src_rotation, vec3& dst_rotation) {
    mat4 temp;
    mat4_invert(&src, &temp);
    mat4 off;
    mat4_mul(&dst, &temp, &off);

    mat4 off_inv;
    mat4_invert(&off, &off_inv);

    mat4 rot;
    mat4_rotate_zxy(&src_rotation, &rot);

    mat4_mul(&off, &rot, &temp);
    mat4_mul(&temp, &off_inv, &rot);

    mat4_get_rotation_zxy(&rot, &dst_rotation);
}

static void set_bone_key_set_ik_target_data(auth_3d_to_mot_data& a2m, const mat4& mat,
    const vec3 rotation, RobBlock* block_top, FcurveKey* fck, bool add_keys,
    BONE_BLK blk, int32_t& curr_block_id, int32_t motion_body_type,
    const vec3& position, const mat4& src, const mat4& dst, const bool disable_rot[3], bool offset = true) {
    vec3 data[2];
    if (offset) {
        data[0] = position;
        mat4_transform_point(&mat, &data[0], &data[0]);
        data[1] = rotation;
        rotate_euler(src, dst, data[1], data[1]);

        int32_t _curr_block_id = curr_block_id;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, false,
            blk, _curr_block_id, motion_body_type, fck, data, 2);

        data[0] = position;
        mat4_transform_point(block_top[blk].node[0].mat_ptr, &data[0], &data[0]);
    }
    else
        data[0] = position;

    data[1] = rotation;
    rotate_euler(src, dst, data[1], data[1]);

    for (int32_t i = 0; i < 3; i++)
        if (disable_rot[i])
            (&data[1].x)[i] = 0.0f;

    set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
        blk, curr_block_id, motion_body_type, fck, data, 2);
}

static void set_bone_key_set_arm_ik_target_data(auth_3d_to_mot_data& a2m, auth_3d_object_hrc* oh,
    const mat4& kata_mat, const mat4& ude_mat, const mat4& te_mat,
    const vec3 c_kata_rotation, RobBlock* block_top, FcurveKey* fck, bool add_keys,
    BONE_BLK tl_up_kata_index, BONE_BLK c_kata_index,
    int32_t& curr_block_id, int32_t motion_body_type) {
    vec3 pos_kata;
    vec3 pos_ude;
    vec3 pos_te;
    mat4_get_translation(&kata_mat, &pos_kata);
    mat4_get_translation(&ude_mat, &pos_ude);
    mat4_get_translation(&te_mat, &pos_te);

    RobBlock* c_kata = &block_top[c_kata_index];
    float_t arm_length = c_kata->len[0][0] + c_kata->len[1][0];
    vec3 pos_ude_mid = vec3::lerp(pos_kata, pos_te, c_kata->len[0][0] / arm_length);

    vec3 tl_up_kata_dir = pos_ude - pos_ude_mid;
    float_t pos_middle_dist = vec3::length(tl_up_kata_dir);
    if (pos_middle_dist < 0.00001f) {
        const vec3 offset = { 0.0f, 0.3f, 0.0f };
        mat4_transform_vector(&kata_mat, &offset, &tl_up_kata_dir);
    }
    else
        tl_up_kata_dir *= 0.3f / pos_middle_dist;

    vec3 data[2];
    vec3 tl_up_kata_pos = tl_up_kata_dir + pos_ude;
    mat4* mat = block_top[BLK_KL_MUNE_B_WJ].node[0].mat_ptr;
    mat4_inverse_transform_point(mat, &tl_up_kata_pos, &data[0]);
    set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
        tl_up_kata_index, curr_block_id, motion_body_type, fck, data);

    data[0] = pos_te;
    data[1] = c_kata_rotation;
    set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
        c_kata_index, curr_block_id, motion_body_type, fck, data, 2);
}

void effchrpv_auth_3d_to_mot::get_body_anim(int32_t frame, bool add_keys) {
    static bool disable_cl_mune_rot[3] = { true, false, true };
    static bool disable_cl_kao_rot[3] = { false, false, true };
    static bool disable_cl_momo_rot[3] = { false, false, true };

    for (auto& i : auth_3d_rob_mot_ids) {
        rob_chara* rob_chr = get_rob_management()->get_rob(rob_ids[i.first]);
        auth_3d_to_mot_data& a2m = i.second;
        auth_3d* auth = a2m.id.get_auth_3d();
        auth_3d_object_hrc* oh = &auth->object_hrc[0];

        float_t auth_frame = auth->frame;
        bool auth_frame_changed = auth->frame_changed;
        bool auth_paused = auth->paused;
        auth->frame = (float_t)frame;
        auth->frame_changed = false;
        auth->paused = true;
        auth->ctrl(rctx_ptr);

        rob_chr->set_disp_flag(oh->node[0].model_transform.visible);

        motion_blend_mot* mot = rob_chr->bone_data->motion_loaded.front();
        RobBlock* block_top = mot->bone_data.block_vec.data();
        FcurveKey* fck = mot->mot_key_data.mot.fck_ptr;
        int32_t curr_block_id = 0;
        int32_t motion_body_type = mot->mot_key_data.motion_body_type;

        vec3 data[2];
        data[0] = 0.0f;
        data[1] = oh->node[a2m.gblctr].model_transform.rotation_value;
        set_bone_key_set_global_data(a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            mot, fck, data, 2);

        data[0] = oh->node[a2m.gblctr].model_transform.translation_value;
        data[0] += oh->node[a2m.n_hara].model_transform.translation_value;
        data[1] = oh->node[a2m.n_hara].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_N_HARA_CP, curr_block_id, motion_body_type, fck, data, 2);

        data[0] = oh->node[a2m.n_hara_y].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KG_HARA_Y, curr_block_id, motion_body_type, fck, data);

        data[0] = oh->node[a2m.j_hara_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_HARA_XZ, curr_block_id, motion_body_type, fck, data);

        data[0] = oh->node[a2m.n_kosi].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_N_HARA, curr_block_id, motion_body_type, fck, data);

        /*data[0] = { 0.0f, 0.945f, 0.0f };
        mat4_transform_point(&oh->node[a2m.j_mune_wj].model_transform.mat, &data[0], &data[0]);
        data[1] = 0.0f;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_CL_MUNE, curr_block_id, motion_body_type, fck, data, 2);*/

        set_bone_key_set_ik_target_data(a2m, oh->node[a2m.j_mune_wj].model_transform.mat,
            oh->node[a2m.j_mune_wj].model_transform.rotation_value, block_top, fck, add_keys,
            BLK_CL_MUNE, curr_block_id, motion_body_type,
            { 0.0f, 0.945f, 0.0f }, n_mune_b_src, n_mune_b_dst, disable_cl_mune_rot);

        data[0] = oh->node[a2m.j_mune_b_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_MUNE_B_WJ, curr_block_id, motion_body_type, fck, data);

        data[0] = oh->node[a2m.j_kubi_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_KUBI, curr_block_id, motion_body_type, fck, data);

        data[0] = { (float_t)(M_PI / 2.0), 0.0f, -(float_t)M_PI };
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_N_KAO, curr_block_id, motion_body_type, fck, data);

        /*data[0] = { 0.0f, 0.34f, 0.0f };
        mat4_transform_point(&oh->node[a2m.j_kao_wj].model_transform.mat, &data[0], &data[0]);
        data[1] = oh->node[a2m.j_kao_wj].model_transform.rotation_value;
        //data[1].x = -data[1].x;
        //data[1].z = -data[1].z;
        //data[1] = 0.0f;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_CL_KAO, curr_block_id, motion_body_type, fck, data, 2);*/

        set_bone_key_set_ik_target_data(a2m, oh->node[a2m.j_kao_wj].model_transform.mat,
            oh->node[a2m.j_kao_wj].model_transform.rotation_value, block_top, fck, add_keys,
            BLK_CL_KAO, curr_block_id, motion_body_type,
            { 0.0f, 0.34f, 0.0f }, n_kao_src, n_kao_dst, disable_cl_kao_rot);

        if (a2m.j_eye_r_wj != -1) {
            data[0] = oh->node[a2m.j_eye_r_wj].model_transform.rotation_value;
            data[0].x += (float_t)(M_PI / 2.0);
            data[0].y = -data[0].z;
            data[0].z = 0.0f;
            set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                BLK_KL_EYE_R, curr_block_id, motion_body_type, fck, data);
        }

        if (a2m.j_eye_l_wj != -1) {
            data[0] = oh->node[a2m.j_eye_l_wj].model_transform.rotation_value;
            data[0].x += (float_t)(M_PI / 2.0);
            data[0].y = -data[0].z;
            data[0].z = 0.0f;
            set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
                BLK_KL_EYE_L, curr_block_id, motion_body_type, fck, data);
        }

        data[0] = oh->node[a2m.n_waki_l].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_N_WAKI_L, curr_block_id, motion_body_type, fck, data);

        data[0] = oh->node[a2m.j_waki_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_WAKI_L_WJ, curr_block_id, motion_body_type, fck, data);

        set_bone_key_set_arm_ik_target_data(a2m, oh,
            oh->node[a2m.j_kata_l_wj].model_transform.mat,
            oh->node[a2m.j_ude_l_wj].model_transform.mat,
            oh->node[a2m.j_te_l_wj].model_transform.mat,
            { -(float_t)(M_PI / 2.0), -(float_t)(M_PI / 2.0), -(float_t)(M_PI / 2.0) },
            block_top, fck, add_keys, BLK_TL_UP_KATA_L, BLK_C_KATA_L,
            curr_block_id, motion_body_type);

        data[0] = oh->node[a2m.j_te_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_TE_L_WJ, curr_block_id, motion_body_type, fck, data);

        data[0] = oh->node[a2m.n_waki_r].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_N_WAKI_R, curr_block_id, motion_body_type, fck, data);

        data[0] = oh->node[a2m.j_waki_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_WAKI_R_WJ, curr_block_id, motion_body_type, fck, data);

        set_bone_key_set_arm_ik_target_data(a2m, oh,
            oh->node[a2m.j_kata_r_wj].model_transform.mat,
            oh->node[a2m.j_ude_r_wj].model_transform.mat,
            oh->node[a2m.j_te_r_wj].model_transform.mat,
            { -(float_t)(M_PI / 2.0), (float_t)(M_PI / 2.0), (float_t)(M_PI / 2.0) },
            block_top, fck, add_keys, BLK_TL_UP_KATA_R, BLK_C_KATA_R,
            curr_block_id, motion_body_type);

        data[0] = oh->node[a2m.j_te_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_TE_R_WJ, curr_block_id, motion_body_type, fck, data);

        data[0] = oh->node[a2m.j_kosi_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_KOSI_XZ, curr_block_id, motion_body_type, fck, data);

        /*mat4_get_translation(&oh->node[a2m.j_asi_l_wj].model_transform.mat, &data[0]);
        data[0].y -= 0.033f;
        if (data[0].y < 0.103f)
            data[0].y = 0.103f;
        data[1] = oh->node[a2m.j_momo_l_wj].model_transform.rotation_value;
        data[1].z -= (float_t)(M_PI / 2.0);
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_CL_MOMO_L, curr_block_id, motion_body_type, fck, data, 2);*/

        mat4_get_translation(&oh->node[a2m.j_asi_l_wj].model_transform.mat, &data[0]);
        data[0].y -= 0.033f;
        if (data[0].y < 0.103f)
            data[0].y = 0.103f;
        set_bone_key_set_ik_target_data(a2m, oh->node[a2m.j_momo_l_wj].model_transform.mat,
            oh->node[a2m.j_momo_l_wj].model_transform.rotation_value, block_top, fck, add_keys,
            BLK_CL_MOMO_L, curr_block_id, motion_body_type,
            data[0], n_momo_l_src, cl_momo_l_dst, disable_cl_momo_rot, false);

        /*mat4_get_translation(&oh->node[a2m.j_asi_r_wj].model_transform.mat, &data[0]);
        data[0].y -= 0.033f;
        if (data[0].y < 0.103f)
            data[0].y = 0.103f;
        data[1] = oh->node[a2m.j_momo_r_wj].model_transform.rotation_value;
        data[1].z -= (float_t)(M_PI / 2.0);
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_CL_MOMO_R, curr_block_id, motion_body_type, fck, data, 2);*/

        mat4_get_translation(&oh->node[a2m.j_asi_r_wj].model_transform.mat, &data[0]);
        data[0].y -= 0.033f;
        if (data[0].y < 0.103f)
            data[0].y = 0.103f;
        set_bone_key_set_ik_target_data(a2m, oh->node[a2m.j_momo_r_wj].model_transform.mat,
            oh->node[a2m.j_momo_r_wj].model_transform.rotation_value, block_top, fck, add_keys,
            BLK_CL_MOMO_R, curr_block_id, motion_body_type,
            data[0], n_momo_r_src, cl_momo_r_dst, disable_cl_momo_rot, false);

        data[0] = oh->node[a2m.j_asi_l_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_ASI_L_WJ_CO, curr_block_id, motion_body_type, fck, data);

        data[0] = oh->node[a2m.j_asi_r_wj].model_transform.rotation_value;
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_ASI_R_WJ_CO, curr_block_id, motion_body_type, fck, data);

        data[0] = { 0.0491406508f, 0.0f, 0.0f };
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_KL_AGO_WJ, curr_block_id, motion_body_type, fck, data);

        data[0] = { 0.0f, 0.0331281610f, 0.0f };
        set_bone_key_set_data(block_top, a2m.bone_keys, a2m.sec_bone_keys, add_keys,
            BLK_N_KUBI_WJ_EX, curr_block_id, motion_body_type, fck, data);

        //auth->frame = auth_frame;
        //auth->frame_changed = auth_frame_changed;
        //auth->paused = auth_paused;
    }
}

void effchrpv_auth_3d_to_mot::get_hand_anim(int32_t frame) {
    for (auto& i : auth_3d_rob_mot_ids) {
        rob_chara* rob_chr = get_rob_management()->get_rob(rob_ids[i.first]);
        auth_3d_to_mot_data& a2m = i.second;

        rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;

        BONE_KIND kind = rob_bone_data->kind;
        motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
        prj::sys_vector<RobBlock>* block_vec = &mot->bone_data.block_vec;
        prj::sys_vector<uint16_t>* bone_indices = &mot->bone_data.bone_indices;

        RobBlock* block_top = block_vec->data();
        for (uint16_t& i : *bone_indices) {
            RobBlock* block = &block_top[i];
            BONE_BLK blk = (BONE_BLK)block->block_id;
            if (!(blk >= BLK_N_HITO_L_EX && blk <= BLK_NL_OYA_C_L_WJ
                || blk >= BLK_N_HITO_R_EX && blk <= BLK_NL_OYA_C_R_WJ))
                continue;

            auto elem = a2m.bone_keys.find(blk);
            if (elem == a2m.bone_keys.end())
                elem = a2m.bone_keys.insert({ blk, {} }).first;

            vec3 rotation;
            mat4_get_rotation_zyx(&block->chain_rot[0], &rotation);
            if (elem->second.x.size() == frame)
                elem->second.x.push_back(rotation.x);
            if (elem->second.y.size() == frame)
                elem->second.y.push_back(rotation.y);
            if (elem->second.z.size() == frame)
                elem->second.z.push_back(rotation.z);
        }
    }
}

void effchrpv_auth_3d_to_mot::load(int32_t frame) {
    if (pv_id != 826)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    for (int32_t& i : rob_mot_ids) {
        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", i + 1);
        uint32_t motion_id = aft_mot_db->get_motion_id(buf);
        rob_chara* rob_chr = get_rob_management()->get_rob(rob_ids[i]);
        rob_chr->replace_rob_motion(motion_id, 0.0f,
            0.0f, false, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
        rob_chr->set_motion_reset_data(motion_id, 0.0f);
        rob_chr->set_motion_skin_param(motion_id, 0.0f);

        sprintf_s(buf, sizeof(buf), "A3D_EFFCHRPV%03dMIK%03d", pv_id, i);
        uint32_t hash = hash_utf8_murmurhash(buf);
        for (auto j : auth_3d_mot_names)
            if (j.second.hash_murmurhash == hash) {
                uint32_t hash = hash_string_murmurhash(j.first);
                auth_3d_rob_mot_ids.insert({ i, { auth_3d_mot_ids[hash] } });
                break;
            }
    }

    for (auto& i : auth_3d_rob_mot_ids) {
        auth_3d_to_mot_data& a2m = i.second;
        auth_3d* auth = a2m.id.get_auth_3d();
        a2m.get_bone_indices(&auth->object_hrc[0]);
    }

    for (auto& i : auth_3d_mot_ids) {
        auth_3d_id& id = i.second;
        id.set_repeat(false);
        id.set_camera_root_update(false);
        id.set_enable(true);
        id.set_paused(false);
        id.set_visibility(true);
        id.set_req_frame(0.0f);
    }

    auto round_mat = [](mat4& mat) {
        mat.row0.x = prj::roundf(mat.row0.x);
        mat.row0.y = prj::roundf(mat.row0.y);
        mat.row0.z = prj::roundf(mat.row0.z);
        mat.row0.w = prj::roundf(mat.row0.w);
        mat.row1.x = prj::roundf(mat.row1.x);
        mat.row1.y = prj::roundf(mat.row1.y);
        mat.row1.z = prj::roundf(mat.row1.z);
        mat.row1.w = prj::roundf(mat.row1.w);
        mat.row2.x = prj::roundf(mat.row2.x);
        mat.row2.y = prj::roundf(mat.row2.y);
        mat.row2.z = prj::roundf(mat.row2.z);
        mat.row2.w = prj::roundf(mat.row2.w);
        mat.row3.x = prj::roundf(mat.row3.x);
        mat.row3.y = prj::roundf(mat.row3.y);
        mat.row3.z = prj::roundf(mat.row3.z);
        mat.row3.w = prj::roundf(mat.row3.w);
    };

    constexpr float_t h_pi = (float_t)(M_PI / 2.0);
    constexpr float_t f_pi = (float_t)M_PI;

    mat4 temp;
    n_hara_cp_src = mat4_identity;
    mat4_rotate_zyx(0.0f, h_pi, 0.0f, &temp);
    mat4_mul(&temp, &n_hara_cp_src, &n_hara_src);
    mat4_rotate_zyx(0.0f, -h_pi, 0.0f, &temp);
    mat4_mul(&temp, &n_hara_src, &n_mune_b_src);
    mat4_rotate_zyx(h_pi, 0.0f, 0.0f, &temp);
    mat4_mul(&temp, &n_mune_b_src, &n_kao_src);
    mat4_rotate_zyx(0.0f, 0.0f, -h_pi, &temp);
    mat4_mul(&temp, &n_hara_src, &n_momo_l_src);
    mat4_rotate_zyx(0.0f, 0.0f, -h_pi, &temp);
    mat4_mul(&temp, &n_hara_src, &n_momo_r_src);

    n_hara_cp_dst = mat4_identity;
    mat4_rotate_zyx(0.0f, h_pi, 0.0f, &temp);
    mat4_mul(&temp, &n_hara_cp_dst, &n_hara_dst);
    mat4_rotate_zyx(0.0f, 0.0f, 0.0f, &temp);
    mat4_mul(&temp, &n_hara_dst, &cl_mune_dst);
    mat4 j_mune_wj_dst;
    mat4_rotate_zyx(0.0f, 0.0f, h_pi, &temp);
    mat4_mul(&temp, &cl_mune_dst, &j_mune_wj_dst);
    mat4_rotate_zyx(0.0f, -h_pi, -h_pi, &temp);
    mat4_mul(&temp, &j_mune_wj_dst, &n_mune_b_dst);
    mat4_rotate_zyx(h_pi, 0.0f, -f_pi, &temp);
    mat4_mul(&temp, &n_mune_b_dst, &n_kao_dst);
    mat4_rotate_zyx(0.0f, 0.0f, 0.0f, &temp);
    mat4_mul(&temp, &n_kao_dst, &cl_kao_dst);
    mat4_rotate_zyx(0.0f, 0.0f, 0.0f, &temp);
    mat4_mul(&temp, &n_hara_dst, &cl_momo_l_dst);
    mat4_rotate_zyx(0.0f, 0.0f, 0.0f, &temp);
    mat4_mul(&temp, &n_hara_dst, &cl_momo_r_dst);

    round_mat(n_hara_cp_src);
    round_mat(n_hara_src);
    round_mat(n_mune_b_src);
    round_mat(n_kao_src);
    round_mat(n_momo_l_src);
    round_mat(n_momo_r_src);

    round_mat(n_hara_dst);
    round_mat(cl_mune_dst);
    round_mat(n_mune_b_dst);
    round_mat(n_kao_dst);
    round_mat(cl_kao_dst);
    round_mat(cl_momo_l_dst);
    round_mat(cl_momo_r_dst);
}

void effchrpv_auth_3d_to_mot::modify_play_param(pvpp* play_param) {
    char buf[0x100];
    play_param->chara.resize(6);
    pvpp_chara* chara = play_param->chara.data();
    for (int32_t i = 1; i < 6; i++) {
        switch (i) {
        case 1:
            chara[i].chara_effect.base_chara = PVPP_CHARA_RIN;
            chara[i].chara_effect_init = true;
            break;
        case 2:
            chara[i].chara_effect.base_chara = PVPP_CHARA_LEN;
            chara[i].chara_effect_init = true;
            break;
        case 3:
            chara[i].chara_effect.base_chara = PVPP_CHARA_LUKA;
            chara[i].chara_effect_init = true;
            break;
        case 4:
            chara[i].chara_effect.base_chara = PVPP_CHARA_MEIKO;
            chara[i].chara_effect_init = true;
            break;
        case 5:
            chara[i].chara_effect.base_chara = PVPP_CHARA_KAITO;
            chara[i].chara_effect_init = true;
            break;
        }
        sprintf_s(buf, sizeof(buf), "PV%03d_%s_P%d_00", 826, "OST", i + 1);

        chara[i].motion.push_back(buf);
    }
}

static void fix_rotation(std::vector<float_t>& vec) {
    if (vec.size() < 2)
        return;

    const float_t h_pi = (float_t)(M_PI / 2.0);

    int32_t curr_rot = 0;
    float_t rot_fix = 0.0f;
    float_t rot_prev = vec.data()[0];
    float_t* i_begin = vec.data() + 1;
    float_t* i_end = vec.data() + vec.size();
    for (float_t* i = i_begin; i != i_end; i++) {
        float_t rot = *i;
        if (rot < -h_pi && rot_prev > h_pi && fabsf(rot - rot_prev) > h_pi) {
            curr_rot++;
            rot_fix = (float_t)(M_PI * 2.0 * (double_t)curr_rot);
        }
        else if (rot > h_pi && rot_prev < -h_pi && fabsf(rot - rot_prev) > h_pi) {
            curr_rot--;
            rot_fix = (float_t)(M_PI * 2.0 * (double_t)curr_rot);
        }

        if (curr_rot)
            *i = rot + rot_fix;
        rot_prev = rot;
    }
}

static mot_key_set_type mot_write_motion_fit_keys_into_curve(mot_data_bake* bake, std::vector<float_t>& values_src,
    prj::shared_ptr<prj::stack_allocator> alloc, uint16_t*& frames, float_t*& values, size_t& keys_count) {
    std::vector<uint16_t> _frames;
    std::vector<float_t> _values;
    mot_key_set_type type = mot_set::fit_keys_into_curve(values_src, _frames, _values);
    switch (type) {
    case MOT_KEY_SET_NONE:
        keys_count = 0;
        frames = 0;
        values = 0;
        break;
    case MOT_KEY_SET_STATIC:
        keys_count = 1;
        {
            std::unique_lock<std::mutex> u_lock(*bake->alloc_mutex);
            frames = 0;
            values = (*bake->alloc)->allocate<float_t>();
        }
        memcpy(values, _values.data(), sizeof(float_t));
        break;
    case MOT_KEY_SET_HERMITE:
        keys_count = _frames.size();
        {
            std::unique_lock<std::mutex> u_lock(*bake->alloc_mutex);
            frames = (*bake->alloc)->allocate<uint16_t>(_frames.data(), keys_count);
            values = (*bake->alloc)->allocate<float_t>(keys_count);
        }
        memcpy(frames, _frames.data(), sizeof(uint16_t) * keys_count);
        memcpy(values, _values.data(), sizeof(float_t) * keys_count);
        break;
    case MOT_KEY_SET_HERMITE_TANGENT:
        keys_count = _frames.size();
        {
            std::unique_lock<std::mutex> u_lock(*bake->alloc_mutex);
            frames = (*bake->alloc)->allocate<uint16_t>(keys_count);
            values = (*bake->alloc)->allocate<float_t>(keys_count * 2);
        }
        memcpy(frames, _frames.data(), sizeof(uint16_t) * keys_count);
        memcpy(values, _values.data(), sizeof(float_t) * keys_count * 2);
        break;
    }
    return type;
}

static void mot_write_motion(mot_data_bake_data* bake_data) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    mot_data_bake* bake = bake_data->bake;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV826_OST_P%d_00", bake_data->performer);

    uint32_t motion_id = aft_mot_db->get_motion_id(buf);

    size_t motion_index = -1;
    const motion_set_info* set_info = bake->set_info;
    for (const motion_info& j : set_info->motion)
        if (j.id == motion_id) {
            motion_index = &j - set_info->motion.data();
            break;
        }

    mot_data* mot_data = &bake->mot_set->mot_data[motion_index];

    uint16_t key_set_count = mot_data->key_set_count - 1;
    if (!key_set_count)
        return;

    std::string* bone_name = aft_mot_db->bone_name.data();
    const std::vector<BODYTYPE>* body_type_table = aft_bone_data->get_body_type_table(BONE_KIND_CMN);
    if (!body_type_table)
        return;

    prj::shared_ptr<prj::stack_allocator>& alloc = *bake->alloc;
    auth_3d_to_mot_data& a2m = *bake_data->data;
    const mot_bone_info* bone_info = mot_data->bone_info_array;
    for (size_t key_set_offset = 0, i = 0; key_set_offset < key_set_count; i++) {
        BONE_BLK blk = (BONE_BLK)aft_bone_data->get_block_index(
            BONE_KIND_CMN, bone_name[bone_info[i].index].c_str());
        if (blk == -1) {
            i++;
            blk = (BONE_BLK)aft_bone_data->get_block_index(
                BONE_KIND_CMN, bone_name[bone_info[i].index].c_str());
            if (blk == -1)
                break;
        }

        const BODYTYPE* bt = &(*body_type_table)[blk];

        auto elem = a2m.bone_keys.find(blk);
        if (elem != a2m.bone_keys.end()) {
            auth_3d_to_mot_keys& keys = elem->second;

            if (bt->ik_type == IKT_0) {
                fix_rotation(keys.x);
                fix_rotation(keys.y);
                fix_rotation(keys.z);
            }

            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset];
            key_set_data_x.frames = 0;
            key_set_data_x.values = 0;
            size_t keys_x_count = 0;
            key_set_data_x.type = mot_write_motion_fit_keys_into_curve(bake, keys.x,
                alloc, key_set_data_x.frames, key_set_data_x.values, keys_x_count);
            key_set_data_x.keys_count = (uint16_t)keys_x_count;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 1];
            key_set_data_y.frames = 0;
            key_set_data_y.values = 0;
            size_t keys_y_count = 0;
            key_set_data_y.type = mot_write_motion_fit_keys_into_curve(bake, keys.y,
                alloc, key_set_data_y.frames, key_set_data_y.values, keys_y_count);
            key_set_data_y.keys_count = (uint16_t)keys_y_count;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 2];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            size_t keys_z_count = 0;
            key_set_data_z.type = mot_write_motion_fit_keys_into_curve(bake, keys.z,
                alloc, key_set_data_z.frames, key_set_data_z.values, keys_z_count);
            key_set_data_z.keys_count = (uint16_t)keys_z_count;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }

        elem = a2m.sec_bone_keys.find(blk);
        if (elem != a2m.sec_bone_keys.end()) {
            auth_3d_to_mot_keys& keys = elem->second;

            fix_rotation(keys.x);
            fix_rotation(keys.y);
            fix_rotation(keys.z);

            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset + 3];
            key_set_data_x.frames = 0;
            key_set_data_x.values = 0;
            size_t keys_x_count = 0;
            key_set_data_x.type = mot_write_motion_fit_keys_into_curve(bake, keys.x,
                alloc, key_set_data_x.frames, key_set_data_x.values, keys_x_count);
            key_set_data_x.keys_count = (uint16_t)keys_x_count;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 4];
            key_set_data_y.frames = 0;
            key_set_data_y.values = 0;
            size_t keys_y_count = 0;
            key_set_data_y.type = mot_write_motion_fit_keys_into_curve(bake, keys.y,
                alloc, key_set_data_y.frames, key_set_data_y.values, keys_y_count);
            key_set_data_y.keys_count = (uint16_t)keys_y_count;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 5];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            size_t keys_z_count = 0;
            key_set_data_z.type = mot_write_motion_fit_keys_into_curve(bake, keys.z,
                alloc, key_set_data_z.frames, key_set_data_z.values, keys_z_count);
            key_set_data_z.keys_count = (uint16_t)keys_z_count;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }

        if (blk == BLK_KL_AGO_WJ) {
            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset];
            key_set_data_x.frames = 0;
            {
                std::unique_lock<std::mutex> u_lock(*bake->alloc_mutex);
                key_set_data_x.values = alloc->allocate<float_t>();
                key_set_data_x.values[0] = 0.0491406508f;
            }
            key_set_data_x.type = MOT_KEY_SET_STATIC;
            key_set_data_x.keys_count = 1;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 1];
            key_set_data_y.frames = 0;
            key_set_data_y.values = 0;
            key_set_data_y.type = MOT_KEY_SET_NONE;
            key_set_data_y.keys_count = 0;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 2];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            key_set_data_z.type = MOT_KEY_SET_NONE;
            key_set_data_z.keys_count = 0;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }
        else if (blk == BLK_N_KUBI_WJ_EX) {
            mot_key_set_data& key_set_data_x = mot_data->key_set_array[key_set_offset];
            key_set_data_x.frames = 0;
            key_set_data_x.values = 0;
            key_set_data_x.type = MOT_KEY_SET_NONE;
            key_set_data_x.keys_count = 1;
            key_set_data_x.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_y = mot_data->key_set_array[key_set_offset + 1];
            key_set_data_y.frames = 0;
            {
                std::unique_lock<std::mutex> u_lock(*bake->alloc_mutex);
                key_set_data_y.values = alloc->allocate<float_t>();
                key_set_data_y.values[0] = 0.0331281610f;
            }
            key_set_data_y.type = MOT_KEY_SET_STATIC;
            key_set_data_y.keys_count = 0;
            key_set_data_y.data_type = MOT_KEY_SET_DATA_F32;

            mot_key_set_data& key_set_data_z = mot_data->key_set_array[key_set_offset + 2];
            key_set_data_z.frames = 0;
            key_set_data_z.values = 0;
            key_set_data_z.type = MOT_KEY_SET_NONE;
            key_set_data_z.keys_count = 0;
            key_set_data_z.data_type = MOT_KEY_SET_DATA_F32;
        }

        key_set_offset += bt->ik_type >= IKT_ROOT ? 6 : 3;
    }

    bake_data->state = 0;
}

void effchrpv_auth_3d_to_mot::post_modify_play_param() {
    if (pv_id != 826)
        return;

    for (int32_t i = 1; i < 6; i++)
        rob_mot_ids.push_back(i);
}

void effchrpv_auth_3d_to_mot::write_mot_set() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "PV%03d", pv_id);
    const motion_set_info* set_info = aft_mot_db->get_motion_set_by_name(buf);
    if (!set_info)
        return;

    std::string farc_file = "mot_" + set_info->name + ".farc";
    if (!aft_data->load_file(&bake, "rom/rob/", farc_file.c_str(), mot_data_bake::load_file))
        return;

    waitable_timer timer;
    while (true) {
        int32_t free_thread_count = 0;
        for (int32_t i = 0; i < bake.threads_count; i++)
            if (!bake.mot_data[i].state)
                free_thread_count++;

        if (free_thread_count && bake.performer < 6)
            for (auto& i : auth_3d_rob_mot_ids) {
                if (!free_thread_count)
                    break;
                else if (bake.performer != i.first)
                    continue;

                std::thread* thread = 0;
                int32_t thread_index = -1;
                for (int32_t j = 0; j < bake.threads_count; j++)
                    if (!bake.mot_data[j].state) {
                        thread = &bake.thread[j];
                        thread_index = j;
                        break;
                    }

                if (!thread)
                    break;

                mot_data_bake_data* bake_data = &bake.mot_data[thread_index];
                bake_data->performer = bake.performer + 1;
                bake_data->data = &i.second;
                bake_data->state = 1;

                if (thread->joinable())
                    thread->join();

                *thread = std::thread(mot_write_motion, bake_data);

                wchar_t buf[0x80];
                swprintf_s(buf, sizeof(buf) / sizeof(wchar_t),
                    L"X PV GAME BAKE PV%03d P%d", bake.pv_id, bake.performer);
                SetThreadDescription((HANDLE)thread->native_handle(), buf);

                bake.performer++;
                free_thread_count--;
            }

        if (free_thread_count == 2)
            break;

        timer.sleep_float(1.0);
    }

    for (int32_t i = 0; i < bake.threads_count; i++)
        if (bake.thread[i].joinable())
            bake.thread[i].join();

    bake.write();
}
