/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob.hpp"
#include "data.hpp"
#include "draw_task.h"
#include "../KKdLib/key_val.hpp"
#include "../KKdLib/str_utils.h"
#include "random.hpp"
#include "stage.hpp"

#define OSG_SET_TEXT_BUF_SIZE 0x400
#define SKP_TEXT_BUF_SIZE 0x400

struct MotFile {
    motion_set_info* mot_set_info;
    mot_set* mot_set;
    p_file_handler file_handler;
    int32_t load_count;

    MotFile();
    virtual ~MotFile();

    bool CheckNotReady();
    void FreeData();
    void LoadFile(std::string* mdata_dir, uint32_t set);
    void ParseFile(void* data, size_t size);
    bool Unload();

    static void ParseFileParent(void* data, void* file_data, size_t size);
};

struct MhdFile {
    mothead* data;
    uint32_t set;
    std::string file_path;
    p_file_handler file_handler;
    int32_t load_count;

    MhdFile();
    virtual ~MhdFile();

    bool CheckNotReady();
    void FreeData();
    void LoadFile(const char* path, const char* file, uint32_t set);
    void ParseFile(void* data);
    mothead* ParseMothead(mothead_file* mhdsf, size_t data);
    mothead_mot* ParseMotheadMot(mothead_mot_file* mhdsf, size_t data);
    bool Unload();

    static void ParseFileParent(void* data, void* file_data, size_t size);
};

struct osage_setting {
    std::map<std::pair<std::string, std::string>, osage_setting_osg_cat> cat;
    std::map<object_info, std::string> obj;

    osage_setting();
    ~osage_setting();

    void parse(key_val* kv);
    rob_osage_parts parse_parts_string(std::string* s);
};

struct struc_380 {
    rob_chara* rob_chara;
    rob_chara_data* rob_chara_data;
    struc_223* field_10;
};

struct exp_func_op1 {
    const char* name;
    float_t(*func)(float_t v1);
};

struct exp_func_op2 {
    const char* name;
    float_t(*func)(float_t v1, float_t v2);
};

struct exp_func_op3 {
    const char* name;
    float_t(*func)(float_t v1, float_t v2, float_t v3);
};

struct mothead_func_data {
    rob_chara* rob_chr;
    rob_chara_data* rob_chr_data;
    rob_chara_data* field_10;
    rob_chara* field_18;
    rob_chara_data* field_20;
    struc_223* field_28;
};

typedef void(*mothead_func)(mothead_func_data*, void*, const mothead_data*, int32_t, motion_database*);

struct mothead_func_struct {
    mothead_func func;
    int32_t flags;
};

struct rob_cmn_mottbl_sub_header {
    int32_t data_offset;
    int32_t field_4;
};

struct rob_cmn_mottbl_header {
    int32_t chara_count;
    int32_t mottbl_indices_count;
    int32_t subheaders_offset;
};

class ReqData {
public:
    chara_index chara_index;
    int32_t count;

    ReqData();
    ReqData(::chara_index chara_index, int32_t count);
    virtual ~ReqData();
    virtual void Reset();
};

class ReqDataObj : public ReqData {
public:
    item_cos_data cos;

    ReqDataObj();
    ReqDataObj(::chara_index chara_index, int32_t count);
    virtual ~ReqDataObj() override;
    virtual void Reset() override;
};

struct osage_set_motion {
    int32_t motion_id;
    std::vector<std::pair<float_t, int32_t>> frames;

    osage_set_motion();
    ~osage_set_motion();
};

struct pv_data_set_motion {
    int32_t motion_id;
    std::pair<float_t, int32_t> frame_stage_index;

    pv_data_set_motion();
    ~pv_data_set_motion();
};

class PvOsageManager : public Task {
public:
    int32_t state;
    int32_t chara_id;
    bool reset;
    int32_t field_74;
    uint32_t motion_index;
    std::vector<pv_data_set_motion> pv_set_motion;
    std::vector<osage_set_motion> osage_set_motion;
    void* pv;//pv_db_pv* pv;
    std::map<float_t, bool> reset_frames_list;
    std::thread* thread;
    std::condition_variable cnd;
    bool disp;
    bool not_reset;
    bool exit;
    int32_t field_D4;
    std::mutex mtx;
    std::mutex disp_mtx;
    std::mutex not_reset_mtx;
    std::mutex exit_mtx;

    PvOsageManager();
    virtual ~PvOsageManager() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    bool GetDisp();
    void Reset();
    void SetNotResetTrue();

    bool sub_1404F7AF0();
    void sub_1404F7BD0(bool not_reset);
    void sub_1404F82F0();

    static void ThreadMain(PvOsageManager* pv_osg_mgr);
};

struct RobThread {
    rob_chara* data;
    void(*func)(rob_chara*);
};

struct RobThreadParent {
    bool exit;
    std::thread* thread;
    std::mutex mtx;
    std::mutex threads_mtx;
    std::condition_variable cnd;
    std::mutex field_28;
    std::condition_variable field_30;
    std::list<RobThread> threads;

    RobThreadParent();
    ~RobThreadParent();

    void AppendRobCharaFunc(rob_chara* rob_chr, void(*rob_chr_func)(rob_chara*));
    void AppendRobThread(RobThread* thread);
    bool CheckThreadsNotNull();
    void sub_14054E0D0();
    void sub_14054E370();

    static void ThreadMain(RobThreadParent* rob_thrd_parent);
};

struct RobThreadHandler {
    RobThreadParent* arr[ROB_CHARA_COUNT];

    RobThreadHandler();
    ~RobThreadHandler();

    void AppendRobCharaFunc(int32_t chara_id, rob_chara* rob_chr, void(*rob_chr_func)(rob_chara*));
    void sub_14054E3F0();
};

struct struc_461 {
    p_file_handler* file_handler;
    item_id id;
    int32_t field_C;
    std::vector<struc_571>* field_10;
    rob_chara* rob_chr;
};

struct struc_462 {
    rob_chara* rob_chr;
    int32_t pv_id;
    int32_t motion_id;
    std::string name;
    int32_t frame;

    struc_462();
    ~struc_462();
};

struct SkinParamManager : public Task {
    int32_t state;
    uint8_t index;
    std::vector<struc_462> field_70;
    std::map<std::pair<object_info, std::pair<int32_t, int32_t>>, std::vector<struc_571>*> field_88;
    std::map<std::pair<object_info, std::pair<int32_t, int32_t>>, std::vector<struc_571>*> field_98;
    std::map<std::pair<object_info, std::pair<int32_t, int32_t>>, std::vector<struc_571>*> field_A8;
    std::list<struc_461*> field_B8;

    SkinParamManager();
    virtual ~SkinParamManager() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    void Reset();

    void sub_14060FBD0();
    bool sub_1406112F0();
};

class TaskRobLoad : public Task {
public:
    bool field_68;
    int32_t field_6C;
    std::list<ReqDataObj> load_req_data_obj;
    std::list<ReqDataObj> free_req_data_obj;
    std::list<ReqDataObj> load_item_req_data_obj;
    std::list<ReqDataObj> loaded_req_data_obj;
    std::list<ReqData> load_req_data;
    std::list<ReqData> free_req_data;
    std::list<ReqData> load_item_req_data;
    std::list<ReqData> loaded_req_data;
    int32_t field_F0;

    TaskRobLoad();
    virtual ~TaskRobLoad() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;

    bool AppendFreeReqData(chara_index chara_index);
    bool AppendFreeReqDataObj(chara_index chara_index, item_cos_data* cos);
    bool AppendLoadReqData(chara_index chara_index);
    bool AppendLoadReqDataObj(chara_index chara_index, item_cos_data* cos);
    void AppendLoadedReqData(ReqData* req_data);
    void AppendLoadedReqDataObj(ReqDataObj* req_data_obj);
    int32_t CtrlFunc1();
    int32_t CtrlFunc2();
    int32_t CtrlFunc3();
    int32_t CtrlFunc4();
    void FreeLoadedReqData(ReqData* req_data);
    void FreeLoadedReqDataObj(ReqDataObj* req_data_obj);
    void LoadCharaItem(chara_index chara_index,
        int32_t item_no, void* data, object_database* obj_db);
    bool LoadCharaItemCheckNotRead(chara_index chara_index, int32_t item_no);
    void LoadCharaItems(chara_index chara_index,
        item_cos_data* cos, void* data, object_database* obj_db);
    bool LoadCharaItemsCheckNotRead(chara_index chara_index, item_cos_data* cos);
    bool LoadCharaItemsCheckNotReadParent(chara_index chara_index, item_cos_data* cos);
    void LoadCharaItemsParent(chara_index chara_index,
        item_cos_data* cos, void* data, object_database* obj_db);
    void LoadCharaObjSetMotionSet(chara_index chara_index,
        void* data, object_database* obj_db, motion_database* mot_db);
    bool LoadCharaObjSetMotionSetCheck(chara_index chara_index);
    void ResetReqData();
    void ResetReqDataObj();
    void UnloadCharaItem(chara_index chara_index, int32_t item_no);
    void UnloadCharaItems(chara_index chara_index, item_cos_data* cos);
    void UnloadCharaItemsParent(chara_index chara_index, item_cos_data* cos);
    void UnloadCharaObjSetMotionSet(chara_index chara_index);
    void UnloadLoadedChara();
};

class RobImplTask : public Task {
public:
    std::list<rob_chara*> init_chara;
    std::list<rob_chara*> ctrl_chara;
    std::list<rob_chara*> free_chara;

    RobImplTask();
    virtual ~RobImplTask() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual bool CheckType(rob_chara_type type) = 0;
    virtual bool IsFrameDependent() = 0;

    void AppendList(rob_chara* rob_chr, std::list<rob_chara*>* list);
    void AppendCtrlCharaList(rob_chara* rob_chr);
    void AppendFreeCharaList(rob_chara* rob_chr);
    void AppendInitCharaList(rob_chara* rob_chr);
    void FreeList(int8_t* chara_id, std::list<rob_chara*>* list);
    void FreeCharaLists();
    void FreeCtrlCharaList(int8_t* chara_id);
    void FreeFreeCharaList(int8_t* chara_id);
    void FreeInitCharaList(int8_t* chara_id);
};

class TaskRobPrepareControl : public RobImplTask {
public:
    TaskRobPrepareControl();
    virtual ~TaskRobPrepareControl() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual bool CheckType(rob_chara_type type) override;
    virtual bool IsFrameDependent() override;
};

class TaskRobPrepareAction : public RobImplTask {
public:
    TaskRobPrepareAction();
    virtual ~TaskRobPrepareAction() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual bool CheckType(rob_chara_type type) override;
    virtual bool IsFrameDependent() override;
};

class TaskRobBase : public RobImplTask {
public:
    TaskRobBase();
    virtual ~TaskRobBase() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual bool CheckType(rob_chara_type type) override;
    virtual bool IsFrameDependent() override;
};

class TaskRobCollision : public RobImplTask {
public:
    TaskRobCollision();
    virtual ~TaskRobCollision() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual bool CheckType(rob_chara_type type) override;
    virtual bool IsFrameDependent() override;
};

class TaskRobInfo : public RobImplTask {
public:
    TaskRobInfo();
    virtual ~TaskRobInfo() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual bool CheckType(rob_chara_type type) override;
    virtual bool IsFrameDependent() override;
};

class TaskRobMotionModifier : public RobImplTask {
public:
    TaskRobMotionModifier();
    virtual ~TaskRobMotionModifier() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual bool CheckType(rob_chara_type type) override;
    virtual bool IsFrameDependent() override;
};

class TaskRobDisp : public RobImplTask {
public:
    TaskRobDisp();
    virtual ~TaskRobDisp() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;
    virtual bool CheckType(rob_chara_type type) override;
    virtual bool IsFrameDependent() override;
};

class TaskRobManager : public Task {
public:
    int32_t ctrl_state;
    int32_t dest_state;
    TaskRobPrepareControl prepare_control;
    TaskRobPrepareAction prepare_action;
    TaskRobBase base;
    TaskRobCollision collision;
    TaskRobInfo info;
    TaskRobMotionModifier motion_modifier;
    TaskRobDisp disp;
    std::list<rob_chara*> init_chara;
    std::list<rob_chara*> load_chara;
    std::list<rob_chara*> free_chara;
    std::list<rob_chara*> loaded_chara;

    TaskRobManager();
    virtual ~TaskRobManager() override;
    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    void AppendFreeCharaList(rob_chara* rob_chr);
    void AppendInitCharaList(rob_chara* rob_chr);
    void AppendLoadedCharaList(rob_chara* rob_chr);
    bool CheckCharaLoaded(rob_chara* rob_chr);
    bool CheckHasRobCharaLoad(rob_chara* rob_chr);
    void CheckTypeAppendInitCharaLists(std::list<rob_chara*>* rob_chr_list);
    void FreeLoadedCharaList(int8_t* chara_id);
};

struct rob_manager_rob_impl {
    RobImplTask* task;
    const char* name;
};

static bone_data* bone_data_init(bone_data* bone);
static float_t bone_data_limit_angle(float_t angle);
static void bone_data_mult_0(bone_data* a1, int32_t skeleton_select);
static void bone_data_mult_1(bone_data* a1, mat4* parent_mat, bone_data* a3, bool solve_ik);
static bool bone_data_mult_1_ik(bone_data* a1, bone_data* a2);
static void bone_data_mult_1_ik_hands(bone_data* a1, vec3* a2);
static void bone_data_mult_1_ik_hands_2(bone_data* a1, vec3* a2, float_t a3);
static void bone_data_mult_1_ik_legs(bone_data* a1, vec3* a2);
static bool bone_data_mult_1_exp_data(bone_data* a1, bone_node_expression_data* a2, bone_data* a3);
static void bone_data_mult_ik(bone_data* a1, int32_t skeleton_select);

static float_t* bone_node_get_exp_data_component(bone_node* a1,
    size_t index, ex_expression_block_stack_type* type);

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_bone_data* rob_bone_data, bone_database* bone_data);
static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    std::vector<bone_database_bone>* bones, vec3* common_translation, vec3* translation);
static void bone_data_parent_load_rob_chara(bone_data_parent* bone);

static void bone_node_expression_data_mat_set(bone_node_expression_data* data,
    vec3* parent_scale, mat4* ex_data_mat, mat4* mat);
static void bone_node_expression_data_reset_scale(bone_node_expression_data* data);
static void bone_node_expression_data_set_position_rotation(bone_node_expression_data* data,
    float_t position_x, float_t position_y, float_t position_z,
    float_t rotation_x, float_t rotation_y, float_t rotation_z);
static void bone_node_expression_data_set_position_rotation_vec3(bone_node_expression_data* data,
    vec3* position, vec3* rotation);

static bool check_module_index_is_501(int32_t module_index);

static float_t ex_expression_block_stack_get_value(ex_expression_block_stack* stack);

static float_t exp_abs(float_t v1);
static float_t exp_acos(float_t v1);
static float_t exp_add(float_t v1, float_t v2);
static float_t exp_and(float_t v1, float_t v2);
static float_t exp_asin(float_t v1);
static float_t exp_atan(float_t v1);
static float_t exp_av(float_t v1);
static float_t exp_ceil(float_t v1);
static float_t exp_cond(float_t v1, float_t v2, float_t v3);
static float_t exp_cos(float_t v1);
static float_t exp_div(float_t v1, float_t v2);
static float_t exp_eq(float_t v1, float_t v2);
static float_t exp_exp(float_t v1);
static float_t exp_floor(float_t v1);
static float_t exp_fmod(float_t v1, float_t v2);
static const exp_func_op1* exp_func_op1_find_func(string* name, const exp_func_op1* array);
static const exp_func_op2* exp_func_op2_find_func(string* name, const exp_func_op2* array);
static const exp_func_op3* exp_func_op3_find_func(string* name, const exp_func_op3* array);
static float_t exp_ge(float_t v1, float_t v2);
static float_t exp_gt(float_t v1, float_t v2);
static float_t exp_le(float_t v1, float_t v2);
static float_t exp_log(float_t v1);
static float_t exp_lt(float_t v1, float_t v2);
static float_t exp_max(float_t v1, float_t v2);
static float_t exp_min(float_t v1, float_t v2);
static float_t exp_mul(float_t v1, float_t v2);
static float_t exp_ne(float_t v1, float_t v2);
static float_t exp_neg(float_t v1);
static float_t exp_or(float_t v1, float_t v2);
static float_t exp_pow(float_t v1, float_t v2);
static float_t exp_rand(float_t v1, float_t v2, float_t v3);
static float_t exp_rand_0_1(float_t v1);
static float_t exp_round(float_t v1);
static float_t exp_sin(float_t v1);
static float_t exp_sqrt(float_t v1);
static float_t exp_sub(float_t v1, float_t v2);
static float_t exp_tan(float_t v1);

static const float_t get_osage_gravity_const();

static void mot_blend_interpolate(mot_blend* a1, std::vector<bone_data>* bones,
    std::vector<uint16_t>* bone_indices, bone_database_skeleton_type skeleton_type);
static void mot_blend_load_motion(mot_blend* a1, int32_t motion_id, motion_database* mot_db);
static void mot_blend_set_duration(mot_blend* a1, float_t duration, float_t step, float_t offset);
static void mot_blend_set_frame(mot_blend* a1, float_t frame);

static void mot_key_frame_interpolate(mot* a1, float_t frame, float_t* value,
    mot_key_set* a4, uint32_t key_set_count, struc_369* a6);
static uint32_t mot_load_last_key_calc(uint16_t keys_count);
static bool mot_data_load_file(mot* a1, const mot_data* a2);

static void mot_key_data_get_key_set_count_by_bone_database_bones(mot_key_data* a1,
    std::vector<bone_database_bone>* a2);
static void mot_key_data_get_key_set_count(mot_key_data* a1, size_t motion_bone_count, size_t ik_bone_count);
static void mot_key_data_init_key_sets(mot_key_data* a1, bone_database_skeleton_type type,
    size_t motion_bone_count, size_t ik_bone_count);
static void mot_key_data_interpolate(mot_key_data* a1, float_t frame,
    uint32_t key_set_offset, uint32_t key_set_count);
static const mot_data* mot_key_data_load_file(mot_key_data* a1, int32_t motion_id, motion_database* mot_db);
static void mot_key_data_reserve_key_sets(mot_key_data* a1);

static void mot_play_frame_data_set_frame(mot_play_frame_data* a1, float_t frame);

static void mothead_func_0(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_1(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_2(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_3(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_4(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_5(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_6(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_7(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_8(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_9(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_10(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_11(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_12(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_13(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_14(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_15(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_16(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_17(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_18(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_19(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_20(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_21(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_22(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_23(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_24(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_25(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_26(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_27(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_28(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_29(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_30(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_31(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_32(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_33(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_34(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_35(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_36(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_37(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_38(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_39(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_40(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_41(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_42(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_43(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_44(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_45(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_46(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_47(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_48(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_49(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_50_set_face_motion_id(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_51(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_52(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_53_set_face_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_54_set_hand_r_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_55_set_hand_l_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_56_set_mouth_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_57_set_eyes_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_58_set_eyelid_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_59_set_rob_chara_head_object(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_60(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_61_set_eyelid_motion_from_face(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_62_rob_parts_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_63(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_64_osage_reset(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_65_motion_skin_param(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_66_osage_step(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_67(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_68(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_69(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_70(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_71_osage_move_cancel(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_72(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_73_rob_hand_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_74(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_75_rob_adjust_global(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_76_rob_arm_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_77_disable_eye_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_78(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_79_rob_chara_coli_ring(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);
static void mothead_func_80_adjust_get_global_trans(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db);

static void motion_blend_mot_interpolate(motion_blend_mot* a1);
static void motion_blend_mot_load_bone_data(motion_blend_mot* a1,
    rob_chara_bone_data* a2, bool(*bone_check_func)(motion_bone_index), bone_database* bone_data);
static bool motion_blend_mot_interpolate_get_reverse(int32_t* a1);
static void motion_blend_mot_load_file(motion_blend_mot* a1, int32_t motion_id,
    MotionBlendType blend_type, float_t blend, bone_database* bone_data, motion_database* mot_db);
static void motion_blend_mot_mult_mat(motion_blend_mot* a1, mat4* mat);
static void motion_blend_mot_set_blend(motion_blend_mot* a1,
    MotionBlendType blend_type, float_t blend);
static void motion_blend_mot_set_duration(motion_blend_mot* mot,
    float_t duration, float_t step, float_t offset);

static void osage_coli_set(osage_coli* osg_coli,
    skin_param_osage_root_coli* skp_root_coli, mat4* mats);
static void osage_coli_ring_set(osage_coli* osg_coli,
    std::vector<skin_param_osage_root_coli>* vec_skp_root_coli, mat4* mats);

static void osage_play_data_manager_get_opd_file_data(object_info obj_info,
    int32_t motion_id, float_t** data, uint32_t* count);

static const osage_setting_osg_cat* osage_setting_data_get_cat_value(
    object_info* obj_info, const char* root_node);
static bool osage_setting_data_obj_has_key(object_info key);

static bool pv_osage_manager_array_get_disp(int32_t* chara_id);
static PvOsageManager* pv_osage_manager_array_ptr_get(int32_t chara_id);
static void pv_osage_manager_array_ptr_set_not_reset_true();

static void rob_base_rob_chara_init(rob_chara* rob_chr);
static void rob_base_rob_chara_ctrl(rob_chara* rob_chr);
static void rob_base_rob_chara_ctrl_thread_main(rob_chara* rob_chr);
static void rob_base_rob_chara_free(rob_chara* rob_chr);

static void rob_disp_rob_chara_init(rob_chara* rob_chr,
    bone_database* bone_data, void* data, object_database* obj_db);
static void rob_disp_rob_chara_ctrl(rob_chara* rob_chr);
static void rob_disp_rob_chara_ctrl_thread_main(rob_chara* rob_chr);
static void rob_disp_rob_chara_disp(rob_chara* rob_chr);
static void rob_disp_rob_chara_free(rob_chara* rob_chr);

static void rob_motion_modifier_rob_chara_ctrl(rob_chara* rob_chr);

static float_t rob_chara_array_get_data_adjust_scale(int32_t chara_id);
static rob_chara_item_equip* rob_chara_array_get_item_equip(int32_t chara_id);
static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    std::vector<bone_database_bone>* bones);
static void rob_chara_bone_data_eyes_xrot_adjust(rob_chara_bone_data* rob_bone_data,
    const struc_241* a2, eyes_adjust* a3);
static float_t rob_chara_bone_data_get_frame(rob_chara_bone_data* rob_bone_data);
static float_t rob_chara_bone_data_get_frame_count(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_get_ik_scale(
    rob_chara_bone_data* rob_bone_data, bone_database* bone_data);
static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index);
static bone_node* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index);
static void rob_chara_bone_data_ik_scale_calculate(
    rob_chara_bone_data_ik_scale* ik_scale, std::vector<bone_data>* bones,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data);
static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data);
static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data);
static void rob_chara_bone_data_interpolate(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_load_eyelid_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db);
static void rob_chara_bone_data_load_eyes_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db);
static void rob_chara_bone_data_load_face_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db);
static void rob_chara_bone_data_load_hand_l_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db);
static void rob_chara_bone_data_load_hand_r_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db);
static void rob_chara_bone_data_load_mouth_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db);
static void rob_chara_bone_data_motion_blend_mot_free(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_list_free(rob_chara_bone_data* rob_bone_data,
    size_t last_index);
static void rob_chara_bone_data_motion_blend_mot_list_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, MotionBlendType blend_type, bone_database* bone_data, motion_database* mot_db);
static void rob_chara_bone_data_reserve(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_set_eyelid_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_eyelid_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_eyelid_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_eyes_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_eyes_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_eyes_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_face_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_face_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_face_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_hand_l_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_hand_l_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_hand_l_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_hand_r_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_hand_r_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_hand_r_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
    std::vector<bone_database_bone>* bones, std::string* motion_bones);
static void rob_chara_bone_data_set_motion_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_mouth_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_mouth_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_mouth_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    uint16_t* parent_indices);
static void rob_chara_bone_data_set_rotation_y(rob_chara_bone_data* rob_bone_data, float_t rotation_y);
static void rob_chara_bone_data_update(rob_chara_bone_data* rob_bone_data, mat4* mat);

static bool rob_chara_item_cos_data_check_for_npr_flag(rob_chara_item_cos_data* itm_cos_data);
static object_info rob_chara_item_cos_data_get_head_object_replace(
    rob_chara_item_cos_data* item_cos_data, int32_t head_object_id);
static void rob_chara_item_cos_data_reload_items(rob_chara_item_cos_data* itm_cos_data,
    int32_t chara_id, bone_database* bone_data, void* data, object_database* obj_db);
static void rob_chara_item_cos_data_set_chara_index(
    rob_chara_item_cos_data* item_cos_data, chara_index chara_index);
static void rob_chara_item_cos_data_set_chara_index_item_nos(
    rob_chara_item_cos_data* item_cos_data, chara_index chara_index, int32_t* items);
static void rob_chara_item_cos_data_set_customize_item(rob_chara_item_cos_data* item_cos_data, int32_t item_no);
static void rob_chara_item_cos_data_set_customize_items(rob_chara_item_cos_data* item_cos_data,
    rob_chara_pv_data_customize_items* customize_items);
static void rob_chara_item_cos_data_set_item(
    rob_chara_item_cos_data* item_cos_data, item_sub_id sub_id, int32_t item_no);
static void rob_chara_item_cos_data_set_item_nos(
    rob_chara_item_cos_data* item_cos_data, int32_t* item_nos);
static void rob_chara_item_cos_data_texture_change_clear(rob_chara_item_cos_data* item_cos_data);
static void rob_chara_item_cos_data_texture_pattern_clear(rob_chara_item_cos_data* item_cos_data);

static void rob_chara_item_equip_add_motion_reset_data(rob_chara_item_equip* rob_item_equip,
    int32_t motion_id, float_t frame, int32_t iterations);
static void rob_chara_item_equip_disp(
    rob_chara_item_equip* rob_itm_equip, int32_t chara_id, render_context* rctx);
static object_info rob_chara_item_equip_get_object_info(
    rob_chara_item_equip* rob_itm_equip, item_id id);
static void rob_chara_item_equip_get_parent_bone_nodes(
    rob_chara_item_equip* rob_itm_equip, bone_node* bone_nodes, bone_database* bone_data);
static void rob_chara_item_equip_load_object_info(
    rob_chara_item_equip* rob_itm_equip, object_info object_info, item_id id,
    bool osage_reset, bone_database* bone_data, void* data, object_database* obj_db);
static void rob_chara_item_equip_object_add_motion_reset_data(rob_chara_item_equip_object* itm_eq_obj,
    int32_t motion_id, float_t frame, int32_t osage_iterations);
static void rob_chara_item_equip_object_check_no_opd(
    rob_chara_item_equip_object* itm_eq_obj, std::vector<opd_blend_data>* opd_blend_data);
static void rob_chara_item_equip_object_clear_ex_data(rob_chara_item_equip_object* itm_eq_obj);
static void rob_chara_item_equip_object_disp(
    rob_chara_item_equip_object* itm_eq_obj, mat4* mat, render_context* rctx);
static int32_t rob_chara_item_equip_object_get_bone_index(
    rob_chara_item_equip_object* itm_eq_obj, const char* name, bone_database* bone_data);
static bone_node* rob_chara_item_equip_object_get_bone_node(
    rob_chara_item_equip_object* itm_eq_obj, int32_t bone_index);
static bone_node* rob_chara_item_equip_object_get_bone_node_by_name(
    rob_chara_item_equip_object* itm_eq_obj, const char* name, bone_database* bone_data);
static void rob_chara_item_equip_object_get_parent_bone_nodes(
    rob_chara_item_equip_object* itm_eq_obj, bone_node* bone_nodes, bone_database* bone_data);
static void rob_chara_item_equip_object_init_ex_data_bone_nodes(
    rob_chara_item_equip_object* itm_eq_obj, obj_skin_ex_data* ex_data);
static void rob_chara_item_equip_object_init_members(rob_chara_item_equip_object* itm_eq_obj, size_t index = 0xDEADBEEF);
static void rob_chara_item_equip_object_load_ex_data(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_ex_data* ex_data, bone_database* bone_data, void* data, object_database* obj_db);
static void rob_chara_item_equip_object_load_object_info_ex_data(
    rob_chara_item_equip_object* itm_eq_obj, object_info object_info, bone_node* bone_nodes,
    bool osage_reset, bone_database* bone_data, void* data, object_database* obj_db);
static bool rob_chara_item_equip_object_set_boc(rob_chara_item_equip_object* itm_eq_obj,
    skin_param_osage_root* skp_root, ExOsageBlock* osg);
static void rob_chara_item_equip_object_set_collision_target_osage(
    rob_chara_item_equip_object* itm_eq_obj, skin_param_osage_root* skp_root, skin_param* skp);
static void rob_chara_item_equip_object_set_motion_reset_data(
    rob_chara_item_equip_object* itm_eq_obj, int32_t motion_id, float_t frame);
static void rob_chara_item_equip_object_set_motion_skin_param(
    rob_chara_item_equip_object* itm_eq_obj, int8_t chara_id, int32_t motion_id, int32_t frame);
static void rob_chara_item_equip_object_set_null_blocks_expression_data(
    rob_chara_item_equip_object* rob_itm_equip, vec3* position, vec3* rotation, vec3* scale);
static void rob_chara_item_equip_object_set_osage_reset(
    rob_chara_item_equip_object* itm_eq_obj);
static void rob_chara_item_equip_object_set_osage_move_cancel(
    rob_chara_item_equip_object* itm_eq_obj, float_t value);
static void rob_chara_item_equip_object_set_texture_pattern(
    rob_chara_item_equip_object* itm_eq_obj, texture_pattern_struct* tex_pat, size_t count);
static void rob_chara_item_equip_object_skp_load(
    rob_chara_item_equip_object* itm_eq_obj, key_val* kv, bone_database* bone_data);
static void rob_chara_item_equip_object_skp_load_file(
    rob_chara_item_equip_object* itm_eq_obj, void* data, bone_database* bone_data, object_database* obj_db);
static void rob_chara_item_equip_reset_init_data(rob_chara_item_equip* rob_itm_equip,
    bone_node* bone_nodes);
static void rob_chara_item_equip_set_item_equip_range(rob_chara_item_equip* rob_itm_equip, bool value);
static void rob_chara_item_equip_set_motion_reset_data(
    rob_chara_item_equip* rob_itm_equip, int32_t motion_id, float_t frame);
static void rob_chara_item_equip_set_motion_skin_param(
    rob_chara_item_equip* rob_itm_equip, int8_t chara_id, int32_t motion_id, int32_t frame);
static void rob_chara_item_equip_set_null_blocks_expression_data(
    rob_chara_item_equip* rob_itm_equip, item_id id, vec3* position, vec3* rotation, vec3* scale);
static void rob_chara_item_equip_set_object_texture_pattern(rob_chara_item_equip* rob_itm_equip,
    item_id id, texture_pattern_struct* tex_pat, size_t count);
static void rob_chara_item_equip_set_opd_blend_data(
    rob_chara_item_equip* rob_itm_equip, std::list<motion_blend_mot*>* a2);
static void rob_chara_item_equip_set_osage_reset(rob_chara_item_equip* rob_itm_equip);
static void rob_chara_item_equip_set_osage_move_cancel(rob_chara_item_equip* rob_itm_equip,
    uint8_t id, float_t value);
static void rob_chara_item_equip_set_osage_step(rob_chara_item_equip* rob_itm_equip, float_t value);
static void rob_chara_item_equip_set_shadow_type(rob_chara_item_equip* rob_itm_equip, int32_t chara_id);
static void rob_chara_item_equip_set_texture_pattern(rob_chara_item_equip* rob_itm_equip,
    texture_pattern_struct* tex_pat, size_t count);

static bool rob_chara_check_for_ageageagain_module(chara_index chara_index, int32_t module_index);
static object_info rob_chara_get_head_object(rob_chara* rob_chr, int32_t head_object_id);
static object_info rob_chara_get_object_info(rob_chara* rob_chr, item_id id);
static void rob_chara_load_default_motion(rob_chara* rob_chr,
    bone_database* bone_data, motion_database* mot_db);
static void rob_chara_load_default_motion_sub(rob_chara* rob_chr, int32_t skeleton_select,
    int32_t motion_id, bone_database* bone_data, motion_database* mot_db);

static void rob_chara_set_adjust(rob_chara* rob_chr, rob_chara_data_adjust* adjust_new,
    rob_chara_data_adjust* adjust, rob_chara_data_adjust* adjust_prev);
static void rob_chara_set_pv_data(rob_chara* rob_chr, int8_t chara_id,
    chara_index chara_index, int32_t module_index, rob_chara_pv_data* pv_data);

static void rob_cmn_mottbl_read(void* a1, void* data, size_t size);

static rob_manager_rob_impl* rob_manager_rob_impls1_get(TaskRobManager* rob_mgr);
static rob_manager_rob_impl* rob_manager_rob_impls2_get(TaskRobManager* rob_mgr);

static void rob_osage_init_data(rob_osage* rob_osg,
    obj_skin_block_osage* osg_data, obj_skin_osage_node* osg_nodes,
    bone_node* ex_data_bone_nodes, obj_skin* skin);
static rob_osage_node* rob_osage_get_node(rob_osage* rob_osg, size_t index);
static float_t* rob_osage_load_opd_data(rob_osage* rob_osg,
    size_t node_index, float_t* opd_data, size_t opd_count);
static void rob_osage_load_skin_param(rob_osage* rob_osg, key_val* kv, const char* name,
    skin_param_osage_root* skp_root, object_info* obj_info, bone_database* bone_data);
static void rob_osage_set_air_res(rob_osage* rob_osg, float_t air_res);
static void rob_osage_coli_set(rob_osage* rob_osg, mat4* mats);
static void rob_osage_set_coli_r(rob_osage* rob_osg, float_t coli_r);
static void rob_osage_set_force(rob_osage* rob_osg, float_t force, float_t force_gain);
static void rob_osage_set_hinge(rob_osage* rob_osg, float_t hinge_y, float_t hinge_z);
static void rob_osage_set_init_rot(rob_osage* rob_osg, float_t init_rot_y, float_t init_rot_z);
static void rob_osage_set_motion_reset_data(rob_osage* rob_osg, int32_t motion_id, float_t frame);
static void rob_osage_set_rot(rob_osage* rob_osg, float_t rotation_y, float_t rotation_z);
static void rob_osage_set_skin_param_osage_root(rob_osage* rob_osg, skin_param_osage_root* skp_root);
static void rob_osage_set_skp_osg_nodes(rob_osage* rob_osg,
    std::vector<skin_param_osage_node>* skp_osg_nodes);
static void rob_osage_set_yz_order(rob_osage* rob_osg, int32_t yz_order);
static void rob_osage_free(rob_osage* rob_osg);

static void skin_param_osage_node_parse(key_val* kv, const char* name,
    std::vector<skin_param_osage_node>* a3, skin_param_osage_root* skp_root);

static void skin_param_osage_root_parse(key_val* kv, const char* name,
    skin_param_osage_root* skp_root, bone_database* bone_data);

static bool task_rob_load_check_load_req_data();

std::map<uint32_t, MotFile> motion_storage_data;
std::map<uint32_t, MhdFile> mothead_storage_data;
osage_setting osage_setting_data;
rob_chara* rob_chara_array;
rob_chara_pv_data* rob_chara_pv_data_array;

static rob_manager_rob_impl rob_manager_rob_impls1[2];
static bool rob_manager_rob_impls1_init = false;
static rob_manager_rob_impl rob_manager_rob_impls2[7];
static bool rob_manager_rob_impls2_init = false;

static PvOsageManager* pv_osage_manager_array_ptr;
static rob_cmn_mottbl_header* rob_cmn_mottbl_data;
static RobThreadHandler* rob_thread_handler;
static SkinParamManager* skin_param_manager;
static TaskRobManager task_rob_manager;
static TaskRobLoad task_rob_load;

static const mothead_mot mothead_mot_null;

static const exp_func_op1 exp_func_op1_array[] = {
    { "neg"     , exp_neg      },
    { "sin"     , exp_sin      },
    { "cos"     , exp_cos      },
    { "tan"     , exp_tan      },
    { "abs"     , exp_abs      },
    { "sqrt"    , exp_sqrt     },
    { "av"      , exp_av       },
    { "floor"   , exp_floor    },
    { "ceil"    , exp_ceil     },
    { "round"   , exp_round    },
    { "asin"    , exp_asin     },
    { "acos"    , exp_acos     },
    { "atan"    , exp_atan     },
    { "log"     , exp_log      },
    { "exp"     , exp_exp      },
    { "rand_0_1", exp_rand_0_1 },
    { 0         , 0            },
};

static const exp_func_op2 exp_func_op2_array[] = {
    { "+"   , exp_add  },
    { "-"   , exp_sub  },
    { "*"   , exp_mul  },
    { "/"   , exp_div  },
    { "%"   , exp_fmod },
    { "=="  , exp_eq   },
    { ">"   , exp_gt   },
    { ">="  , exp_ge   },
    { "<"   , exp_lt   },
    { "<="  , exp_le   },
    { "!="  , exp_ne   },
    { "&&"  , exp_and  },
    { "||"  , exp_or   },
    { "min" , exp_min  },
    { "max" , exp_max  },
    { "fmod", exp_fmod },
    { "pow" , exp_pow  },
    { 0     , 0        },
};

static const exp_func_op3 exp_func_op3_array[] = {
    { "rand", exp_rand },
    { "cond", exp_cond },
    { 0     , 0        },
};

static const mothead_func_struct mothead_func_array[] = {
    { mothead_func_0 , 1 },
    { mothead_func_1 , 1 },
    { mothead_func_2 , 0 },
    { mothead_func_3 , 0 },
    { mothead_func_4 , 0 },
    { mothead_func_5 , 0 },
    { mothead_func_6 , 0 },
    { mothead_func_7 , 0 },
    { mothead_func_8 , 1 },
    { mothead_func_9 , 0 },
    { mothead_func_10, 0 },
    { mothead_func_11, 0 },
    { mothead_func_12, 0 },
    { mothead_func_13, 0 },
    { mothead_func_14, 0 },
    { mothead_func_15, 0 },
    { mothead_func_16, 0 },
    { mothead_func_17, 0 },
    { mothead_func_18, 0 },
    { mothead_func_19, 0 },
    { mothead_func_20, 0 },
    { mothead_func_21, 0 },
    { mothead_func_22, 0 },
    { mothead_func_23, 0 },
    { mothead_func_24, 0 },
    { mothead_func_25, 1 },
    { mothead_func_26, 0 },
    { mothead_func_27, 0 },
    { mothead_func_28, 0 },
    { mothead_func_29, 0 },
    { mothead_func_30, 0 },
    { mothead_func_31, 1 },
    { mothead_func_32, 0 },
    { mothead_func_33, 0 },
    { mothead_func_34, 0 },
    { mothead_func_35, 0 },
    { mothead_func_36, 0 },
    { mothead_func_37, 0 },
    { mothead_func_38, 0 },
    { mothead_func_39, 0 },
    { mothead_func_40, 0 },
    { mothead_func_41, 0 },
    { mothead_func_42, 0 },
    { mothead_func_43, 0 },
    { mothead_func_44, 0 },
    { mothead_func_45, 0 },
    { mothead_func_46, 0 },
    { mothead_func_47, 0 },
    { mothead_func_48, 0 },
    { mothead_func_49, 0 },
    { mothead_func_50_set_face_motion_id, 0 },
    { mothead_func_51, 0 },
    { mothead_func_52, 0 },
    { mothead_func_53_set_face_mottbl_motion, 0 },
    { mothead_func_54_set_hand_r_mottbl_motion, 0 },
    { mothead_func_55_set_hand_l_mottbl_motion, 0 },
    { mothead_func_56_set_mouth_mottbl_motion, 0 },
    { mothead_func_57_set_eyes_mottbl_motion, 0 },
    { mothead_func_58_set_eyelid_mottbl_motion, 0 },
    { mothead_func_59_set_rob_chara_head_object, 0 },
    { mothead_func_60, 0 },
    { mothead_func_61_set_eyelid_motion_from_face, 0 },
    { mothead_func_62_rob_parts_adjust, 0 },
    { mothead_func_63, 0 },
    { mothead_func_64_osage_reset, 0 },
    { mothead_func_65_motion_skin_param, 0 },
    { mothead_func_66_osage_step, 0 },
    { mothead_func_67, 0 },
    { mothead_func_68, 0 },
    { mothead_func_69, 0 },
    { mothead_func_70, 0 },
    { mothead_func_71_osage_move_cancel, 0 },
    { mothead_func_72, 0 },
    { mothead_func_73_rob_hand_adjust, 0 },
    { mothead_func_74, 0 },
    { mothead_func_75_rob_adjust_global, 0 },
    { mothead_func_76_rob_arm_adjust, 0 },
    { mothead_func_77_disable_eye_motion, 0 },
    { mothead_func_78, 0 },
    { mothead_func_79_rob_chara_coli_ring, 0 },
    { mothead_func_80_adjust_get_global_trans, 0 },
};

static const struc_218 stru_140A24B50[] = {
    { { 0.02f , -0.005f,  0.0f   }, 0.09f , 0xAB, 0 },
    { { 0.17f , -0.02f ,  0.03f  }, 0.09f , 0x05, 1 },
    { { 0.17f , -0.02f , -0.03f  }, 0.09f , 0x05, 1 },
    { { 0.0f  ,  0.02f ,  0.0f   }, 0.04f , 0x08, 0 },
    { { 0.005f,  0.0f  ,  0.08f  }, 0.12f , 0x0A, 1 },
    { { 0.03f ,  0.0f  ,  0.015f }, 0.055f, 0x8A, 1 },
    { { 0.15f ,  0.0f  ,  0.01f  }, 0.05f , 0x8A, 0 },
    { { 0.05f ,  0.0f  ,  0.01f  }, 0.05f , 0x8B, 0 },
    { { 0.17f ,  0.0f  ,  0.015f }, 0.06f , 0x8B, 0 },
    { { 0.08f ,  0.0f  ,  0.0f   }, 0.07f , 0x8C, 1 },
    { { 0.03f ,  0.0f  , -0.015f }, 0.055f, 0x67, 1 },
    { { 0.15f ,  0.0f  , -0.01f  }, 0.05f , 0x67, 0 },
    { { 0.05f ,  0.0f  , -0.01f  }, 0.05f , 0x68, 0 },
    { { 0.17f ,  0.0f  , -0.015f }, 0.06f , 0x68, 0 },
    { { 0.08f ,  0.0f  ,  0.0f   }, 0.07f , 0x69, 1 },
    { { 0.04f , -0.02f , -0.02f  }, 0.13f , 0xB3, 1 },
    { { 0.26f , -0.02f , -0.025f }, 0.09f , 0xB3, 0 },
    { { 0.08f ,  0.0f  , -0.02f  }, 0.09f , 0xB4, 0 },
    { { 0.28f ,  0.0f  , -0.02f  }, 0.09f , 0xB4, 0 },
    { { 0.03f , -0.02f , -0.01f  }, 0.07f , 0xB5, 0 },
    { { 0.02f , -0.02f , -0.02f  }, 0.035f, 0xB6, 0 },
    { { 0.04f , -0.02f ,  0.02f  }, 0.13f , 0xAD, 1 },
    { { 0.26f , -0.02f ,  0.025f }, 0.09f , 0xAD, 0 },
    { { 0.09f ,  0.0f  ,  0.02f  }, 0.09f , 0xAE, 0 },
    { { 0.28f ,  0.0f  ,  0.02f  }, 0.09f , 0xAE, 0 },
    { { 0.03f , -0.02f ,  0.01f  }, 0.07f , 0xAF, 0 },
    { { 0.02f , -0.02f ,  0.02f  }, 0.035f, 0xB0, 0 },
};

static const struc_218 stru_140A25090[] = {
    { { -0.00391f, -0.08831f,  0.0f     }, 0.09f , 0xAB, 0 },
    { {  0.17f   , -0.02f   ,  0.03f    }, 0.09f , 0x05, 1 },
    { {  0.17f   , -0.02f   , -0.03f    }, 0.09f , 0x05, 1 },
    { {  0.0f    ,  0.02f   ,  0.0f     }, 0.04f , 0x08, 0 },
    { {  0.05025f,  0.0f    , -0.00181f }, 0.12f , 0x0A, 1 },
    { {  0.03f   ,  0.0f    ,  0.015f   }, 0.055f, 0x8A, 1 },
    { {  0.15f   ,  0.0f    ,  0.01f    }, 0.05f , 0x8A, 0 },
    { {  0.05f   ,  0.0f    ,  0.01f    }, 0.05f , 0x8B, 0 },
    { {  0.17f   ,  0.0f    ,  0.015f   }, 0.06f , 0x8B, 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , 0x8C, 1 },
    { {  0.03f   ,  0.0f    , -0.015f   }, 0.055f, 0x67, 1 },
    { {  0.15f   ,  0.0f    , -0.01f    }, 0.05f , 0x67, 0 },
    { {  0.05f   ,  0.0f    , -0.01f    }, 0.05f , 0x68, 0 },
    { {  0.17f   ,  0.0f    , -0.015f   }, 0.06f , 0x68, 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , 0x69, 1 },
    { {  0.04f   , -0.02f   , -0.02f    }, 0.13f , 0xB3, 1 },
    { {  0.26f   , -0.02f   , -0.025f   }, 0.09f , 0xB3, 0 },
    { {  0.08f   ,  0.0f    , -0.02f    }, 0.09f , 0xB4, 0 },
    { {  0.28f   ,  0.0f    , -0.02f    }, 0.09f , 0xB4, 0 },
    { {  0.03f   , -0.02f   , -0.01f    }, 0.07f , 0xB5, 0 },
    { {  0.02f   , -0.02f   , -0.02f    }, 0.035f, 0xB6, 0 },
    { {  0.04f   , -0.02f   ,  0.02f    }, 0.13f , 0xAD, 1 },
    { {  0.26f   , -0.02f   ,  0.025f   }, 0.09f , 0xAD, 0 },
    { {  0.08f   ,  0.0f    ,  0.02f    }, 0.09f , 0xAE, 0 },
    { {  0.28f   ,  0.0f    ,  0.02f    }, 0.09f , 0xAE, 0 },
    { {  0.03f   , -0.02f   ,  0.01f    }, 0.07f , 0xAF, 0 },
    { {  0.02f   , -0.02f   ,  0.02f    }, 0.035f, 0xB0, 0 },
};

static const struc_218 stru_140A24E00[] = {
    { { 0.02f    , -0.005f,  0.0f   }, 0.09f , 0xAB, 0 },
    { { 0.165f   , -0.02f ,  0.015f }, 0.105f, 0x05, 1 },
    { { 0.165f   , -0.02f , -0.015f }, 0.105f, 0x05, 1 },
    { { 0.0f     ,  0.02f ,  0.0f   }, 0.04f , 0x08, 0 },
    { { 0.005f   ,  0.0f  ,  0.08f  }, 0.12f , 0x0A, 1 },
    { { 0.03f    ,  0.0f  ,  0.015f }, 0.055f, 0x8A, 1 },
    { { 0.15f    ,  0.0f  ,  0.01f  }, 0.05f , 0x8A, 0 },
    { { 0.05f    ,  0.0f  ,  0.01f  }, 0.05f , 0x8B, 0 },
    { { 0.17f    ,  0.0f  ,  0.015f }, 0.06f , 0x8B, 0 },
    { { 0.08f    ,  0.0f  ,  0.0f   }, 0.07f , 0x8C, 1 },
    { { 0.03f    ,  0.0f  , -0.015f }, 0.055f, 0x67, 1 },
    { { 0.15f    ,  0.0f  , -0.01f  }, 0.05f , 0x67, 0 },
    { { 0.05f    ,  0.0f  , -0.01f  }, 0.05f , 0x68, 0 },
    { { 0.17f    ,  0.0f  , -0.015f }, 0.06f , 0x68, 0 },
    { { 0.08f    ,  0.0f  ,  0.0f   }, 0.07f , 0x69, 1 },
    { { 0.04f    , -0.02f , -0.02f  }, 0.13f , 0xB3, 1 },
    { { 0.26f    , -0.02f , -0.025f }, 0.09f , 0xB3, 0 },
    { { 0.08f    ,  0.0f  , -0.02f  }, 0.09f , 0xB4, 0 },
    { { 0.28f    ,  0.0f  , -0.02f  }, 0.09f , 0xB4, 0 },
    { { 0.03f    , -0.02f , -0.01f  }, 0.07f , 0xB5, 0 },
    { { 0.02f    , -0.02f , -0.02f  }, 0.035f, 0xB6, 0 },
    { { 0.04f    , -0.02f ,  0.02f  }, 0.13f , 0xAD, 1 },
    { { 0.26f    , -0.02f ,  0.025f }, 0.09f , 0xAD, 0 },
    { { 0.08f    ,  0.0f  ,  0.02f  }, 0.09f , 0xAE, 0 },
    { { 0.28f    ,  0.0f  ,  0.02f  }, 0.09f , 0xAE, 0 },
    { { 0.03f    , -0.02f ,  0.01f  }, 0.07f , 0xAF, 0 },
    { { 0.02f    , -0.02f ,  0.02f  }, 0.035f, 0xB0, 0 },
};

static const struc_218 stru_140A25340[] = {
    { { -0.00391f, -0.08831f,  0.0f     }, 0.09f , 0xAB, 0 },
    { {  0.165f  , -0.02f   ,  0.015f   }, 0.105f, 0x05, 1 },
    { {  0.165f  , -0.02f   , -0.015f   }, 0.105f, 0x05, 1 },
    { {  0.0f    ,  0.02f   ,  0.0f     }, 0.04f , 0x08, 0 },
    { {  0.05025f,  0.0f    , -0.00181f }, 0.12f , 0x0A, 1 },
    { {  0.03f   ,  0.0f    ,  0.015f   }, 0.055f, 0x8A, 1 },
    { {  0.15f   ,  0.0f    ,  0.01f    }, 0.05f , 0x8A, 0 },
    { {  0.05f   ,  0.0f    ,  0.01f    }, 0.05f , 0x8B, 0 },
    { {  0.17f   ,  0.0f    ,  0.015f   }, 0.06f , 0x8B, 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , 0x8C, 1 },
    { {  0.03f   ,  0.0f    , -0.015f   }, 0.055f, 0x67, 1 },
    { {  0.15f   ,  0.0f    , -0.01f    }, 0.05f , 0x67, 0 },
    { {  0.05f   ,  0.0f    , -0.01f    }, 0.05f , 0x68, 0 },
    { {  0.17f   ,  0.0f    , -0.015f   }, 0.06f , 0x68, 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , 0x69, 1 },
    { {  0.04f   , -0.02f   , -0.02f    }, 0.13f , 0xB3, 1 },
    { {  0.26f   , -0.02f   , -0.025f   }, 0.09f , 0xB3, 0 },
    { {  0.08f   ,  0.0f    , -0.02f    }, 0.09f , 0xB4, 0 },
    { {  0.28f   ,  0.0f    , -0.02f    }, 0.09f , 0xB4, 0 },
    { {  0.03f   , -0.02f   , -0.01f    }, 0.07f , 0xB5, 0 },
    { {  0.02f   , -0.02f   , -0.02f    }, 0.035f, 0xB6, 0 },
    { {  0.04f   , -0.02f   ,  0.02f    }, 0.13f , 0xAD, 1 },
    { {  0.26f   , -0.02f   ,  0.025f   }, 0.09f , 0xAD, 0 },
    { {  0.08f   ,  0.0f    ,  0.02f    }, 0.09f , 0xAE, 0 },
    { {  0.28f   ,  0.0f    ,  0.02f    }, 0.09f , 0xAE, 0 },
    { {  0.03f   , -0.02f   ,  0.01f    }, 0.07f , 0xAF, 0 },
    { {  0.02f   , -0.02f   ,  0.02f    }, 0.035f, 0xB0, 0 },
};

static const chara_init_data chara_init_data_array[] = {
    {
        0x0000, BONE_DATABASE_SKELETON_MIKU,
        {
            object_info(0x0000, 0x0000),
            object_info(0x00F7, 0x0000),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        1, stru_140A24B50, stru_140A25090,
        1, 0, { 0x01, 0x01, 0x1E, 0x1D },
        {
            object_info(0x00F7, 0x0000),
            object_info(0x011D, 0x0000),
            object_info(0x011E, 0x0000),
            object_info(0x011F, 0x0000),
            object_info(0x0120, 0x0000),
            object_info(0x0121, 0x0000),
            object_info(0x0122, 0x0000),
            object_info(0x020F, 0x0000),
            object_info(0x0251, 0x0000),
        },
        {
            object_info(),
            object_info(0x0124, 0x0000),
            object_info(0x0123, 0x0000),
            object_info(0x0125, 0x0000),
            object_info(0x0126, 0x0000),
            object_info(0x0127, 0x0000),
            object_info(0x0128, 0x0000),
            object_info(0x0259, 0x0000),
            object_info(0x0254, 0x0000),
            object_info(0x0252, 0x0000),
            object_info(0x0253, 0x0000),
            object_info(0x0255, 0x0000),
            object_info(0x0256, 0x0000),
            object_info(0x0258, 0x0000),
            object_info(0x0257, 0x0000),
        },
    },
    {
        0x0107, BONE_DATABASE_SKELETON_RIN,
        {
            object_info(0x0000, 0x0000),
            object_info(0x004A, 0x0107),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        9, stru_140A24B50, stru_140A25090,
        1, 0, { 0x02, 0x02, 0x01, 0x01 },
        {
            object_info(0x004A, 0x0107),
            object_info(0x004E, 0x0107),
            object_info(0x004F, 0x0107),
            object_info(0x0050, 0x0107),
            object_info(0x0051, 0x0107),
            object_info(0x0052, 0x0107),
            object_info(0x0053, 0x0107),
            object_info(0x00A6, 0x0107),
            object_info(0x00C3, 0x0107),
        },
        {
            object_info(),
            object_info(0x0055, 0x0107),
            object_info(0x0054, 0x0107),
            object_info(0x0056, 0x0107),
            object_info(0x0057, 0x0107),
            object_info(0x0058, 0x0107),
            object_info(0x0059, 0x0107),
            object_info(0x00CB, 0x0107),
            object_info(0x00C6, 0x0107),
            object_info(0x00C4, 0x0107),
            object_info(0x00C5, 0x0107),
            object_info(0x00C7, 0x0107),
            object_info(0x00C8, 0x0107),
            object_info(0x00CA, 0x0107),
            object_info(0x00C9, 0x0107),
        },
    },
    {
        0x0105, BONE_DATABASE_SKELETON_LEN,
        {
            object_info(0x0000, 0x0000),
            object_info(0x0040, 0x0105),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        5, stru_140A24B50, stru_140A25090,
        0, 0, { 0x02, 0x03, 0x01, 0x01 },
        {
            object_info(0x0040, 0x0105),
            object_info(0x0041, 0x0105),
            object_info(0x0042, 0x0105),
            object_info(0x0043, 0x0105),
            object_info(0x0044, 0x0105),
            object_info(0x0045, 0x0105),
            object_info(0x0046, 0x0105),
            object_info(0x007F, 0x0105),
            object_info(0x0098, 0x0105),
        },
        {
            object_info(),
            object_info(0x0048, 0x0105),
            object_info(0x0047, 0x0105),
            object_info(0x0049, 0x0105),
            object_info(0x004A, 0x0105),
            object_info(0x004B, 0x0105),
            object_info(0x004C, 0x0105),
            object_info(0x00A0, 0x0105),
            object_info(0x009B, 0x0105),
            object_info(0x0099, 0x0105),
            object_info(0x009A, 0x0105),
            object_info(0x009C, 0x0105),
            object_info(0x009D, 0x0105),
            object_info(0x009F, 0x0105),
            object_info(0x009E, 0x0105),
        },
    },
    {
        0x0106, BONE_DATABASE_SKELETON_LUKA,
        {
            object_info(0x0000, 0x0000),
            object_info(0x004C, 0x0106),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        6, stru_140A24E00, stru_140A25340,
        0, 0, { 0x01, 0x04, 0x01, 0x01 },
        {
            object_info(0x004C, 0x0106),
            object_info(0x004D, 0x0106),
            object_info(0x004E, 0x0106),
            object_info(0x004F, 0x0106),
            object_info(0x0050, 0x0106),
            object_info(0x0051, 0x0106),
            object_info(0x0052, 0x0106),
            object_info(0x008C, 0x0106),
            object_info(0x00A1, 0x0106),
        },
        {
            object_info(),
            object_info(0x0054, 0x0106),
            object_info(0x0053, 0x0106),
            object_info(0x0055, 0x0106),
            object_info(0x0056, 0x0106),
            object_info(0x0057, 0x0106),
            object_info(0x0058, 0x0106),
            object_info(0x00A9, 0x0106),
            object_info(0x00A4, 0x0106),
            object_info(0x00A2, 0x0106),
            object_info(0x00A3, 0x0106),
            object_info(0x00A5, 0x0106),
            object_info(0x00A6, 0x0106),
            object_info(0x00A8, 0x0106),
            object_info(0x00A7, 0x0106),
        },
    },
    {
        0x0108, BONE_DATABASE_SKELETON_NERU,
        {
            object_info(0x0000, 0x0000),
            object_info(0x0019, 0x0108),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        8, stru_140A24B50, stru_140A25090,
        0, 0, { 0x02, 0x02, 0x02, 0x02 },
        {
            object_info(0x0019, 0x0108),
            object_info(0x001A, 0x0108),
            object_info(0x001B, 0x0108),
            object_info(0x001C, 0x0108),
            object_info(0x001D, 0x0108),
            object_info(0x001E, 0x0108),
            object_info(0x001F, 0x0108),
            object_info(0x002A, 0x0108),
            object_info(0x002B, 0x0108),
        },
        {
            object_info(),
            object_info(0x0021, 0x0108),
            object_info(0x0020, 0x0108),
            object_info(0x0022, 0x0108),
            object_info(0x0023, 0x0108),
            object_info(0x0024, 0x0108),
            object_info(0x0025, 0x0108),
            object_info(0x0033, 0x0108),
            object_info(0x002E, 0x0108),
            object_info(0x002C, 0x0108),
            object_info(0x002D, 0x0108),
            object_info(0x002F, 0x0108),
            object_info(0x0030, 0x0108),
            object_info(0x0032, 0x0108),
            object_info(0x0031, 0x0108),
        },
    },
    {
        0x0109, BONE_DATABASE_SKELETON_HAKU,
        {
            object_info(0x0000, 0x0000),
            object_info(0x001F, 0x0109),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        3, stru_140A24E00, stru_140A25340,
        1, 0, { 0x01, 0x01, 0x02, 0x02 },
        {
            object_info(0x001F, 0x0109),
            object_info(0x0020, 0x0109),
            object_info(0x0021, 0x0109),
            object_info(0x0022, 0x0109),
            object_info(0x0023, 0x0109),
            object_info(0x0024, 0x0109),
            object_info(0x0025, 0x0109),
            object_info(0x0032, 0x0109),
            object_info(0x0033, 0x0109),
        },
        {
            object_info(),
            object_info(0x0027, 0x0109),
            object_info(0x0026, 0x0109),
            object_info(0x0028, 0x0109),
            object_info(0x0029, 0x0109),
            object_info(0x002A, 0x0109),
            object_info(0x002B, 0x0109),
            object_info(0x003B, 0x0109),
            object_info(0x0036, 0x0109),
            object_info(0x0034, 0x0109),
            object_info(0x0035, 0x0109),
            object_info(0x0037, 0x0109),
            object_info(0x0038, 0x0109),
            object_info(0x003A, 0x0109),
            object_info(0x0039, 0x0109),
        },
    },
    {
        0x010D, BONE_DATABASE_SKELETON_KAITO,
        {
            object_info(0x0000, 0x0000),
            object_info(0x003C, 0x010D),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        4, stru_140A24B50, stru_140A25090,
        0, 0, { 0x00, 0x00, 0x01, 0x01 },
        {
            object_info(0x003C, 0x010D),
            object_info(0x0050, 0x010D),
            object_info(0x003D, 0x010D),
            object_info(0x003E, 0x010D),
            object_info(0x003F, 0x010D),
            object_info(0x0040, 0x010D),
            object_info(0x0041, 0x010D),
            object_info(0x006D, 0x010D),
            object_info(0x0077, 0x010D),
        },
        {
            object_info(),
            object_info(0x0044, 0x010D),
            object_info(0x0043, 0x010D),
            object_info(0x0045, 0x010D),
            object_info(0x0046, 0x010D),
            object_info(0x0047, 0x010D),
            object_info(0x0048, 0x010D),
            object_info(0x007F, 0x010D),
            object_info(0x007A, 0x010D),
            object_info(0x0078, 0x010D),
            object_info(0x0079, 0x010D),
            object_info(0x007B, 0x010D),
            object_info(0x007C, 0x010D),
            object_info(0x007E, 0x010D),
            object_info(0x007D, 0x010D),
        },
    },
    {
        0x010E, BONE_DATABASE_SKELETON_MEIKO,
        {
            object_info(0x0000, 0x0000),
            object_info(0x0040, 0x010E),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        7, stru_140A24E00, stru_140A25340,
        1, 0, { 0x00, 0x00, 0x01, 0x01 },
        {
            object_info(0x0040, 0x010E),
            object_info(0x0041, 0x010E),
            object_info(0x0042, 0x010E),
            object_info(0x0043, 0x010E),
            object_info(0x0044, 0x010E),
            object_info(0x0045, 0x010E),
            object_info(0x0046, 0x010E),
            object_info(0x007B, 0x010E),
            object_info(0x0085, 0x010E),
        },
        {
            object_info(),
            object_info(0x0048, 0x010E),
            object_info(0x0047, 0x010E),
            object_info(0x0049, 0x010E),
            object_info(0x004A, 0x010E),
            object_info(0x004B, 0x010E),
            object_info(0x004C, 0x010E),
            object_info(0x008D, 0x010E),
            object_info(0x0088, 0x010E),
            object_info(0x0086, 0x010E),
            object_info(0x0087, 0x010E),
            object_info(0x0089, 0x010E),
            object_info(0x008A, 0x010E),
            object_info(0x008C, 0x010E),
            object_info(0x008B, 0x010E),
        },
    },
    {
        0x010F, BONE_DATABASE_SKELETON_SAKINE,
        {
            object_info(0x0000, 0x0000),
            object_info(0x001A, 0x010F),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        10, stru_140A24E00, stru_140A25340,
        1, 0, { 0x01, 0x01, 0x02, 0x02 },
        {
            object_info(0x001A, 0x010F),
            object_info(0x001B, 0x010F),
            object_info(0x001C, 0x010F),
            object_info(0x001D, 0x010F),
            object_info(0x001E, 0x010F),
            object_info(0x001F, 0x010F),
            object_info(0x0020, 0x010F),
            object_info(0x0031, 0x010F),
            object_info(0x0033, 0x010F),
        },
        {
            object_info(),
            object_info(0x0022, 0x010F),
            object_info(0x0021, 0x010F),
            object_info(0x0023, 0x010F),
            object_info(0x0024, 0x010F),
            object_info(0x0025, 0x010F),
            object_info(0x0026, 0x010F),
            object_info(0x003B, 0x010F),
            object_info(0x0036, 0x010F),
            object_info(0x0034, 0x010F),
            object_info(0x0035, 0x010F),
            object_info(0x0037, 0x010F),
            object_info(0x0038, 0x010F),
            object_info(0x003A, 0x010F),
            object_info(0x0039, 0x010F),
        },
    },
    {
        0x063D, BONE_DATABASE_SKELETON_TETO,
        {
            object_info(0x0000, 0x0000),
            object_info(0x0000, 0x063D),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
            object_info(),
        },
        467, stru_140A24B50, stru_140A25090,
        1, 0, { 0x01, 0x01, 0x02, 0x02 },
        {
            object_info(0x0000, 0x063D),
            object_info(0x0001, 0x063D),
            object_info(0x0002, 0x063D),
            object_info(0x0003, 0x063D),
            object_info(0x0004, 0x063D),
            object_info(0x0005, 0x063D),
            object_info(0x0006, 0x063D),
            object_info(0x0012, 0x063D),
            object_info(0x0013, 0x063D),
        },
        {
            object_info(),
            object_info(0x0008, 0x063D),
            object_info(0x0007, 0x063D),
            object_info(0x0009, 0x063D),
            object_info(0x000A, 0x063D),
            object_info(0x000B, 0x063D),
            object_info(0x000C, 0x063D),
            object_info(0x001B, 0x063D),
            object_info(0x0016, 0x063D),
            object_info(0x0014, 0x063D),
            object_info(0x0015, 0x063D),
            object_info(0x0017, 0x063D),
            object_info(0x0018, 0x063D),
            object_info(0x001A, 0x063D),
            object_info(0x0019, 0x063D),
        },
    }
};

static const motion_bone_index rob_ik_hand_leg_bones[] = {
    MOTION_BONE_C_KATA_R, MOTION_BONE_C_KATA_L,
    MOTION_BONE_CL_MOMO_R, MOTION_BONE_CL_MOMO_L,
};

static const motion_bone_index dword_140A2DC90[] = {
    MOTION_BONE_C_KATA_R, MOTION_BONE_KL_TE_R_WJ, MOTION_BONE_N_HITO_R_EX,
    MOTION_BONE_NL_HITO_R_WJ, MOTION_BONE_NL_HITO_B_R_WJ, MOTION_BONE_NL_HITO_C_R_WJ,
    MOTION_BONE_N_KO_R_EX, MOTION_BONE_NL_KO_R_WJ, MOTION_BONE_NL_KO_B_R_WJ,
    MOTION_BONE_NL_KO_C_R_WJ, MOTION_BONE_N_KUSU_R_EX, MOTION_BONE_NL_KUSU_R_WJ,
    MOTION_BONE_NL_KUSU_B_R_WJ, MOTION_BONE_NL_KUSU_C_R_WJ, MOTION_BONE_N_NAKA_R_EX,
    MOTION_BONE_NL_NAKA_R_WJ, MOTION_BONE_NL_NAKA_B_R_WJ, MOTION_BONE_NL_NAKA_C_R_WJ,
    MOTION_BONE_N_OYA_R_EX, MOTION_BONE_NL_OYA_R_WJ, MOTION_BONE_NL_OYA_B_R_WJ,
    MOTION_BONE_NL_OYA_C_R_WJ, MOTION_BONE_N_STE_R_WJ_EX, MOTION_BONE_N_SUDE_R_WJ_EX,
    MOTION_BONE_N_SUDE_B_R_WJ_EX, MOTION_BONE_N_HIJI_R_WJ_EX, MOTION_BONE_N_UP_KATA_R_EX,
    MOTION_BONE_N_SKATA_R_WJ_CD_EX, MOTION_BONE_N_SKATA_B_R_WJ_CD_CU_EX,
    MOTION_BONE_N_SKATA_C_R_WJ_CD_CU_EX, MOTION_BONE_NONE
};

static const motion_bone_index dword_140A2DD10[] = {
    MOTION_BONE_C_KATA_L, MOTION_BONE_KL_TE_L_WJ, MOTION_BONE_N_HITO_L_EX,
    MOTION_BONE_NL_HITO_L_WJ, MOTION_BONE_NL_HITO_B_L_WJ, MOTION_BONE_NL_HITO_C_L_WJ,
    MOTION_BONE_N_KO_L_EX, MOTION_BONE_NL_KO_L_WJ, MOTION_BONE_NL_KO_B_L_WJ,
    MOTION_BONE_NL_KO_C_L_WJ, MOTION_BONE_N_KUSU_L_EX, MOTION_BONE_NL_KUSU_L_WJ,
    MOTION_BONE_NL_KUSU_B_L_WJ, MOTION_BONE_NL_KUSU_C_L_WJ, MOTION_BONE_N_NAKA_L_EX,
    MOTION_BONE_NL_NAKA_L_WJ, MOTION_BONE_NL_NAKA_B_L_WJ, MOTION_BONE_NL_NAKA_C_L_WJ,
    MOTION_BONE_N_OYA_L_EX, MOTION_BONE_NL_OYA_L_WJ, MOTION_BONE_NL_OYA_B_L_WJ,
    MOTION_BONE_NL_OYA_C_L_WJ, MOTION_BONE_N_STE_L_WJ_EX, MOTION_BONE_N_SUDE_L_WJ_EX,
    MOTION_BONE_N_SUDE_B_L_WJ_EX, MOTION_BONE_N_HIJI_L_WJ_EX, MOTION_BONE_N_UP_KATA_L_EX,
    MOTION_BONE_N_SKATA_L_WJ_CD_EX, MOTION_BONE_N_SKATA_B_L_WJ_CD_CU_EX,
    MOTION_BONE_N_SKATA_C_L_WJ_CD_CU_EX, MOTION_BONE_NONE
};

static const motion_bone_index dword_140A2DD90[] = {
    MOTION_BONE_CL_MOMO_R, MOTION_BONE_KL_ASI_R_WJ_CO, MOTION_BONE_KL_TOE_R_WJ,
    MOTION_BONE_N_HIZA_R_WJ_EX, MOTION_BONE_N_MOMO_A_R_WJ_CD_EX,
    MOTION_BONE_N_MOMO_B_R_WJ_EX, MOTION_BONE_N_MOMO_C_R_WJ_EX, MOTION_BONE_NONE
};

static const motion_bone_index dword_140A2DDB0[] = {
    MOTION_BONE_CL_MOMO_L, MOTION_BONE_KL_ASI_L_WJ_CO, MOTION_BONE_KL_TOE_L_WJ,
    MOTION_BONE_N_HIZA_L_WJ_EX, MOTION_BONE_N_MOMO_A_L_WJ_CD_EX,
    MOTION_BONE_N_MOMO_B_L_WJ_EX, MOTION_BONE_N_MOMO_C_L_WJ_EX, MOTION_BONE_NONE
};

static const rob_bone_index dword_140A2DDD0[] = {
    ROB_BONE_NONE, ROB_BONE_KL_TE_R_WJ, ROB_BONE_KL_TE_L_WJ, ROB_BONE_KL_ASI_R_WJ_CO,
    ROB_BONE_KL_ASI_L_WJ_CO, ROB_BONE_J_SUNE_R_WJ, ROB_BONE_J_SUNE_L_WJ,
    ROB_BONE_UDE_R_WJ, ROB_BONE_UDE_L_WJ, ROB_BONE_CL_KAO, ROB_BONE_NONE,
    ROB_BONE_NONE, ROB_BONE_NONE, ROB_BONE_NONE, ROB_BONE_NONE, ROB_BONE_NONE,
    ROB_BONE_NONE, ROB_BONE_KL_TE_R_WJ, ROB_BONE_KL_TE_L_WJ, ROB_BONE_KL_ASI_R_WJ_CO,
    ROB_BONE_KL_ASI_L_WJ_CO, ROB_BONE_J_SUNE_R_WJ, ROB_BONE_J_SUNE_L_WJ,
    ROB_BONE_UDE_R_WJ, ROB_BONE_UDE_L_WJ, ROB_BONE_CL_KAO, ROB_BONE_NONE,
};

static const motion_bone_index rob_motion_c_kata_bones[] = {
    MOTION_BONE_C_KATA_L,
    MOTION_BONE_C_KATA_R,
};

static const rob_bone_index rob_kl_te_bones[] = {
    ROB_BONE_KL_TE_L_WJ,
    ROB_BONE_KL_TE_R_WJ
};

static const motion_bone_index* off_140C9E000[] = {
    dword_140A2DC90,
    dword_140A2DD10,
};

static const motion_bone_index* off_140C9E020[] = {
    dword_140A2DD10,
    dword_140A2DC90,
};

static const struc_241 chara_some_data_array[] = {
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0436332300305367f, 0.0610865242779255f, -0.0802851468324661f, 0.113446399569511f,
        0.0148999998345971f, -0.0212999992072582f, 0.0f, 1.0f,
        1.0f, -3.8f, 6.0f, -3.8f,
        6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0750491619110107f, 0.104719758033752f, -0.0733038261532783f, 0.125663697719574f,
        0.033500000834465f, -0.0111999996006489f, 0.0f, 1.0f,
        1.0f, -5.5f, 10.0f, -5.5f,
        6.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0837758034467697f, 0.0855211317539215f, -0.104719758033752f, 0.120427720248699f,
        0.0166999995708466f, -0.00540000014007092f, 0.0f, 1.0f,
        1.0f, -6.5f, 6.0f, -6.0f,
        2.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0349065847694874f, 0.0698131695389748f, -0.0872664600610733f, 0.113446399569511f,
        0.011400000192225f, 0.00810000021010637f, 0.0f, 1.0f,
        1.0f, -3.5f, 6.0f, -3.5f,
        6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0349065847694874f, 0.0733038261532783f, -0.0907571166753769f, 0.13962633907795f,
        0.0151000004261732f, -0.0439999997615814f, 0.0f, 1.0f,
        1.0f, -2.6f, 6.0f, -2.6f,
        6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0523598790168762f, 0.122173048555851f, -0.0872664600610733f, 0.130899697542191f,
        0.0221999995410442f, -0.000199999994947575f, 0.0f, 1.0f,
        1.0f, -4.0f, 7.5f, -3.5f,
        6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0733038261532783f, 0.101229101419449f, -0.0785398185253143f, 0.113446399569511f,
        0.025000000372529f, -0.0230999998748302f, 0.0f, 1.0f,
        1.0f, -4.0f, 6.0f, -3.8f,
        3.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0663225129246712f, 0.0593411959707737f, -0.0785398185253143f, 0.113446399569511f,
        0.0118000004440546f, -0.00510000018402934f, 0.0f, 1.0f,
        1.0f, -5.6f, 6.0f, -5.6f,
        5.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0645771846175194f, 0.0663225129246712f, -0.0698131695389748f, 0.113446399569511f,
        0.0229000002145767f, -0.0148999998345971f, 0.0f, 1.0f,
        1.0f, -5.0f, 4.0f, -5.0f,
        2.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0698131695389748f, 0.0698131695389748f, -0.113446399569511f, 0.122173048555851f,
        0.0137000000104308f, -0.00800000037997961f, 0.0f, 1.0f,
        1.0f, -5.5f, 6.0f, -5.0f,
        4.0f,
    },
};

static const skeleton_rotation_offset skeleton_rotation_offset_array[] = {
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, (float_t)(-M_PI * 2.0), 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { (float_t)M_PI, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 1, { (float_t)-M_PI, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { (float_t)M_PI, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 1, { (float_t)-M_PI, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 0, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 1, 1, 0, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
    { 0, 1, 1, { 0.0f, 0.0f, 0.0f }, },
};

extern render_context* rctx_ptr;

const chara_init_data* chara_init_data_get(chara_index chara_index) {
    if (chara_index >= CHARA_MIKU && chara_index <= CHARA_TETO)
        return &chara_init_data_array[chara_index];
    return 0;
}

int32_t chara_init_data_get_chara_size_index(chara_index chara_index) {
    const chara_init_data* chara_init_data = chara_init_data_get(chara_index);
    if (chara_init_data)
        return chara_init_data->field_848.chara_size_index;
    return 1;
}

float_t chara_size_table_get_value(uint32_t index) {
    static const float_t chara_size_table[] = {
        1.07f, 1.0f, 0.96f, 0.987f, 1.025f, 1.045f
    };

    if (index >= 0 && index <= 4)
        return chara_size_table[index];
    return 1.0f;
}

float_t chara_pos_adjust_y_table_get_value(uint32_t index) {
    static const float_t chara_pos_adjust_y_table[] = {
        0.071732f, 0.0f, -0.03859f, 0.0f, 0.0f
    };

    if (index >= 0 && index <= 4)
        return chara_pos_adjust_y_table[index];
    return 0.0f;
}

void motion_set_load_mothead(uint32_t set, std::string* mdata_dir, motion_database* mot_db) {
    motion_set_info* set_info = mot_db->get_motion_set_by_id(set);
    if (!set_info)
        return;

    std::string file = "mothead_";
    file += set_info->name;
    file += ".bin";

    std::string path = "rom/rob/";
    if (mdata_dir && rctx_ptr->data->check_file_exists(mdata_dir->c_str(), file.c_str()))
        path = *mdata_dir;

    auto elem = mothead_storage_data.find(set);
    if (elem == mothead_storage_data.end())
        elem = mothead_storage_data.insert({ set, {} }).first;

    elem->second.LoadFile(path.c_str(), file.c_str(), set);
}

void motion_set_load_motion(uint32_t set, std::string* mdata_dir, motion_database* mot_db) {
    motion_set_info* set_info = mot_db->get_motion_set_by_id(set);
    if (!set_info)
        return;

    auto elem = motion_storage_data.find(set);
    if (elem == motion_storage_data.end())
        elem = motion_storage_data.insert({ set, {} }).first;

    elem->second.mot_set_info = set_info;
    elem->second.LoadFile(mdata_dir, set);
}

void motion_set_unload_motion(uint32_t set) {
    auto elem = motion_storage_data.find(set);
    if (elem != motion_storage_data.end() && elem->second.Unload())
        motion_storage_data.erase(elem);
}

void motion_set_unload_mothead(uint32_t set) {
    auto elem = mothead_storage_data.find(set);
    if (elem != mothead_storage_data.end() && elem->second.Unload())
        mothead_storage_data.erase(elem);
}

void pv_osage_manager_array_reset(int32_t chara_id) {
    pv_osage_manager_array_ptr_get(chara_id)->Reset();
}

void rob_init() {
    if (!pv_osage_manager_array_ptr)
        pv_osage_manager_array_ptr = new PvOsageManager[CHARA_MAX];

    if (!rob_chara_array)
        rob_chara_array = new rob_chara[ROB_CHARA_COUNT];

    if (!rob_chara_pv_data_array)
        rob_chara_pv_data_array = new rob_chara_pv_data[ROB_CHARA_COUNT];

    {
        p_file_handler rob_mot_tbl_file_handler;
        rob_mot_tbl_file_handler.read_file(rctx_ptr->data, "rom/rob/", "rob_mot_tbl.bin");
        rob_mot_tbl_file_handler.set_callback_data(0, rob_cmn_mottbl_read, 0);
        rob_mot_tbl_file_handler.read_now();
        rob_mot_tbl_file_handler.free_data();
    }

    if (!rob_thread_handler)
        rob_thread_handler = new RobThreadHandler;

    if (!skin_param_manager)
        skin_param_manager = new SkinParamManager[ROB_CHARA_COUNT];
}

void rob_free() {
    if (pv_osage_manager_array_ptr) {
        delete[] pv_osage_manager_array_ptr;
        pv_osage_manager_array_ptr = 0;
    }

    if (rob_chara_array) {
        delete[] rob_chara_array;
        rob_chara_array = 0;
    }

    if (rob_chara_pv_data_array) {
        delete[] rob_chara_pv_data_array;
        rob_chara_pv_data_array = 0;
    }

    free(rob_cmn_mottbl_data);

    if (rob_thread_handler) {
        delete rob_thread_handler;
        rob_thread_handler = 0;
    }

    if (skin_param_manager) {
        delete[] skin_param_manager;
        skin_param_manager = 0;
    }
}

static void sub_1405EE878(ExNodeBlock* node) {

}

static void rob_chara_item_equip_object_ctrl_iterate_nodes(rob_chara_item_equip_object* itm_eq_obj, int32_t osage_iterations) {
    if (!itm_eq_obj->node_blocks.size())
        return;

    for (ExNodeBlock*& i : itm_eq_obj->node_blocks)
        i->Field_48();

    for (; osage_iterations; osage_iterations--) {
        if (itm_eq_obj->field_1B8 && itm_eq_obj->node_blocks.size())
            for (int32_t i = 0; i < 6; i++)
                for (ExNodeBlock*& j : itm_eq_obj->node_blocks)
                    j->Field_18(i, true);

        for (ExNodeBlock*& i : itm_eq_obj->node_blocks)
            i->Field_50();
    }
}

static void rob_chara_item_equip_object_load_opd_data(rob_chara_item_equip_object* itm_eq_obj) {
    if (!itm_eq_obj->osage_blocks.size() && !itm_eq_obj->cloth_blocks.size())
        return;

    rob_chara_item_equip* rob_itm_equip = itm_eq_obj->item_equip;
    size_t index = 0;
    for (opd_blend_data& i : rob_itm_equip->opd_blend_data) {
        float_t* opd_data = 0;
        uint32_t opd_count = 0;
        osage_play_data_manager_get_opd_file_data(itm_eq_obj->obj_info, i.motion_id, &opd_data, &opd_count);
        if (!opd_data)
            break;

        for (ExOsageBlock*& j : itm_eq_obj->osage_blocks)
            opd_data = j->LoadOpdData(index, opd_data, opd_count);

        for (ExClothBlock*& j : itm_eq_obj->cloth_blocks)
            opd_data = j->LoadOpdData(index, opd_data, opd_count);
        index++;
    }
}

static void rob_chara_item_equip_ctrl_iterate_nodes(rob_chara_item_equip* rob_itm_equip, uint8_t iterations = 0) {
    for (int32_t i = ITEM_BODY; i < ITEM_MAX; ++i)
        rob_chara_item_equip_object_ctrl_iterate_nodes(&rob_itm_equip->item_equip_object[i], iterations);
}

static void rob_chara_item_equip_object_ctrl(rob_chara_item_equip_object* itm_eq_obj) {
    if (itm_eq_obj->osage_iterations > 0) {
        rob_chara_item_equip_object_ctrl_iterate_nodes(itm_eq_obj, itm_eq_obj->osage_iterations);
        itm_eq_obj->osage_iterations = 0;
    }

    for (ExNodeBlock*& i : itm_eq_obj->node_blocks)
        i->Field_10();

    if (itm_eq_obj->use_opd) {
        itm_eq_obj->use_opd = false;
        rob_chara_item_equip_object_load_opd_data(itm_eq_obj);
        for (ExNodeBlock*& i : itm_eq_obj->node_blocks)
            i->SetOsagePlayData();
    }
    else {
        if (itm_eq_obj->field_1B8 && itm_eq_obj->node_blocks.size())
            for (int32_t i = 0; i < 6; i++)
                for (ExNodeBlock*& j : itm_eq_obj->node_blocks)
                    j->Field_18(i, false);

        for (ExNodeBlock*& i : itm_eq_obj->node_blocks)
            i->Field_20();
    }

    for (ExNodeBlock*& i : itm_eq_obj->node_blocks)
        i->Field_58();
}

static void rob_chara_item_equip_ctrl(rob_chara_item_equip* rob_itm_equip) {
    if (!rob_itm_equip->disable_update)
        for (int32_t i = rob_itm_equip->first_item_equip_object; i < rob_itm_equip->max_item_equip_object; i++)
            rob_chara_item_equip_object_ctrl(&rob_itm_equip->item_equip_object[i]);
}

static float_t rob_chara_get_trans_scale(rob_chara* rob_chr, int32_t bone, vec3* trans) {
    if (bone > 26)
        return 0.0f;
    *trans = rob_chr->data.field_1E68.field_DF8[bone].trans;
    return rob_chr->data.field_1E68.field_DF8[bone].scale;
}

mat4* rob_chara_bone_data_get_mats_mat(rob_chara_bone_data* rob_bone_data, size_t index) {
    if (index < rob_bone_data->mats.size())
        return &rob_bone_data->mats[index];
    return 0;
}

static void sub_140509D30(rob_chara* rob_chr) {
    struc_195* v39 = rob_chr->data.field_1E68.field_DF8;
    struc_195* v40 = rob_chr->data.field_1E68.field_1230;
    struc_195* v41 = rob_chr->data.field_1E68.field_1668;
    mat4u* v42 = rob_chr->data.field_1E68.field_78;
    mat4u* v43 = rob_chr->data.field_1E68.field_738;

    const struc_218* v3 = rob_chr->chara_init_data->field_828;
    const struc_218* v6 = rob_chr->chara_init_data->field_830;
    mat4* m;
    mat4 mat;
    for (int32_t i = 0; i < 27; i++) {
        m = rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, v3->bone_index);
        mat4_translate_mult(m, v3->bone_offset.x, v3->bone_offset.y, v3->bone_offset.z, &mat);
        *v42 = mat;

        m = rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, v6->bone_index);
        mat4_translate_mult(m, v3->bone_offset.x, v3->bone_offset.y, v3->bone_offset.z, &mat);
        *v43 = mat;

        float_t chara_scale = rob_chr->data.adjust_data.scale;

        vec3 v23;
        vec3_mult_scalar(*(vec3*)&v42->row3, chara_scale, v23);
        vec3_add(v23, rob_chr->data.adjust_data.trans, v23);
        *(vec3*)&v42->row3 = v23;

        vec3 v29;
        vec3_mult_scalar(*(vec3*)&v43->row3, chara_scale, v23);
        vec3_add(v23, rob_chr->data.adjust_data.trans, v29);
        *(vec3*)&v43->row3 = v29;

        float_t v20 = v3->scale * chara_scale;
        v39->scale = v20;
        v39->field_24 = v20;
        v39->prev_trans = v39->trans;
        v39->trans = v23;

        float_t v19 = v6->scale * chara_scale;
        v40->scale = v19;
        v40->field_24 = v19;
        v40->prev_trans = v40->trans;
        v40->trans = v29;

        v41->scale = v19;
        v41->field_24 = v19;
        v41->prev_trans = v41->trans;
        v41->trans = v29;

        v39++;
        v40++;
        v41++;
        v42++;
        v43++;
        v3++;
        v6++;
    }
}

mat4* rob_chara::get_bone_data_mat(size_t index) {
    return rob_chara_bone_data_get_mat(bone_data, index);
}

float_t rob_chara::get_frame() {
    return rob_chara_bone_data_get_frame(bone_data);
}

float_t rob_chara::get_frame_count() {
    return rob_chara_bone_data_get_frame_count(bone_data);
}

static int16_t sub_14054FE90(rob_chara* rob_chr, bool a3) {
    int16_t v3 = rob_chr->data.miku_rot.rot_y_int16;
    if (!a3 || ~rob_chr->data.field_0 & 0x10) {
        int16_t v4 = rob_chr->data.field_8.field_B8.field_8;
        if (v4)
            return v3 + v4;
    }

    if (rob_chr->data.field_1588.field_0.field_20.field_0 & 0x100)
        return v3 + rob_chr->data.field_1588.field_0.field_274;
    return v3;
}

static motion_bone_index rob_motion_c_kata_bone_get(int32_t index) {
    if (index == 0)
        return MOTION_BONE_C_KATA_L;
    else if (index == 1)
        return MOTION_BONE_C_KATA_R;
    else
        return MOTION_BONE_NONE;
}

static void rob_chara_bone_data_set_motion_arm_length(rob_chara_bone_data* rob_bone_data,
    motion_bone_index motion_bone_index, float_t value) {
    motion_blend_mot* v3 = rob_bone_data->motion_loaded.front();
    v3->bone_data.bones.data()[motion_bone_index].arm_length = value;
}

static void rob_chara_bone_data_set_disable_eye_motion(rob_chara_bone_data* rob_bone_data, bool value) {
    rob_bone_data->disable_eye_motion = value;
}

static bool rob_osage_check_osage_setting_parts_bit(rob_osage* rob_osg, int32_t parts_bits) {
    if (rob_osg->osage_setting.parts == -1)
        return 0;
    return _bittest((const LONG*)&parts_bits, rob_osg->osage_setting.parts);
}

static void sub_1405F9510(ExOsageBlock* a1, bool a2) {
    a1->rob.field_1F0F = a2;
}

static void sub_1405F9520(rob_chara_item_equip_object* a1, int32_t parts_bits, bool a3) {
    for (ExOsageBlock*& i : a1->osage_blocks)
        if (rob_osage_check_osage_setting_parts_bit(&i->rob, parts_bits))
            sub_1405F9510(i, a3);
}

static void sub_140513EE0(rob_chara_item_equip* rob_itm_equip, int32_t a2, bool a3) {
    sub_1405F9520(&rob_itm_equip->item_equip_object[a2 == 8 ? ITEM_OUTER : ITEM_KAMI], 1 << a2, a3);
}

static void sub_140419D30(rob_chara_bone_data* rob_bone_data,
    float_t frame, float_t step, float_t frame_count) {
    mot_play_data* v5 = &rob_bone_data->motion_loaded.front()->mot_play_data;
    v5->frame_data.frame = frame;
    v5->frame_data.step_prev = v5->frame_data.step;
    v5->frame_data.step = step;
    v5->frame_data.frame_count = frame_count;
    v5->frame_data.last_frame = frame_count - 1.0f;
    v5->frame_data.field_14 = -1.0f;
    v5->frame_data.field_18 = -1;
    v5->frame_data.field_1C = -1;
    v5->frame_data.field_20 = 0.0f;
    v5->frame_data.field_24 = -1.0f;
    v5->frame_data.field_28 = 0;
    v5->frame_data.field_2C = -1;
    v5->field_30 = 0;
    v5->field_34 = 0;
    v5->field_38 = 0.0f;
    v5->field_40 = 0;
    v5->field_48 = 0;
}

static void sub_14041C650(rob_chara_bone_data* rob_bone_data, int32_t a2) {
    rob_bone_data->motion_loaded.front()->mot_play_data.frame_data.field_18 = a2;
}

static void sub_14041BB50(rob_chara_bone_data* rob_bone_data, int32_t a2) {
    rob_bone_data->motion_loaded.front()->mot_play_data.frame_data.field_1C = a2;
}

static void sub_14041C990(rob_chara_bone_data* rob_bone_data, float_t a2, int32_t a3, float_t a4) {
    motion_blend_mot* v4 = rob_bone_data->motion_loaded.front();
    v4->mot_play_data.frame_data.field_20 = a2;
    v4->mot_play_data.frame_data.field_24 = a4;
    v4->mot_play_data.frame_data.field_2C = a3;
}

static void sub_14041B9D0(rob_chara_bone_data* rob_bone_data) {
    struc_312* v1 = &rob_bone_data->field_958;
    v1->field_58 = false;
    v1->field_59 = false;
    v1->field_5C = vec3_null;
    v1->field_68 = vec3_null;
    v1->field_74 = vec3_null;
    v1->field_80 = vec3_null;
    v1->field_8C = 1.0f;
    v1->bones = 0;
    v1->step = 1.0f;
}

static void sub_14041D6F0(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308*  v3 = &rob_bone_data->motion_loaded.front()->field_4F8;
    v3->field_BD = v3->field_BC;
    v3->field_BC = a2;
}

static void sub_14041D720(rob_chara_bone_data* a1, bool a2) {
    a1->motion_loaded.front()->field_4F8.field_BD = a2;
}

static float_t sub_14054FDE0(rob_chara* rob_chr) {
    if (rob_chr->data.field_1588.field_0.field_2C0 >= 0.0f)
        return rob_chr->data.field_1588.field_0.field_2C0;
    else if (rob_chr->data.field_0 & 0x04)
        return 10.0;
    else if ((rob_chr->data.field_1588.field_0.field_10.field_0 & 0x4000)
        || (rob_chr->data.field_1588.field_0.field_20.field_8 & 0x08))
        return 5.0f;
    else if (rob_chr->data.motion.motion_id == rob_chr->data.motion.prev_motion_id)
        return 4.0f;
    else if (rob_chr->data.field_1588.field_0.field_20.field_0 & 0x200) {
        if (~rob_chr->data.field_0 & 0x80 || rob_chr->data.field_0 & 0x40)
            return 1.0f;
    }
    else {
        if (~rob_chr->data.field_1588.field_0.field_20.field_0 & 0x8000
            || ~rob_chr->data.field_1588.field_0.field_20.field_0 & 0x01)
            return 7.0f;
    }
    return 3.0f;
}

static float_t sub_14054FEE0(rob_chara* a1) {
    if (a1->data.field_1588.field_0.field_2C4 >= 0.0f)
        return a1->data.field_1588.field_0.field_2C4;
    else if (a1->data.field_1588.field_0.field_20.field_0 & 0x200)
        return 14.0f;
    else
        return 7.0f;
}

static void sub_14041D310(rob_chara_bone_data* rob_bone_data, float_t a2, float_t a3, int32_t a4) {
    motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
    if (mot->blend)
        mot->blend->Field_10(a2, a3, a4);
}

static void sub_14041C9D0(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.front()->field_4F8;
    v2->field_8 = (uint8_t)(v2->field_0 & 1);
    if (a2)
        v2->field_0 |= 0x01;
    else
        v2->field_0 &= ~0x01;
}

static void sub_14041C680(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.front()->field_4F8;
    if (a2)
        v2->field_0 |= 0x02;
    else
        v2->field_0 &= ~0x02;
}

static void sub_14041D2D0(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.front()->field_4F8;
    if (a2)
        v2->field_0 |= 0x04;
    else
        v2->field_0 &= ~0x04;
}

static void sub_14041BC40(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.front()->field_4F8;
    if (a2)
        v2->field_0 |= 0x08;
    else
        v2->field_0 &= ~0x08;
}

static void sub_14041D6C0(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.front()->field_4F8;
    if (a2)
        v2->field_0 |= 0x10;
    else
        v2->field_0 &= ~0x10;
}

static void sub_14041D340(rob_chara_bone_data* rob_bone_data, bool a2) {
    struc_308* v2 = &rob_bone_data->motion_loaded.front()->field_4F8;
    if (a2)
        v2->field_0 |= 0x20;
    else
        v2->field_0 &= ~0x20;
}

static void sub_140414F00(struc_308* a1, float_t a2) {
    a1->prev_eyes_adjust = a1->eyes_adjust;
    a1->eyes_adjust = a2;
}

static void sub_14041D270(rob_chara_bone_data* rob_bone_data, float_t a2) {
    sub_140414F00(&rob_bone_data->motion_loaded.front()->field_4F8, a2);
}

static void sub_14041D2A0(rob_chara_bone_data* rob_bone_data, float_t a2) {
    rob_bone_data->motion_loaded.front()->field_4F8.prev_eyes_adjust = a2;
}

static void sub_140555B00(rob_chara* rob_chr, bool a2) {
    bool v3;
    if (!a2)
        v3 = 0;
    else if (rob_chr->data.motion.prev_motion_id == -1)
        v3 = 0;
    else {
        v3 = ~rob_chr->data.field_1588.field_0.field_20.field_0 & 0x400
            || ~rob_chr->data.field_1588.field_0.field_10.field_0 & 0x40
            || rob_chr->data.field_1588.field_0.field_20.field_0 & 0x100000;
    }

    bool v6;
    int32_t v7;
    if (rob_chr->data.field_1588.field_0.field_52) {
        v6 = 0;
        v7 = 2;
    }
    else if (rob_chr->data.field_1588.field_0.field_20.field_0 & 0x8000) {
        v6 = 0;
        v7 = 2;
    }
    else {
        v6 = 1;
        v7 = 1;
    }

    if (rob_chr->data.field_1588.field_0.field_52 != 3 && rob_chr->data.field_8.field_1A4) {
        v6 = 0;
        v7 = 2;
    }

    float_t v8 = rob_chr->data.motion.frame_data.frame;
    float_t v9 = 0.0f;
    float_t v10 = 0.0f;
    if (v3)
        v10 = sub_14054FDE0(rob_chr);

    float_t v11 = v8 + v10;
    if (v6)
        v9 = sub_14054FEE0(rob_chr);

    float_t v12 = rob_chr->data.field_1588.field_0.field_8;
    float_t v13 = v12 - v9;
    if (v3 && v6 && (v13 < v11 || v13 <= v8)) {
        float_t v14 = rob_chr->data.field_1588.field_0.field_8;
        v14 = (v12 - v8) * 0.5f;
        int32_t v15 = (int32_t)v14;
        if (v14 != -0.0f && (float_t)v15 != v14)
            v14 = (float_t)(v14 < 0.0f ? v15 - 1 : v15);

        float_t v16 = v14 + v8;
        if (v11 > v16) {
            float_t v17;
            if (v13 < v16) {
                v13 = v16;
                v17 = v16;
                v9 = v12 - v16;
            }
            else
                v17 = rob_chr->data.field_1588.field_0.field_8 - v9;
            v10 = v17 - v8;
        }
        else {
            v13 = v8 + v10;
            v9 = v12 - v11;
        }
    }
    rob_chara_bone_data_set_motion_duration(rob_chr->bone_data, v9, 1.0f, 1.0f);

    sub_14041D310(rob_chr->bone_data, v9, v13, v7);
    sub_14041D340(rob_chr->bone_data, !!(rob_chr->data.field_1588.field_0.field_54 & 0x40));
    sub_14041D6C0(rob_chr->bone_data, (rob_chr->data.field_1588.field_0.field_40.field_0 & 0x10)
        && (rob_chr->data.field_1588.field_0.field_20.field_0 & 0x08));
    sub_14041D6F0(rob_chr->bone_data,
        !!(rob_chr->data.field_1588.field_0.field_10.field_8 & 0x18000000));
    sub_14041D720(rob_chr->bone_data,
        !!(rob_chr->data.field_1588.field_0.field_30.field_8 & 0x18000000));
    sub_14041D2A0(rob_chr->bone_data,
        (float_t)((double_t)rob_chr->data.field_1588.field_0.field_276 * M_PI * (1.0 / 32768.0)));
    sub_14041D270(rob_chr->bone_data,
        (float_t)((double_t)rob_chr->data.field_1588.field_0.field_274 * M_PI * (1.0 / 32768.0)));
}

static void mothead_apply2_inner(struc_380* a1, int32_t type, const mothead_data2* a3) {
    struc_223* v19 = a1->field_10;
    void* v35 = a3->data;
    switch (type) {
    case 0: {
        if (a1->field_10->field_0.field_58 != -1)
            break;

        uint32_t v8 = 0;
        if (!a3 || a3->type < 0)
            break;

        std::list<std::pair<void*, uint32_t>> v31;
        while (a3->type != 0)
            if ((a3++)->type < 0)
                return;

        if (!a3->data)
            break;

        do {
            if (a3->type == 0) {
                void* v10 = a3->data;
                if (!v10)
                    break;

                v8 += ((uint16_t*)v10)[11];
                v31.push_back({ v10, v8 });
            }
            a3++;
        } while (a3->type >= 0);

        if (!v8 || !v31.size())
            break;

        if (v31.size() > 1) {
            uint32_t v15 = rand_state_array_get_int(1) % v8;
            for (std::pair<void*, uint32_t>& i : v31)
                if (v15 < i.second) {
                    v35 = i.first;
                    break;
                }
        }

        v19->field_0.field_58 = ((int16_t*)v35)[0];
        v19->field_0.field_5C = ((int32_t*)v35)[1];
        v19->field_0.field_60 = ((int32_t*)v35)[2];
        v19->field_0.field_64 = ((int32_t*)v35)[3];
        v19->field_0.field_68 = ((int32_t*)v35)[4];
        v19->field_0.field_6C = ((int16_t*)v35)[5];
        v19->field_0.field_70 = (float_t)((int16_t*)v35)[12];
        v19->field_0.field_74 = (float_t)((int16_t*)v35)[13];
    } break;
    case 1: {
        v19->field_0.field_78 = (float_t)((int16_t*)v35)[0];
        if (((int16_t*)v35)[1] >= 0)
            v19->field_0.field_7C = (float_t)((int16_t*)v35)[1];
        if (((int16_t*)v35)[2] >= 0)
            v19->field_0.field_80 = (float_t)((int16_t*)v35)[2];
        v19->field_0.field_20.field_0 |= 2;
        a1->rob_chara_data->motion.field_24 = 0;
    } break;
    case 2: {
        v19->field_0.field_1E8 = ((int32_t*)v35)[0];
        v19->field_0.field_1EC = (float_t)((int16_t*)v35)[2];
        v19->field_0.field_1F0 = (float_t)((int16_t*)v35)[3];
        v19->field_0.field_1F4 = (float_t)((int16_t*)v35)[4];
        v19->field_0.field_1F8 = (float_t)((int16_t*)v35)[5];
        v19->field_0.field_1FC = ((float_t*)v35)[3];
        v19->field_0.field_200 += ((float_t*)v35)[4];
        a1->rob_chara_data->field_8.field_B8.field_10.y = -get_osage_gravity_const() * ((float_t*)v35)[4];
        if (v19->field_0.field_1F4 < v19->field_0.field_1F0)
            v19->field_0.field_1F4 = v19->field_0.field_1F0;
    } break;
    case 40: {
        v19->field_0.field_238 = ((int16_t*)v35)[0];
        if ((a1->rob_chara_data->motion.field_28 & 8) != 0)
            v19->field_0.field_23C = -((float_t*)v35)[1];
        else
            v19->field_0.field_23C = ((float_t*)v35)[1];
        v19->field_0.field_240 = ((int32_t*)v35)[2];
        a1->rob_chara_data->field_8.field_B8.field_10.y = -get_osage_gravity_const() * ((float_t*)v35)[3];
    } break;
    case 41: {
        if (v19->field_0.field_244 <= 0)
            v19->field_0.field_248 = a3;
        v19->field_0.field_244++;
    } break;
    case 46: {
        if (v19->field_0.field_10.field_0 & 0x100) {
            if (a1->rob_chara_data->motion.field_28 & 8)
                v19->field_0.field_274 = -((int16_t*)v35)[0];
            else
                v19->field_0.field_274 = ((int16_t*)v35)[0];
        }
    } break;
    case 51: {
        if (a1->rob_chara_data->motion.field_28 & 8)
            v19->field_0.field_2B8 = -((int16_t*)v35)[0];
        else
            v19->field_0.field_2B8 = ((int16_t*)v35)[0];
    } break;
    case 55: {
        v19->field_0.field_2BC = ((int32_t*)v35)[0];
        v19->field_0.field_2C0 = ((float_t*)v35)[1];
        v19->field_0.field_2C4 = ((float_t*)v35)[2];
    } break;
    case 64: {
        float_t v31 = ((float_t*)v35)[0];
        if (a1->rob_chara_data->motion.field_28 & 8)
            v31 = -v31;
        v19->field_0.field_318.x = v31;
        v19->field_0.field_318.y = ((float_t*)v35)[1];
        v19->field_0.field_318.z = ((float_t*)v35)[2];
    } break;
    case 66: {
        v19->field_0.field_324 = ((float_t*)v35)[0];
        if (v19->field_0.field_324 < 0.0f)
            v19->field_0.field_324 = -v19->field_0.field_324;
    } break;
    case 68: {
        if (((int32_t*)v35)[0] > 0)
            v19->field_0.iterations = ((int32_t*)v35)[0];
    } break;
    }
}

static void mothead_apply2(struc_223* a1, rob_chara* rob_chr, const mothead_data2* a3) {
    if (!a3 || !a3->data)
        return;

    struc_380 v5;
    v5.rob_chara = rob_chr;
    v5.rob_chara_data = &rob_chr->data;
    v5.field_10 = &rob_chr->data.field_1588;

    while (a3->data && a3->type >= 0) {
        mothead_apply2_inner(&v5, a3->type, a3);
        a3++;
    }
}

static void sub_14053A9C0(struc_223* a1, rob_chara* rob_chr,
    uint32_t motion_id, float_t frame_count, motion_database* mot_db) {
    a1->field_0.motion_id = motion_id;
    a1->field_0.field_78 = 0.0f;
    a1->field_0.frame_count = frame_count;
    a1->field_0.field_7C = frame_count - 1.0f;
    a1->field_0.field_80 = frame_count - 1.0f;
    a1->field_0.field_1F0 = frame_count - 1.0f;
    a1->field_0.field_1F4 = frame_count - 1.0f;

    const mothead_mot* v7 = mothead_storage_get_mot_by_motion_id(motion_id, mot_db);
    if (!v7)
        return;

    a1->field_330.field_0.field_0 = v7->data.data();
    a1->field_330.field_0.field_8 = v7->data.data();
    a1->field_7A0 = v7->field_28.data();
    a1->motion_set_id = mot_db->get_motion_set_id_by_motion_id(motion_id);
    a1->field_0.field_10 = v7->field_0;
    a1->field_0.field_20 = v7->field_0;
    a1->field_0.field_20.field_0 &= ~0x4000;
    if (a1->field_0.field_10.field_0 & 0x100)
        a1->field_0.field_274 = 0x8000;
    if (a1->field_0.field_10.field_0 & 0x200000)
        a1->field_0.field_2B8 = (int16_t)0x8000;

    a1->field_0.field_50 = v7->field_10;
    a1->field_0.field_52 = v7->field_12;
    if (v7->field_12 & 0x40) {
        a1->field_0.field_52 = v7->field_12 & ~0x40;
        a1->field_0.field_54 |= 0x40;
    }

    mothead_apply2(a1, rob_chr, v7->field_18.data());

    if (a1->field_0.field_58 == 1)
        a1->field_0.field_8 = frame_count - 1.0f;
    else
        a1->field_0.field_8 = a1->field_0.frame_count;

    if (a1->field_0.field_20.field_0 & 0x200000)
        a1->field_0.field_20.field_0 |= 0x100;
    if (a1->field_0.field_20.field_C & 0x20000 && ~rob_chr->data.field_3D9D & 0x04)
        a1->field_0.field_20.field_C |= 0x400;

    a1->field_0.field_C = 0;
    a1->field_0.field_274 += a1->field_0.field_2B8;
    a1->field_0.field_A0 += a1->field_0.field_2B8;

    const mothead_data* v11 = v7->data.data();
    if (!v11)
        return;

    mothead_data_type v12 = v11->type;
    if (v11->type < 0)
        return;

    while (v12 != MOTHEAD_DATA_TYPE_11)
        if ((v12 = (v11++)->type) < 0)
            return;

    void* d = v11->data;
    while (d) {
        a1->field_0.field_C += *(int16_t*)d;

        mothead_data_type v17 = (v11++)->type;
        if (v17 < 0)
            break;

        while (v17 != MOTHEAD_DATA_TYPE_11)
            if ((v17 = (v11++)->type) < 0)
                return;
        d = v11->data;
    }
}

static void sub_140539750(struc_223* a1);

void rob_chara::load_motion(int32_t motion_id, bool a3, float_t frame,
    MotionBlendType blend_type, bone_database* bone_data, motion_database* mot_db) {
    data.miku_rot.rot_y_int16 = sub_14054FE90(this, false);
    if (~data.field_1588.field_0.field_20.field_0 & 0x10) {
        data.miku_rot.position.x = data.miku_rot.field_24.x;
        data.miku_rot.position.z = data.miku_rot.field_24.z;
    }

    if (data.field_1588.field_0.field_20.field_8 & 0x8000000)
        data.miku_rot.position.y = data.miku_rot.field_24.y;

    if (a3)
        data.motion.field_28 ^= (data.motion.field_28 ^ ~data.motion.field_28) & 0x04;

    if (data.motion.field_28 & 0x04)
        data.motion.field_28 |= 0x08;
    else
        data.motion.field_28 &= ~0x08;

    data.motion.field_28 &= 0x2F;
    data.motion.field_28 |= 0x10;
    data.motion.field_2C = 0;
    data.motion.field_30 = 0;

    rob_chara_data_adjust* parts_adjust = data.motion.parts_adjust;
    for (int32_t i = 0; i < 13; i++, parts_adjust++)
        parts_adjust->reset();

    data.motion.adjust_global.reset();

    data.adjust_data.offset = vec3_null;
    data.adjust_data.offset_x = true;
    data.adjust_data.offset_y = false;
    data.adjust_data.offset_z = true;
    data.adjust_data.get_global_trans = false;

    rob_chara_data_arm_adjust* arm_adjust = data.motion.arm_adjust;
    rob_chara_data_hand_adjust* hand_adjust = data.motion.hand_adjust;
    rob_chara_data_hand_adjust* hand_adjust_prev = data.motion.hand_adjust_prev;
    for (int32_t i = 0; i < 2; i++, arm_adjust++, hand_adjust++, hand_adjust_prev++) {
        hand_adjust->reset();
        hand_adjust_prev->reset();
        arm_adjust->reset();
        rob_chara_bone_data_set_motion_arm_length(this->bone_data,
            rob_motion_c_kata_bone_get(i), 0.0f);
    }
    rob_chara_bone_data_set_disable_eye_motion(this->bone_data, false);

    rob_chara_item_equip_set_osage_move_cancel(item_equip, 0, 0.0f);
    for (int32_t i = 0; i < 13; i++)
        sub_140513EE0(item_equip, i, false);
    data.field_1588.field_0.field_30 = data.field_1588.field_0.field_10;
    data.field_1588.field_0.field_40 = data.field_1588.field_0.field_20;
    data.field_1588.field_0.field_276 = data.field_1588.field_0.field_274;
    sub_140539750(&data.field_1588);
    sub_14053A9C0(&data.field_1588, this, motion_id,
        motion_storage_get_mot_data_frame_count(motion_id, mot_db), mot_db);
    if (data.field_1588.field_0.field_40.field_C & 0x20)
        data.field_1588.field_0.field_20.field_0 &= ~0x08;
    if (data.field_1588.field_0.field_40.field_C & 0x40)
        data.field_1588.field_0.field_20.field_8 |= 0x10000000;
    if (~data.field_1588.field_0.field_20.field_0 & 0x10 && frame > 0.0f)
        data.field_1588.field_0.field_20.field_0 &= ~0x08;

    data.motion.prev_motion_id = data.motion.motion_id;
    data.motion.frame_data.frame = frame;
    data.motion.field_24 = 0;
    data.motion.motion_id = motion_id;
    if (data.motion.step_data.step < 0.0f)
        data.motion.step_data.frame = 1.0f;
    else
        data.motion.step_data.frame = data.motion.step_data.step;

    float_t step = data.motion.step;
    if (step < 0.0f)
        step = data.motion.step_data.frame;
    rob_chara_item_equip_set_osage_step(item_equip, step);
    rob_chara_bone_data_motion_load(this->bone_data, motion_id, blend_type, bone_data, mot_db);
    sub_140419D30(this->bone_data, data.motion.step_data.frame,
        data.motion.step_data.frame, data.field_1588.field_0.frame_count);
    sub_14041C650(this->bone_data, 1);
    if (data.field_1588.field_0.field_58 == 1) {
        sub_14041BB50(this->bone_data, 1);
        sub_14041C990(this->bone_data, data.field_1588.field_0.field_70,
            data.field_1588.field_0.field_6C, -1.0f);
    }
    rob_chara_bone_data_set_rotation_y(this->bone_data,
        (float_t)((double_t)data.field_1588.field_0.field_2B8 * M_PI * (1.0 / 32768.0)));
    sub_14041BC40(this->bone_data, !!(data.field_1588.field_0.field_20.field_8 & 0x20000000));
    sub_14041B9D0(this->bone_data);
    if (type == ROB_CHARA_TYPE_2) {
        sub_14041D340(this->bone_data, !!(data.field_1588.field_0.field_54 & 0x40));
        sub_14041D6C0(this->bone_data, (data.field_1588.field_0.field_40.field_0 & 0x10)
            && (data.field_1588.field_0.field_20.field_0 & 0x08));
        sub_14041D6F0(this->bone_data,
            !!(data.field_1588.field_0.field_10.field_8 & 0x18000000));
        sub_14041D720(this->bone_data,
            !!(data.field_1588.field_0.field_30.field_8 & 0x18000000));
        sub_14041D2A0(this->bone_data,
            (float_t)((double_t)data.field_1588.field_0.field_276 * M_PI * (1.0 / 32768.0)));
        sub_14041D270(this->bone_data,
            (float_t)((double_t)data.field_1588.field_0.field_274 * M_PI * (1.0 / 32768.0)));
    }
    else
        sub_140555B00(this, blend_type >= MOTION_BLEND_FREEZE && blend_type <= MOTION_BLEND_CROSS);
    data.field_3DE0 = 0;
}

static void sub_140514130(rob_chara_item_equip* rob_itm_equip, item_id id, bool a3) {
    if (a3)
        rob_itm_equip->field_18[id] &= ~0x01;
    else
        rob_itm_equip->field_18[id] |= 0x01;
}

static void sub_140514110(rob_chara_item_equip* rob_itm_equip, item_id id, bool a3) {
    if (a3)
        rob_itm_equip->field_18[id] &= ~0x02;
    else
        rob_itm_equip->field_18[id] |= 0x02;
}

static void sub_140513C60(rob_chara_item_equip* rob_itm_equip, item_id id, bool a3) {
    if (a3)
        rob_itm_equip->field_18[id] &= ~0x04;
    else
        rob_itm_equip->field_18[id] |= 0x04;
}

void rob_chara::reset_data(rob_chara_pv_data* pv_data,
    bone_database* bone_data, motion_database* mot_db) {
    this->bone_data->reset();
    rob_chara_bone_data_init_data(this->bone_data,
        BONE_DATABASE_SKELETON_COMMON, chara_init_data->skeleton_type, bone_data);
    data.reset();
    data_prev.reset();
    rob_chara_item_equip_get_parent_bone_nodes(item_equip,
        rob_chara_bone_data_get_node(this->bone_data, 0), bone_data);
    type = pv_data->type;
    rob_chara_data* chr_data = &data;
    chr_data->miku_rot.rot_y_int16 = pv_data->rot_y_int16;
    chr_data->miku_rot.field_6 = pv_data->rot_y_int16;
    chr_data->adjust_data.scale = chara_size_table_get_value(this->pv_data.chara_size_index);
    chr_data->adjust_data.height_adjust = this->pv_data.height_adjust;
    chr_data->adjust_data.pos_adjust_y = chara_pos_adjust_y_table_get_value(this->pv_data.chara_size_index);
    rob_chara_bone_data_eyes_xrot_adjust(this->bone_data, chara_some_data_array, &pv_data->eyes_adjust);
    rob_chara_bone_data_get_ik_scale(this->bone_data, bone_data);
    rob_chara_load_default_motion(this, bone_data, mot_db);
    chr_data->field_8.field_4.field_0 = 1;
    chr_data->field_8.field_4.motion_id = get_rob_cmn_mottbl_motion_id(0);
    chr_data->field_8.field_0 = 0;
    chr_data->field_8.field_4.field_10 = pv_data->field_5;
    load_motion(chr_data->field_8.field_4.motion_id,
        pv_data->field_5, 0.0f, MOTION_BLEND, bone_data, mot_db);
    set_parts_disp(ITEM_MAX, true);
    item_equip->field_DC = 0;
    sub_140513C60(item_equip, ITEM_ATAMA, false);
    sub_140513C60(item_equip, ITEM_TE_R, false);
    sub_140513C60(item_equip, ITEM_TE_L, false);
    object_info head_obj = rob_chara_get_head_object(this, 0);
    chr_data->motion.field_150.head_object = head_obj;
    chr_data->motion.field_3B0.head_object = head_obj;
    set_parts_disp(ITEM_MAX, true);
}

void rob_chara::set_bone_data_frame(float_t frame) {
    rob_chara_bone_data_set_frame(bone_data, frame);
    rob_chara_bone_data_interpolate(bone_data);
    rob_chara_bone_data_update(bone_data, 0);
}

void rob_chara::set_chara_height_adjust(bool value) {
    data.adjust_data.height_adjust = value;
}

void rob_chara::set_chara_pos_adjust(vec3* value) {
    data.adjust_data.pos_adjust = *value;
}

void rob_chara::set_chara_pos_adjust_y(float_t value) {
    data.adjust_data.pos_adjust_y = value;
}

void rob_chara::set_chara_size(float_t value) {
    data.adjust_data.scale = value;
}

static bool sub_14053F290(RobPartialMotion* a1);
static bool sub_14053F280(RobPartialMotion* a1);
static bool sub_14053F2A0(RobPartialMotion* a1);
static bool sub_14053F270(RobPartialMotion* a1);
static bool sub_14053F2B0(RobPartialMotion* a1);

static void rob_chara_set_eyelid_motion(rob_chara* rob_chr,
    RobEyelidMotion* motion, int32_t type, motion_database* mot_db) {
    if (type == 1 || type == 2) {
        rob_chr->data.motion.field_3B0.eyelid.data = motion->data;
        if (rob_chr->data.motion.field_29 & 0x80)
            return;
    }
    else {
        rob_chr->data.motion.field_150.eyelid.data = motion->data;
        if (rob_chr->data.motion.field_29 & 0x80)
            return;
    }

    rob_chara_bone_data_load_eyelid_motion(rob_chr->bone_data, motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_eyelid_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_eyelid_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_eyelid_anim_duration(rob_chr->bone_data,
        motion->data.duration, motion->data.step, motion->data.offset);
}

static void rob_chara_set_eyes_motion(rob_chara* rob_chr,
    RobEyesMotion* motion, int32_t type, motion_database* mot_db) {
    if (type == 1 || type == 2) {
        rob_chr->data.motion.field_3B0.eye.data = motion->data;
        if (~rob_chr->data.motion.field_29 & 0x40)
            return;
    }
    else {
        rob_chr->data.motion.field_150.eye.data = motion->data;
        if (rob_chr->data.motion.field_29 & 0x40)
            return;
    }

    rob_chara_bone_data_load_eyes_motion(rob_chr->bone_data, motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_eyes_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_eyes_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_eyes_anim_duration(rob_chr->bone_data,
        motion->data.duration, motion->data.step, motion->data.offset);
}

static void rob_chara_set_face_motion(rob_chara* rob_chr,
    RobFaceMotion* motion, int32_t type, motion_database* mot_db) {
    if (type == 2 || type == 1) {
        rob_chr->data.motion.field_3B0.face.data = motion->data;
        if (~rob_chr->data.motion.field_29 & 0x04)
            return;
    }
    else {
        rob_chr->data.motion.field_150.face.data = motion->data;
        if (rob_chr->data.motion.field_29 & 0x04)
            return;
    }

    rob_chara_bone_data_load_face_motion(rob_chr->bone_data, motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_face_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_face_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_face_anim_duration(rob_chr->bone_data,
        motion->data.duration, motion->data.step, motion->data.offset);
}

static void rob_chara_set_hand_l_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, motion_database* mot_db) {
    if (type == 2)
        rob_chr->data.motion.field_3B0.hand_l.data = motion->data;
    else if (type == 1)
        rob_chr->data.motion.field_150.hand_l.data = motion->data;
    else
        rob_chr->data.motion.hand_l.data = motion->data;

    if (~rob_chr->data.motion.field_29 & 0x08 || rob_chr->data.motion.field_2A & 0x04)
        return;

    rob_chara_bone_data_load_hand_l_motion(rob_chr->bone_data, motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_hand_l_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_hand_l_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_hand_l_anim_duration(rob_chr->bone_data,
        motion->data.duration, motion->data.step, motion->data.offset);
}

static void rob_chara_set_hand_r_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, motion_database* mot_db) {
    if (type == 2)
        rob_chr->data.motion.field_3B0.hand_r.data = motion->data;
    else if (type == 1)
        rob_chr->data.motion.field_150.hand_r.data = motion->data;
    else
        rob_chr->data.motion.hand_r.data = motion->data;

    if (~rob_chr->data.motion.field_29 & 0x10 || rob_chr->data.motion.field_2A & 0x08)
        return;

    rob_chara_bone_data_load_hand_r_motion(rob_chr->bone_data, motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_hand_r_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_hand_r_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_hand_r_anim_duration(rob_chr->bone_data,
        motion->data.duration, motion->data.step, motion->data.offset);
}

static void rob_chara_set_mouth_motion(rob_chara* rob_chr,
    RobMouthMotion* motion, int32_t type, motion_database* mot_db) {
    if (type == 1 || type == 2) {
        rob_chr->data.motion.field_3B0.mouth.data = motion->data;
        if (~rob_chr->data.motion.field_29 & 0x20)
            return;
    }
    else {
        rob_chr->data.motion.field_150.mouth.data = motion->data;
        if (rob_chr->data.motion.field_29 & 0x20)
            return;
    }

    rob_chara_bone_data_load_mouth_motion(rob_chr->bone_data, motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_mouth_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_mouth_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_mouth_anim_duration(rob_chr->bone_data,
        motion->data.duration, motion->data.step, motion->data.offset);
}

static void sub_1405500F0(rob_chara* rob_chr) {
    rob_chr->data.motion.field_29 &= ~0x01;
    int32_t v1 = rob_chr->data.motion.field_150.field_1C0;
    if (v1 == 2 || v1 == 3)
        return;

    switch (rob_chr->data.motion.field_150.face.data.mottbl_index) {
    case 0x07:
    case 0x09:
    case 0x0B:
    case 0x0D:
    case 0x13:
    case 0x15:
    case 0x19:
    case 0x1B:
    case 0x1D:
    case 0x1F:
    case 0x21:
    case 0x23:
    case 0x25:
    case 0x27:
    case 0x29:
    case 0x2B:
    case 0x2D:
    case 0x2F:
    case 0x31:
    case 0x33:
    case 0x35:
    case 0x37:
    case 0x41:
    case 0x43:
    case 0x45:
    case 0x47:
    case 0x49:
    case 0x4B:
    case 0x4D:
    case 0x4F:
    case 0x51:
    case 0x53:
    case 0x55:
    case 0x57:
    case 0x59:
    case 0x5B:
    case 0x5D:
    case 0x5F:
    case 0x61:
    case 0x63:
    case 0x65:
    case 0x67:
    case 0x69:
    case 0x6B:
        rob_chr->data.motion.field_29 |= 0x01;
        break;
    }
}

void rob_chara::set_eyelid_mottbl_motion_from_face(int32_t a2,
    float_t duration, float_t value, float_t offset, motion_database* mot_db) {
    if (!a2)
        a2 = data.motion.field_150.field_1C0;

    RobEyelidMotion v13;
    switch (a2) {
    case 0:
    case 2:
        v13.data = data.motion.field_150.face.data;
        break;
    case 1:
        value = 1.0f;
    case 3: {
        int32_t mottbl_index = data.motion.field_150.face.data.mottbl_index;
        switch (mottbl_index) {
        case 0x07:
        case 0x09:
        case 0x0B:
        case 0x0D:
        case 0x0F:
        case 0x11:
        case 0x13:
        case 0x15:
        case 0x17:
        case 0x19:
        case 0x1B:
        case 0x1D:
        case 0x1F:
        case 0x21:
        case 0x23:
        case 0x25:
        case 0x27:
        case 0x29:
        case 0x2B:
        case 0x2D:
        case 0x2F:
        case 0x31:
        case 0x33:
        case 0x35:
        case 0x37:
        case 0x3D:
        case 0x3F:
        case 0x41:
        case 0x43:
        case 0x45:
        case 0x47:
        case 0x49:
        case 0x4B:
        case 0x4D:
        case 0x4F:
        case 0x51:
        case 0x53:
        case 0x55:
        case 0x57:
        case 0x59:
        case 0x5B:
        case 0x5D:
        case 0x5F:
        case 0x61:
        case 0x63:
        case 0x65:
        case 0x67:
        case 0x69:
        case 0x6B:
        case 0x6D:
        case 0x6F:
        case 0x71:
        case 0x73:
        case 0x7F:
        case 0x81:
        case 0xEC:
        case 0xEE:
        case 0xF0:
        case 0xF2:
            v13.data.mottbl_index = ++mottbl_index;
            break;
        }

        int32_t motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
        v13.data.motion_id = motion_id;
        if (motion_id == -1) {
            duration = 0.0f;
            a2 = 0;
            v13.data = data.motion.field_150.face.data;
        }
        else if (value <= 0.0f) {
            a2 = 0;
            v13.data = data.motion.field_150.face.data;
        }
        else {
            v13.data.state = 0;
            v13.data.frame = (v13.data.frame_count - 1.0f) * value;
            v13.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        }
    } break;
    }
    v13.data.offset = offset;
    v13.data.duration = duration;

    data.motion.field_150.field_1C0 = a2;
    sub_1405500F0(this);
    data.motion.field_29 &= ~0x02;
    data.motion.field_150.time = 0.0f;
    rob_chara_set_eyelid_motion(this, &v13, 0, mot_db);
}

void rob_chara::set_eyelid_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t duration,
    float_t a7, float_t step, int32_t a9, float_t offset, motion_database* mot_db) {
    RobEyelidMotion v13;
    v13.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    v13.data.mottbl_index = mottbl_index;
    v13.data.state = state;
    v13.data.play_frame_step = step;
    v13.data.duration = duration;
    v13.data.offset = offset;
    v13.data.field_24 = a7;
    if (sub_14053F290(&v13)) {
        v13.data.frame_data = &data.motion.frame_data;
        v13.data.step_data = &data.motion.step_data;
    }
    v13.data.field_38 = a9;
    if (v13.data.motion_id != -1) {
        v13.data.frame_count = motion_storage_get_mot_data_frame_count(v13.data.motion_id, mot_db);
        v13.data.frame = (v13.data.frame_count - 1.0f) * value;
    }

    if (!type) {
        data.motion.field_29 &= ~2;
        data.motion.field_150.time = 0.0f;
    }
    rob_chara_set_eyelid_motion(this, &v13, type, mot_db);
}

void rob_chara::set_eyes_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t duration,
    float_t a7, float_t step, int32_t a9, float_t offset, motion_database* mot_db) {
    RobEyesMotion v13;
    v13.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    v13.data.mottbl_index = mottbl_index;
    v13.data.state = state;
    v13.data.play_frame_step = step;
    v13.data.duration = duration;
    v13.data.offset = offset;
    v13.data.field_24 = a7;
    if (sub_14053F290(&v13)) {
        v13.data.frame_data = &data.motion.frame_data;
        v13.data.step_data = &data.motion.step_data;
    }
    v13.data.field_38 = a9;
    if (v13.data.motion_id != -1) {
        v13.data.frame_count = motion_storage_get_mot_data_frame_count(v13.data.motion_id, mot_db);
        v13.data.frame = (v13.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_eyes_motion(this, &v13, type, mot_db);
}

static void sub_140553970(rob_chara* rob_chr, object_info a2, int32_t type) {
    if (type == 1)
        rob_chr->data.motion.field_150.head_object = a2;
    else
        rob_chr->data.motion.field_3B0.head_object = a2;
}

void rob_chara::set_face_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t duration,
    float_t a7, float_t step, int32_t a9, float_t offset, bool a11, motion_database* mot_db) {
    RobFaceMotion v27;
    v27.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    v27.data.mottbl_index = mottbl_index;
    v27.data.state = state;
    v27.data.play_frame_step = step;
    v27.data.duration = duration;
    v27.data.offset = offset;
    v27.data.field_24 = a7;
    if (sub_14053F290(&v27)) {
        v27.data.frame_data = &data.motion.frame_data;
        v27.data.step_data = &data.motion.step_data;
    }
    v27.data.field_38 = a9;
    if (v27.data.motion_id != -1) {
        v27.data.frame_count = motion_storage_get_mot_data_frame_count(v27.data.motion_id, mot_db);
        v27.data.frame = (v27.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_face_motion(this, &v27, type, mot_db);

    int32_t head_object_id = 0;
    switch (mottbl_index) {
    case 57:
        head_object_id = 1;
        break;
    case 59:
        head_object_id = 7;
        break;
    case 117:
        head_object_id = 2;
        break;
    case 119:
        head_object_id = 3;
        break;
    case 121:
        head_object_id = 4;
        break;
    case 123:
        head_object_id = 5;
        break;
    case 125:
        head_object_id = 6;
        break;
    }

    object_info v17 = rob_chara_get_head_object(this, head_object_id);
    if (head_object_id) {
        sub_140553970(this, v17, type);
        duration = 0.0f;
    }
    else if (type == 2) {
        if (rob_chara_get_object_info(this, ITEM_ATAMA) != v17) {
            sub_140553970(this, v17, 2);
            duration = 0.0f;
        }

    }
    else if (data.motion.field_150.head_object != v17) {
        sub_140553970(this, v17, type);
        data.motion.field_150.eye.data.duration = 0.0f;
        rob_chara_set_eyes_motion(this, &data.motion.field_150.eye, 0, mot_db);
        duration = 0.0f;
    }

    if (type == 1 || type == 2) {
        if (a11)
            set_eyelid_mottbl_motion_from_face(2, duration, -1.0f, offset, mot_db);
        else if (data.motion.field_29 & 0x02) {
            set_eyelid_mottbl_motion_from_face(
                data.motion.field_150.field_1C0, 3.0f, -1.0f, 0.0f, mot_db);
            data.motion.field_29 |= 0x02;
        }
        else {
            float_t v21 = -1.0f;
            if (data.motion.field_150.field_1C0 == 3) {
                float_t v23 = data.motion.field_150.eyelid.data.frame_count - 1.0f;
                if (v23 > 0.0f)
                    v21 = data.motion.field_150.eyelid.data.frame / v23;

                float_t v25 = bone_data->eyelid.blend.duration;
                float_t v26 = bone_data->eyelid.blend.frame;
                if (v25 > v26) {
                    duration = v25 - v26;
                    offset = 1.0f;
                }
            }
            set_eyelid_mottbl_motion_from_face(0, duration, v21, offset, mot_db);
        }
    }
    else {
        RobEyelidMotion v28;
        v28.data = v27.data;
        v28.data.duration = duration;
        v28.data.offset = offset;
        data.motion.field_29 |= 0x80;
        rob_chara_set_eyelid_motion(this, &v28, 2, mot_db);
    }
}

void rob_chara::set_frame(float_t frame) {
    rob_chara_bone_data_set_frame(bone_data, frame);
    rob_chara_bone_data_interpolate(bone_data);
    rob_chara_bone_data_update(bone_data, 0);
    sub_140509D30(this);
}

void rob_chara::set_hand_l_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t duration,
    float_t a7, float_t step, int32_t a9, float_t offset, motion_database* mot_db) {
    RobHandMotion v13;
    v13.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    v13.data.mottbl_index = mottbl_index;
    v13.data.state = state;
    v13.data.play_frame_step = step;
    v13.data.duration = duration;
    v13.data.offset = offset;
    v13.data.field_24 = a7;
    if (sub_14053F290(&v13)) {
        v13.data.frame_data = &data.motion.frame_data;
        v13.data.step_data = &data.motion.step_data;
    }
    v13.data.field_38 = a9;
    if (v13.data.motion_id != -1) {
        v13.data.frame_count = motion_storage_get_mot_data_frame_count(v13.data.motion_id, mot_db);
        v13.data.frame = (v13.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_hand_l_motion(this, &v13, type, mot_db);
}

void rob_chara::set_hand_r_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t duration,
    float_t a7, float_t step, int32_t a9, float_t offset, motion_database* mot_db) {
    RobHandMotion v13;
    v13.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    v13.data.mottbl_index = mottbl_index;
    v13.data.state = state;
    v13.data.play_frame_step = step;
    v13.data.duration = duration;
    v13.data.offset = offset;
    v13.data.field_24 = a7;
    if (sub_14053F290(&v13)) {
        v13.data.frame_data = &data.motion.frame_data;
        v13.data.step_data = &data.motion.step_data;
    }
    v13.data.field_38 = a9;
    if (v13.data.motion_id != -1) {
        v13.data.frame_count = motion_storage_get_mot_data_frame_count(v13.data.motion_id, mot_db);
        v13.data.frame = (v13.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_hand_r_motion(this, &v13, type, mot_db);
}

bool rob_chara::set_motion_id(int32_t motion_id,
    float_t frame, float_t duration, bool a5, bool set_motion_reset_data,
    MotionBlendType blend_type, bone_database* bone_data, motion_database* mot_db) {
    if (!a5 && data.motion.motion_id == motion_id)
        return false;

    if (duration <= 0.0f)
        blend_type = MOTION_BLEND;
    load_motion(motion_id, 0, frame, blend_type, bone_data, mot_db);
    rob_chara_bone_data_set_motion_duration(this->bone_data, duration, 1.0f, 0.0f);
    data.motion.field_28 |= 0x80;
    data.motion.frame_data.last_set_frame = frame;
    set_motion_skin_param(motion_id, (float_t)(int32_t)frame);
    if (duration == 0.0f) {
        if (set_motion_reset_data)
            this->set_motion_reset_data(motion_id, frame);

        item_equip->item_equip_object[ITEM_TE_L].osage_iterations = 60;
        item_equip->item_equip_object[ITEM_TE_R].osage_iterations = 60;

        /*if (rob_chara_check_for_ageageagain_module(chara_index, module_index)) {
            sub_1405430F0(rob_chr->chara_id, 1);
            sub_1405430F0(rob_chr->chara_id, 2);
        }*/
    }
    return true;
}

void rob_chara::set_motion_reset_data(int32_t motion_id, float_t frame) {
    rob_chara_item_equip_set_motion_reset_data(item_equip, motion_id, frame);
}

void rob_chara::set_motion_skin_param(int32_t motion_id, float_t frame) {
    rob_chara_item_equip_set_motion_skin_param(item_equip,
        chara_id, motion_id, (int32_t)roundf(frame));
}

void rob_chara::set_mouth_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t duration,
    float_t a7, float_t step, int32_t a9, float_t offset, motion_database* mot_db) {
    RobMouthMotion v13;
    v13.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    v13.data.mottbl_index = mottbl_index;
    v13.data.state = state;
    v13.data.play_frame_step = step;
    v13.data.duration = duration;
    v13.data.offset = offset;
    v13.data.field_24 = a7;
    if (sub_14053F290(&v13)) {
        v13.data.frame_data = &data.motion.frame_data;
        v13.data.step_data = &data.motion.step_data;
    }
    v13.data.field_38 = a9;
    if (v13.data.motion_id != -1) {
        v13.data.frame_count = motion_storage_get_mot_data_frame_count(v13.data.motion_id, mot_db);
        v13.data.frame = (v13.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_mouth_motion(this, &v13, type, mot_db);
}

void rob_chara::set_osage_move_cancel(uint8_t id, float_t value) {
    rob_chara_item_equip_set_osage_move_cancel(item_equip, id, value);
    /*if (id <= 1) {
        sub_1405436A0(chara_id, 1, value);
        sub_1405436A0(chara_id, 2, value);
    }*/
}

void rob_chara::set_osage_reset() {
    rob_chara_item_equip_set_osage_reset(item_equip);
}

void rob_chara::set_osage_step(float_t value) {
    data.motion.step = value;
    if (value < 0.0f)
        value = data.motion.step_data.frame;
    rob_chara_item_equip_set_osage_step(item_equip, value);
}

void rob_chara::set_parts_disp(item_id id, bool disp) {
    if (id < ITEM_BODY)
        return;
    else if (id < ITEM_ITEM16)
        item_equip->item_equip_object[id].disp = disp;
    else if (id == ITEM_MAX)
        for (int32_t i = ITEM_ATAMA; i <= ITEM_ITEM16; i++) {
            item_equip->item_equip_object[i].disp = disp;
            /*if (index == ITEM_ATAMA
                && rob_chara_check_for_ageageagain_module(chara_index, module_index)) {
                sub_140543780(chara_id, 1, disp);
                sub_140543780(chara_id, 2, disp);
                sub_1405430F0(chara_id, 1);
                sub_1405430F0(chara_id, 2);
            }*/
        }
}

void rob_chara::set_visibility(bool value) {
    data.field_0 &= ~0x01;
    data.field_3 &= ~0x01;
    data.field_0 |= value ? 0x01 : 0x00;
    data.field_3 |= value ? 0x01 : 0x00;
    /*if (rob_chara_check_for_ageageagain_module(chara_index, module_index)) {
        sub_140543780(chara_id, 1, value);
        sub_140543780(chara_id, 2, value);
    }*/
}

static float_t bone_data_limit_angle(float_t angle) {
    bool neg;
    if (angle < 0.0f) {
        angle = -angle;
        neg = true;
    }
    else
        neg = false;

    angle = fmodf(angle + (float_t)M_PI, (float_t)(M_PI * 2.0)) - (float_t)M_PI;
    if (neg)
        return -angle;
    else
        return angle;
}

static void bone_data_mult_0(bone_data* a1, int32_t skeleton_select) {
    if (a1->flags != 0)
        return;

    mat4 mat;
    if (a1->has_parent)
        mat = *a1->parent_mat;
    else
        mat = mat4_identity;

    if (a1->type == BONE_DATABASE_BONE_POSITION) {
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        a1->rot_mat[0] = mat4u_identity;
    }
    else if (a1->type == BONE_DATABASE_BONE_TYPE_1) {
        mat4_mult_vec3_inv_trans(&mat, &a1->trans, &a1->trans);
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        a1->rot_mat[0] = mat4u_identity;
    }
    else {
        mat4 rot_mat;
        if (a1->type == BONE_DATABASE_BONE_POSITION_ROTATION) {
            rot_mat = a1->rot_mat[0];
            mat4_rotate_z_mult(&rot_mat, a1->rotation.z, &rot_mat);
        }
        else {
            a1->trans = a1->base_translation[skeleton_select];
            mat4_rotate_z(a1->rotation.z, &rot_mat);
        }

        mat4_rotate_y_mult(&rot_mat, a1->rotation.y, &rot_mat);

        if (a1->motion_bone_index == MOTION_BONE_KL_EYE_L
            || a1->motion_bone_index == MOTION_BONE_KL_EYE_R) {
            if (a1->rotation.x > 0.0f)
                a1->rotation.x *= a1->eyes_xrot_adjust_pos;
            else if (a1->rotation.x < 0.0f)
                a1->rotation.x *= a1->eyes_xrot_adjust_neg;
        }

        mat4_rotate_x_mult(&rot_mat, a1->rotation.x, &rot_mat);
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        mat4_mult(&rot_mat, &mat, &mat);
        a1->rot_mat[0] = rot_mat;
    }

    *a1->node[0].mat = mat;

    bone_data_mult_ik(a1, skeleton_select);
}

static void bone_data_mult_1(bone_data* a1, mat4* parent_mat, bone_data* a3, bool solve_ik) {
    mat4 mat;
    mat4 rot_mat;
    if (a1->has_parent)
        mat = *a1->parent_mat;
    else
        mat = *parent_mat;

    if (a1->type != BONE_DATABASE_BONE_TYPE_1 && a1->type != BONE_DATABASE_BONE_POSITION) {
        if (a1->type != BONE_DATABASE_BONE_POSITION_ROTATION)
            a1->trans = a1->base_translation[1];

        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        if (solve_ik) {
            a1->node[0].exp_data.rotation = vec3_null;
            if (!a1->flags) {
                rot_mat = a1->rot_mat[0];
                mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
            }
            else if (bone_data_mult_1_exp_data(a1, &a1->node[0].exp_data, a3)) {
                vec3 rotation = a1->node[0].exp_data.rotation;
                mat4_rotate(rotation.x, rotation.y, rotation.z, &rot_mat);
                a1->rot_mat[0] = rot_mat;
            }
            else {
                *a1->node[0].mat = mat;

                if (bone_data_mult_1_ik(a1, a3)) {
                    mat4_invrot_normalized(&mat, &rot_mat);
                    mat4_mult(a1->node[0].mat, &rot_mat, &rot_mat);
                    mat4_clear_trans(&rot_mat, &rot_mat);
                    mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
                    a1->rot_mat[0] = rot_mat;
                }
                else
                    a1->rot_mat[0] = mat4u_identity;
            }
        }

        rot_mat = a1->rot_mat[0];
        mat4_mult(&rot_mat, &mat, &mat);
    }
    else {
        mat4_translate_mult(&mat, a1->trans.x, a1->trans.y, a1->trans.z, &mat);
        if (solve_ik)
            a1->node[0].exp_data.rotation = vec3_null;
    }

    *a1->node[0].mat = mat;
    if (solve_ik) {
        a1->node[0].exp_data.position = a1->trans;
        bone_node_expression_data_reset_scale(&a1->node[0].exp_data);
    }

    if (a1->type < BONE_DATABASE_BONE_HEAD_IK_ROTATION)
        return;

    rot_mat = a1->rot_mat[1];
    mat4_mult(&rot_mat, &mat, &mat);
    *a1->node[1].mat = mat;
    mat4_translate_mult(&mat, a1->ik_segment_length[1], 0.0f, 0.0f, &mat);

    if (a1->type == BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        *a1->node[2].mat = mat;
        if (!solve_ik)
            return;

        a1->node[1].exp_data.position = vec3_null;
        rot_mat = a1->rot_mat[1];
        mat4_get_rotation(&rot_mat, &a1->node[1].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[1].exp_data);
        bone_node_expression_data_set_position_rotation(&a1->node[2].exp_data,
            a1->ik_segment_length[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    else {
        rot_mat = a1->rot_mat[2];
        mat4_mult(&rot_mat, &mat, &mat);
        *a1->node[2].mat = mat;
        mat4_translate_mult(&mat, a1->ik_2nd_segment_length[1], 0.0f, 0.0f, &mat);
        *a1->node[3].mat = mat;
        if (!solve_ik)
            return;

        a1->node[1].exp_data.position = vec3_null;
        rot_mat = a1->rot_mat[1];
        mat4_get_rotation(&rot_mat, &a1->node[1].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[1].exp_data);
        a1->node[2].exp_data.position.x = a1->ik_segment_length[1];
        *(vec2*)&a1->node[2].exp_data.position.y = vec2_null;
        rot_mat = a1->rot_mat[2];
        mat4_get_rotation(&rot_mat, &a1->node[2].exp_data.rotation);
        bone_node_expression_data_reset_scale(&a1->node[2].exp_data);
        bone_node_expression_data_set_position_rotation(&a1->node[3].exp_data,
            a1->ik_2nd_segment_length[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}

static bool bone_data_mult_1_ik(bone_data* a1, bone_data* a2) {
    bone_node* v5;
    vec3 v30;

    switch (a1->motion_bone_index) {
    case MOTION_BONE_N_SKATA_L_WJ_CD_EX:
        v5 = a2[MOTION_BONE_C_KATA_L].node;
        mat4_get_translation(v5[2].mat, &v30);
        bone_data_mult_1_ik_hands(a1, &v30);
        break;
    case MOTION_BONE_N_SKATA_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_C_KATA_R].node;
        mat4_get_translation(v5[2].mat, &v30);
        bone_data_mult_1_ik_hands(a1, &v30);
        break;
    case MOTION_BONE_N_SKATA_B_L_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_L_EX].node;
        mat4_get_translation(v5[0].mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_B_R_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_R_EX].node;
        mat4_get_translation(v5[0].mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_C_L_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_L_EX].node;
        mat4_get_translation(v5[0].mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.5f);
        break;
    case MOTION_BONE_N_SKATA_C_R_WJ_CD_CU_EX:
        v5 = a2[MOTION_BONE_N_UP_KATA_R_EX].node;
        mat4_get_translation(v5[0].mat, &v30);
        bone_data_mult_1_ik_hands_2(a1, &v30, 0.5f);
        break;
    case MOTION_BONE_N_MOMO_A_L_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_L].node;
        mat4_get_translation(v5[2].mat, &v30);
        mat4_mult_vec3_inv_trans(a1->node[0].mat, &v30, &v30);
        vec3_negate(v30, v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    case MOTION_BONE_N_MOMO_A_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_R].node;
        mat4_get_translation(v5[2].mat, &v30);
        mat4_mult_vec3_inv_trans(a1->node[0].mat, &v30, &v30);
        vec3_negate(v30, v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    case MOTION_BONE_N_HARA_CD_EX:
        v5 = a2[MOTION_BONE_KL_MUNE_B_WJ].node;
        mat4_get_translation(v5[0].mat, &v30);
        mat4_mult_vec3_inv_trans(a1->node[0].mat, &v30, &v30);
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    default:
        return false;
    }
    return true;
}

static void bone_data_mult_1_ik_hands(bone_data* a1, vec3* a2) {
    vec3 v15;
    mat4_mult_vec3_inv_trans(a1->node[0].mat, a2, &v15);

    float_t len;
    vec3_length_squared(v15, len);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vec3_mult_scalar(v15, 1.0f / len, v15);

    vec3 v17;
    v17.x = -v15.z * v15.x - v15.z;
    v17.y = -v15.y * v15.z;
    v17.z = v15.x * v15.x + v15.y * v15.y + v15.x;
    vec3_length_squared(v17, len);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vec3_mult_scalar(v17, 1.0f / len, v17);

    vec3 v16;
    vec3_cross(v17, v15, v16);

    mat4 rot_mat = mat4_identity;
    *(vec3*)&rot_mat.row0 = v15;
    *(vec3*)&rot_mat.row1 = v16;
    *(vec3*)&rot_mat.row2 = v17;

    mat4_mult(&rot_mat, a1->node[0].mat, a1->node[0].mat);
}

static void bone_data_mult_1_ik_hands_2(bone_data* a1, vec3* a2, float_t a3) {
    float_t len;
    vec3 v8;

    mat4_mult_vec3_inv_trans(a1->node[0].mat, a2, &v8);
    v8.x = 0.0f;
    vec3_length(v8, len);
    if (len <= 0.000001f)
        return;

    vec3_mult_scalar(v8, 1.0f / len, v8);
    float_t angle = atan2f(v8.z, v8.y);
    mat4_rotate_x_mult(a1->node[0].mat, angle * a3, a1->node[0].mat);
}

static void bone_data_mult_1_ik_legs(bone_data* a1, vec3* a2) {
    vec3 v9 = *a2;

    float_t len;
    vec3_length_squared(v9, len);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vec3_mult_scalar(v9, 1.0f / len, v9);

    vec3 v8;
    v8.x = -v9.z * v9.x;
    v8.y = -v9.z * v9.y - v9.z;
    v8.z = v9.y * v9.y - v9.x * -v9.x + v9.y;

    vec3_length_squared(v8, len);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vec3_mult_scalar(v8, 1.0f / len, v8);

    vec3 v10;
    vec3_cross(v9, v8, v10);

    mat4 rot_mat = mat4_identity;
    *(vec3*)&rot_mat.row0 = v10;
    *(vec3*)&rot_mat.row1 = v9;
    *(vec3*)&rot_mat.row2 = v8;

    mat4_mult(&rot_mat, a1->node[0].mat, a1->node[0].mat);
}

static bool bone_data_mult_1_exp_data(bone_data* a1, bone_node_expression_data* exp_data, bone_data* a3) {
    float_t v10;
    bone_node* v11;
    vec3 v15;
    mat4 dst;
    mat4 mat;

    bool ret = true;
    switch (a1->motion_bone_index) {
    case MOTION_BONE_N_EYE_L_WJ_EX:
        exp_data->rotation.x = -a3[MOTION_BONE_KL_EYE_L].node[0].exp_data.rotation.x;
        exp_data->rotation.y = a3[MOTION_BONE_KL_EYE_L].node[0].exp_data.rotation.y * (float_t)(-1.0 / 2.0);
        break;
    case MOTION_BONE_N_EYE_R_WJ_EX:
        exp_data->rotation.x = -a3[MOTION_BONE_KL_EYE_R].node[0].exp_data.rotation.x;
        exp_data->rotation.y = a3[MOTION_BONE_KL_EYE_R].node[0].exp_data.rotation.y * (float_t)(-1.0 / 2.0);
        break;
    case MOTION_BONE_N_KUBI_WJ_EX:
        dst = a3[MOTION_BONE_CL_KAO].rot_mat[0];
        mat = a3[MOTION_BONE_CL_KAO].rot_mat[1];
        mat4_mult(&mat, &dst, &dst);
        mat4_get_rotation(&dst, &v15);
        v10 = v15.z;
        if (v10 < (float_t)-M_PI_2)
            v10 += (float_t)(M_PI * 2.0);
        exp_data->rotation.y = (v10 - (float_t)M_PI_2) * 0.2f;
        break;
    case MOTION_BONE_N_HITO_L_EX:
    case MOTION_BONE_N_HITO_R_EX:
        exp_data->rotation.z = (float_t)(-9.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_KO_L_EX:
        exp_data->rotation.x = (float_t)(-8.0 * DEG_TO_RAD);
        exp_data->rotation.z = (float_t)(16.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_KUSU_L_EX:
    case MOTION_BONE_N_KUSU_R_EX:
        exp_data->rotation.z = (float_t)(9.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_NAKA_L_EX:
    case MOTION_BONE_N_NAKA_R_EX:
        exp_data->rotation.z = (float_t)(-1.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_OYA_L_EX:
        exp_data->rotation.x = (float_t)(75.0 * DEG_TO_RAD);
        exp_data->rotation.y = (float_t)(12.0 * DEG_TO_RAD);
        exp_data->rotation.z = (float_t)(-24.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_STE_L_WJ_EX:
        exp_data->rotation.x = a3[MOTION_BONE_KL_TE_L_WJ].node[0].exp_data.rotation.x;
        break;
    case MOTION_BONE_N_SUDE_L_WJ_EX:
    case MOTION_BONE_N_SUDE_B_L_WJ_EX:
        v11 = &a3[MOTION_BONE_KL_TE_L_WJ].node[0];
        exp_data->rotation.x = bone_data_limit_angle(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_SUDE_R_WJ_EX:
    case MOTION_BONE_N_SUDE_B_R_WJ_EX:
        v11 = &a3[MOTION_BONE_KL_TE_R_WJ].node[0];
        exp_data->rotation.x = bone_data_limit_angle(v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_HIJI_L_WJ_EX:
        exp_data->rotation.z = a3[MOTION_BONE_C_KATA_L].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_UP_KATA_L_EX:
    case MOTION_BONE_N_UP_KATA_R_EX:
        break;
    case MOTION_BONE_N_KO_R_EX:
        exp_data->rotation.x = (float_t)(8.0 * DEG_TO_RAD);
        exp_data->rotation.z = (float_t)(16.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_OYA_R_EX:
        exp_data->rotation.x = (float_t)(-75.0 * DEG_TO_RAD);
        exp_data->rotation.y = (float_t)(-12.0 * DEG_TO_RAD);
        exp_data->rotation.z = (float_t)(-24.0 * DEG_TO_RAD);
        break;
    case MOTION_BONE_N_STE_R_WJ_EX:
        exp_data->rotation.x = a3[MOTION_BONE_KL_TE_R_WJ].node[0].exp_data.rotation.x;
        break;
    case MOTION_BONE_N_HIJI_R_WJ_EX:
        exp_data->rotation.z = a3[MOTION_BONE_C_KATA_R].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_HIZA_L_WJ_EX:
        exp_data->rotation.z = a3[MOTION_BONE_CL_MOMO_L].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_HIZA_R_WJ_EX:
        exp_data->rotation.z = a3[MOTION_BONE_CL_MOMO_R].node[2].exp_data.rotation.z * (float_t)(1.0 / 2.0);
        break;
    case MOTION_BONE_N_MOMO_B_L_WJ_EX:
    case MOTION_BONE_N_MOMO_C_L_WJ_EX:
        v11 = &a3[MOTION_BONE_CL_MOMO_L].node[0];
        exp_data->rotation.y = bone_data_limit_angle(v11->exp_data.rotation.y
            + v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_MOMO_B_R_WJ_EX:
    case MOTION_BONE_N_MOMO_C_R_WJ_EX:
        v11 = &a3[MOTION_BONE_CL_MOMO_R].node[0];
        exp_data->rotation.y = bone_data_limit_angle(v11->exp_data.rotation.y
            + v11->exp_data.rotation.x) * (float_t)(1.0 / 3.0);
        break;
    case MOTION_BONE_N_HARA_B_WJ_EX:
        exp_data->rotation.y = a3[MOTION_BONE_CL_MUNE].node[0].exp_data.rotation.y * (float_t)(1.0 / 3.0)
            + a3[MOTION_BONE_KL_KOSI_Y].node[0].exp_data.rotation.y * (float_t)(2.0 / 3.0);
        break;
    case MOTION_BONE_N_HARA_C_WJ_EX:
        exp_data->rotation.y = a3[MOTION_BONE_CL_MUNE].node[0].exp_data.rotation.y * (float_t)(2.0 / 3.0)
            + a3[MOTION_BONE_KL_KOSI_Y].node[0].exp_data.rotation.y * (float_t)(1.0 / 3.0);
        break;
    default:
        ret = false;
        break;
    }
    return ret;
}

static void bone_data_mult_ik(bone_data* a1, int32_t skeleton_select) {
    if (a1->type < BONE_DATABASE_BONE_HEAD_IK_ROTATION)
        return;

    mat4 mat = *a1->node[0].mat;

    vec3 v30;
    mat4_mult_vec3_inv_trans(&mat, &a1->ik_target, &v30);
    float_t v6;
    float_t v8;
    vec2_length_squared(*(vec2*)&v30, v6);
    vec3_length_squared(v30, v8);
    float_t v9 = sqrtf(v6);
    float_t v10 = sqrtf(v8);
    mat4 rot_mat;
    if (v9 > 0.000001f && v8 > 0.000001f) {
        mat4_rotate_z_sin_cos(v30.y / v9, v30.x / v9, &rot_mat);
        mat4_rotate_y_mult_sin_cos(&rot_mat, -v30.z / v10, v9 / v10, &rot_mat);
        mat4_mult(&rot_mat, &mat, &mat);
    }
    else
        rot_mat = mat4_identity;

    if (a1->pole_target_mat) {
        vec3 pole_target;
        mat4_get_translation(a1->pole_target_mat, &pole_target);
        mat4_mult_vec3_inv_trans(&mat, &pole_target, &pole_target);
        float_t pole_target_length;
        vec2_length(*(vec2*)&pole_target.y, pole_target_length);
        if (pole_target_length > 0.000001f) {
            vec3_mult_scalar(pole_target, 1.0f / pole_target_length, pole_target);
            float_t rot_cos = pole_target.y;
            float_t rot_sin = pole_target.z;
            mat4_rotate_x_mult_sin_cos(&mat, rot_sin, rot_cos, &mat);
            mat4_rotate_x_mult_sin_cos(&rot_mat, rot_sin, rot_cos, &rot_mat);
        }
    }

    if (a1->type == BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        a1->rot_mat[1] = rot_mat;
        *a1->node[1].mat = mat;
        mat4_translate_mult(&mat, a1->ik_segment_length[skeleton_select], 0.0f, 0.0f, &mat);
        *a1->node[2].mat = mat;
        return;
    }

    float_t ik_segment_length = a1->ik_segment_length[skeleton_select];
    float_t ik_2nd_segment_length = a1->ik_2nd_segment_length[skeleton_select];
    float_t rot_sin;
    float_t rot_cos;
    float_t rot_2nd_sin;
    float_t rot_2nd_cos;
    if (v8 > 0.000001f) {
        if (a1->arm_length > 0.0001f) {
            float_t v27 = (ik_segment_length + ik_2nd_segment_length) * a1->arm_length;
            if (v10 > v27) {
                v10 = v27;
                v8 = v27 * v27;
            }
        }

        float_t v28 = (v8 - ik_2nd_segment_length * ik_2nd_segment_length) / ik_segment_length;
        rot_cos = (v28 + ik_segment_length) / (2.0f * v10);
        rot_2nd_cos = (v28 - ik_segment_length) / (2.0f * ik_2nd_segment_length);

        rot_cos = clamp(rot_cos, -1.0f, 1.0f);
        rot_2nd_cos = clamp(rot_2nd_cos, -1.0f, 1.0f);

        rot_sin = sqrtf(1.0f - rot_cos * rot_cos);
        rot_2nd_sin = sqrtf(1.0f - rot_2nd_cos * rot_2nd_cos);
        if (a1->type == BONE_DATABASE_BONE_LEGS_IK_ROTATION)
            rot_sin = -rot_sin;
        else
            rot_2nd_sin = -rot_2nd_sin;
    }
    else {
        rot_sin = 0.0f;
        rot_cos = 1.0f;
        rot_2nd_sin = 0.0f;
        rot_2nd_cos = -1.0f;
    }

    mat4_rotate_z_mult_sin_cos(&mat, rot_sin, rot_cos, &mat);
    *a1->node[1].mat = mat;
    mat4_rotate_z_mult_sin_cos(&rot_mat, rot_sin, rot_cos, &rot_mat);
    a1->rot_mat[1] = rot_mat;
    mat4_translate_mult(&mat, ik_segment_length, 0.0f, 0.0f, &mat);
    mat4_rotate_z_mult_sin_cos(&mat, rot_2nd_sin, rot_2nd_cos, &mat);
    *a1->node[2].mat = mat;
    mat4_rotate_z_sin_cos(rot_2nd_sin, rot_2nd_cos, &rot_mat);
    a1->rot_mat[2] = rot_mat;
    mat4_translate_mult(&mat, ik_2nd_segment_length, 0.0f, 0.0f, &mat);
    *a1->node[3].mat = mat;
}

static vec3* bone_data_set_key_data(bone_data* data, vec3* keyframe_data,
    bone_database_skeleton_type skeleton_type, bool get_data, bool reverse_x) {
    bone_database_bone_type type = data->type;
    if (type == BONE_DATABASE_BONE_POSITION_ROTATION) {
        if (get_data) {
            data->trans = *keyframe_data;
            if (reverse_x)
                data->trans.x = -data->trans.x;
        }
        keyframe_data++;
    }
    else if (type >= BONE_DATABASE_BONE_HEAD_IK_ROTATION
        && type <= BONE_DATABASE_BONE_LEGS_IK_ROTATION) {
        if (get_data) {
            data->ik_target = *keyframe_data;
            if (reverse_x)
                data->ik_target.x = -data->ik_target.x;
        }
        keyframe_data++;
    }

    if (get_data) {
        if (type == BONE_DATABASE_BONE_TYPE_1 || type == BONE_DATABASE_BONE_POSITION) {
            data->trans = *keyframe_data;
            if (reverse_x)
                data->trans.x = -data->trans.x;
        }
        else if (!data->flags) {
            data->rotation = *keyframe_data;
            if (reverse_x) {
                const skeleton_rotation_offset* rot_off = skeleton_rotation_offset_array;
                size_t index = data->motion_bone_index;
                if (rot_off[index].x)
                    data->rotation.x = rot_off[index].rotation.x - data->rotation.x;
                if (rot_off[index].y)
                    data->rotation.y = rot_off[index].rotation.y - data->rotation.y;
                if (rot_off[index].z)
                    data->rotation.z = rot_off[index].rotation.z - data->rotation.z;
            }
        }
    }
    keyframe_data++;
    return keyframe_data;
}

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_bone_data* rob_bone_data, bone_database* bone_data) {
    const char* base_name = bone_database_skeleton_type_to_string(rob_bone_data->base_skeleton_type);
    const char* name = bone_database_skeleton_type_to_string(rob_bone_data->skeleton_type);
    std::vector<bone_database_bone>* common_bones = 0;
    std::vector<vec3>* common_translation = 0;
    std::vector<vec3>* translation = 0;
    if (!bone_data->get_skeleton_bones(base_name, &common_bones)
        || !bone_data->get_skeleton_positions(base_name, &common_translation)
        || !bone_data->get_skeleton_positions(name, &translation))
        return;

    bone->rob_bone_data = rob_bone_data;
    bone_data_parent_load_rob_chara(bone);
    bone_data_parent_load_bone_database(bone, common_bones, common_translation->data(), translation->data());
}

static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    std::vector<bone_database_bone>* bones, vec3* common_translation, vec3* translation) {
    rob_chara_bone_data* rob_bone_data = bone->rob_bone_data;
    size_t chain_pos = 0;
    size_t total_bone_count = 0;
    size_t ik_bone_count = 0;

    bone_data* bone_node = bone->bones.data();
    for (bone_database_bone& i : *bones ) {
        bone_node->motion_bone_index = (motion_bone_index)(&i - bones->data());
        bone_node->type = i.type;
        bone_node->mirror = i.mirror;
        bone_node->parent = i.parent;
        bone_node->flags = i.flags;
        if (i.type >= BONE_DATABASE_BONE_POSITION_ROTATION)
            bone_node->key_set_count = 6;
        else
            bone_node->key_set_count = 3;
        bone_node->base_translation[0] = *common_translation++;
        bone_node->base_translation[1] = *translation++;
        bone_node->node = &rob_bone_data->nodes[total_bone_count];
        bone_node->has_parent = i.has_parent;
        if (i.has_parent)
            bone_node->parent_mat = &rob_bone_data->mats[i.parent];
        else
            bone_node->parent_mat = 0;

        bone_node->pole_target_mat = 0;
        switch (i.type) {
        case BONE_DATABASE_BONE_ROTATION:
        case BONE_DATABASE_BONE_TYPE_1:
        case BONE_DATABASE_BONE_POSITION:
            chain_pos++;
            total_bone_count++;
            break;
        case BONE_DATABASE_BONE_POSITION_ROTATION:
            chain_pos++;
            ik_bone_count++;
            total_bone_count++;
            break;
        case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
            bone_node->ik_segment_length[0] = (common_translation++)->x;
            bone_node->ik_segment_length[1] = (translation++)->x;

            chain_pos += 2;
            ik_bone_count++;
            total_bone_count += 3;
            break;
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            bone_node->ik_segment_length[0] = (common_translation++)->x;
            bone_node->ik_segment_length[1] = (translation++)->x;
            bone_node->ik_2nd_segment_length[0] = (common_translation++)->x;
            bone_node->ik_2nd_segment_length[1] = (translation++)->x;

            mat4* pole_target = 0;
            if (i.pole_target)
                pole_target = &rob_bone_data->mats[i.pole_target];
            bone_node->pole_target_mat = pole_target;

            chain_pos += 3;
            ik_bone_count++;
            total_bone_count += 4;
            break;
        }
        bone_node++;
    }
}

static void bone_data_parent_load_bone_indices_from_mot(bone_data_parent* a1,
    const mot_data* a2, bone_database* bone_data, motion_database* mot_db) {
    if (!a2)
        return;

    std::vector<uint16_t>& bone_indices = a1->bone_indices;
    bone_indices.clear();

    uint16_t key_set_count = a2->key_set_count - 1;
    if (!key_set_count)
        return;

    bone_database_skeleton_type skeleton_type = a1->rob_bone_data->base_skeleton_type;
    const char* skeleton_type_string = bone_database_skeleton_type_to_string(skeleton_type);
    std::string* bone_names = mot_db->bone_name.data();

    const mot_bone_info* bone_info = a2->bone_info.data();
    for (size_t key_set_offset = 0, i = 0; key_set_offset < key_set_count; i++) {
        int32_t bone_index = bone_data->get_skeleton_bone_index(skeleton_type_string,
            bone_names[bone_info[i].index].c_str());
        if (bone_index == -1) {
            bone_index = bone_data->get_skeleton_bone_index(skeleton_type_string,
                bone_names[bone_info[++i].index].c_str());
            if (bone_index == -1)
                break;
        }
        bone_indices.push_back((uint16_t)bone_index);

        ::bone_data* bone_node = &a1->bones.data()[bone_index];
        bone_node->key_set_offset = (int32_t)key_set_offset;
        bone_node->frame = -1.0f;
        if (bone_node->type >= BONE_DATABASE_BONE_POSITION_ROTATION)
            key_set_offset += 6;
        else
            key_set_offset += 3;
    }
}

static void bone_data_parent_load_rob_chara(bone_data_parent* bone) {
    rob_chara_bone_data* rob_bone_data = bone->rob_bone_data;
    if (!rob_bone_data)
        return;

    bone->motion_bone_count = rob_bone_data->motion_bone_count;
    bone->ik_bone_count = rob_bone_data->ik_bone_count;
    bone->chain_pos = rob_bone_data->chain_pos;
    bone->bones.clear();
    bone->bones.resize(rob_bone_data->motion_bone_count);
    bone->global_key_set_count = 6;
    bone->bone_key_set_count = (uint32_t)((bone->motion_bone_count + bone->ik_bone_count) * 3);
}

static float_t* bone_node_get_exp_data_component(bone_node* a1,
    size_t index, ex_expression_block_stack_type* type) {
    switch (index) {
    case 0:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.position.x;
    case 1:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.position.y;
    case 2:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.position.z;
    case 3:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN;
        return &a1->exp_data.rotation.x;
    case 4:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN;
        return &a1->exp_data.rotation.y;
    case 5:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN;
        return &a1->exp_data.rotation.z;
    case 6:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.scale.x;
    case 7:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.scale.y;
    case 8:
        *type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &a1->exp_data.scale.z;
    default:
        return 0;
    }
}

static void bone_node_expression_data_mat_set(bone_node_expression_data* data,
    vec3* parent_scale, mat4* ex_data_mat, mat4* mat) {
    vec3 position;
    vec3_mult(data->position, *parent_scale, position);
    mat4_translate_mult(ex_data_mat, position.x, position.y, position.z, ex_data_mat);
    vec3 rotation = data->rotation;
    mat4_rotate_mult(ex_data_mat, rotation.x, rotation.y, rotation.z, ex_data_mat);
    vec3_mult(data->scale, *parent_scale, data->parent_scale);
    *mat = *ex_data_mat;
    mat4_scale_rot(mat, data->parent_scale.x, data->parent_scale.y, data->parent_scale.z, mat);
}

static void bone_node_expression_data_reset_scale(bone_node_expression_data* data) {
    data->scale = vec3_identity;
    data->parent_scale = vec3_identity;
}

static void bone_node_expression_data_set_position_rotation(bone_node_expression_data* data,
    float_t position_x, float_t position_y, float_t position_z,
    float_t rotation_x, float_t rotation_y, float_t rotation_z) {
    data->position.x = position_x;
    data->position.y = position_y;
    data->position.z = position_z;
    data->rotation.x = rotation_x;
    data->rotation.z = rotation_z;
    data->rotation.y = rotation_y;
    data->scale = vec3_identity;
    data->parent_scale = vec3_identity;
}

static void bone_node_expression_data_set_position_rotation_vec3(bone_node_expression_data* data,
    vec3* position, vec3* rotation) {
    data->position = *position;
    data->rotation = *rotation;
    data->scale = vec3_identity;
    data->parent_scale = vec3_identity;
}

static void sub_14047EE90(rob_osage* rob_osg, mat4* mat) {
    if (rob_osg->reset_data_list) {
        std::list<rob_osage_node_reset_data>::iterator reset_data_list = rob_osg->reset_data_list->begin();
        rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
        rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (rob_osage_node* i = i_begin; i != i_end; i++)
            i->reset_data = *reset_data_list++;
        rob_osg->reset_data_list = 0;
    }

    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++) {
        mat4_mult_vec3(mat, &i->reset_data.trans_diff, &i->trans_diff);
        mat4_mult_vec3_trans(mat, &i->reset_data.trans, &i->trans);
    }
    rob_osg->parent_mat = *rob_osg->parent_mat_ptr;
}

static void sub_14047F110(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, bool init_rot) {
    vec3 position;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, position);
    mat4_translate_mult(mat, position.x, position.y, position.z, mat);

    vec3 rotation = rob_osg->exp_data.rotation;
    mat4_rotate_mult(mat, rotation.x, rotation.y, rotation.z, mat);

    skin_param* skin_param = rob_osg->skin_param_ptr;
    vec3 rot = skin_param->rot;
    if (init_rot)
        vec3_add(rot, skin_param->init_rot, rot);
    mat4_rotate_mult(mat, rot.x, rot.y, rot.z, mat);
}

static bool sub_140482FF0(mat4* mat, vec3* trans, skin_param_hinge* hinge, vec3* rot, int32_t* yz_order) {
    bool clipped = false;
    float_t z_rot;
    float_t y_rot;
    if (*yz_order == 1) {
        y_rot = atan2f(-trans->z, trans->x);
        z_rot = atan2f(trans->y, sqrtf(trans->x * trans->x + trans->z * trans->z));
        if (hinge) {
            if (y_rot > hinge->ymax) {
                y_rot = hinge->ymax;
                clipped = true;
            }

            if (y_rot < hinge->ymin) {
                y_rot = hinge->ymin;
                clipped = true;
            }

            if (z_rot > hinge->zmax) {
                z_rot = hinge->zmax;
                clipped = true;
            }

            if (z_rot < hinge->zmin) {
                z_rot = hinge->zmin;
                clipped = true;
            }
        }
        mat4_rotate_y_mult(mat, y_rot, mat);
        mat4_rotate_z_mult(mat, z_rot, mat);
    }
    else {
        z_rot = atan2f(trans->y, trans->x);
        y_rot = atan2f(-trans->z, sqrtf(trans->x * trans->x + trans->y * trans->y));
        if (hinge) {
            if (y_rot > hinge->ymax) {
                y_rot = hinge->ymax;
                clipped = true;
            }

            if (y_rot < hinge->ymin) {
                y_rot = hinge->ymin;
                clipped = true;
            }

            if (z_rot > hinge->zmax) {
                z_rot = hinge->zmax;
                clipped = true;
            }

            if (z_rot < hinge->zmin) {
                z_rot = hinge->zmin;
                clipped = true;
            }
        }
        mat4_rotate_z_mult(mat, z_rot, mat);
        mat4_rotate_y_mult(mat, y_rot, mat);
    }

    if (rot) {
        rot->y = y_rot;
        rot->z = z_rot;
    }
    return clipped;
}

static void sub_1404803B0(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, bool has_children_node) {
    mat4 v47 = *mat;
    vec3 v45;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v45);
    mat4_mult_vec3_trans(&v47, &v45, &rob_osg->nodes[0].trans);
    if (rob_osg->osage_reset && !rob_osg->field_1F0E) {
        rob_osg->field_1F0E = true;
        sub_14047EE90(rob_osg, mat);
    }

    if (!rob_osg->field_1F0C) {
        rob_osg->field_1F0C = true;

        float_t move_cancel = rob_osg->skin_param_ptr->move_cancel;
        if (rob_osg->move_cancel == 1.0f)
            move_cancel = 1.0f;
        if (move_cancel < 0.0f)
            move_cancel = rob_osg->move_cancel;

        if (move_cancel > 0.0f) {
            rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
            rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
            for (rob_osage_node* i = i_begin; i != i_end; i++) {
                mat4 parent_mat;
                vec3 v46;
                parent_mat = rob_osg->parent_mat;
                mat4_mult_vec3_inv_trans(&parent_mat, &i->trans, &v46);
                parent_mat = *rob_osg->parent_mat_ptr;
                mat4_mult_vec3_trans(&parent_mat, &v46, &v46);
                vec3_sub(v46, i->trans, v46);
                vec3_mult_scalar(v46, move_cancel, v46);
                vec3_add(i->trans, v46, i->trans);
            }
        }
    }

    if (has_children_node) {
        sub_14047F110(rob_osg, &v47, parent_scale, 0);
        *rob_osg->nodes[0].bone_node_mat = v47;
        rob_osage_node* v29 = rob_osg->nodes.data();
        rob_osage_node* v30_begin = rob_osg->nodes.data() + 1;
        rob_osage_node* v30_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (rob_osage_node* v30 = v30_begin; v30 != v30_end; v29++, v30++) {
            vec3 v46;
            mat4_mult_vec3_inv_trans(&v47, &v30->trans, &v46);
            bool v32 = sub_140482FF0(&v47, &v46, &v30->data_ptr->skp_osg_node.hinge,
                &v30->reset_data.rotation, &rob_osg->yz_order);
            *v30->bone_node_ptr->ex_data_mat = v47;
            float_t v34;
            vec3_distance(v30->trans, v29->trans, v34);
            float_t v35 = parent_scale->x * v30->length;
            bool v36;
            if (v34 >= fabsf(v35)) {
                v35 = v34;
                v36 = false;
            }
            else
                v36 = true;
            mat4_translate_mult(&v47, v35, 0.0f, 0.0f, &v47);
            if (v32 || v36)
                mat4_get_translation(&v47, &v30->trans);
        }

        if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
            mat4 v48 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
            mat4_translate_mult(&v48, parent_scale->x * rob_osg->node.length, 0.0f, 0.0f, &v48);
            *rob_osg->node.bone_node_ptr->ex_data_mat = v48;
        }
    }
}

static void sub_140482300(vec3* a1, vec3* a2, vec3* a3, float_t osage_gravity_const, float_t weight) {
    weight *= osage_gravity_const;
    vec3 diff;
    vec3_sub(*a2, *a3, diff);
    float_t dist;
    vec3_length(diff, dist);
    if (dist <= fabsf(osage_gravity_const)) {
        diff.y = osage_gravity_const;
        vec3_length(diff, dist);
    }
    vec3_mult_scalar(diff, weight * diff.y * (1.0f / (dist * dist)), diff);
    vec3_negate(diff, diff);
    diff.y -= weight;
    *a1 = diff;
}

static void sub_140482F30(vec3* pos1, vec3* pos2, float_t length) {
    vec3 diff;
    vec3_sub(*pos1, *pos2, diff);
    float_t dist;
    vec3_length_squared(diff, dist);
    if (dist > length * length) {
        vec3_mult_scalar(diff, length / sqrtf(dist), diff);
        vec3_add(*pos2, diff, *pos1);
    }
}

static void sub_140482490(rob_osage_node* node, float_t step, float_t a3) {
    if (step != 1.0f) {
        vec3 v4;
        vec3_sub(node->trans, node->trans_orig, v4);

        float_t v9;
        vec3_length(v4, v9);
        if (v9 != 0.0f)
            vec3_mult_scalar(v4, 1.0f / v9, v4);
        vec3_mult_scalar(v4, step * v9, v4);
        vec3_add(node->trans_orig, v4, node->trans);
    }

    sub_140482F30(&node[0].trans, &node[-1].trans, node->length * a3);
    if (node->sibling_node)
        sub_140482F30(&node->trans, &node->sibling_node->trans, node->max_distance);
}

static void sub_140482180(rob_osage_node* node, float_t a2) {
    float_t v2 = node->data_ptr->skp_osg_node.coli_r + a2;
    if (v2 <= node->trans.y)
        return;

    node->trans.y = v2;
    vec3 v3;
    vec3_sub(node->trans, node[-1].trans, v3);
    vec3_normalize(v3, v3);
    vec3_mult_scalar(v3, node->length, v3);
    vec3_add(node[-1].trans, v3, node->trans);
    node->trans_diff = vec3_null;
    node->field_C8 += 1.0f;
}

static void sub_14047C800(rob_osage* rob_osg, mat4* mat, vec3* parent_scale,
    float_t step, bool disable_wind, bool ring_coli, bool has_children_node) {
    if (!rob_osg->nodes.size())
        return;

    const float_t osage_gravity_const = get_osage_gravity_const();
    sub_1404803B0(rob_osg, mat, parent_scale, false);

    rob_osage_node* v17 = &rob_osg->nodes[0];
    v17->trans_orig = v17->trans;
    vec3 v113;
    vec3_mult(rob_osg->exp_data.position, *parent_scale, v113);

    mat4 v130 = *mat;
    mat4_mult_vec3_trans(&v130, &v113, &v17->trans);
    vec3_sub(v17->trans, v17->trans_orig, v17->trans_diff);
    sub_14047F110(rob_osg, &v130, parent_scale, 0);
    *rob_osg->nodes[0].bone_node_mat = v130;
    *rob_osg->nodes[0].bone_node_ptr->ex_data_mat = v130;

    v113 = { 1.0f, 0.0f, 0.0f };
    vec3 v128;
    mat4_mult_vec3(&v130, &v113, &v128);

    float_t v25 = parent_scale->x;
    if (!rob_osg->osage_reset && step <= 0.0f)
        return;

    bool stiffness = rob_osg->skin_param_ptr->stiffness > 0.0f;

    rob_osage_node* v30 = rob_osg->nodes.data();
    rob_osage_node* v26_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* v26_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* v26 = v26_begin; v26 != v26_end; v26++, v30++) {
        rob_osage_node_data* v31 = v26->data_ptr;
        float_t weight = v31->skp_osg_node.weight;
        vec3 v111;
        if (!rob_osg->set_external_force) {
            sub_140482300(&v111, &v26->trans, &v30->trans, osage_gravity_const, weight);

            if (v26 != v26_end - 1) {
                vec3 v112;
                sub_140482300(&v112, &v26->trans, &v26[1].trans, osage_gravity_const, weight);
                vec3_add(v111, v112, v111);
                vec3_mult_scalar(v111, 0.5f, v111);
            }
        }
        else
            vec3_mult_scalar(rob_osg->external_force, 1.0f / weight, v111);

        vec3 v127;
        vec3_mult_scalar(v128, v31->force * v26->force, v127);
        float_t v41 = (1.0f - rob_osg->field_1EB4) * (1.0f - rob_osg->skin_param_ptr->air_res);

        vec3 v126;
        vec3_add(v111, v127, v126);
        vec3_mult_scalar(v26->trans_diff, v41, v111);
        vec3_sub(v126, v111, v126);
        vec3_mult_scalar(v26->external_force, weight, v111);
        vec3_add(v126, v111, v126);

        if (!disable_wind) {
            vec3_mult_scalar(rob_osg->wind_direction, rob_osg->skin_param_ptr->wind_afc, v111);
            vec3_add(v126, v111, v126);
        }

        if (stiffness) {
            vec3 v116;
            vec3_sub(v26->trans_diff, v30->trans_diff, v116);
            vec3_mult_scalar(v116, 1.0f - rob_osg->skin_param_ptr->air_res, v116);
            vec3_sub(v126, v116, v126);
        }

        float_t v49 = 1.0f / (weight - (weight - 1.0f) * v31->skp_osg_node.inertial_cancel);
        vec3_mult_scalar(v126, v49, v26->field_28);
    }

    if (stiffness) {
        mat4 v131 = v130;
        vec3 v111;
        mat4_get_translation(&v131, &v111);

        rob_osage_node* v55_begin = rob_osg->nodes.data() + 1;
        rob_osage_node* v55_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (rob_osage_node* v55 = v55_begin; v55 != v55_end; v55++) {
            mat4_mult_vec3_trans(&v131, &v55->field_94, &v128);

            vec3 v123;
            vec3_add(v55->trans_diff, v55->field_28, v123);
            vec3 v126;
            vec3_add(v55->trans, v123, v126);
            sub_140482F30(&v126, &v111, v25 * v55->length);
            vec3 v117;
            vec3_sub(v128, v126, v117);
            vec3_mult_scalar(v117, rob_osg->skin_param_ptr->stiffness, v117);
            float_t weight = v55->data_ptr->skp_osg_node.weight;
            float_t v74 = 1.0f / (weight - (weight - 1.0f) * v55->data_ptr->skp_osg_node.inertial_cancel);
            vec3_mult_scalar(v117, v74, v117);
            vec3_add(v55->field_28, v117, v55->field_28);
            vec3_add(v55->trans, v117, v126);
            vec3_add(v126, v123, v126);

            vec3 v129;
            mat4_mult_vec3_inv_trans(&v131, &v126, &v129);

            sub_140482FF0(&v131, &v129, 0, 0, &rob_osg->yz_order);

            float_t v58;
            vec3_distance(v111, v126, v58);
            mat4_translate_mult(&v131, v58, 0.0f, 0.0f, &v131);

            v111 = v126;
        }
    }

    rob_osage_node* v82_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* v82_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* v82 = v82_begin; v82 != v82_end; v82++) {
        v82->trans_orig = v82->trans;
        vec3_add(v82->field_28, v82->trans_diff, v82->trans_diff);
        vec3_add(v82->trans_diff, v82->trans, v82->trans);
    }

    if (rob_osg->nodes.size() > 1) {
        rob_osage_node* v90_begin = rob_osg->nodes.data() + rob_osg->nodes.size() - 2;
        rob_osage_node* v90_end = rob_osg->nodes.data();
        for (rob_osage_node* v90 = v90_begin; v90 != v90_end; v90--)
            sub_140482F30(&v90[0].trans, &v90[1].trans, v25 * v90->child_length);
    }

    if (ring_coli) {
        rob_osage_node* v91 = &rob_osg->nodes.data()[0];
        float_t v93 = v91->data_ptr->skp_osg_node.coli_r;
        float_t v97;
        if (v91->trans.x < rob_osg->ring.ring_rectangle_x - v93
            || v91->trans.z < rob_osg->ring.ring_rectangle_y - v93
            || v91->trans.x > rob_osg->ring.ring_rectangle_x + rob_osg->ring.ring_rectangle_width
            || v91->trans.z > rob_osg->ring.ring_rectangle_y + rob_osg->ring.ring_rectangle_height)
            v97 = rob_osg->ring.ring_out_height;
        else
            v97 = rob_osg->ring.ring_height;

        float_t v96 = v97 + v93;
        rob_osage_node* v98_begin = rob_osg->nodes.data() + 1;
        rob_osage_node* v98_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (auto v98 = v98_begin; v98 != v98_end; v98++) {
            sub_140482490(v98, step, v25);
            sub_140482180(v98, v96);
        }
    }

    if (has_children_node) {
        rob_osage_node* v100 = rob_osg->nodes.data();
        rob_osage_node* v99_begin = rob_osg->nodes.data() + 1;
        rob_osage_node* v99_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (rob_osage_node* v99 = v99_begin; v99 != v99_end; v99++, v100++) {
            vec3 v129;
            mat4_mult_vec3_inv_trans(&v130, &v99->trans, &v129);
            bool v102 = sub_140482FF0(&v130, &v129,
                &v99->data_ptr->skp_osg_node.hinge,
                &v99->reset_data.rotation, &rob_osg->yz_order);
            *v99->bone_node_ptr->ex_data_mat = v130;

            float_t v104;
            vec3_distance_squared(v99->trans, v100->trans, v104);
            float_t v105 = v25 * v99->length;

            bool v106;
            if (v104 >= v105 * v105) {
                v105 = sqrtf(v104);
                v106 = false;
            }
            else
                v106 = true;

            mat4_translate_mult(&v130, v105, 0.0f, 0.0f, &v130);
            if (v102 || v106)
                mat4_get_translation(&v130, &v99->trans);
        }

        if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
            mat4 v131 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
            mat4_translate_mult(&v131, v25 * rob_osg->node.length, 0.0f, 0.0f, &v131);
            *rob_osg->node.bone_node_ptr->ex_data_mat = v131;
        }
    }
    rob_osg->field_2A0 = false;
}

static int32_t sub_140483DE0(vec3* a1, vec3* a2, vec3* a3, float_t radius) {
    vec3 v5;
    float_t length;
    vec3_sub(*a2, *a3, v5);
    vec3_length_squared(v5, length);
    if (length > 0.000001f && length < radius * radius) {
        vec3_mult_scalar(v5, radius / sqrtf(length) - 1.0f, *a1);
        return 1;
    }
    return 0;
}

static int32_t sub_140484540(vec3* a1, vec3* a2, osage_coli* coli, float_t radius) {
    vec3 v11;
    vec3_sub(*a2, coli->bone0_pos, v11);

    float_t v17;
    vec3_dot(v11, coli->bone_pos_diff, v17);
    if (v17 < 0.0f)
        return sub_140483DE0(a1, a2, &coli->bone0_pos, radius);

    float_t v19 = coli->bone_pos_diff_length_squared;
    if (fabsf(v19) <= 0.000001f)
        return sub_140483DE0(a1, a2, &coli->bone0_pos, radius);
    if (v17 > v19)
        return sub_140483DE0(a1, a2, &coli->bone1_pos, radius);

    float_t v20;
    vec3_length_squared(v11, v20);
    float_t v22 = fabsf(v20 - v17 * v17 / v19);
    if (v22 <= 0.000001f || v22 >= radius * radius)
        return 0;

    vec3 v29;
    vec3_mult_scalar(coli->bone_pos_diff, v17 / v19, v29);
    vec3_sub(v11, v29, v29);
    vec3_mult_scalar(v29, radius / sqrtf(v22) - 1.0f, *a1);
    return 1;
}

static int32_t sub_140484780(vec3* a1, vec3* a2, vec3* a3, vec3* a4, float_t radius) {
    float_t v3;
    vec3_dot(*a2, *a4, v3);
    float_t v4;
    vec3_dot(*a3, *a4, v4);
    float_t v5 = v3 - v4 - radius;
    if (v5 >= 0.0f)
        return 0;

    vec3_mult_scalar(*a4, -v5, *a1);
    return 1;
}

static int32_t sub_140483EA0(vec3* a1, vec3* a2, osage_coli* a3, float_t radius) {
    if (a3->bone_pos_diff_length <= 0.000001f)
        return sub_140483DE0(a1, a2, &a3->bone0_pos, radius);

    float_t v61 = a3->bone_pos_diff_length * 0.5f;
    float_t v13 = sqrtf(radius * radius + v61 * v61);

    float_t v20;
    vec3 v54;
    vec3_sub(*a2, a3->bone0_pos, v54);
    vec3_length(v54, v20);

    float_t v21;
    vec3 v57;
    vec3_sub(*a2, a3->bone1_pos, v57);
    vec3_length(v57, v21);

    if (v20 <= 0.000001f || v21 <= 0.000001f)
        return 0;

    float_t v22 = v20 + v21;
    if (v21 + v20 >= v13 * 2.0f)
        return 0;

    float_t v25 = 1.0f / a3->bone_pos_diff_length;

    vec3 v63;
    vec3_add(a3->bone0_pos, a3->bone1_pos, v63);
    vec3_mult_scalar(v63, 0.5f, v63);
    vec3_sub(*a2, v63, v63);

    float_t v58;
    vec3_length(v63, v58);
    if (v58 != 0.0f)
        vec3_mult_scalar(v63, 1.0f / v58, v63);

    float_t v36;
    vec3_dot(v63, a3->bone_pos_diff, v36);
    v36 *= v25;
    float_t v37 = v36 * v36;
    v37 = min(v37, 1.0f);
    v22 *= 0.5f;
    float_t v38 = v22 * v22 - v61;
    if (v38 < 0.0f)
        return 0;

    float_t v39 = sqrtf(v38);
    if (v39 <= 0.000001f)
        return 0;

    float_t v42 = sqrtf(1.0f - v37);
    v22 = (v13 / v22 - 1.0f) * v36 * v58;
    v42 = (radius / v39 - 1.0f) * v42 * v58;
    float_t v43 = sqrtf(v42 * v42 + v22 * v22);
    if (v20 != 0.0f)
        vec3_mult_scalar(v54, 1.0f / v20, v54);

    if (v21 != 0.0f)
        vec3_mult_scalar(v57, 1.0f / v21, v57);

    vec3 v48;
    vec3_add(v54, v57, v48);
    vec3_normalize(v48, v48);
    vec3_mult_scalar(v48, v43, *a1);
    return 1;
}

static int32_t sub_140483B30(vec3* a1, vec3* a2, osage_coli* a3, float_t radius) {
    vec3 bone_pos_diff;
    vec3_add(a3->bone0_pos, a3->bone1_pos, bone_pos_diff);

    vec3 v29;
    vec3 v30;
    for (int32_t i = 0; i < 3; i++) {
        float_t v13 = ((float_t*)a2)[i];
        float_t v14 = ((float_t*)&a3->bone0_pos)[i] - v13;
        ((float_t*)&v30)[i] = v14;
        if (v14 > radius)
            return 0;

        v14 = v13 - ((float_t*)&bone_pos_diff)[i];
        ((float_t*)&v29)[i] = v14;
        if (v14 > radius)
            return 0;
    }

    vec3 v28 = vec3_null;
    vec3 v32 = vec3_null;
    vec3 v27 = vec3_identity;

    float_t v16 = 0.0f;
    for (int32_t i = 0; i < 3; i++) {
        float_t v18 = ((float_t*)&v30)[i];
        float_t v19 = ((float_t*)&v29)[i];
        if (fabsf(v18) >= fabsf(v19)) {
            ((float_t*)&v28)[i] = v19;
            ((float_t*)&v27)[i] = 1.0f;
            v18 = v19;
        }
        else {
            ((float_t*)&v28)[i] = v18;
            ((float_t*)&v27)[i] = -1.0f;
        }

        if (v18 > 0.0f) {
            float_t v20 = v18 * v18;
            ((float_t*)&v32)[i] = v20;
            v16 += v20;
        }
    }

    if (v16 > radius * radius)
        return 0;

    if (v16 > 0.0f) {
        vec3_mult(v27, v32, v32);
        vec3_mult_scalar(v32, 1.0f / v16, v32);
        vec3_mult_scalar(v32, radius - sqrtf(v16), *a1);
    }
    else {
        float_t v25 = ((float_t*)&v28)[0];
        int32_t v26 = 0;
        for (int32_t i = 0; i < 3; i++) {
            if (((float_t*)&v28)[i] > v25) {
                v25 = ((float_t*)&v28)[i];
                v26 = i;
            }
        }
        ((float_t*)a1)[v26] -= ((float_t*)&v27)[v26] * (v25 - radius);
    }
    return 1;
}

static int32_t sub_140485220(vec3* a1, skin_param_osage_node* a2, osage_coli* a3, float_t* a4) {
    if (!a3)
        return 0;

    int32_t v8 = 0;
    while (a3->type) {
        int32_t v11 = 0;
        vec3 v31 = vec3_null;
        switch (a3->type) {
        case SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_BALL:
            v11 = sub_140483DE0(&v31, a1, &a3->bone0_pos, a3->radius + a2->coli_r);
            break;
        case SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_CYLINDER:
            v11 = sub_140484540(&v31, a1, a3, a3->radius + a2->coli_r);
            break;
        case SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_PLANE:
            v11 = sub_140484780(&v31, a1, &a3->bone0_pos, &a3->bone1_pos, a2->coli_r);
            break;
        case SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_ELLIPSE:
            v11 = sub_140483EA0(&v31, a1, a3, a3->radius + a2->coli_r);
            break;
        case SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_5:
            v11 = sub_140483B30(&v31, a1, a3, a2->coli_r);
            break;
        }

        if (a4 && v11 > 0) {
            if (a3->field_34 < *a4)
                *a4 = a3->field_34;
        }
        a3++;
        v8 += v11;
        vec3_add(*a1, v31, *a1);
    }
    return v8;
}

static int32_t sub_1404851C0(vec3* a1, skin_param_osage_node* a2, std::vector<osage_coli>* a3, float_t* a4) {
    if (!a3->size() || a3->back().type)
        return 0;
    return sub_140485220(a1, a2, a3->data(), a4);
}

static int32_t sub_140485180(osage_coli* a1, vec3* a2, skin_param_osage_node* a3) {
    return sub_140485220(a2, a3, a1, 0);
}

static void sub_14047D8C0(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5) {
    if (!rob_osg->osage_reset && step <= 0.0f)
        return;

    osage_coli* coli = rob_osg->coli;
    osage_coli* coli_ring = rob_osg->coli_ring;
    std::vector<osage_coli>* vec_ring_coli = &rob_osg->ring.coli;

    float_t v9 = 0.0f;
    if (step > 0.0f)
        v9 = 1.0f / step;

    mat4 v64;
    if (a5)
        v64 = *rob_osg->nodes.data()[0].bone_node_mat;
    else {
        v64 = *mat;

        vec3 trans;
        vec3_mult(rob_osg->exp_data.position, *parent_scale, trans);
        mat4_mult_vec3_trans(&v64, &trans, &rob_osg->nodes.data()[0].trans);
        sub_14047F110(rob_osg, &v64, parent_scale, 0);
    }

    float_t v60 = -1000.0f;
    if (a5) {
        float_t v22;
        rob_osage_node* v16 = &rob_osg->nodes.data()[0];
        float_t v18 = v16->data_ptr->skp_osg_node.coli_r;
        if (v16->trans.x < rob_osg->ring.ring_rectangle_x - v18
            || v16->trans.z < rob_osg->ring.ring_rectangle_y - v18
            || v16->trans.x > rob_osg->ring.ring_rectangle_x + rob_osg->ring.ring_rectangle_width
            || v16->trans.z > rob_osg->ring.ring_rectangle_y + rob_osg->ring.ring_rectangle_height)
            v22 = rob_osg->ring.ring_out_height;
        else
            v22 = rob_osg->ring.ring_height;
        v60 = v22 + v18;
    }

    float_t v23 = 0.2f;
    if (step < 1.0f)
        v23 = 0.2f / (2.0f - step);

    if (rob_osg->skin_param_ptr->colli_tgt_osg) {
        std::vector<rob_osage_node>* v24 = rob_osg->skin_param_ptr->colli_tgt_osg;
        rob_osage_node* v27_begin = rob_osg->nodes.data() + 1;
        rob_osage_node* v27_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (rob_osage_node* v27 = v27_begin; v27 != v27_end; v27++) {
            vec3 v62 = vec3_null;

            rob_osage_node* v30_begin = v24->data() + 1;
            rob_osage_node* v30_end = v24->data() + v24->size();
            for (rob_osage_node* v30 = v30_begin; v30 != v30_end; v30++)
                sub_140483DE0(&v62, &v27->trans, &v30->trans,
                    v27->data_ptr->skp_osg_node.coli_r + v30->data_ptr->skp_osg_node.coli_r);
            vec3_add(v27->trans, v62, v27->trans);
        }
    }

    rob_osage_node* v35_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* v35_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* v35 = v35_begin; v35 != v35_end; v35++) {
        skin_param_osage_node* skp_osg_node = &v35->data_ptr->skp_osg_node;
        float_t v37 = (1.0f - rob_osg->field_1EB4) * rob_osg->skin_param_ptr->friction;
        if (a5) {
            sub_140482490(&v35[0], step, parent_scale->x);
            v35->field_C8 = (float_t)sub_1404851C0(&v35->trans,
                skp_osg_node, vec_ring_coli, &v35->field_CC);
            if (!rob_osg->field_1F0F) {
                v35->field_C8 += (float_t)sub_140485180(coli_ring, &v35->trans, skp_osg_node);
                v35->field_C8 += (float_t)sub_140485180(coli, &v35->trans, skp_osg_node);
            }
            sub_140482180(&v35[0], v60);
        }
        else
            sub_140482F30(&v35[0].trans, &v35[-1].trans, v35[0].length * parent_scale->x);

        vec3 v63;
        mat4_mult_vec3_inv_trans(&v64, &v35->trans, &v63);
        bool v40 = sub_140482FF0(&v64, &v63, &skp_osg_node->hinge,
            &v35->reset_data.rotation, &rob_osg->yz_order);
        v35->bone_node_ptr->exp_data.parent_scale = *parent_scale;
        *v35->bone_node_ptr->ex_data_mat = v64;

        if (v35->bone_node_mat) {
            mat4* v41 = v35->bone_node_mat;
            mat4_scale_rot(&v64, parent_scale->x, parent_scale->y, parent_scale->z, v41);
        }

        float_t v42 = v35->length * parent_scale->x;
        float_t v44;
        bool v45;
        vec3_distance_squared(v35[0].trans, v35[-1].trans, v44);
        if (v44 >= v42 * v42) {
            v42 = sqrtf(v44);
            v45 = false;
        }
        else
            v45 = true;

        mat4_translate_mult(&v64, v42, 0.0f, 0.0f, &v64);
        v35->reset_data.length = v42;
        if (v40 || v45)
            mat4_get_translation(&v64, &v35->trans);

        vec3 v62;
        vec3_sub(v35->trans, v35->trans_orig, v62);
        vec3_mult_scalar(v62, v9, v35->trans_diff);

        if (v35->field_C8 > 0.0f) {
            if (v35->field_CC < v37)
                v37 = v35->field_CC;
            vec3_mult_scalar(v35->trans_diff, v37, v35->trans_diff);
        }

        float_t v55;
        vec3_length_squared(v35->trans_diff, v55);
        if (v55 > v23 * v23)
            vec3_mult_scalar(v35->trans_diff, v23 / sqrtf(v55), v35->trans_diff);

        mat4_mult_vec3_inv_trans(mat, &v35->trans, &v35->reset_data.trans);
        mat4_mult_vec3_inv(mat, &v35->trans_diff, &v35->reset_data.trans_diff);
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v65 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_translate_mult(&v65, rob_osg->node.length * parent_scale->x, 0.0f, 0.0f, &v65);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v65;
        mat4_scale_rot(&v65, parent_scale->x, parent_scale->y, parent_scale->z, &v65);
        *rob_osg->node.bone_node_mat = v65;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *parent_scale;
    }
}

static void rob_osage_set_nodes_external_force(rob_osage* rob_osg, vec3* external_force, float_t strength) {
    vec3 v4 ;
    if (external_force)
        v4 = *external_force;
    else
        v4 = vec3_null;

    size_t exf = rob_osg->osage_setting.exf;
    size_t v8 = 0;
    if (exf >= 4) {
        float_t strength4 = strength * strength * strength * strength;
        v8 = ((exf - 4) / 4 + 1) * 4;
        for (size_t v10 = v8 / 4; v10; v10--)
            vec3_mult_scalar(v4, strength4, v4);
    }

    if (v8 < exf)
        for (size_t v12 = exf - v8; v12; v12--)
            vec3_mult_scalar(v4, strength, v4);

    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++) {
        i->external_force = v4;
        vec3_mult_scalar(v4, strength, v4);
    }
}

static void rob_osage_set_nodes_force(rob_osage* rob_osg, float_t force) {
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++)
        i->force = force;
}

static void sub_140480260(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool v5) {
    if (!v5) {
        rob_osage_set_nodes_external_force(rob_osg, 0, 1.0f);
        rob_osage_set_nodes_force(rob_osg, 1.0f);
        rob_osg->set_external_force = false;
        rob_osg->external_force = vec3_null;
    }

    rob_osg->field_2A0 = true;
    rob_osg->field_2A1 = false;
    rob_osg->field_2A4 = -1.0f;
    rob_osg->field_1F0C = false;
    rob_osg->osage_reset = false;
    rob_osg->field_1F0E = false;

    for (auto i = rob_osg->nodes.begin(); i != rob_osg->nodes.end(); i++) {
        i->field_C8 = 0.0f;
        i->field_CC = 1.0f;
    }

    rob_osg->parent_mat = *rob_osg->parent_mat_ptr;
}

static void sub_14047C770(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step, bool a5) {
    sub_14047C800(rob_osg, mat, parent_scale, step, a5, false, false);
    sub_14047D8C0(rob_osg, mat, parent_scale, step, true);
    sub_140480260(rob_osg, mat, parent_scale, step, a5);
}

static void sub_14047C750(rob_osage* rob_osg, mat4* mat, vec3* parent_scale, float_t step) {
    sub_14047C770(rob_osg, mat, parent_scale, step, false);
}

static void sub_14047ECA0(rob_osage* rob_osg, float_t step) {
    if (step <= 0.0f)
        return;

    std::vector<osage_coli>* vec_coli = &rob_osg->ring.coli;
    osage_coli* coli_ring = rob_osg->coli_ring;
    osage_coli* coli = rob_osg->coli;
    bool v11 = rob_osg->field_1F0F;
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++) {
        rob_osage_node_data* data = i->data_ptr;
        i->field_C8 += (float_t)sub_1404851C0(&i->trans, &data->skp_osg_node, vec_coli, &i->field_CC);
        if (v11)
            continue;

        for (rob_osage_node*& j : data->boc) {
            j->field_C8 += (float_t)sub_140485180(coli_ring, &j->trans, &data->skp_osg_node);
            j->field_C8 += (float_t)sub_140485180(coli, &j->trans, &data->skp_osg_node);
        }

        i->field_C8 += (float_t)sub_140485180(coli_ring, &i->trans, &data->skp_osg_node);
        i->field_C8 += (float_t)sub_140485180(coli, &i->trans, &data->skp_osg_node);
    }
}

void sub_140484E10(vec3* a1, vec3* a2, vec3* a3, vec3* a4) {
    vec3 v6;
    vec3_sub(*a3, *a2, v6);

    vec3 v7;
    vec3_sub(*a4, *a2, v7);

    float_t v8;
    vec3_dot(v7, v6, v8);
    if (v8 < 0.0f) {
        *a1 = *a2;
        return;
    }

    float_t v9;
    vec3_length_squared(v6, v9);
    if (v9 <= 0.000001f)
        *a1 = *a2;
    else if (v8 <= v9) {
        float_t v10 = v8 / v9;
        vec3_mult_scalar(v6, v8 / v9, v6);
        vec3_add(*a2, v6, *a1);
    }
    else
        *a1 = *a3;
}

static int32_t sub_140484450(vec3* a1, vec3* a2, vec3* a3, vec3* a4, float_t radius) {
    vec3 v8;
    sub_140484E10(&v8, a2, a3, a4);
    return sub_140483DE0(a1, &v8, a4, radius);
}

static void sub_140484850(vec3* a1, vec3* a2, vec3* a3, osage_coli* coli) {
    vec3 v56;
    vec3_sub(*a3, *a2, v56);

    vec3 v60;
    vec3_cross(v56, coli->bone_pos_diff, v60);

    float_t v46;
    vec3_length_squared(v60, v46);
    if (v46 <= 0.000001f) {
        vec3 v56;
        vec3 v57;
        vec3 v58;
        vec3 v59;
        sub_140484E10(&v59, &coli->bone0_pos, &coli->bone1_pos, a2);
        sub_140484E10(&v58, &coli->bone0_pos, &coli->bone1_pos, a3);
        sub_140484E10(&v57, a2, a3, &coli->bone0_pos);
        sub_140484E10(&v56, a2, a3, &coli->bone1_pos);

        float_t v48;
        float_t v51;
        float_t v52;
        float_t v55;
        vec3_distance_squared(*a3, coli->bone0_pos, v48);
        vec3_distance_squared(coli->bone0_pos, v57, v51);
        vec3_distance_squared(coli->bone1_pos, v56, v52);
        vec3_distance_squared(*a2, v59, v55);
        *a1 = *a2;

        if (v55 > v48) {
            *a1 = *a3;
            v55 = v48;
        }

        if (v55 > v51) {
            *a1 = v57;
            v55 = v51;
        }

        if (v55 > v52)
            *a1 = v56;
    }
    else {
        float_t v25;
        vec3_length(v56, v25);
        if (v25 != 0.0f)
            vec3_mult_scalar(v56, 1.0f / v25, v56);

        float_t v30 = 1.0f / coli->bone_pos_diff_length;
        vec3 v61;
        float_t v34;
        vec3_mult_scalar(coli->bone_pos_diff, v30, v61);
        vec3_dot(v61, v56, v34);
        vec3_mult_scalar(v61, v34, v61);
        vec3_sub(v61, v56, v61);
        vec3_sub(coli->bone0_pos, *a2, v60);
        float_t v35;
        vec3_dot(v61, v60, v35);
        v35 /= (v34 * v34 - 1.0f);
        if (v35 < 0.0f)
            *a1 = *a2;
        else if (v35 <= v25) {
            vec3_mult_scalar(v56, v35, v56);
            vec3_add(*a2, v56, *a1);
        }
        else
            *a1 = *a3;

    }
}

static int32_t sub_1404844A0(vec3* a1, vec3* a2, vec3* a3, osage_coli* a4, float_t radius) {
    vec3 v8;
    sub_140484850(&v8, a2, a3, a4);
    return sub_140484540(a1, &v8, a4, radius);
}

static int32_t sub_1404844F0(vec3* a1, vec3* a2, vec3* a3, osage_coli* a4, float_t radius) {
    vec3 v8;
    sub_140484850(&v8, a2, a3, a4);
    return sub_140483EA0(a1, &v8, a4, radius);
}

static int32_t sub_140485000(vec3* a1, vec3* a2, skin_param_osage_node* a3, osage_coli* a4) {
    if (!a4)
        return 0;

    int32_t v8 = 0;
    while (a4->type) {
        int32_t type = a4->type;
        vec3 v21 = vec3_null;
        switch (a4->type) {
        case SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_BALL:
            v8 += sub_140484450(&v21, a1, a2, &a4->bone0_pos, a3->coli_r + a4->radius);
            break;
        case SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_CYLINDER:
            v8 += sub_1404844A0(&v21, a1, a2, a4, a3->coli_r + a4->radius);
            break;
        case SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_ELLIPSE:
            v8 += sub_1404844F0(&v21, a1, a2, a4, a3->coli_r + a4->radius);
            break;
        }

        if (v8 > 0) {
            vec3_add(*a1, v21, *a1);
            vec3_add(*a2, v21, *a2);
        }
        a4++;
    }
    return v8;
}

static void sub_14047D620(rob_osage* rob_osg, float_t step) {
    if (step < 0.0f && rob_osg->field_1F0F)
        return;

    vec3 v7 = rob_osg->nodes.data()[0].trans;
    osage_coli* coli = rob_osg->coli;
    osage_coli* coli_ring = rob_osg->coli_ring;
    int32_t coli_type = rob_osg->skin_param_ptr->coli_type;
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++) {
        rob_osage_node_data* data = i->data_ptr;
        for (rob_osage_node*& j : data->boc) {
            float_t v17 = (float_t)(
                sub_140485000(&i->trans, &j->trans, &data->skp_osg_node, coli)
                + sub_140485000(&i->trans, &j->trans, &data->skp_osg_node, coli_ring));
            i->field_C8 += v17;
            j->field_C8 += v17;
        }

        if (coli_type && (coli_type != 1 || i != i_begin)) {
            float_t v20 = (float_t)(
                sub_140485000(&i[0].trans, &i[-1].trans, &data->skp_osg_node, coli)
                + sub_140485000(&i[0].trans, &i[-1].trans, &data->skp_osg_node, coli_ring));
            i[0].field_C8 += v20;
            i[-1].field_C8 += v20;
        }
    }
    rob_osg->nodes.data()[0].trans = v7;
}

static void sub_14047F990(rob_osage* rob_osg, mat4* a2, vec3* a3, bool a4) {
    if (!rob_osg->nodes.size())
        return;

    vec3 v76;
    vec3_mult(rob_osg->exp_data.position, *a3, v76);
    mat4_mult_vec3_trans(a2, &v76, &v76);
    vec3 v74 = v76;
    rob_osage_node* v12 = &rob_osg->nodes[0];
    v12->trans = v76;
    v12->trans_orig = v76;
    v12->trans_diff = vec3_null;

    float_t ring_height;
    rob_osage_node* v14 = &rob_osg->nodes[0];
    float_t coli_r = v14->data_ptr->skp_osg_node.coli_r;
    if (v14->trans.x < rob_osg->ring.ring_rectangle_x - coli_r
        || v14->trans.z < rob_osg->ring.ring_rectangle_y - coli_r
        || v14->trans.x > rob_osg->ring.ring_rectangle_x + rob_osg->ring.ring_rectangle_width
        || v14->trans.z > rob_osg->ring.ring_rectangle_y + rob_osg->ring.ring_rectangle_height)
        ring_height = rob_osg->ring.ring_out_height;
    else
        ring_height = rob_osg->ring.ring_height;

    mat4 v78 = *a2;
    sub_14047F110(rob_osg, &v78, a3, true);
    vec3 v60 = { 1.0f, 0.0f, 0.0f };
    mat4_mult_vec3(&v78, &v60, &v60);

    osage_coli* coli_ring = rob_osg->coli_ring;
    osage_coli* coli = rob_osg->coli;
    float_t v25 = ring_height + coli_r;
    float_t v16 = a3->x;
    rob_osage_node* i = rob_osg->nodes.data();
    rob_osage_node* j_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* j_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* j = j_begin; j != j_end; i++, j++) {
        vec3_normalize(v60, v60);
        vec3_mult_scalar(v60, v16 * j->length, v60);
        vec3_add(i->trans, v60, v74);
        if (a4 && j->sibling_node)
            sub_140482F30(&v74, &j->sibling_node->trans, j->max_distance);

        skin_param_osage_node* v38 = &j->data_ptr->skp_osg_node;
        sub_140485180(coli_ring, &v74, v38);
        sub_140485180(coli, &v74, v38);

        float_t v39 = v25 + v38->coli_r;
        if (v74.y < v39 && v39 < 1001.0f) {
            v74.y = v39;
            vec3_sub(v74, i->trans, v74);
            vec3_normalize(v74, v74);
            vec3_mult_scalar(v74, v16 * j->length, v74);
            vec3_add(v74, i->trans, v74);
        }
        j->trans = v74;
        j->trans_diff = vec3_null;
        vec3 v77;
        mat4_mult_vec3_inv_trans(&v78, &j->trans, &v77);
        sub_140482FF0(&v78, &v77, &j->data_ptr->skp_osg_node.hinge,
            &j->reset_data.rotation, &rob_osg->yz_order);
        j->bone_node_ptr->exp_data.parent_scale = *a3;
        *j->bone_node_ptr->ex_data_mat = v78;
        if (j->bone_node_mat)
            mat4_scale_rot(&v78, a3->x, a3->y, a3->z, j->bone_node_mat);

        float_t v55;
        vec3_distance(j->trans, i->trans, v55);
        float_t v56 = v16 * j->length;
        if (v55 >= fabsf(v56))
            v56 = v55;
        mat4_translate_mult(&v78, v56, 0.0f, 0.0f, &v78);
        mat4_get_translation(&v78, &j->trans);
        vec3_sub(j->trans, i->trans, v60);
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v79 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_translate_mult(&v79, v16 * rob_osg->node.length, 0.0f, 0.0f, &v79);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v79;
        mat4_scale_rot(&v79, a3->x, a3->y, a3->z, &v79);
        *rob_osg->node.bone_node_mat = v79;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *a3;
    }
}

static bool sub_14053D1B0(vec3* l_trans, vec3* r_trans,
    vec3* u_trans, vec3* d_trans, vec3* a5, vec3* a6, vec3* a7) {
    float_t length;
    vec3_sub(*d_trans, *u_trans, *a5);
    vec3_length_squared(*a5, length);
    if (length <= 0.000001f)
        return false;
    vec3_mult_scalar(*a5, 1.0f / sqrtf(length), *a5);

    vec3 v9;
    vec3_sub(*r_trans, *l_trans, v9);
    vec3_cross(v9, *a5, *a6);
    vec3_length_squared(*a6, length);
    if (length <= 0.000001f)
        return false;
    vec3_mult_scalar(*a6, 1.0f / sqrtf(length), *a6);

    vec3_cross(*a5, *a6, *a7);
    vec3_normalize(*a7, *a7);
    return true;
}

static void sub_14053CE30(rob_osage_node_data_normal_ref* normal_ref, mat4* a2) {
    if (!normal_ref->field_0)
        return;

    vec3 n_trans;
    vec3 u_trans;
    vec3 d_trans;
    vec3 l_trans;
    vec3 r_trans;
    mat4_get_translation(normal_ref->n->bone_node_ptr->ex_data_mat, &n_trans);
    mat4_get_translation(normal_ref->u->bone_node_ptr->ex_data_mat, &u_trans);
    mat4_get_translation(normal_ref->d->bone_node_ptr->ex_data_mat, &d_trans);
    mat4_get_translation(normal_ref->l->bone_node_ptr->ex_data_mat, &l_trans);
    mat4_get_translation(normal_ref->r->bone_node_ptr->ex_data_mat, &r_trans);

    vec3 v27;
    vec3 v26;
    vec3 v28;
    if (sub_14053D1B0(&l_trans, &r_trans, &u_trans, &d_trans, &v27, &v26, &v28)) {
        mat4 v34;
        *(vec3*)&v34.row0 = v28;
        *(vec3*)&v34.row1 = v26;
        *(vec3*)&v34.row2 = v27;
        *(vec3*)&v34.row3 = n_trans;
        v34.row0.w = 0.0f;
        v34.row1.w = 0.0f;
        v34.row2.w = 0.0f;
        v34.row3.w = 1.0f;

        mat4 v33 = normal_ref->mat;
        mat4_mult(&v33, &v34, a2);
    }
}

static void sub_14047E1C0(rob_osage* rob_osg, vec3* scale) {
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++)
        if (i->data_ptr->normal_ref.field_0) {
            sub_14053CE30(&i->data_ptr->normal_ref, i->bone_node_mat);
            mat4_scale_rot(i->bone_node_mat, scale->x, scale->y, scale->z, i->bone_node_mat);
        }
}

static void sub_140482DF0(struc_477* dst, struc_477* src0, struc_477* src1, float_t blend) {
    dst->length = lerp(src0->length, src1->length, blend);
    vec3_lerp_scalar(src0->angle, src1->angle, dst->angle, blend);
}

static void sub_140482100(struc_476* a1, opd_blend_data* a2, struc_477* a3) {
    if (!a2->field_C)
        a1->field_0 = *a3;
    else if (a2->type == MOTION_BLEND_FREEZE) {
        if (a2->blend == 0.0f)
            a1->field_10 = a1->field_0;
        sub_140482DF0(&a1->field_0, &a1->field_10, a3, a2->blend);
    }
    else if (a2->type == MOTION_BLEND_CROSS)
        sub_140482DF0(&a1->field_0, &a1->field_0, a3, a2->blend);
}

static void sub_14047E240(rob_osage* rob_osg, mat4* a2, vec3* a3, std::vector<opd_blend_data>* a4) {
    if (!a4->size())
        return;

    vec3 v63;
    vec3_mult(rob_osg->exp_data.position, *a3, v63);

    mat4 v85;
    mat4_mult_vec3_trans(&v85, &v63, &rob_osg->nodes[0].trans);

    sub_14047F110(rob_osg, &v85, a3, 0);
    *rob_osg->nodes[0].bone_node_mat = v85;
    *rob_osg->nodes[0].bone_node_ptr->ex_data_mat = v85;

    size_t v17 = a4->size() - 1;
    for (auto i = a4->end(); i != a4->begin(); v17--) {
        i--;
        mat4 v87 = v85;
        float_t frame = i->frame;
        vec3 v22 = rob_osg->nodes[0].trans;
        vec3 v25 = v22;
        if (frame >= i->frame_count)
            frame = 0.0f;

        int32_t frame_int = (int32_t)frame;
        if (frame != -0.0f && (float_t)frame_int != frame)
            frame_int = (frame < 0.0f ? frame_int - 1 : frame_int);

        int32_t curr_key = frame_int;
        int32_t next_key = frame_int + 1;
        if ((float_t)next_key >= i->frame_count)
            next_key = 0;

        float_t blend = frame - (float_t)frame_int;
        float_t inv_blend = 1.0f - blend;
        rob_osage_node* j_begin = rob_osg->nodes.data() + 1;
        rob_osage_node* j_end = rob_osg->nodes.data() + rob_osg->nodes.size();
        for (rob_osage_node* j = j_begin; j != j_end; j++) {
            opd_vec3_data* opd = &j->opd_data[v17];
            float_t* opd_x = opd->x;
            float_t* opd_y = opd->y;
            float_t* opd_z = opd->z;

            vec3 v62;
            v62.x = opd_x[curr_key];
            v62.y = opd_y[curr_key];
            v62.z = opd_z[curr_key];

            vec3 v77;
            v77.x = opd_x[next_key];
            v77.y = opd_y[next_key];
            v77.z = opd_z[next_key];

            vec3 v84;
            mat4_mult_vec3_trans(a2, &v62, &v84);

            vec3 v83;
            mat4_mult_vec3_trans(a2, &v77, &v83);

            vec3 v82;
            vec3_mult_scalar(v83, blend, v82);

            vec3 v81;
            vec3_mult_scalar(v84, inv_blend, v81);
            vec3_add(v82, v81, v82);

            vec3 v86;
            mat4_mult_vec3_inv_trans(&v87, &v82, &v86);

            vec3 v64 = vec3_null;
            sub_140482FF0(&v87, &v86, 0, &v64, &rob_osg->yz_order);
            mat4_set_translation(&v87, &v82);

            float_t v50;
            vec3_distance(v83, v25, v50);

            float_t v49;
            vec3_distance(v84, v22, v49);

            struc_477 v72;
            v72.angle = v64;
            v72.length = v49 * inv_blend + v50 * blend;
            sub_140482100(&j->field_1B0, &i[0], &v72);

            v22 = v84;
            v25 = v83;
        }
    }

    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++) {
        float_t v54 = i->field_1B0.field_0.angle.y;
        float_t v55 = i->field_1B0.field_0.angle.z;
        v54 = clamp(v54, (float_t)-M_PI, (float_t)M_PI);
        v55 = clamp(v55, (float_t)-M_PI, (float_t)M_PI);
        mat4_rotate_z_mult(&v85, v55, &v85);
        mat4_rotate_y_mult(&v85, v54, &v85);
        i->bone_node_ptr->exp_data.parent_scale = *a3;
        *i->bone_node_ptr->ex_data_mat = v85;
        if (i->bone_node_mat) {
            mat4 mat = v85;
            mat4_scale_rot(&mat, a3->x, a3->y, a3->z, &mat);
            *i->bone_node_mat = mat;
        }
        mat4_translate_mult(&v85, i->field_1B0.field_0.length, 0.0f, 0.0f, &v85);
        i->trans_orig = i->trans;
        mat4_get_translation(&v85, &i->trans);
    }

    if (rob_osg->nodes.size() && rob_osg->node.bone_node_mat) {
        mat4 v87 = *rob_osg->nodes.back().bone_node_ptr->ex_data_mat;
        mat4_translate_mult(&v87, rob_osg->node.length * a3->x, 0.0f, 0.0f, &v87);
        *rob_osg->node.bone_node_ptr->ex_data_mat = v87;

        mat4_scale_rot(&v87, a3->x, a3->y, a3->z, &v87);
        *rob_osg->node.bone_node_mat = v87;
        rob_osg->node.bone_node_ptr->exp_data.parent_scale = *a3;
    }
}

static void rob_osage_add_motion_reset_data(rob_osage* rob_osg, int32_t motion_id, float_t frame) {
    int32_t frame_int = (int32_t)roundf(frame * 1000.0f);

    auto elem = rob_osg->motion_reset_data.find({ motion_id, frame_int });
    if (elem != rob_osg->motion_reset_data.end())
        rob_osg->motion_reset_data.erase(elem);

    std::list<rob_osage_node_reset_data> v28;
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++)
        v28.push_back(i->reset_data);

    rob_osg->motion_reset_data.insert({ { motion_id, frame_int }, v28 });
}

static void rob_osage_init_data(rob_osage* rob_osg,
    obj_skin_block_osage* osg_data, obj_skin_osage_node* osg_nodes,
    bone_node* ex_data_bone_nodes, obj_skin* skin) {
    rob_osg->reset();
    bone_node_expression_data_set_position_rotation_vec3(
        &rob_osg->exp_data,
        &osg_data->base.position,
        &osg_data->base.rotation);
    std::vector<rob_osage_node>& nodes = rob_osg->nodes;
    rob_osg->nodes.clear();
    rob_osg->nodes.resize(osg_data->count + 1ULL);

    rob_osage_node* external = &nodes[0];
    nodes[0].reset();
    external->child_length = osg_nodes->length;
    bone_node* external_bone_node = &ex_data_bone_nodes[osg_data->external_name_index & 0x7FFF];
    external->bone_node_ptr = external_bone_node;
    external->bone_node_mat = external_bone_node->mat;
    *external_bone_node->ex_data_mat = mat4_identity;
    external->sibling_node = 0;

    bone_node* parent_bone_node = external->bone_node_ptr;
    for (uint32_t i = 0; i < osg_data->count; i++) {
        obj_skin_osage_node* v14 = &osg_nodes[i];
        rob_osage_node* node = &nodes[i + 1LL];
        node->reset();
        node->length = v14[0].length;
        if (i != osg_data->count - 1)
            node->child_length = v14[1].length;
        bone_node* node_bone_node = &ex_data_bone_nodes[v14->name_index & 0x7FFF];
        node->bone_node_ptr = node_bone_node;
        node->bone_node_mat = node_bone_node->mat;
        node_bone_node->parent = parent_bone_node;
        parent_bone_node = node->bone_node_ptr;
    }

    rob_osg->node.reset();
    rob_osg->node.length = osg_nodes[osg_data->count - 1].length;
    bone_node* v22 = &ex_data_bone_nodes[osg_data->name_index & 0x7FFF];
    rob_osg->node.bone_node_ptr = v22;
    rob_osg->node.bone_node_mat = v22->mat;
    v22->parent = parent_bone_node;
    *rob_osg->node.bone_node_ptr->ex_data_mat = mat4_identity;

    if (osg_data->count) {
        obj_skin_osage_node* v23 = osg_data->nodes;
        size_t v24 = 0;
        rob_osage_node* v26 = &nodes[1];
        for (size_t i = 0; i < osg_data->count; i++) {
            vec3 v27 = vec3_null;
            if (v23)
                v27 = v23[i].rotation;

            mat4 mat;
            mat4_rotate(v27.x, v27.y, v27.z, &mat);

            vec3 v49;
            v49 = vec3_null;
            v49.x = v26[i].length;
            mat4_mult_vec3(&mat, &v49, &v26[i].field_94);
        }
    }

    if (osg_data->count) {
        obj_skin_osage_node* v23 = osg_data->nodes;
        rob_osage_node* v26 = &nodes[1];
        size_t v33 = 0;
        for (uint32_t i = 0; i < osg_data->count; i++) {
            v26[i].mat = mat4_null;

            obj_skin_bone* v36 = skin->bones;
            for (uint32_t j = 0; j < skin->bones_count; j++, v36++)
                if (v36->id == osg_nodes->name_index) {
                    mat4 mat = skin->bones[j].inv_bind_pose_mat;
                    mat4_inverse_normalized(&mat, &mat);
                    v26[i].mat = mat;
                    break;
                }

            osg_nodes++;
        }
    }

    rob_osage_node* v38 = &rob_osg->nodes.back();
    if (memcmp(&v38->mat, &mat4_null, sizeof(mat4))) {
        mat4 mat = v38->mat;
        mat4_translate_mult(&mat, rob_osg->node.length, 0.0f, 0.0f, &mat);
        rob_osg->node.mat = mat;
    }
}

static bool check_module_index_is_501(int32_t module_index) {
    if (module_index == -1 || module_index >= 502)
        return false;
    return module_index == 501;
}

static float_t ex_expression_block_stack_get_value(ex_expression_block_stack* stack) {
    float_t value = 0.0f;
    switch (stack->type) {
    case EX_EXPRESSION_BLOCK_STACK_NUMBER:
        value = stack->number.value;
        break;
    case EX_EXPRESSION_BLOCK_STACK_VARIABLE:
        value = *stack->var.value;
        break;
    case EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN:
        value = *stack->var_rad.value * RAD_TO_DEG_FLOAT;
        break;
    case EX_EXPRESSION_BLOCK_STACK_OP1: {
        float_t v1 = ex_expression_block_stack_get_value(stack->op1.v1);
        if (stack->op1.func)
            value = stack->op1.func(v1);
    } break;
    case EX_EXPRESSION_BLOCK_STACK_OP2: {
        float_t v1 = ex_expression_block_stack_get_value(stack->op2.v1);
        float_t v2 = ex_expression_block_stack_get_value(stack->op2.v2);
        if (stack->op2.func)
            value = stack->op2.func(v1, v2);
    } break;
    case EX_EXPRESSION_BLOCK_STACK_OP3: {
        float_t v1 = ex_expression_block_stack_get_value(stack->op1.v1);
        float_t v2 = ex_expression_block_stack_get_value(stack->op2.v2);
        float_t v3 = ex_expression_block_stack_get_value(stack->op3.v3);
        if (stack->op3.func)
            value = stack->op3.func(v1, v2, v3);
    } break;
    }
    return value;
}

static float_t exp_abs(float_t v1) {
    return fabsf(v1);
}

static float_t exp_acos(float_t v1) {
    return acosf(v1) * RAD_TO_DEG_FLOAT;
}

static float_t exp_add(float_t v1, float_t v2) {
    return v1 + v2;
}

static float_t exp_and(float_t v1, float_t v2) {
    return v1 != 0.0f && v2 != 0.0f ? 1.0f : 0.0f;
}

static float_t exp_asin(float_t v1) {
    return asinf(v1) * RAD_TO_DEG_FLOAT;
}

static float_t exp_atan(float_t v1) {
    return atanf(v1) * RAD_TO_DEG_FLOAT;
}

static float_t exp_av(float_t v1) {
    return v1 * 0.1f;
}

static float_t exp_ceil(float_t v1) {
    return ceilf(v1);
}

static float_t exp_cond(float_t v1, float_t v2, float_t v3) {
    return v1 != 0.0f ? v2 : v3;
}

static float_t exp_cos(float_t v1) {
    return cosf(fmodf(v1, 360.0f) * DEG_TO_RAD_FLOAT);
}

static float_t exp_div(float_t v1, float_t v2) {
    return v1 / v2;
}

static float_t exp_eq(float_t v1, float_t v2) {
    return v1 == v2 ? 1.0f : 0.0f;
}

static float_t exp_exp(float_t v1) {
    return expf(v1);
}

static float_t exp_floor(float_t v1) {
    return floorf(v1);
}

static float_t exp_fmod(float_t v1, float_t v2) {
    return fmodf(v1, v2);
}

static const exp_func_op1* exp_func_op1_find_func(string* name, const exp_func_op1* array) {
    char* name_str = string_data(name);
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static const exp_func_op2* exp_func_op2_find_func(string* name, const exp_func_op2* array) {
    char* name_str = string_data(name);
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static const exp_func_op3* exp_func_op3_find_func(string* name, const exp_func_op3* array) {
    char* name_str = string_data(name);
    while (array->name) {
        if (!str_utils_compare(name_str, array->name))
            return array;
        array++;
    }
    return 0;
}

static float_t exp_ge(float_t v1, float_t v2) {
    return v1 >= v2 ? 1.0f : 0.0f;
}

static float_t exp_gt(float_t v1, float_t v2) {
    return v1 > v2 ? 1.0f : 0.0f;
}

static float_t exp_le(float_t v1, float_t v2) {
    return v1 <= v2 ? 1.0f : 0.0f;
}

static float_t exp_log(float_t v1) {
    return logf(v1);
}

static float_t exp_lt(float_t v1, float_t v2) {
    return v1 < v2 ? 1.0f : 0.0f;
}

static float_t exp_max(float_t v1, float_t v2) {
    return max(v1, v2);
}

static float_t exp_min(float_t v1, float_t v2) {
    return min(v1, v2);
}

static float_t exp_mul(float_t v1, float_t v2) {
    return v1 * v2;
}

static float_t exp_ne(float_t v1, float_t v2) {
    return v1 != v2 ? 1.0f : 0.0f;
}

static float_t exp_neg(float_t v1) {
    return -v1;
}

static float_t exp_or(float_t v1, float_t v2) {
    return v1 != 0.0f || v2 != 0.0f ? 1.0f : 0.0f;
}

static float_t exp_pow(float_t v1, float_t v2) {
    return powf(v1, v2);
}

static float_t exp_rand(float_t v1, float_t v2, float_t v3) {
    return v1 != 0.0f ? v2 : v3;
}

static float_t exp_rand_0_1(float_t v1) {
    return 0.0f;
}

static float_t exp_round(float_t v1) {
    return roundf(v1);
}

static float_t exp_sin(float_t v1) {
    return sinf(fmodf(v1, 360.0f) * DEG_TO_RAD_FLOAT);
}

static float_t exp_sqrt(float_t v1) {
    return sqrtf(v1);
}

static float_t exp_sub(float_t v1, float_t v2) {
    return v1 - v2;
}

static float_t exp_tan(float_t v1) {
    return tanf(fmodf(v1, 360.0f) * DEG_TO_RAD_FLOAT);
}

static const float_t get_osage_gravity_const() {
    return 0.00299444468691945f;
}

static bool sub_140410250(struc_313* a1, size_t a2) {
    return !!(a1->bitfield.data()[a2 >> 5] & (1 << (a2 & 0x1F)));
}

static void mot_blend_interpolate(mot_blend* a1, std::vector<bone_data>* bones,
    std::vector<uint16_t>* bone_indices, bone_database_skeleton_type skeleton_type) {
    if (!a1->mot_key_data.key_sets_ready || !a1->mot_key_data.mot_data || a1->field_30)
        return;

    float_t frame = a1->mot_play_data.frame_data.frame;
    vec3* keyframe_data = (vec3*)a1->mot_key_data.key_set_data.data();
    bone_data* bones_data = bones->data();
    for (uint16_t& i : *bone_indices) {
        bone_data* data = &bones_data[i];
        bool get_data = sub_140410250(&a1->field_0.field_8, data->motion_bone_index);
        if (get_data) {
            mot_key_data_interpolate(&a1->mot_key_data, frame, data->key_set_offset, data->key_set_count);
            data->frame = frame;
        }
        keyframe_data = bone_data_set_key_data(data, keyframe_data, skeleton_type, get_data, 0);
    }
}

static void mot_blend_load_motion(mot_blend* a1, int32_t motion_id, motion_database* mot_db) {
    mot_key_data_load_file(&a1->mot_key_data, motion_id, mot_db);
    a1->blend.Reset();
}

static void sub_14040F650(struc_313* a1, size_t a2) {
    size_t v7 = (a2 + 31) >> 5;
    if (v7 < a1->bitfield.size())
        a1->bitfield.resize(v7);
    a1->motion_bone_count = a2;

    int32_t v9 = (int32_t)a2 & 0x1F;
    if (v9)
        a1->bitfield.data()[v7 - 1] &= (1 << v9) - 1;
}

static void sub_1404122C0(struc_313* a1, struc_314* a2, struc_314* a3, struc_314* a4) {
    uint32_t* bitfield = a1->bitfield.data();
    uint32_t* v4 = bitfield;
    uint32_t* v5 = a3->field_0;
    size_t v6 = a3->field_8;
    size_t v7 = a1->motion_bone_count;
    size_t v8 = 0;

    struc_314 v25;
    v25.field_0 = bitfield;
    v25.field_8 = 0;
    if (v7) {
        size_t v12 = v6 + 32 * (v5 - v4);
        v25.field_0 = &bitfield[v12 >> 5];
        v25.field_8 = v12 & 0x1F;
    }

    struc_314 v26;
    v26.field_0 = bitfield;
    v26.field_8 = 0;
    if (v7) {
        size_t v17 = a4->field_8 + 32 * (a4->field_0 - v4);
        v26.field_0 = &v4[v17 >> 5];
        v26.field_8 = v17 & 0x1F;
    }

    size_t v18 = v25.field_8 + 32 * (v25.field_0 - v4);
    if (v25.field_0 != v26.field_0 || v25.field_8 != v26.field_8) {
        size_t v19 = 0;
        if (v7) {
            v4 += v7 >> 5;
            v19 = a1->motion_bone_count & 0x1F;
        }

        size_t v20 = v26.field_8;
        uint32_t* v21 = v26.field_0;
        size_t v22 = v25.field_8;
        uint32_t* v23 = v25.field_0;
        while (v21 != v4 || v20 != v19) {
            if ((1 << v20) & *v21)
                *v23 |= 1 << v22;
            else
                *v23 &= ~(1 << v22);
            if (v22 >= 0x1F) {
                v22 = 0;
                v23++;
            }
            else
                v22++;

            if (v20 >= 0x1F) {
                v20 = 0;
                v21++;
            }
            else
                v20++;
        }
        sub_14040F650(a1, v22 + 32 * (v23 - a1->bitfield.data()));
    }

    a2->field_0 = &a1->bitfield.data()[v18 >> 5];
    a2->field_8 = v18 & 0x1F;
}

static void sub_1404119A0(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.data();
    struc_314 v3;
    if (a1->motion_bone_count) {
        v3.field_0 = &bitfield[a1->motion_bone_count >> 5];
        v3.field_8 = a1->motion_bone_count & 0x1F;
    }
    else {
        v3.field_0 = bitfield;
        v3.field_8 = 0;
    }

    struc_314 v4;
    v4.field_0 = bitfield;
    v4.field_8 = 0;

    struc_314 v5;
    sub_1404122C0(a1, &v5, &v4, &v3);
}

static void mot_blend_set_duration(mot_blend* a1, float_t duration, float_t step, float_t offset) {
    a1->blend.SetDuration(duration, step, offset);
}

static void mot_blend_set_frame(mot_blend* a1, float_t frame) {
    mot_play_frame_data_set_frame(&a1->mot_play_data.frame_data, frame);
}

static void mot_key_frame_interpolate(mot* a1, float_t frame, float_t* value,
    mot_key_set* a4, uint32_t key_set_count, struc_369* a6) {
    if (a6->field_0 == 4) {
        int32_t frame_int = (int32_t)frame;
        if (frame != -0.0f && (float_t)frame_int != frame)
            frame = (float_t)(frame < 0.0f ? frame_int - 1 : frame_int);
    }

    if (!key_set_count)
        return;

    for (uint32_t i = key_set_count; i; i--, a4++, value++) {
        mot_key_set_type type = a4->type;
        const float_t* values = a4->values;
        if (type == MOT_KEY_SET_NONE) {
            *value = 0.0f;
            continue;
        }
        else if (type == MOT_KEY_SET_STATIC) {
            *value = values[0];
            continue;
        }
        else if (type != MOT_KEY_SET_HERMITE && type != MOT_KEY_SET_HERMITE_TANGENT) {
            *value = 0.0f;
            continue;
        }

        int32_t current_key = a4->current_key;
        int32_t keys_count = a4->keys_count;
        const uint16_t* frames = a4->frames;
        int32_t frame_int = (int32_t)frame;
        size_t key_index;
        if (current_key > keys_count
            || (frame_int > a4->last_key + frames[current_key])
            || current_key > 0 && frame_int < frames[current_key - 1]) {
            const uint16_t* key = frames;
            size_t length = keys_count;
            size_t temp;
            while (length > 0)
                if (frame_int < key[temp = length / 2])
                    length /= 2;
                else {
                    key += temp + 1;
                    length -= temp + 1;
                }
            key_index = key - frames;
        }
        else {
            const uint16_t* key = &frames[current_key];
            for (const uint16_t* v17 = &frames[keys_count]; key != v17; key++)
                if (frame_int < *key)
                    break;
            key_index = key - frames;
        }

        bool found = false;
        if (key_index < keys_count)
            for (; key_index < keys_count; key_index++)
                if ((float_t)frames[key_index] >= frame) {
                    found = true;
                    break;
                }

        if (found) {
            float_t next_frame = (float_t)frames[key_index];
            if (fabsf(next_frame - frame) > 0.000001f && key_index > 0) {
                float_t curr_frame = (float_t)frames[key_index - 1];
                float_t t = (frame - curr_frame) / (next_frame - curr_frame);
                if (type == MOT_KEY_SET_HERMITE) {
                    values += key_index - 1;
                    float_t p1 = values[0];
                    float_t p2 = values[1];
                    *value = (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
                }
                else {
                    values += key_index * 2 - 2;
                    float_t p1 = values[0];
                    float_t p2 = values[2];
                    float_t t1 = values[1];
                    float_t t2 = values[3];
                    *value = ((t - 1.0f) * t1 + t * t2) * (t - 1.0f) * (frame - curr_frame)
                        + (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
                }
                a4->current_key = (int32_t)key_index;
                continue;
            }
        }

        if (key_index > 0)
            key_index--;
        if (type == MOT_KEY_SET_HERMITE)
            *value = values[key_index];
        else
            *value = values[2 * key_index];
        a4->current_key = (int32_t)key_index;
    }
}

static uint32_t mot_load_last_key_calc(uint16_t keys_count) {
    uint16_t v2 = keys_count >> 8;
    uint32_t v3 = 16;
    if (v2) {
        v3 = 8;
        keys_count = v2;
    }

    if (keys_count >> 4) {
        v3 -= 4;
        keys_count >>= 4;
    }

    if (keys_count >> 2) {
        v3 -= 2;
        keys_count >>= 2;
    }

    if (keys_count & ~1)
        keys_count = 2;
    return v3 - keys_count;
}

static bool mot_data_load_file(mot* a1, const mot_data* a2) {
    a1->frame_count = 0;
    if (!a2)
        return 0;

    a1->frame_count = a2->frame_count;

    int32_t key_set_count = a2->key_set_count;
    bool skeleton_select = a2->skeleton_select != 0;
    a1->key_set_count = key_set_count;
    a1->field_4 = a2->info & 0x8000;

    const mot_key_set_data* key_set_file = a2->key_set.data();
    mot_key_set* key_set = a1->key_sets;

    for (int32_t i = 0; i < key_set_count; i++, key_set++, key_set_file++) {
        key_set->type = key_set_file->type;
        if (key_set->type == MOT_KEY_SET_STATIC)
            key_set->values = key_set_file->values.data();
        else if (key_set->type != MOT_KEY_SET_NONE) {
            uint16_t keys_count = key_set_file->keys_count;
            key_set->frames = key_set_file->frames.data();
            key_set->values = key_set_file->values.data();

            key_set->keys_count = keys_count;
            key_set->current_key = 0;

            uint16_t last_key = a1->frame_count;
            if (keys_count > 1) {
                if (keys_count > last_key)
                    keys_count = a1->frame_count;
                last_key = (16 - mot_load_last_key_calc(keys_count)) * a1->frame_count / keys_count;
            }
            key_set->last_key = last_key;
        }
    }
    return skeleton_select;
}

static void mot_key_data_get_key_set_count_by_bone_database_bones(mot_key_data* a1,
    std::vector<bone_database_bone>* a2) {
    size_t object_bone_count;
    size_t motion_bone_count;
    size_t total_bone_count;
    size_t ik_bone_count;
    size_t chain_pos;
    bone_database_bones_calculate_count(a2, &object_bone_count,
        &motion_bone_count, &total_bone_count, &ik_bone_count, &chain_pos);
    mot_key_data_get_key_set_count(a1, motion_bone_count, ik_bone_count);
}

static void mot_key_data_get_key_set_count(mot_key_data* a1, size_t motion_bone_count, size_t ik_bone_count) {
    a1->key_set_count = (motion_bone_count + ik_bone_count) * 3 + 16;
}

static void mot_key_data_init_key_sets(mot_key_data* a1, bone_database_skeleton_type type,
    size_t motion_bone_count, size_t ik_bone_count) {
    mot_key_data_get_key_set_count(a1, motion_bone_count, ik_bone_count);
    mot_key_data_reserve_key_sets(a1);
    a1->skeleton_type = type;
}

static void mot_key_data_interpolate(mot_key_data* a1, float_t frame,
    uint32_t key_set_offset, uint32_t key_set_count) {
    mot_key_frame_interpolate(&a1->mot, frame,
        &a1->key_set_data.data()[key_set_offset],
        &a1->key_set.data()[key_set_offset], key_set_count, &a1->field_68);
}

static const mot_data* mot_key_data_load_file(mot_key_data* a1, int32_t motion_id, motion_database* mot_db) {
    if (a1->motion_id != motion_id) {
        const mot_data* mot_data = motion_storage_get_mot_data(motion_id, mot_db);
        a1->mot_data = mot_data;
        if (mot_data) {
            a1->skeleton_select = mot_data_load_file(&a1->mot, mot_data);
            a1->motion_id = motion_id;
        }
        else {
            a1->skeleton_select = 0;
            a1->motion_id = -1;
        }
    }
    a1->frame = -1.0f;
    a1->field_68.field_0 = 0;
    a1->field_68.field_4 = 0.0f;
    return a1->mot_data;
}

static void mot_key_data_reserve_key_sets(mot_key_data* a1) {
    a1->key_set.clear();
    a1->key_set.resize(a1->key_set_count);
    a1->key_set_data.clear();
    a1->key_set_data.resize(a1->key_set_count);

    a1->mot.key_sets = a1->key_set.data();
    a1->key_sets_ready = true;
}

static void mot_play_frame_data_set_frame(mot_play_frame_data* a1, float_t frame) {
    float_t v2 = a1->field_14;
    a1->frame = frame;
    a1->field_28 = false;
    if (v2 >= 0.0f && (a1->field_18 == 2 && frame <= v2) || frame >= v2)
        a1->field_14 = -1.0f;
}

static void mothead_func_0(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_1(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_2(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr_data->field_1588.field_0.field_20.field_0 |= 0x100;
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    if (rob_chr_data->motion.field_28 & 8)
        rob_chr_data->field_1588.field_0.field_274 -= ((int16_t*)data)[0];
    else
        rob_chr_data->field_1588.field_0.field_274 += ((int16_t*)data)[0];
}

static void mothead_func_3(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    int32_t v4 = ((int32_t*)data)[0];
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    (&rob_chr_data->field_1588.field_0.field_20.field_0)[v4 >> 5] |= 1 << (v4 & 0x1F);
}

static void mothead_func_4(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    int32_t v4 = ((int32_t*)data)[0];
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    (&rob_chr_data->field_1588.field_0.field_20.field_0)[v4 >> 5] &= ~(1 << (v4 & 0x1F));
}

static void mothead_func_5(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    rob_chr_data->motion.field_28 ^= (rob_chr_data->motion.field_28 ^ ~rob_chr_data->motion.field_28) & 0x04;
}

static void mothead_func_6(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr_data->field_8.field_1B4 = 0;
    func_data->rob_chr_data->field_1588.field_0.field_20.field_0 |= 0x400;
    func_data->rob_chr_data->field_1588.field_0.field_20.field_0 |= 0x800;
}

static void sub_140551AF0(rob_chara* rob_chr) {
    if (~rob_chr->data.field_0 & 0x02) {
        rob_chr->data.miku_rot.position.y = rob_chr->data.miku_rot.field_24.y;
        rob_chr->data.field_0 |= 0x02;
        if (~(rob_chr->data.field_1588.field_0.field_1E8 & 0x01)) {
            rob_chr->data.miku_rot.position.x = rob_chr->data.miku_rot.field_24.x;
            rob_chr->data.miku_rot.position.z = rob_chr->data.miku_rot.field_24.z;
        }
    }
    rob_chr->data.field_1588.field_0.field_20.field_0 |= 0x40;
}

static void mothead_func_7(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    int16_t v4 = ((int16_t*)data)[0];
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    if (rob_chr_data->motion.field_28 & 0x08)
        v4 = -v4;
    float_t v8 = (float_t)((double_t)(int16_t)(v4 + rob_chr_data->field_1588.field_0.field_2B8
        + rob_chr_data->miku_rot.rot_y_int16) * M_PI * (1.0 / 32768.0));
    float_t v9 = sinf(v8) * ((float_t*)data)[1];
    float_t v10 = get_osage_gravity_const() * ((float_t*)data)[2];
    float_t v11 = cosf(v8) * ((float_t*)data)[1];
    rob_chr_data->miku_rot.field_30.x = v9;
    rob_chr_data->miku_rot.field_30.y = v10;
    rob_chr_data->miku_rot.field_30.z = v11;
    sub_140551AF0(func_data->rob_chr);
}

static void mothead_func_8(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    //sub_14036D130(0, &((int16_t*)data)[2]);
}

static void mothead_func_9(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_10(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_11(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_12(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_13(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    /*rob_chara* rob_chr; // r8
    rob_chara_data* v5; // r9
    int32_t v6; // ecx
    int32_t v7; // eax
    char* v8; // rax
    char v9; // al
    int64_t v10; // [rsp+20h] [rbp-28h] BYREF
    int32_t v11; // [rsp+28h] [rbp-20h]
    __int16 v12; // [rsp+2Ch] [rbp-1Ch]
    bool v13; // [rsp+2Eh] [rbp-1Ah]
    int32_t v14; // [rsp+30h] [rbp-18h]

    rob_chr = func_data->rob_chr;
    v5 = func_data->rob_chr_data;
    v6 = ((int32_t*)data)[1];
    v10 = -1i64;
    v11 = -1;
    v12 = 0;
    v13 = 0;
    v14 = 0;
    LODWORD(v10) = rob_chr->chara_id;
    v11 = ((int32_t*)data)[0];
    v12 = (v5->motion.field_28 & 8) != 0;
    if (v6 & 0x01) {
        v8 = (char*)rob_chr->field_20;
        if (!v8)
            return;
        v7 = *v8;
    }
    else
        v7 = rob_chr->chara_id;
    HIDWORD(v10) = v7;
    v9 = v5->field_8.field_150;
    HIBYTE(v12) = v9;
    if ((v6 & 2) != 0)
        HIBYTE(v12) = v9 == 0;
    v14 = ((int32_t*)data)[2];
    v13 = (v6 & 4) != 0;
    sub_1401FCE40((int64_t)&v10);*/
}

static void mothead_func_14(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_15(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_16(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_17(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    /*__int16 v4; // r14
    rob_chara_data* v5; // rsi
    float_t v8; // xmm3_4
    float_t v9; // xmm3_4
    float_t v10; // xmm0_4
    float_t* v11; // rbx
    float_t v12; // xmm3_4
    float_t v13; // xmm0_4
    float_t v14; // xmm0_4
    float_t v15; // xmm4_4
    float_t v16; // xmm6_4
    float_t v17; // xmm5_4
    float_t v18; // xmm6_4
    int32_t v19; // eax
    __int16* v20; // rcx
    float_t v21; // xmm2_4
    float_t v22; // xmm0_4
    rob_chara_data* v23; // rax
    float_t v24; // xmm1_4
    float_t v25; // xmm3_4
    rob_chara_data* v26; // rax
    float_t v27; // xmm3_4
    float_t v28; // xmm0_4
    __m128 v29; // xmm2
    float_t v30; // xmm0_4
    float_t v31; // xmm1_4
    rob_chara_data* v32; // rax
    float_t v33; // xmm2_4
    float_t v34; // xmm1_4
    float_t v35; // xmm0_4
    rob_chara_data* v36; // r14
    float_t v37; // xmm0_4
    vec3 a3; // [rsp+20h] [rbp-30h] BYREF
    vec3 v39; // [rsp+30h] [rbp-20h] BYREF
    __int16 a1; // [rsp+70h] [rbp+20h] BYREF

    v4 = ((int16_t*)data)[1];
    v5 = func_data->rob_chr_data;
    if (v4 & 0x02) {
        v8 = v5->field_1E68.field_1EB4[0].field_0;
        a1 = v5->miku_rot.field_2;
        if (v8 > 4.0)
            return;
        v9 = v8 - ((float_t*)data)[5];
        v10 = ((float_t*)data)[1];
        if (v9 < v10 && v10 >= 0.0f || (v10 = ((float_t*)data)[2], v9 > v10) && v10 >= 0.0f)
            v9 = v10;
    LABEL_8:
        v11 = &v5->field_1588.field_330.field_20.x;
        sub_1405357B0(&a1, &v5->miku_rot.field_24, &v5->field_1588.field_330.field_20, v9);
        v5->field_1588.field_330.field_18 = 1;
        goto LABEL_27;
    }
    if (v4 & 0x04) {
        v12 = v5->field_1E68.field_1EB4[2].field_0;
        a1 = v5->miku_rot.field_2 + 0x8000;
        if (v12 > 4.0)
            return;
        v9 = v12 - ((float_t*)data)[5];
        v13 = ((float_t*)data)[1];
        if (v9 < v13 && v13 >= 0.0f || (v13 = ((float_t*)data)[2], v9 > v13) && v13 >= 0.0f)
            v9 = v13;
        goto LABEL_8;
    }
    if (~v4 & 0x01)
        return;
    v14 = ((float_t*)data)[2];
    v15 = ((float_t*)data)[3];
    v16 = ((float_t*)data)[5];
    v17 = ((float_t*)data)[4];
    v5->field_1588.field_330.field_18 = 2;
    v18 = -v16;
    if (v18 >= *((float_t*)data + 1)) {
        if (v14 < 0.0f) {
            if ((v4 & 0x20) != 0)
                v14 = ((float_t*)data)[1];
            v18 = v18 - (float_t)-v14;
        }
    }
    else
        v18 = ((float_t*)data)[1];
    v5->field_1588.field_330.field_20.z = v17;
    v5->field_1588.field_330.field_20.y = 0.0f;
    v11 = &v5->field_1588.field_330.field_20.x;
    *(int64_t*)&v39.x = 0i64;
    v39.z = 0.0f;
    v5->field_1588.field_330.field_20.x = v15;
    v19 = ((int16_t*)data)[0];
    *(int64_t*)&a3.x = 0i64;
    a3.z = 0.0f;
    sub_140533530(func_data, (float_t)v19, &a3);
    sub_140533530(func_data, func_data->rob_chr_data->motion.frame_data.frame, &v39);
    v20 = &func_data->rob_chr_data->miku_rot.field_2;
    a3.x = a3.x - v39.x;
    a3.z = a3.z - v39.z;
    sub_140535710(v20, &a3.x, &a3.x);
    a1 = func_data->field_10->miku_rot.field_2;
    sub_140535710(&a1, &v5->field_1588.field_330.field_20.x, &v5->field_1588.field_330.field_20.x);
    v21 = v5->field_1588.field_330.field_20.x - a3.x;
    v22 = v5->field_1588.field_330.field_20.z - a3.z;
    v5->field_1588.field_330.field_20.x = v21;
    v5->field_1588.field_330.field_20.z = v22;
    v23 = func_data->field_10;
    v24 = v23->miku_rot.field_24.y + v5->field_1588.field_330.field_20.y;
    v25 = v23->miku_rot.field_24.z + v5->field_1588.field_330.field_20.z;
    v5->field_1588.field_330.field_20.x = v21 + v23->miku_rot.field_24.x;
    v26 = func_data->rob_chr_data;
    v5->field_1588.field_330.field_20.y = v24;
    v5->field_1588.field_330.field_20.z = v25;
    v27 = v25 - v26->miku_rot.field_24.z;
    v28 = v5->field_1588.field_330.field_20.x - v26->miku_rot.field_24.x;
    v5->field_1588.field_330.field_20.y = v24 - v26->miku_rot.field_24.y;
    v5->field_1588.field_330.field_20.z = v27;
    v5->field_1588.field_330.field_20.x = v28;
    v5->field_1588.field_330.field_20.y = 0.0f;
    v29 = (__m128)LODWORD(v5->field_1588.field_330.field_20.x);
    v29.m128_f32[0] = (float_t)((float_t)(v29.m128_f32[0] * v29.m128_f32[0])
        + (float_t)(v5->field_1588.field_330.field_20.y * v5->field_1588.field_330.field_20.y))
        + (float_t)(v5->field_1588.field_330.field_20.z * v5->field_1588.field_330.field_20.z);
    if (_mm_sqrt_ps(v29).m128_f32[0] > v18) {
        vec3_normalize(&v5->field_1588.field_330.field_20);
        v30 = v18 * *v11;
        v31 = v18 * v5->field_1588.field_330.field_20.y;
        v5->field_1588.field_330.field_20.z = v18 * v5->field_1588.field_330.field_20.z;
        *v11 = v30;
        v5->field_1588.field_330.field_20.y = v31;
    }
    v32 = func_data->rob_chr_data;
    v33 = v32->miku_rot.field_24.z + v5->field_1588.field_330.field_20.z;
    v34 = v32->miku_rot.field_24.y + v5->field_1588.field_330.field_20.y;
    *v11 = v32->miku_rot.field_24.x + *v11;
    v5->field_1588.field_330.field_20.y = v34;
    v5->field_1588.field_330.field_20.z = v33;
LABEL_27:
    v35 = (float_t)((int16_t*)data)[0];
    v5->field_1588.field_330.field_1C = v35;
    if (v4 & 0x08) {
        v36 = func_data->rob_chr_data;
        if (v35 < v36->field_1588.field_0.field_1F4)
            v35 = v36->field_1588.field_0.field_1F4;
        v37 = v35 - v36->motion.frame_data.frame;
        if (v37 < 1.0)
            v37 = 1.0;
        v36->miku_rot.field_30.x = (float_t)(*v11 - v36->miku_rot.field_24.x) * (float_t)(1.0 / v37);
        v36->miku_rot.field_30.z = (float_t)(v5->field_1588.field_330.field_20.z - v36->miku_rot.field_24.z)
            * (float_t)(1.0 / v37);
        v36->miku_rot.field_30.y = get_osage_gravity_const() * ((float_t*)data)[6];
        func_data->rob_chr_data->field_8.field_B8.field_10.y = 0.0f;
        sub_140551AF0(func_data->rob_chr);
    }*/
}

static void mothead_func_18(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_19(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_20(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_21(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_22(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_23(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_24(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_25(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_26(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_27(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_28(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_29(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    /*char* v5; // rbx
    __int16 v8; // r9
    int32_t v9; // eax
    int32_t v10; // eax
    rob_chara_data* v11; // rdx
    uint16_t v12; // r8
    int64_t v13; // r9
    float_t v14; // xmm5_4
    float_t v15; // xmm2_4
    float_t v16; // xmm3_4
    float_t v17; // xmm6_4
    float_t v18; // xmm7_4
    float_t v19; // xmm8_4
    float_t v20; // xmm9_4
    float_t v21; // xmm10_4
    float_t v22; // xmm4_4
    int64_t v23; // rcx
    __int16 v24; // ax
    int32_t* v25; // [rsp+88h] [rbp+10h] BYREF

    v5 = (char*)data + 52;
    v25 = (int32_t*)((char*)data + 52);
    if (!sub_1405333D0(&v25)
        || ((v8 = *((int16_t*)data + 24)) != 0
            || (v9 = *(int32_t*)&v5[4 * ((uint64_t)(int32_t)func_data->rob_chr->chara_index >> 5)],
                _bittest(&v9, func_data->rob_chr->chara_index & 0x1F)))
        && (v8 != 1
            || (v10 = *(int32_t*)&v5[4 * ((uint64_t)(int32_t)func_data->rob_chr->chara_index >> 5)],
                !_bittest(&v10, func_data->rob_chr->chara_index & 0x1F)))) {
        v11 = func_data->rob_chr_data;
        v12 = ((int16_t*)data)[1];
        LOWORD(v13) = ((int16_t*)data)[3];
        v14 = ((float_t*)data)[2];
        v15 = ((float_t*)data)[4];
        v16 = ((float_t*)data)[8];
        v17 = ((float_t*)data)[3];
        v18 = ((float_t*)data)[6];
        v19 = ((float_t*)data)[7];
        v20 = ((float_t*)data)[9];
        v21 = ((float_t*)data)[10];
        v22 = ((float_t*)data)[11];
        if (v11->motion.field_28 & 8) {
            v12 = word_140A2D430[v12];
            v13 = word_140A2D440[(__int16)v13];
            if (stru_140A2D480[v13].field_0) {
                v14 = -v14;
                v18 = -v18;
                v20 = -v20;
            }

            if (stru_140A2D480[v13].field_1) {
                v17 = -v17;
                v19 = -v19;
                v21 = -v21;
            }

            if (stru_140A2D480[v13].field_2) {
                v15 = -v15;
                v16 = -v16;
                v22 = -v22;
            }
        }
        v23 = v12;
        v11->field_1588.field_330.field_33C[v23].field_0 = *((int16_t*)data + 2);
        v11->field_1588.field_330.field_33C[v23].frame = (float_t)mhd_data->frame;
        v11->field_1588.field_330.field_33C[v23].field_8 = (float_t)*(__int16*)data;
        if (((int16_t*)data)[2] != 1) {
            v11->field_1588.field_330.field_33C[v23].field_C = v13;
            v24 = *((int16_t*)data + 10);
            v11->field_1588.field_330.field_33C[v23].field_10.x = v14;
            v11->field_1588.field_330.field_33C[v23].field_1C.x = v18;
            v11->field_1588.field_330.field_33C[v23].field_28.x = v20;
            v11->field_1588.field_330.field_33C[v23].field_10.y = v17;
            v11->field_1588.field_330.field_33C[v23].field_E = v24;
            v11->field_1588.field_330.field_33C[v23].field_1C.y = v19;
            v11->field_1588.field_330.field_33C[v23].field_28.y = v21;
            v11->field_1588.field_330.field_33C[v23].field_10.z = v15;
            v11->field_1588.field_330.field_33C[v23].field_1C.z = v16;
            v11->field_1588.field_330.field_33C[v23].field_28.z = v22;
        }
    }*/
}

static void mothead_func_30(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr_data->miku_rot.field_30.x = 0.0f;
    func_data->rob_chr_data->miku_rot.field_30.z = 0.0f;
}

static void mothead_func_31(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_32(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    /*rob_chara_data* v4; // rbx
    int32_t v5; // er11
    float_t v7; // xmm1_4
    float_t v8; // xmm0_4
    float_t v9; // xmm2_4
    int32_t v10; // esi
    int32_t v11; // er10
    int64_t v12; // r8
    int64_t v13; // rdi
    int64_t v14; // rdx
    int32_t v15; // ecx
    int64_t v16; // r9

    v4 = func_data->rob_chr_data;
    v5 = ((int32_t*)data)[1];
    v7 = (float_t)(v4->field_1588.field_0.field_8 - 1.0) - v4->motion.frame_data.frame;
    v8 = (float_t)((int16_t*)data)[0];
    if ((v4->motion.field_28 & 8) != 0)
        v5 = sub_140533440(v5);
    v9 = ((float_t*)data)[2];
    if (v7 >= v8)
        v7 = v8;
    v10 = ((int32_t*)data)[3];
    v11 = 1;
    v12 = 236i64;
    v13 = 3i64;
    do {
        if (v11 & v10) {
            v14 = 0i64;
            v15 = 1;
            v16 = 27i64;
            do {
                if (v15 & v5) {
                    *((uint8_t*)&v4->field_1588.field_0.field_270 + v12 + v14) = 1;
                    *((float_t*)&v4->field_1588.field_0.motion_id + v12 + v14) = v9;
                    *(&v4->field_1588.field_0.field_B0[12].field_4 + v12 + v14) = v7;
                }
                v15 = __ROL4__(v15, 1);
                ++v14;
                --v16;
            } while (v16);
        }
        v11 = __ROL4__(v11, 1);
        v12 += 27i64;
        --v13;
    } while (v13);*/
}

static uint64_t sub_1405333D0(uint32_t a1) {
    uint64_t v1 = a1;
    v1 = (((v1 & 0x55555555) + ((v1 >> 1) & 0x55555555)) & 0x33333333)
        + ((((v1 & 0x55555555) + ((v1 >> 1) & 0x55555555)) >> 2) & 0x33333333);
    return (((((v1 & 0xF0F0F0F) + ((v1 >> 4) & 0xF0F0F0F)) & 0xFF00FF)
        + ((((v1 & 0xF0F0F0F) + ((v1 >> 4) & 0xF0F0F0F)) >> 8) & 0xFF00FF)) >> 16)
        + (((v1 & 0xF) + ((v1 >> 4) & 0xF)) + (((v1 & 0xF0F) + ((v1 >> 4) & 0xF0F)) >> 8));
}

static void mothead_func_33(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    uint32_t* v5 = (uint32_t*)((size_t)data + 16);
    if (sub_1405333D0(((uint32_t*)((size_t)data + 16))[0])
        && !(v5[(uint64_t)(int32_t)func_data->rob_chr->chara_index >> 5]
            & (1 << (func_data->rob_chr->chara_index & 0x1F))))
        return;

    uint32_t* v10 = (uint32_t*)func_data->rob_chr->field_20;
    if (sub_1405333D0(((uint32_t*)((size_t)data + 20))[0]) && !(func_data->rob_chr->field_20
        && (((uint32_t*)data + ((uint64_t)v10[4] >> 5))[5] & (1 << (v10[4] & 0x1F)))))
        return;

    struc_223* v8 = &func_data->rob_chr_data->field_1588;
    v8->field_330.field_310 = (float_t)mhd_data->frame;
    v8->field_330.field_314 = (float_t)((int16_t*)data)[0];
    v8->field_330.field_318 = ((uint8_t*)data)[2];
    v8->field_330.field_31C = ((int32_t*)data)[1];
    v8->field_330.field_320 = ((float_t*)data)[2];
    v8->field_330.field_324 = func_data->rob_chr_data->motion.field_138;
    v8->field_330.field_328 = ((float_t*)data)[3];
    v8->field_330.field_32C = func_data->rob_chr_data->motion.field_140.y;
}

static void mothead_func_34(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_35(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_36(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_37(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_38(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_39(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_40(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_41(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_42(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_43(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_44(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_45(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_46(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_47(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_48(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_49(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void rob_chara_set_face_motion_id(rob_chara* rob_chr, int32_t motion_id, float_t frame,
    int32_t state, float_t duration, float_t a6, float_t step, int32_t a8, float_t offset, motion_database* mot_db);

static void mothead_func_50_set_face_motion_id(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara_set_face_motion_id(func_data->rob_chr, ((int32_t*)data)[0],
        ((float_t*)data)[1], 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_51(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_52(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_53_set_face_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
     func_data->rob_chr->set_face_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
         ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, false, mot_db);
}

static void mothead_func_54_set_hand_r_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr->set_hand_r_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 12.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_55_set_hand_l_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr->set_hand_l_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 12.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_56_set_mouth_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr->set_mouth_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_57_set_eyes_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr->set_eyes_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_58_set_eyelid_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr->set_eyelid_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_eyelid_motion_id(rob_chara* rob_chr, int32_t motion_id, float_t frame,
    int32_t state, float_t duration, float_t a6, float_t step, int32_t a8, float_t offset, motion_database* mot_db);
static void rob_chara_set_eyes_motion_id(rob_chara* rob_chr, int32_t motion_id, float_t frame,
    int32_t state, float_t duration, float_t a6, float_t step, int32_t a8, float_t offset, motion_database* mot_db);

static void rob_chara_set_head_object(rob_chara* rob_chr, int32_t head_object_id, motion_database* mot_db) {
    rob_chr->data.motion.field_150.head_object = rob_chara_get_head_object(rob_chr, head_object_id);
    rob_chara_set_face_motion_id(rob_chr, -1, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
    rob_chara_set_eyelid_motion_id(rob_chr, -1, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
    rob_chara_set_eyes_motion_id(rob_chr, -1, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
    rob_chr->bone_data->field_788.field_91 = true;
}

static void mothead_func_59_set_rob_chara_head_object(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara_set_head_object(func_data->rob_chr, ((int32_t*)data)[0], mot_db);
}

static void sub_14041C950(rob_chara_bone_data* rob_bone_data, uint8_t a2, bool a3,
    float_t a4, float_t a5, float_t a6, float_t a7, float_t a8, uint8_t a9) {
    struc_258* v11 = &rob_bone_data->field_788;
    bool v10 = !v11->field_8E && a3 || !a3 && v11->field_8E;

    v11->field_8C = a2;
    if (!v11->field_8E && a3)
        v11->field_8D = 1;
    v11->field_8E = a3;
    if (!v11->field_90 && a3)
        v11->field_8F = 1;
    v11->field_90 = a3;
    v11->field_94 = a4 < 0.0f ? 1.0f : a4;
    v11->field_98 = a4 < 0.0f ? 1.0f : a5;
    v11->field_9C = a4 < 0.0f ? 1.0f : a7;
    v11->field_A0 = a4 < 0.0f ? 1.0f : a6;
    v11->field_A4 = 0.0f;
    v11->field_A8 = 1.0f;
    v11->field_AC = 0.0f;
    v11->field_B0 = a8;
    v11->field_1C4 = a9;
}

static void sub_140554830(rob_chara* rob_chr, uint8_t a2, uint8_t a3,
    float_t a4, float_t a5, float_t a6, float_t a7, float_t a8, uint8_t a9) {
    sub_14041C950(rob_chr->bone_data, a2, a3, a4, a5, a6, a7, a8, a9);
    if (fabsf(a6) <= 0.000001f)
        rob_chr->data.field_2 |= 0x80;
}

static void mothead_func_60(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    sub_140554830(func_data->rob_chr, 1, ((int32_t*)data)[0] != 0,
        ((float_t*)data)[1], ((float_t*)data)[2], ((float_t*)data)[3] * 6.0f, 0.25f, 0.0f, 0);
}

static void mothead_func_61_set_eyelid_motion_from_face(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr->set_eyelid_mottbl_motion_from_face(
        ((int32_t*)data)[0], ((float_t*)data)[1] * 6.0f, -1.0f, 0.0f, mot_db);
}

static void rob_chara_set_parts_adjust_by_index(rob_chara* rob_chr,
    rob_osage_parts index, rob_chara_data_adjust* adjust) {
    if (index >= ROB_OSAGE_PARTS_LEFT && index <= ROB_OSAGE_PARTS_ANGEL_R)
        rob_chara_set_adjust(rob_chr, adjust,
            &rob_chr->data.motion.parts_adjust[index],
            &rob_chr->data.motion.parts_adjust_prev[index]);
}

static void mothead_func_62_rob_parts_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;

    rob_chara_data_adjust v16;
    v16.reset();

    int8_t v5 = ((int8_t*)data)[5];
    if (v5 >= 0 && v5 <= 6) {
        v16.enable = true;
        v16.frame = rob_chr->data.motion.frame_data.frame - mhd_data->frame;
        v16.transition_frame = rob_chr->data.motion.frame_data.frame - mhd_data->frame;
        v16.motion_id = rob_chr->data.motion.motion_id;
        v16.set_frame = (float_t)mhd_data->frame;
        v16.force_duration = ((float_t*)data)[0];
        v16.type = v5;
        v16.cycle_type = ((int8_t*)data)[7];
        v16.ignore_gravity = !!((uint8_t*)data)[6];
        v16.external_force.x = ((float_t*)data)[2];
        v16.external_force.y = ((float_t*)data)[3];
        v16.external_force.z = ((float_t*)data)[4];
        v16.external_force_cycle_strength.x = ((float_t*)data)[5];
        v16.external_force_cycle_strength.y = ((float_t*)data)[6];
        v16.external_force_cycle_strength.z = ((float_t*)data)[7];
        v16.external_force_cycle.x = ((float_t*)data)[8];
        v16.external_force_cycle.y = ((float_t*)data)[9];
        v16.external_force_cycle.z = ((float_t*)data)[10];
        v16.cycle = ((float_t*)data)[11];
        v16.force = ((float_t*)data)[13];
        v16.phase = ((float_t*)data)[12];
        v16.strength = ((float_t*)data)[14];
        v16.strength_transition = ((float_t*)data)[15];
    }
    rob_chara_set_parts_adjust_by_index(rob_chr, (rob_osage_parts)((uint8_t*)data)[4], &v16);
}

static void mothead_func_63(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {

}

static void mothead_func_64_osage_reset(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr->set_osage_reset();
}

static void mothead_func_65_motion_skin_param(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;
    int32_t motion_id = rob_chr->data.motion.motion_id;
    float_t _frame = (float_t)mhd_data->frame;
    rob_chr->set_motion_skin_param(motion_id, _frame);
    rob_chr->set_motion_reset_data(motion_id, _frame);
}

static void mothead_func_66_osage_step(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr->set_osage_step(((float_t*)data)[0]);
}

static void mothead_func_67(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    bool v5 = false;
    bool v6 = false;
    switch (((int32_t*)data)[0]) {
    case 0:
        v6 = true;
        break;
    case 1:
        v5 = true;
        break;
    case 2:
        v6 = true;
        v5 = true;
        break;
    }

    rob_chara_bone_data* rob_bone_data = func_data->rob_chr->bone_data;
    rob_bone_data->field_958.field_58 = v6;
    rob_bone_data->field_958.field_59 = v5;
    rob_bone_data->field_958.field_8C = ((float_t*)data)[1];
}

static void mothead_func_68(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    motion_blend_mot* v2 = func_data->rob_chr->bone_data->motion_loaded.front();
    v2->mot_key_data.frame = -1.0f;
    v2->mot_key_data.field_68.field_0 = ((int32_t*)data)[0];
    v2->mot_key_data.field_68.field_4 = ((float_t*)data)[0];
}

static void mothead_func_69(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    /*float_t v5 = (float_t)((int32_t*)data)[0];
    sub_14041BEF0(func_data->rob_chr->bone_data, v5);
    pv_game* v6 = pv_game_data_get();
    if (v6)
        pv_game::set_data_itmpv_max_frame(v6, func_data->rob_chr->chara_id, v5);*/
}

static void mothead_func_70(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    /*pv_game* v5 = pv_game_data_get();
    if (!v5)
        return;

    int32_t v6 = v5->data.camera_auth_3d_uid;
    if (v6 == -1)
        return;

    int32_t id;
    pv_game::get_data_auth_3d(v5, &id, v6);
    if (auth_3d_data_check_id_not_empty(&id))
        auth_3d_data_set_max_frame(&id, (float_t)((int32_t*)data)[0]);*/
}

static void mothead_func_71_osage_move_cancel(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr->set_osage_move_cancel(((uint8_t*)data)[0], ((float_t*)data)[1]);
}

static void mothead_func_72(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara_adjust_data* v6 = &func_data->rob_chr->data.adjust_data;
    v6->offset.x = ((float_t*)data)[0];
    v6->offset.y = ((float_t*)data)[1];
    v6->offset.z = ((float_t*)data)[2];
    v6->offset_x = !!((uint8_t*)data)[24];
    v6->offset_y = !!((uint8_t*)data)[25];
    v6->offset_z = !!((uint8_t*)data)[26];
}

static void mothead_func_73_rob_hand_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara_motion* v4 = &func_data->rob_chr->data.motion;
    int32_t hand = ((int16_t*)data)[0];

    rob_chara_data_hand_adjust* hand_adjust = &v4->hand_adjust[hand];
    rob_chara_data_hand_adjust* hand_adjust_prev = &v4->hand_adjust_prev[hand];
    *hand_adjust_prev = *hand_adjust;

    hand_adjust->reset();
    hand_adjust->type = (rob_chara_data_hand_adjust_type)((int16_t*)data)[4];
    hand_adjust->scale = ((float_t*)data)[3];
    hand_adjust->duration = ((float_t*)data)[1];
    hand_adjust->current_time = v4->frame_data.frame - (float_t)mhd_data->frame;
    hand_adjust->scale_select = ((int16_t*)data)[1];

    switch (hand_adjust->scale_select) {
    case 0:
        hand_adjust->enable = true;
        hand_adjust->rotation_blend = hand_adjust_prev->rotation_blend;
        hand_adjust->scale_blend = 1.0f;
        hand_adjust->enable_scale = false;
        hand_adjust->offset = hand_adjust_prev->offset;
        hand_adjust->disable_x = hand_adjust_prev->disable_x;
        hand_adjust->disable_y = hand_adjust_prev->disable_y;
        hand_adjust->disable_z = hand_adjust_prev->disable_z;
        hand_adjust->arm_length = hand_adjust_prev->arm_length;
        hand_adjust->field_40 = hand_adjust_prev->field_40;
        break;
    case 1:
        hand_adjust->enable = true;
        hand_adjust->rotation_blend = ((float_t*)data)[4];
        hand_adjust->scale_blend = ((float_t*)data)[9];
        hand_adjust->enable_scale = !!((uint8_t*)data)[32];
        hand_adjust->disable_x = !!((uint8_t*)data)[33];
        hand_adjust->disable_y = !!((uint8_t*)data)[34];
        hand_adjust->disable_z = !!((uint8_t*)data)[35];
        hand_adjust->offset.x = ((float_t*)data)[5];
        hand_adjust->offset.y = ((float_t*)data)[6];
        hand_adjust->offset.z = ((float_t*)data)[7];
        hand_adjust->arm_length = ((float_t*)data)[10];
        hand_adjust->field_40 = ((int32_t*)data)[11];
        break;
    }
}

static bool rob_osage_check_parts_bits(rob_osage* rob_osg, rob_osage_parts_bit parts_bits) {
    if (rob_osg->osage_setting.parts != ROB_OSAGE_PARTS_NONE)
        return !!(parts_bits & (1 << rob_osg->osage_setting.parts));
    return false;
}

static void sub_1405F9520(rob_chara_item_equip_object* itm_eq_obj, rob_osage_parts_bit a2, bool a3) {
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
        if (rob_osage_check_parts_bits(&i->rob, a2))
            i->rob.field_1F0F = a3;
}

static void sub_140513EE0(rob_chara_item_equip* rob_itm_eq, rob_osage_parts a2, bool a3) {
    sub_1405F9520(&rob_itm_eq->item_equip_object[a2 == ROB_OSAGE_PARTS_MUFFLER ? ITEM_OUTER : ITEM_KAMI],
        (rob_osage_parts_bit)(1 << a2), a3);
}

static void sub_140555120(rob_chara* rob_chr, rob_osage_parts a2, bool a3) {
    sub_140513EE0(rob_chr->item_equip, a2, a3);
}

static void mothead_func_74(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    sub_140555120(func_data->rob_chr, (rob_osage_parts)((uint8_t*)data)[0], !!((uint8_t*)data)[1]);
}

void rob_chara_set_adjust_global(rob_chara* rob_chr, rob_chara_data_adjust* a2) {
    rob_chara_set_adjust(rob_chr, a2,
        &rob_chr->data.motion.adjust_global, &rob_chr->data.motion.adjust_global_prev);
}

static void mothead_func_75_rob_adjust_global(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;

    rob_chara_data_adjust v14;
    v14.reset();

    int8_t v5 = ((int8_t*)data)[4];
    if (v5 >= 0 && v5 <= 5) {
        v14.enable = true;
        v14.frame = rob_chr->data.motion.frame_data.frame - mhd_data->frame;
        v14.transition_frame = rob_chr->data.motion.frame_data.frame - mhd_data->frame;
        v14.motion_id = rob_chr->data.motion.motion_id;
        v14.set_frame = (float_t)mhd_data->frame;
        v14.force_duration = ((float_t*)data)[0];
        v14.type = v5;
        v14.cycle_type = ((int8_t*)data)[5];
        v14.external_force.x = ((float_t*)data)[2];
        v14.external_force.y = ((float_t*)data)[3];
        v14.external_force.z = ((float_t*)data)[4];
        v14.external_force_cycle_strength.x = ((float_t*)data)[5];
        v14.external_force_cycle_strength.y = ((float_t*)data)[6];
        v14.external_force_cycle_strength.z = ((float_t*)data)[7];
        v14.external_force_cycle.x = ((float_t*)data)[8];
        v14.external_force_cycle.y = ((float_t*)data)[9];
        v14.external_force_cycle.z = ((float_t*)data)[10];
        v14.cycle = ((float_t*)data)[11];
        v14.phase = ((float_t*)data)[12];
    }
    rob_chara_set_adjust_global(rob_chr, &v14);
}

static void mothead_func_76_rob_arm_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;

    rob_chara_data_arm_adjust* arm_adjust = &rob_chr->data.motion.arm_adjust[((int16_t*)data)[0]];
    arm_adjust->enable = true;
    arm_adjust->prev_value = arm_adjust->value;
    arm_adjust->value = 0.0f;
    arm_adjust->next_value = ((float_t*)data)[2];
    arm_adjust->duration = ((float_t*)data)[1];
}

static void mothead_func_77_disable_eye_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    rob_chara_bone_data_set_disable_eye_motion(func_data->rob_chr->bone_data, ((uint8_t*)data)[0] != 0);
}

static void mothead_func_78(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    static const int16_t word_140A2D430[] = {
        1, 0, 3, 2, 4, 0, 0, 0
    };

    rob_chara_data* v4 = func_data->rob_chr_data;
    uint16_t v5 = ((uint8_t*)data)[2];
    if (v4->motion.field_28 & 8)
        v5 = word_140A2D430[v5];

    struc_306* v7 = &v4->field_1588.field_330.field_33C[v5];
    int32_t v8 = v7->field_38;
    int32_t v9 = v7->field_3C;
    int32_t v10 = v7->field_40;
    int32_t v11 = v7->field_48;

    v7->field_0 = -1;
    v7->frame = -1.0f;
    v7->field_8 = -1.0f;
    v7->field_C = 0;
    v7->field_E = 0;

    int32_t v12 = v7->field_34;
    int32_t v13 = v7->field_44;
    v7->field_10 = vec3_null;
    v7->field_1C = vec3_null;
    v7->field_28 = vec3_null;
    v7->field_38 = 0;
    v7->field_34 = 0;
    v7->field_3C = 0;
    v7->field_40 = 0;
    v7->field_44 = 0;
    v7->field_48 = 0;

    uint8_t v14 = ((uint8_t*)data)[3];
    if (v14 != 1) {
        v12 = ((int32_t*)data)[1];
        v8 = ((int32_t*)data)[2];
        v9 = ((int32_t*)data)[3];
        v10 = ((int32_t*)data)[4];
        v13 = ((int32_t*)data)[5];
        v11 = ((int32_t*)data)[6];
    }

    v7->field_0 = v14;
    v7->frame = (float_t)mhd_data->frame;
    v7->field_8 = (float_t)((int16_t*)data)[1];
    v7->field_C = 26;
    v7->field_34 = v12;
    v7->field_38 = v8;
    v7->field_3C = v9;
    v7->field_40 = v10;
    v7->field_44 = v13;
    v7->field_48 = v11;
}

static void mothead_func_79_rob_chara_coli_ring(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    //rob_chara_set_coli_ring(func_data->rob_chr, ((int8_t*)data)[0]);
}

static void mothead_func_80_adjust_get_global_trans(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, motion_database* mot_db) {
    func_data->rob_chr_data->adjust_data.get_global_trans = ((uint8_t*)data)[0];
}

static void motion_blend_mot_interpolate(motion_blend_mot* a1) {
    vec3* keyframe_data = (vec3*)a1->mot_key_data.key_set_data.data();
    bool reverse = motion_blend_mot_interpolate_get_reverse(&a1->field_4F8.field_0);
    float_t frame = a1->mot_play_data.frame_data.frame;

    bone_database_skeleton_type skeleton_type = a1->bone_data.rob_bone_data->base_skeleton_type;
    bone_data* bones_data = a1->bone_data.bones.data();
    for (uint16_t& i : a1->bone_data.bone_indices) {
        bone_data* data = &bones_data[i];
        bool get_data = sub_140410250(&a1->field_0.field_8, data->motion_bone_index);
        if (reverse && data->mirror != 255)
            data = &bones_data[data->mirror];

        if (get_data && frame != data->frame) {
            mot_key_data_interpolate(&a1->mot_key_data, frame, data->key_set_offset, data->key_set_count);
            data->frame = frame;
        }

        keyframe_data = bone_data_set_key_data(data, keyframe_data, skeleton_type, get_data, reverse);
    }

    uint32_t bone_key_set_count = a1->bone_data.bone_key_set_count;
    if (frame != a1->mot_key_data.frame) {
        mot_key_data_interpolate(&a1->mot_key_data, frame,
            bone_key_set_count, a1->bone_data.global_key_set_count);
        a1->mot_key_data.frame = frame;
    }

    keyframe_data = (vec3*)&a1->mot_key_data.key_set_data.data()[bone_key_set_count];
    vec3 global_trans = keyframe_data[0];
    vec3 global_rotation = keyframe_data[1];
    if (reverse)
        vec3_negate(global_trans, global_trans);
    a1->bone_data.global_trans = global_trans;
    a1->bone_data.global_rotation = global_rotation;

    float_t rotation_y = a1->bone_data.rotation_y;
    mat4 mat;
    mat4_rotate_y(rotation_y, &mat);
    mat4_translate_mult(&mat, global_trans.x, global_trans.y, global_trans.z, &mat);
    mat4_rotate_mult(&mat, global_rotation.x, global_rotation.y, global_rotation.z, &mat);
    for (bone_data& i : a1->bone_data.bones)
        switch (i.type) {
        case BONE_DATABASE_BONE_TYPE_1:
            mat4_mult_vec3_trans(&mat, &i.trans, &i.trans);
            break;
        case BONE_DATABASE_BONE_POSITION_ROTATION: {
            mat4 rot_mat;
            mat4_clear_trans(&mat, &rot_mat);
            i.rot_mat[0] = rot_mat;
            mat4_mult_vec3_trans(&mat, &i.trans, &i.trans);
        } break;
        case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            mat4_mult_vec3_trans(&mat, &i.ik_target, &i.ik_target);
            break;
        }
}

static bool motion_blend_mot_interpolate_get_reverse(int32_t* a1) {
    return *a1 & 0x01 && ~*a1 & 0x08 || ~*a1 & 0x01 && *a1 & 0x08;
}

static void sub_1404146F0(struc_240* a1) {
    uint32_t* v1 = a1->field_8.bitfield.data();
    int32_t v2 = MOTION_BONE_N_HARA_CP;
    size_t v4 = 0;
    while (true) {
        size_t v5 = a1->field_8.motion_bone_count;
        uint32_t* v6 = a1->field_8.bitfield.data();
        size_t v7 = 0;
        if (v5) {
            v6 += a1->field_8.motion_bone_count >> 5;
            v7 = a1->field_8.motion_bone_count & 0x1F;
        }
        if (v1 == v6 && v4 == v7)
            break;

        if (a1->bone_check_func((motion_bone_index)v2))
            *v1 |= 1 << v4;
        else
            *v1 &= ~(1 << v4);
        v2++;

        if (v4 >= 0x1F) {
            v4 = 0;
            v1++;
        }
        else
            v4++;
    }
}

size_t sub_14040EB30(struc_313* a1, struc_314* a2, size_t a3) {
    size_t v5 = a2->field_8 + 32 * (a2->field_0 - a1->bitfield.data());
    if (!a3)
        return v5;

    a1->bitfield.resize((a1->motion_bone_count + a3 + 31) >> 5);

    uint32_t* bitfield = a1->bitfield.data();
    size_t v7 = a1->motion_bone_count;
    if (!v7) {
        a1->motion_bone_count = a3;
        return v5;
    }

    struc_314 v18;
    v18.field_0 = &bitfield[v7 >> 5];
    v18.field_8 = a1->motion_bone_count & 0x1F;
    size_t v11 = a3 + v7;
    a1->motion_bone_count = v11;

    struc_314 v17;
    v17.field_0 = &bitfield[v11 >> 5];
    v17.field_8 = v11 & 0x1F;

    uint32_t* v12 = &bitfield[v5 >> 5];
    size_t v13 = v18.field_8;
    uint32_t* v14 = v18.field_0;
    size_t v15 = v17.field_8;
    uint32_t*  v16 = v17.field_0;
    while (v12 != v14 || (v5 & 0x1F) != v13) {
        if (v13)
            v13--;
        else {
            v13 = 31;
            v14--;
        }

        if (v15)
            v15--;
        else {
            v15 = 31;
            v16--;
        }

        if ((1 << v13) & *v14)
            *v16 |= 1 << v15;
        else
            *v16 &= ~(1 << v15);
    }
    return v5;
}

static void sub_14040E980(struc_313* a1, struc_314* a2, struc_314* a3, size_t a4, bool* a5) {
    uint32_t* bitfield = a1->bitfield.data();
    struc_314 v19 = *a3;
    size_t v8 = sub_14040EB30(a1, &v19, a4);
    size_t v10 = v8 + a4;
    size_t v11 = v8;
    uint32_t* v12 = &bitfield[v10 >> 5];
    size_t v13 = v10 & 0x1F;

    uint32_t* v15 = &bitfield[v8 >> 5];
    size_t v14 = v8 & 0x1F;
    while (v15 != v12 || v14 != v13) {
        if (*a5)
            *v15 |= 1 << v14;
        else
            *v15 &= ~(1 << v14);

        if (v14 >= 0x1F) {
            v14 = 0;
            v15++;
        }
        else
            v14++;
    }

    a2->field_0 = &bitfield[v11 >> 5];
    a2->field_8 = v11 & 0x1F;
}

static void sub_140414550(struc_313* a1, size_t a2, bool a3) {
    uint32_t* bitfield = a1->bitfield.data();
    size_t motion_bone_count = a1->motion_bone_count;
    if (a2 > motion_bone_count) {
        struc_314 v7;
        v7.field_0 = bitfield;
        v7.field_8 = 0;
        if (motion_bone_count) {
            v7.field_0 = &bitfield[motion_bone_count >> 5];
            v7.field_8 = motion_bone_count & 0x1F;
        }

        struc_314 v8 = v7;
        sub_14040E980(a1, &v7, &v8, a2 - motion_bone_count, &a3);
    }
    else if (a2 < motion_bone_count) {
        struc_314 v8;
        v8.field_0 = bitfield;
        v8.field_8 = 0;
        if (motion_bone_count) {
            v8.field_0 = &bitfield[motion_bone_count >> 5];
            v8.field_8 = motion_bone_count & 0x1F;
        }

        struc_314 v7;
        v7.field_0 = &bitfield[a2 >> 5];
        v7.field_8 = a2 & 0x1F;

        struc_314 v9;
        sub_1404122C0(a1, &v9, &v7, &v8);
    }
}

static void sub_140413350(struc_240* a1, bool(*bone_check_func)(motion_bone_index), size_t motion_bone_count) {
    a1->bone_check_func = bone_check_func;
    a1->motion_bone_count = motion_bone_count;
    sub_1404119A0(&a1->field_8);
    sub_140414550(&a1->field_8, motion_bone_count, 0);
    sub_1404146F0(a1);
}

static void motion_blend_mot_load_bone_data(motion_blend_mot* a1,
    rob_chara_bone_data* a2, bool(*bone_check_func)(motion_bone_index), bone_database* bone_data) {
    bone_data_parent_data_init(&a1->bone_data, a2, bone_data);
    mot_key_data_init_key_sets(
        &a1->mot_key_data,
        a1->bone_data.rob_bone_data->base_skeleton_type,
        a1->bone_data.motion_bone_count,
        a1->bone_data.ik_bone_count);
    sub_140413350(&a1->field_0, bone_check_func, a1->bone_data.motion_bone_count);
}

static void sub_1401EB1D0(bone_data* a1, int32_t skeleton_select) {
    if (!a1->flags) {
        switch (a1->type) {
        case BONE_DATABASE_BONE_ROTATION:
            a1->rot_mat_prev[0][skeleton_select] = a1->rot_mat[0];
            break;
        case BONE_DATABASE_BONE_TYPE_1:
        case BONE_DATABASE_BONE_POSITION:
        case BONE_DATABASE_BONE_POSITION_ROTATION:
            a1->trans_prev[skeleton_select] = a1->trans;
            a1->rot_mat_prev[0][skeleton_select] = a1->rot_mat[0];
            break;
        case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
            a1->rot_mat_prev[0][skeleton_select] = a1->rot_mat[0];
            break;
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
            a1->rot_mat_prev[0][skeleton_select] = a1->rot_mat[0];
            a1->rot_mat_prev[1][skeleton_select] = a1->rot_mat[1];
            break;
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            a1->rot_mat_prev[0][skeleton_select] = a1->rot_mat[0];
            a1->rot_mat_prev[1][skeleton_select] = a1->rot_mat[1];
            a1->rot_mat_prev[2][skeleton_select] = a1->rot_mat[2];
            break;
        }
    }
}

static void sub_140412E10(motion_blend_mot* a1, int32_t skeleton_select) {
    for (bone_data& i : a1->bone_data.bones) {
        if ((1 << (i.motion_bone_index & 0x1F))
            & a1->field_0.field_8.bitfield.data()[i.motion_bone_index >> 5]) {
            sub_1401EB1D0(&i, skeleton_select);
            if (i.type == BONE_DATABASE_BONE_POSITION_ROTATION && (a1->field_4F8.field_0 & 0x02))
                vec3_add(i.trans_prev[skeleton_select], a1->field_4F8.field_90, i.trans_prev[skeleton_select]);
        }
    }
}

static void sub_140412F20(mot_blend* a1, std::vector<bone_data>* a2) {
    for (bone_data& i : *a2)
        if (((1 << (i.motion_bone_index & 0x1F))
            & a1->field_0.field_8.bitfield.data()[i.motion_bone_index >> 5]) != 0)
            sub_1401EB1D0(&i, 0);
}

static void motion_blend_mot_load_file(motion_blend_mot* a1, int32_t motion_id,
    MotionBlendType blend_type, float_t blend, bone_database* bone_data, motion_database* mot_db) {
    motion_blend_mot_set_blend(a1, blend_type, blend);
    const mot_data* v6 = mot_key_data_load_file(&a1->mot_key_data, motion_id, mot_db);
    bone_data_parent* v7 = &a1->bone_data;
    if (bone_data) {
        bone_data_parent_load_bone_indices_from_mot(v7, v6, bone_data, mot_db);
        ::bone_data* bone = a1->bone_data.bones.data();
        for (uint16_t& i : a1->bone_data.bone_indices)
            bone[i].frame = -1.0f;
    }
    else {
        rob_chara_bone_data* rob_bone_data = v7->rob_bone_data;
        for (mat4& i : rob_bone_data->mats)
            i = mat4_identity;

        for (mat4& i : rob_bone_data->mats2)
            i = mat4_identity;

        a1->bone_data.bone_indices.clear();
    }

    a1->field_4F8.field_8C = false;
    a1->field_4F8.field_4C = a1->field_4F8.mat;
    a1->field_4F8.mat = mat4u_identity;
}

static void motion_blend_mot_mult_mat(motion_blend_mot* a1, mat4* mat) {
    if (mat) {
        a1->field_4F8.mat = *mat;
        a1->field_4F8.field_8C = true;
    }
    else {
        a1->field_4F8.mat = mat4u_identity;
        a1->field_4F8.field_8C = false;
    }

    mat4 m = a1->field_4F8.mat;
    bone_data* v3 = a1->bone_data.bones.data();
    for (bone_data& v4 : a1->bone_data.bones)
        bone_data_mult_1(&v4, &m, v3, true);
}

static void motion_blend_mot_set_blend(motion_blend_mot* a1,
    MotionBlendType blend_type, float_t blend) {
    switch (blend_type) {
    case MOTION_BLEND:
    default:
        a1->blend = 0;
        break;
    case MOTION_BLEND_FREEZE:
        a1->blend = &a1->freeze;
        a1->freeze.Reset();
        sub_140412E10(a1, 0);
        break;
    case MOTION_BLEND_CROSS:
        a1->blend = &a1->cross;
        a1->cross.Reset();
        break;
    case MOTION_BLEND_COMBINE:
        a1->blend = &a1->combine;
        a1->combine.Reset();
        a1->combine.blend = clamp(blend, 0.0f, 1.0f);
        break;
    }
}

static void motion_blend_mot_set_duration(motion_blend_mot* mot,
    float_t duration, float_t step, float_t offset) {
    if (mot->blend)
        mot->blend->SetDuration(duration, step, offset);
}

static void opd_data_decode(int16_t* src_data, size_t count, uint8_t shift, float_t* dst_data) {
    float_t div = (float_t)(1 << shift);
    size_t index = 0;
    for (size_t i = 0; i < count; i++, index++) {
        if (src_data[i] & 1) {
            float_t first_val = dst_data[index - 1];
            float_t last_val = src_data[i + 1] / div;
            float_t diff_val = last_val - first_val;

            int32_t lerp_count = (int32_t)((src_data[i] >> 1) - (index - 1));
            float_t scale =  1.0f / (float_t)lerp_count;
            for (int32_t j = 1; j < lerp_count; j++, index++)
                dst_data[index] = (float_t)j * diff_val * scale + first_val;
            dst_data[index] = last_val;
            i++;
        }
        else
            dst_data[index] = src_data[i] / div;
    }
}

static void osage_coli_set(osage_coli* osg_coli,
    skin_param_osage_root_coli* skp_root_coli, mat4* mats) {
    osg_coli->type = (skin_param_osage_root_coli_type)0;
    if (!skp_root_coli || !mats)
        return;

    for (; skp_root_coli->type; osg_coli++, skp_root_coli++) {
        osg_coli->type = skp_root_coli->type;
        osg_coli->radius = skp_root_coli->radius;
        mat4 mat = mats[skp_root_coli->bone0_index];
        mat4_mult_vec3_trans(&mat, &skp_root_coli->bone0_pos, &osg_coli->bone0_pos);
        if (skp_root_coli->type == SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_CYLINDER
            || skp_root_coli->type == SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_ELLIPSE) {
            mat = mats[skp_root_coli->bone1_index];
            mat4_mult_vec3_trans(&mat, &skp_root_coli->bone1_pos, &osg_coli->bone1_pos);
            vec3_sub(osg_coli->bone1_pos, osg_coli->bone0_pos, osg_coli->bone_pos_diff);
            vec3_length_squared(osg_coli->bone_pos_diff, osg_coli->bone_pos_diff_length_squared);
            osg_coli->bone_pos_diff_length = sqrtf(osg_coli->bone_pos_diff_length_squared);
            if (osg_coli->bone_pos_diff_length < 0.01f)
                osg_coli->type = SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_BALL;
        }
        else if (skp_root_coli->type == SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_PLANE)
            mat4_mult_vec3(&mat, &skp_root_coli->bone1_pos, &osg_coli->bone1_pos);
    }
    osg_coli->type = (skin_param_osage_root_coli_type)0;
}

static void osage_coli_ring_set(osage_coli* osg_coli,
    std::vector<skin_param_osage_root_coli>* vec_skp_root_coli, mat4* mats) {
    if (!vec_skp_root_coli->size()) {
        osg_coli->type = (skin_param_osage_root_coli_type)0;
        osg_coli->radius = 0.0f;
        osg_coli->bone0_pos = vec3_null;
        osg_coli->bone1_pos = vec3_null;
        osg_coli->bone_pos_diff = vec3_null;
        osg_coli->bone_pos_diff_length = 0.0f;
        osg_coli->bone_pos_diff_length_squared = 0.0f;
        osg_coli->field_34 = 1.0f;
    }
    else
        osage_coli_set(osg_coli, vec_skp_root_coli->data(), mats);
}

static void osage_play_data_manager_get_opd_file_data(object_info obj_info,
    int32_t motion_id, float_t** data, uint32_t* count) {
    //osage_play_data_manager_get()->GetOpdFileData(v8, obj_info, motion_id, data, count);
}

static const osage_setting_osg_cat* osage_setting_data_get_cat_value(
    object_info* obj_info, const char* root_node) {
    auto elem_obj = osage_setting_data.obj.find(*obj_info);
    if (elem_obj == osage_setting_data.obj.end())
        return 0;

    std::pair<std::string, std::string> key = { elem_obj->second, root_node };
    auto elem_cat = osage_setting_data.cat.find(key);
    if (elem_cat == osage_setting_data.cat.end())
        return 0;

    return &elem_cat->second;
}

static bool osage_setting_data_obj_has_key(object_info key) {
    return osage_setting_data.obj.find(key) != osage_setting_data.obj.end();
}

static bool pv_osage_manager_array_get_disp(int32_t* chara_id) {
    return pv_osage_manager_array_ptr_get(*chara_id)->GetDisp();
}

static PvOsageManager* pv_osage_manager_array_ptr_get(int32_t chara_id) {
    if (chara_id >= CHARA_MAX)
        chara_id = 0;
    return &pv_osage_manager_array_ptr[chara_id];
}

static void pv_osage_manager_array_ptr_set_not_reset_true() {
    for (int32_t i = 0; i < CHARA_MAX; i++)
        pv_osage_manager_array_ptr_get(i)->SetNotResetTrue();
}

static void rob_base_rob_chara_init(rob_chara* rob_chr) {
    //sub_14054F4A0(&rob_chr->rob_touch);
    //sub_14054F830(&rob_chr->rob_touch, rob_chr->pv_data.field_70 != 0);
}

static void sub_140414900(struc_308* a1, mat4* a2) {
    if (a2) {
        a1->field_8C = true;
        a1->mat = *a2;
    }
    else {
        a1->field_8C = false;
        a1->mat = mat4u_identity;
    }
}

static mat4u* sub_140504E80(rob_chara* rob_chr) {
    mat4 mat;
    mat4_rotate_y((float_t)((double_t)rob_chr->data.miku_rot.rot_y_int16 * M_PI * (1.0 / 32768.0)), &mat);
    mat4_set_translation(&mat, &rob_chr->data.miku_rot.position);
    rob_chr->data.miku_rot.field_6C = mat;
    return &rob_chr->data.miku_rot.field_6C;
}

static void sub_14041DA50(rob_chara_bone_data* rob_bone_data, mat4* mat) {
    for (bone_node& i : rob_bone_data->nodes)
        mat4_mult(i.mat, mat, i.mat);
    sub_140414900(&rob_bone_data->motion_loaded.front()->field_4F8, mat);
}

static void sub_140507F60(rob_chara* rob_chr) {
    mat4 v2 = *sub_140504E80(rob_chr);
    sub_14041DA50(rob_chr->bone_data, &v2);
    rob_chr->data.miku_rot.field_48 = rob_chr->data.miku_rot.field_24;

    mat4* v3 = rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, ROB_BONE_N_HARA);
    mat4_get_translation(v3, &rob_chr->data.miku_rot.field_24);
    vec3_sub(rob_chr->data.miku_rot.field_24,
        rob_chr->data.miku_rot.field_48, rob_chr->data.miku_rot.field_54);

    mat4* v8 = rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, ROB_BONE_N_HARA_CP);
    vec3 v10 = { 0.0f, 0.0f, 1.0f };
    mat4_mult_vec3(v8, &v10, &v10);
    rob_chr->data.miku_rot.field_6 = (int32_t)((float_t)(atan2f(v10.x, v10.z) * 32768.0f) * (1.0 / M_PI));
}

void rob_chara_data_adjust_ctrl(rob_chara* rob_chr, rob_chara_data_adjust* adjust,
    rob_chara_data_adjust* adjust_prev) {
    float_t cycle = adjust->cycle;
    float_t v7 = 0.0f;
    adjust->frame += rob_chr->data.motion.step_data.frame;
    if (rob_chr->data.field_1588.field_0.field_58 == 1)
        cycle *= (60.0f / rob_chr->data.field_1588.field_0.field_8);
    cycle *= adjust->frame * 2.0f * (float_t)M_PI * (float_t)(1.0 / 60.0f);

    switch (adjust->cycle_type) {
    case 1:
        v7 = cosf(adjust->phase * (float_t)M_PI + cycle);
        break;
    case 2:
        v7 = sinf(adjust->phase * (float_t)M_PI + cycle);
        break;
    }

    vec3 v8;
    vec3 v9;
    vec3_add_scalar(adjust->external_force_cycle, v7, v8);
    vec3_mult(adjust->external_force, adjust->external_force_cycle_strength, v9);
    vec3_mult(v8, v9, v9);
    vec3_add(v9, adjust->external_force, adjust->curr_external_force);

    if (adjust->type == 1) {
        mat4 mat = mat4_identity;
        mat4_rotate_y_mult(&mat, (float_t)((double_t)rob_chr->data.miku_rot.rot_y_int16
            * M_PI * (1.0 / 32768.0)), &mat);
        mat4_mult_vec3(&mat, &adjust->curr_external_force, &adjust->curr_external_force);
    }
    else if (adjust->type == 2) {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, 9)->ex_data_mat;
        if (mat)
            mat4_mult_vec3(mat, &adjust->curr_external_force, &adjust->curr_external_force);
    }
    else if (adjust->type == 3) {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, 10)->ex_data_mat;
        if (mat)
            mat4_mult_vec3(mat, &adjust->curr_external_force, &adjust->curr_external_force);
    }
    else if (adjust->type == 4) {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, 15)->ex_data_mat;
        if (mat)
            mat4_mult_vec3(mat, &adjust->curr_external_force, &adjust->curr_external_force);
    }
    else if (adjust->type == 5) {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, 177)->ex_data_mat;
        if (mat) {
            adjust->curr_external_force.z = -adjust->curr_external_force.z;
            mat4_mult_vec3(mat, &adjust->curr_external_force, &adjust->curr_external_force);
        }
    }

    if (adjust->ignore_gravity)
        adjust->curr_external_force.y = get_osage_gravity_const() + adjust->curr_external_force.y;

    adjust->curr_force = adjust->force;
    adjust->curr_strength = adjust->strength;

    bool transition_frame_step = false;
    if (adjust->force_duration > adjust->transition_frame
        && fabsf(adjust->force_duration - adjust->transition_frame) > 0.000001f) {
        transition_frame_step = true;
        float_t blend = (adjust->transition_frame + 1.0f) / (adjust->force_duration + 1.0f);
        vec3_lerp_scalar(adjust_prev->curr_external_force,
            adjust->curr_external_force, adjust->curr_external_force, blend);
        adjust->curr_force = lerp(adjust_prev->curr_force, adjust->curr_force, blend);
    }

    if (adjust->strength_transition > adjust->transition_frame
        && fabsf(adjust->strength_transition - adjust->transition_frame) > 0.000001f) {
        float_t blend = (adjust->transition_frame + 1.0f) / (adjust->strength_transition + 1.0f);
        adjust->curr_strength = lerp(adjust_prev->curr_strength, adjust->curr_strength, blend);
    }
    else if (!transition_frame_step)
        return;

    adjust->transition_frame += rob_chr->data.motion.step_data.frame;
}

static void rob_chara_item_equip_object_set_parts_external_force(
    rob_chara_item_equip_object* itm_eq_obj, int32_t parts_bits,
    vec3* weight, float_t force, float_t strength) {
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks) {
        rob_osage* rob_osg = &i->rob;
        if (rob_osage_check_osage_setting_parts_bit(rob_osg, parts_bits)) {
            rob_osage_set_nodes_external_force(rob_osg, weight, strength);
            rob_osage_set_nodes_force(rob_osg, force);
        }
    }
}

static void rob_chara_item_equip_set_parts_external_force(rob_chara_item_equip* rob_itm_eq,
    int32_t parts, vec3* external_force, float_t force, float_t strength) {
    item_id id = (item_id)(parts == ROB_OSAGE_PARTS_MUFFLER ? ITEM_OUTER : ITEM_KAMI);
    if (!osage_setting_data_obj_has_key(rob_itm_eq->item_equip_object[id].obj_info))
        return;

    switch (parts) {
    case ROB_OSAGE_PARTS_SHORT_L:
    case ROB_OSAGE_PARTS_SHORT_R:
        rob_itm_eq->parts_short = true;
        break;
    case ROB_OSAGE_PARTS_APPEND_L:
    case ROB_OSAGE_PARTS_APPEND_R:
        rob_itm_eq->parts_append = true;
        break;
    case ROB_OSAGE_PARTS_WHITE_ONE_L:
        rob_itm_eq->parts_white_one_l = true;
        break;
    }

    int32_t parts_bits = 1 << parts;
    if (!rob_itm_eq->parts_short) {
        if (parts == ROB_OSAGE_PARTS_LEFT)
            parts_bits |= ROB_OSAGE_PARTS_SHORT_L_BIT;
        else if (parts == ROB_OSAGE_PARTS_RIGHT)
            parts_bits |= ROB_OSAGE_PARTS_SHORT_R_BIT;
    }

    if (!rob_itm_eq->parts_append) {
        if (parts == ROB_OSAGE_PARTS_LEFT)
            parts_bits |= ROB_OSAGE_PARTS_APPEND_L_BIT;
        else if (parts == ROB_OSAGE_PARTS_RIGHT)
            parts_bits |= ROB_OSAGE_PARTS_APPEND_R_BIT;
    }

    if (!rob_itm_eq->parts_white_one_l && parts == ROB_OSAGE_PARTS_LONG_C)
        parts_bits |= ROB_OSAGE_PARTS_WHITE_ONE_L_BIT;

    rob_chara_item_equip_object_set_parts_external_force(
        &rob_itm_eq->item_equip_object[id], parts_bits, external_force, force, strength);
}

static void rob_chara_item_equip_object_set_external_force(
    rob_chara_item_equip_object* itm_eq_obj, vec3* external_force) {
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks) {
        i->rob.set_external_force = true;
        i->rob.external_force = *external_force;
    }

    /*for (ExClothBlock*& i : itm_eq_obj->cloth_blocks) {
        i->set_external_force = true;
        i->external_force = *external_force;
    }*/
}

static void rob_chara_item_equip_set_external_force(
    rob_chara_item_equip* rob_itm_eq, vec3* external_force) {
    for (int32_t i = rob_itm_eq->first_item_equip_object; i < rob_itm_eq->max_item_equip_object; i++)
        rob_chara_item_equip_object_set_external_force(&rob_itm_eq->item_equip_object[i], external_force);
}

static void rob_chara_adjust_ctrl(rob_chara* rob_chr) {
    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;

    for (int32_t i = 0; i < 13; i++) {
        rob_chara_data_adjust* parts_adjust = &rob_chr->data.motion.parts_adjust[i];
        if (parts_adjust->enable) {
            rob_chara_data_adjust_ctrl(rob_chr, parts_adjust, &rob_chr->data.motion.parts_adjust_prev[i]);
            rob_chara_item_equip_set_parts_external_force(rob_itm_equip, i,
                &parts_adjust->curr_external_force, parts_adjust->curr_force, parts_adjust->curr_strength);
        }
    }

    rob_chara_data_adjust* adjust_global = &rob_chr->data.motion.adjust_global;
    if (adjust_global->enable) {
        rob_chara_data_adjust_ctrl(rob_chr, adjust_global, &rob_chr->data.motion.adjust_global_prev);
        rob_chara_item_equip_set_external_force(rob_itm_equip, &adjust_global->curr_external_force);
    }

    rob_itm_equip->parts_short = false;
    rob_itm_equip->parts_append = false;
    rob_itm_equip->parts_white_one_l = false;
}

static void sub_140505B20(rob_chara* rob_chr) {
    if (rob_chr->data.field_1588.field_0.field_20.field_0 & 0x100)
        rob_chr->data.miku_rot.field_2 = rob_chr->data.miku_rot.rot_y_int16
        + rob_chr->data.field_1588.field_0.field_274;
    else
        rob_chr->data.miku_rot.field_2 = rob_chr->data.miku_rot.rot_y_int16;

    if (rob_chr->data.field_0 & 2 && rob_chr->data.miku_rot.field_30.y > -0.000001f)
        rob_chr->data.field_1 |= 0x80;
    else
        rob_chr->data.field_1 &= ~0x80;
}

static vec3* rob_chara_bone_data_get_global_trans(rob_chara_bone_data* rob_bone_data) {
    return &rob_bone_data->motion_loaded.front()->bone_data.global_trans;
}

static void rob_chara_data_adjuct_set_trans(rob_chara_adjust_data* rob_chr_adj,
    vec3* trans, bool pos_adjust, vec3* global_trans) {
    float_t scale = rob_chr_adj->scale;
    vec3 _offset = rob_chr_adj->offset;
    if (global_trans)
        _offset.y += global_trans->y;

    vec3 _trans = *trans;
    if (rob_chr_adj->height_adjust)
        _trans.y += rob_chr_adj->pos_adjust_y;
    else {
        vec3 temp;
        vec3_sub(_trans, _offset, temp);
        vec3_mult_scalar(temp, scale, temp);
        vec3_add(temp, _offset, temp);

        if (!rob_chr_adj->offset_x)
            _trans.x = temp.x;
        if (!rob_chr_adj->offset_y)
            _trans.y = temp.y;
        if (!rob_chr_adj->offset_z)
            _trans.z = temp.z;
    }

    if (pos_adjust)
        vec3_add(_trans, rob_chr_adj->pos_adjust, _trans);

    vec3 v10;
    vec3_mult_scalar(*trans, scale, v10);
    vec3_sub(_trans, v10, rob_chr_adj->trans);
}

static void rob_chara_set_data_adjust_mat(rob_chara* rob_chr,
    rob_chara_adjust_data* rob_chr_adj, bool pos_adjust) {
    vec3 v12;
    mat4* v6 = rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, ROB_BONE_N_HARA_CP);
    mat4_get_translation(v6, &v12);
    vec3* global_trans = 0;
    if (rob_chr_adj->get_global_trans)
        global_trans = rob_chara_bone_data_get_global_trans(rob_chr->bone_data);
    rob_chara_data_adjuct_set_trans(rob_chr_adj, &v12, pos_adjust, global_trans);

    mat4 mat;
    float_t scale = rob_chr_adj->scale;
    mat4_scale(scale, scale, scale, &mat);
    mat4_set_translation(&mat, &rob_chr_adj->trans);
    rob_chr_adj->mat = mat;
}

static void rob_chara_set_data_adjust_mat_pos_adjust_true(rob_chara* rob_chr) {
    rob_chara_set_data_adjust_mat(rob_chr, &rob_chr->data.adjust_data, true);
}

static void rob_base_rob_chara_ctrl(rob_chara* rob_chr) {
    sub_140507F60(rob_chr);
    rob_chara_adjust_ctrl(rob_chr);
    sub_140505B20(rob_chr);
    //sub_14036D130(7, &rob_chr);
    rob_chara_set_data_adjust_mat_pos_adjust_true(rob_chr);
}

static void sub_140514680(rob_chara* rob_chr) {
    rob_chr->data.field_8.field_0 = 0;
    rob_chr->data.field_8.field_4 = rob_chr->data.field_8.field_2C;
}

static void sub_140514540(rob_chara* rob_chr) {
    rob_chr->data.field_8.field_18 = rob_chr->data.field_8.field_4;
    rob_chr->data.field_8.field_40 = rob_chr->data.field_8.field_2C;
    if (rob_chr->data.field_8.field_2C.field_0 == 1)
        sub_140514680(rob_chr);
    rob_chr->data.field_8.field_2C.field_0 = 0;
}

static void sub_1405145F0(rob_chara* rob_chr, bone_database* bone_data, motion_database* mot_db) {
    if (rob_chr->data.field_8.field_0)
        return;

    float_t frame;
    if (rob_chr->data.motion.field_28 >= 0)
        frame = 0.0f;
    else
        frame = rob_chr->data.motion.frame_data.last_set_frame;
    rob_chr->load_motion(rob_chr->data.field_8.field_4.motion_id,
        rob_chr->data.field_8.field_4.field_10, frame, MOTION_BLEND, bone_data, mot_db);
    rob_chara_bone_data_set_motion_duration(rob_chr->bone_data, 0.0f, 1.0f, 1.0f);
    rob_chr->set_motion_reset_data(rob_chr->data.field_8.field_4.motion_id, 0.0f);
    rob_chr->data.field_8.field_0++;
}

static void sub_1405144C0(rob_chara* rob_chr, bone_database* bone_data, motion_database* mot_db) {
    if (rob_chr->data.field_8.field_4.field_0 == 1) {
        //sub_14036D130(6, &rob_chr);
        sub_1405145F0(rob_chr, bone_data, mot_db);
    }
}

static void sub_140514520(rob_chara* rob_chr, bone_database* bone_data, motion_database* mot_db) {
    sub_140514540(rob_chr);
    sub_1405144C0(rob_chr, bone_data, mot_db);
}

static void sub_14054BC70(RobSubAction* a1) {
    if (!a1->data.field_18)
        return;

    switch (a1->data.field_18->type) {
    case SUB_ACTION_EXECUTE_CRY:
        a1->data.field_0 = &a1->data.cry;
        break;
    case SUB_ACTION_EXECUTE_SHAKE_HAND:
        a1->data.field_0 = &a1->data.shake_hand;
        break;
    case SUB_ACTION_EXECUTE_EMBARRASSED:
        a1->data.field_0 = &a1->data.embarrassed;
        break;
    case SUB_ACTION_EXECUTE_ANGRY:
        a1->data.field_0 = &a1->data.angry;
        break;
    case SUB_ACTION_EXECUTE_LAUGH:
        a1->data.field_0 = &a1->data.laugh;
        break;
    case SUB_ACTION_EXECUTE_COUNT_NUM:
        a1->data.field_0 = &a1->data.count_num;
        break;
    default:
        a1->data.field_0 = 0;
        break;
    }

    if (a1->data.field_0)
        a1->data.field_0->Field_10(a1->data.field_10);

    if (a1->data.field_18)
        delete a1->data.field_18;
    a1->data.field_18 = 0;
}

static void sub_14054CC80(rob_chara* a1) {
    sub_14054BC70(&a1->data.rob_sub_action);
    if (a1->data.rob_sub_action.data.field_0) {
        a1->data.rob_sub_action.data.field_0->Field_18(a1);
        a1->data.rob_sub_action.data.field_0 = 0;
    }

    if (a1->data.rob_sub_action.data.field_8)
        a1->data.rob_sub_action.data.field_8->Field_20(a1);
}

static uint64_t sub_1405054E0(uint32_t a1) {
    uint64_t v1 = a1;
    v1 = (((v1 & 0x55555555) + ((v1 >> 1) & 0x55555555)) & 0x33333333)
        + ((((v1 & 0x55555555) + ((v1 >> 1) & 0x55555555u)) >> 2) & 0x33333333);
    return (((((v1 & 0xF0F0F0F) + ((v1 >> 4) & 0xF0F0F0F)) & 0xFF00FF)
        + ((((v1 & 0xF0F0F0F) + ((v1 >> 4) & 0xF0F0F0F)) >> 8) & 0xFF00FF)) >> 16)
        + (((v1 & 0xF) + ((v1 >> 4) & 0xF)) + (((v1 & 0xF0F) + ((v1 >> 4) & 0xF0F)) >> 8));
}

static bool sub_14054F930(uint32_t a1, uint32_t* a2) {
    size_t v2 = 0;
    while (a1 == *a2) {
        v2++;
        a2++;
        if (v2)
            return true;
    }
    return false;
}

static void* sub_140551F60(rob_chara* rob_chr, uint32_t* a2) {
    int16_t v2 = rob_chr->data.field_1588.field_0.field_244;
    const mothead_data2* v3 = rob_chr->data.field_1588.field_0.field_248;
    if (!v2)
        return 0;

    while (v3) {
        if (v3->type == 41) {
            void*v5 = v3->data;
            uint32_t v8 = ((uint32_t*)v5)[0];
            if (v8 & 0x10) {
                if (sub_1405054E0(v8) == 1)
                    return v5;
            }
            else {
                if (sub_14054F930(v8, a2))
                    return v5;

                if (sub_1405054E0(*a2 & v8))
                    return v5;
            }
        }

        v3++;
        if (!--v2)
            return 0;
    }
    return 0;
}

static void sub_1405077D0(rob_chara* rob_chr, bone_database* bone_data, motion_database* mot_db) {
    void* v2 = sub_140551F60(rob_chr, &rob_chr->data.motion.field_30);
    void* v3 = v2;
    if (!v2)
        rob_chr->data.motion.field_30 = 0;
    else if ((float_t)((int16_t*)v2)[2] <= rob_chr->data.motion.frame_data.frame) {
        int32_t v4 = ((int32_t*)v2)[2];
        uint32_t v5 = ((uint32_t*)v2)[0];
        if (v5 & 0x400000 && sub_1405054E0(v5) == 1)
            v4 = rob_chr->get_rob_cmn_mottbl_motion_id(0);
        if (v4 != -1) {
            rob_chr->load_motion(v4, !!(((uint8_t*)v3)[12] & 0x01), 0.0f, MOTION_BLEND, bone_data, mot_db);
            //sub_14036D130(8, &rob_chr);
        }
        rob_chr->data.motion.field_30 = 0;
    }
}

static bool sub_140419E90(rob_chara_bone_data* rob_bone_data) {
    mot_play_data* v2 = &rob_bone_data->motion_loaded.front()->mot_play_data;
    if (v2->field_34)
        return v2->field_38 <= 0.0f;
    else if (v2->frame_data.field_18 == 1)
        return v2->frame_data.field_1C > 3 && v2->frame_data.last_frame < v2->frame_data.frame;
    else if (v2->frame_data.field_18 == 2)
        return v2->frame_data.field_1C > 3 && v2->frame_data.frame < 0.0f;
    return false;
}

static uint8_t sub_14041A0D0(rob_chara_bone_data* rob_bone_data) {
    return rob_bone_data->motion_loaded.front()->mot_play_data.frame_data.field_28;
}

static void sub_140505310(rob_chara* rob_chr, bone_database* bone_data, motion_database* mot_db) {
    switch (rob_chr->data.field_1588.field_0.field_58) {
    case -1: {
        rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x81;
        rob_chr->data.motion.frame_data.frame = rob_chr->data.field_1588.field_0.field_8;
    } break;
    case 0: {
        int32_t motion_id = rob_chr->data.field_1588.field_0.field_5C;
        if (motion_id == -1)
            rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x81;
        else
            rob_chr->load_motion(motion_id, !!(rob_chr->data.field_1588.field_0.field_60 & 0x01),
                0.0f, MOTION_BLEND, bone_data, mot_db);
    } break;
    case 1: {
        struc_223* v1 = &rob_chr->data.field_1588;
        int32_t v3 = rob_chr->data.field_1588.field_0.field_6C;
        if (v3 > 0) {
            int32_t motion_id = rob_chr->data.field_1588.field_0.field_64;
            if (motion_id != -1) {
                if (rob_chr->data.motion.field_2C >= v3) {
                    int32_t v6 = mothead_storage_get_mot_by_motion_id(motion_id, mot_db)->field_0.field_0;
                    if (~v6 & 0x80000 && ~v6 & 0x100000)
                        rob_chr->load_motion(motion_id, !!(rob_chr->data.field_1588.field_0.field_68 & 0x01),
                            0.0f, MOTION_BLEND, bone_data, mot_db);
                }
                else if (v1->field_0.field_20.field_0 & 0x80000) {
                    int16_t* v8 = (int16_t*)rob_chr->data.field_8.field_148;
                    rob_chr->data.field_8.field_146 = v1->field_0.field_220;
                    if (v8)
                        v8[711] = v1->field_0.field_220;
                }
                else if (rob_chr->data.field_1588.field_0.field_20.field_0 & 0x100000)
                    rob_chr->data.field_8.field_0 = 1;
            }
        }

        rob_chr->data.motion.frame_data.frame = rob_chara_bone_data_get_frame(rob_chr->bone_data);
        v1->field_330.field_0.field_0 = v1->field_330.field_0.field_8;
        rob_chr->data.motion.field_2C++;
        return;
    } break;
    case 2:
    case 3:
    case 5: {
        rob_chr->load_motion(rob_chr->get_rob_cmn_mottbl_motion_id(0),
            false, 0.0f, MOTION_BLEND, bone_data, mot_db);
    } break;
    }
}

static void sub_140505980(rob_chara* rob_chr) {
    int32_t v3;
    int32_t v4;
    if (rob_chr->data.motion.field_24) {
        if (rob_chr->data.motion.field_24 != 1) {
            if (rob_chr->data.motion.field_24 != 2)
                return;
            goto LABEL_23;
        }
    }
    else {
        if (rob_chr->data.field_1588.field_0.field_78 > rob_chr->data.motion.frame_data.frame)
            return;

        if (rob_chr->data.field_1588.field_0.field_10.field_0 & 0x4000) {
            rob_chr->data.field_1588.field_0.field_20.field_0 |= 0x4000;
            if ((rob_chr->data.field_1588.field_0.field_10.field_0 & 0x20000) != 0)
                rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x20000;
        }
        rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x02;
        rob_chr->data.motion.field_24 = 1;
    }

    if (rob_chr->data.field_1588.field_0.field_7C > rob_chr->data.motion.frame_data.frame)
        return;

    rob_chr->data.field_1588.field_0.field_20.field_0 |= 4;
    rob_chr->data.field_1588.field_0.field_20.field_8 &= ~0x400000;
    v3 = rob_chr->data.field_1588.field_0.field_20.field_0;
    if (v3 & 0x4000) {
        rob_chr->data.field_1588.field_0.field_20.field_0 = v3 & ~0x4000;
        rob_chr->data.field_1588.field_0.field_88 &= ~0x02;
        if (rob_chr->data.field_1588.field_0.field_88 & 0x04)
            rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x8000;
    }
    else if (v3 & 0x2000000
        && (v3 & 0x01) != 0
        && ~rob_chr->data.field_1588.field_0.field_20.field_4 & 0x10000000
        && ~rob_chr->data.field_1588.field_0.field_20.field_4 & 0x20000000)
        rob_chr->data.field_1588.field_0.field_20.field_0 = v3 & ~0x01;

    v4 = rob_chr->data.field_1588.field_0.field_10.field_0;
    if (v4 & 0x10000 && v4 & 0x20000)
        rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x20000;
    rob_chr->data.motion.field_24 = 2;

LABEL_23:
    float_t v5 = rob_chr->data.field_1588.field_0.field_80;
    if (v5 >= 0.0f && v5 <= rob_chr->data.motion.frame_data.frame) {
        if (v5 < (rob_chr->data.field_1588.field_0.field_8 - 1.0f)
            || rob_chr->data.field_1588.field_0.field_58 == -1
            || rob_chr->data.field_1588.field_0.field_58 == 2) {
            rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x2020081;
            rob_chr->data.field_2 |= 0x20;
        }
        rob_chr->data.motion.field_24 = 3;
    }
}

static void mothead_apply(struc_223* a1, rob_chara* rob_chr, float_t frame, motion_database* mot_db) {
    a1->field_330.field_338 = 0;

    mothead_func_data v7;
    v7.rob_chr = rob_chr;
    v7.rob_chr_data = &rob_chr->data;
    if (rob_chr->field_20)
        v7.field_10 = (rob_chara_data*)((size_t)rob_chr->field_20 + 0x440);
    else
        v7.field_10 = 0;

    struc_377* v4 = &a1->field_330.field_0;
    v7.field_18 = rob_chr;
    v7.field_28 = &rob_chr->data.field_1588;
    v7.field_20 = &rob_chr->data;
    if (!v4->field_0)
        return;

    int32_t frame_int = (int32_t)frame;
    while (v4->field_0->frame < frame_int && v4->field_0->type >= MOTHEAD_DATA_TYPE_0) {
        if (v4->field_0->type >= MOTHEAD_DATA_MAX) {
            v4->field_0++;
            continue;
        }

        mothead_data_type v4a = v4->field_0->type;
        mothead_func func = mothead_func_array[v4a].func;
        if (func) {
            if (~mothead_func_array[v4a].flags & 0x01 || frame_int <= 0)
                func(&v7, v4->field_0->data, v4->field_0, frame_int, mot_db);
            else {
                frame_int -= v4->field_0->frame;
                if (frame_int <= 1)
                    func(&v7, v4->field_0->data, v4->field_0, frame_int, mot_db);
            }
        }
        v4->field_0++;
    }
}

static void sub_1405044B0(rob_chara* rob_chr) {
    float_t v2 = rob_chr->bone_data->ik_scale.ratio0;
    float_t v4 = v2;
    if (!rob_chr->data.field_1588.field_330.field_31C) {
        rob_chr->data.motion.field_138 = v2;
        rob_chr->data.motion.field_13C = v2;
        rob_chr->data.motion.field_140 = vec3_null;
        return;
    }

    float_t v5 = rob_chr->data.field_1588.field_330.field_324;
    float_t v6 = v2;
    float_t v7 = rob_chr->data.field_1588.field_330.field_32C;
    float_t v8 = 0.0f;
    if (rob_chr->data.field_1588.field_330.field_318 == 1) {
        switch (rob_chr->data.field_1588.field_330.field_31C) {
        case 1:
            v6 = 1.0f;
            break;
        case 2:
            v6 = rob_chr->bone_data->ik_scale.ratio0;
        case 3:
            v6 = rob_chr->bone_data->ik_scale.ratio1;
        case 4:
            v6 = rob_chr->bone_data->ik_scale.ratio2;
        case 5:
            v6 = rob_chr->bone_data->ik_scale.ratio3;
        case 6:
            if (!rob_chr->field_20)
                break;
            v6 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio0;
            if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
                v6 *= v4;
        case 7:
            if (!rob_chr->field_20)
                break;
            v6 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio1;
            if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
                v6 *= v4;
        case 8:
            if (!rob_chr->field_20)
                break;
            v6 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio2;
            if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
                v6 *= v4;
        case 9:
            if (!rob_chr->field_20)
                break;
            v6 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio3;
            if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
                v6 *= v4;
            break;
        case 10:
            if (rob_chr->data.field_1588.field_330.field_320 > 0.0f)
                v6 = rob_chr->data.field_1588.field_330.field_320;
            break;
        default:
            break;
        }
        v8 = rob_chr->data.field_1588.field_330.field_328;
    }

    float_t v15 = rob_chr->data.motion.frame_data.frame;
    float_t v16 = rob_chr->data.field_1588.field_330.field_314;
    if (v15 < v16) {
        float_t v17 = rob_chr->data.field_1588.field_330.field_310;
        if (v15 < v17) {
            v6 = v5;
            v8 = v7;
        }
        else if (fabsf(v16 - v17) > 0.000001f && v16 - v17 > 0.0f) {
            float_t v19 = (v16 - v15) / (v16 - v17);
            v6 = v6 + (v5 - v6) * v19;
            v8 = v8 + (v7 - v8) * v19;
        }
    }

    rob_chr->data.motion.field_138 = v6;
    rob_chr->data.motion.field_13C = v4;
    rob_chr->data.motion.field_140.x = 0.0f;
    rob_chr->data.motion.field_140.y = v8;
    rob_chr->data.motion.field_140.z = 0.0f;
}

static void sub_1405070E0(rob_chara* rob_chr, bone_database* bone_data, motion_database* mot_db) {
    if (sub_140419E90(rob_chr->bone_data) || sub_14041A0D0(rob_chr->bone_data))
        sub_140505310(rob_chr, bone_data, mot_db);
    sub_140505980(rob_chr);
    mothead_apply(&rob_chr->data.field_1588, rob_chr, rob_chr->data.motion.frame_data.frame, mot_db);
    sub_1405044B0(rob_chr);
}

static void rob_chara_data_arm_adjust_ctrl(rob_chara* rob_chr, int32_t index, rob_chara_data_arm_adjust* arm_adjust) {
    if (!arm_adjust->enable)
        return;

    float_t prev_value = arm_adjust->prev_value;
    float_t next_value = arm_adjust->next_value;
    float_t duration = arm_adjust->duration;
    float_t frame = arm_adjust->frame;
    if (duration <= frame || fabsf(duration - frame) <= 0.000001f) {
        arm_adjust->enable = false;
        arm_adjust->value = next_value;
        return;
    }

    float_t blend = (frame + 1.0f) / (duration + 1.0f);
    float_t inv_blend = 1.0f - blend;
    if (prev_value <= 0.0001f || prev_value >= 1.0f) {
        if (next_value != 0.0f)
            arm_adjust->value = next_value * blend + inv_blend;
        else
            arm_adjust->value = 0.0f;
    }
    else {
        if (next_value != 0.0f)
            arm_adjust->value = next_value * blend + prev_value * inv_blend;
        else
            arm_adjust->value = blend + prev_value * inv_blend;
    }
    arm_adjust->frame += rob_chr->data.motion.step_data.frame;

    rob_chara_bone_data_set_motion_arm_length(rob_chr->bone_data,
        rob_motion_c_kata_bone_get(index), arm_adjust->value);
}

static void rob_chara_arm_adjust_ctrl(rob_chara* rob_chr) {
    for (int32_t i = 0; i < 2; i++)
        rob_chara_data_arm_adjust_ctrl(rob_chr, i, &rob_chr->data.motion.arm_adjust[i]);
}

static void rob_chara_set_face_motion(rob_chara* rob_chr,
    RobFaceMotion* motion, int32_t type, motion_database* mot_db);

static bool sub_14053F290(RobPartialMotion* a1) {
    return a1->data.state == 1;
}

static bool sub_14053F280(RobPartialMotion* a1) {
    return a1->data.state == 2;
}

static bool sub_14053F2A0(RobPartialMotion* a1) {
    return a1->data.state == 3;
}

static bool sub_14053F270(RobPartialMotion* a1) {
    return a1->data.state == 4;
}

static bool sub_14053F2B0(RobPartialMotion* a1) {
    return a1->data.state <= 4;
}

static bool sub_14053F1B0(RobPartialMotion* a1) {
    if (!sub_14053F2B0(a1))
        return false;

    if (sub_14053F280(a1)) {
        if (a1->data.field_38 > 3 && (a1->data.frame_count - 1.0f) < a1->data.frame)
            return true;
    }
    else if (sub_14053F270(a1)) {
        if (a1->data.field_38 > 3 && a1->data.frame < 0.0f)
            return true;
    }
    else if (sub_14053F2A0(a1)) {
        return a1->data.field_24 <= 0.0f;
    }
    return false;
}

static void rob_chara_set_face_motion_id(rob_chara* rob_chr, int32_t motion_id, float_t frame,
    int32_t state, float_t duration, float_t a6, float_t step, int32_t a8, float_t offset, motion_database* mot_db) {
    RobFaceMotion v12;
    v12.data.duration = duration;
    v12.data.offset = offset;
    if (motion_id != -1) {
        v12.data.motion_id = motion_id;
        v12.data.state = state;
        v12.data.frame = frame;
        v12.data.play_frame_step = step;
        v12.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        v12.data.field_24 = a6;
        if (sub_14053F290(&v12)) {
            v12.data.frame_data = &rob_chr->data.motion.frame_data;
            v12.data.step_data = &rob_chr->data.motion.step_data;
        }
        v12.data.field_38 = a8;
    }
    rob_chara_set_face_motion(rob_chr, &v12, 0, mot_db);
}

static void sub_1405509D0(rob_chara* rob_chr, motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.face))
        rob_chara_set_face_motion_id(rob_chr, -1, 0.0f, -1, 6.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_hand_l_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, motion_database* mot_db);

static void sub_140553E30(rob_chara* rob_chr, int32_t motion_id, float_t frame,
    int32_t state, float_t duration, float_t a6, float_t step, int32_t a8, float_t offset, motion_database* mot_db) {
    RobHandMotion v12;
    v12.data.duration = duration;
    v12.data.offset = offset;
    if (motion_id != -1) {
        v12.data.motion_id = motion_id;
        v12.data.state = state;
        v12.data.frame = frame;
        v12.data.play_frame_step = step;
        v12.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        v12.data.field_24 = a6;
        if (sub_14053F290(&v12)) {
            v12.data.frame_data = &rob_chr->data.motion.frame_data;
            v12.data.step_data = &rob_chr->data.motion.step_data;
        }
        v12.data.field_38 = a8;
    }
    rob_chara_set_hand_l_motion(rob_chr, &v12, 0, mot_db);
}

static void sub_140550A40(rob_chara* rob_chr, motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.hand_l))
        sub_140553E30(rob_chr, -1, 0.0f, -1, 12.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_hand_r_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, motion_database* mot_db);

static void sub_140554370(rob_chara* rob_chr, int32_t motion_id, float_t frame,
    int32_t state, float_t duration, float_t a6, float_t step, int32_t a8, float_t offset, motion_database* mot_db) {
    RobHandMotion v12;
    v12.data.duration = duration;
    v12.data.offset = offset;
    if (motion_id != -1) {
        v12.data.motion_id = motion_id;
        v12.data.state = state;
        v12.data.frame = frame;
        v12.data.play_frame_step = step;
        v12.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        v12.data.field_24 = a6;
        if (sub_14053F290(&v12)) {
            v12.data.frame_data = &rob_chr->data.motion.frame_data;
            v12.data.step_data = &rob_chr->data.motion.step_data;
        }
        v12.data.field_38 = a8;
    }
    rob_chara_set_hand_r_motion(rob_chr, &v12, 0, mot_db);
}

static void sub_140550AB0(rob_chara* rob_chr, motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.hand_r))
        sub_140553E30(rob_chr, -1, 0.0f, -1, 12.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_mouth_motion(rob_chara* rob_chr,
    RobMouthMotion* motion, int32_t type, motion_database* mot_db);

static void sub_140554C40(rob_chara* rob_chr, int32_t motion_id, float_t frame,
    int32_t state, float_t duration, float_t a6, float_t step, int32_t a8, float_t offset, motion_database* mot_db) {
    RobMouthMotion v12;
    v12.data.duration = duration;
    v12.data.offset = offset;
    if (motion_id != -1) {
        v12.data.motion_id = motion_id;
        v12.data.state = state;
        v12.data.frame = frame;
        v12.data.play_frame_step = step;
        v12.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        v12.data.field_24 = a6;
        if (sub_14053F290(&v12)) {
            v12.data.frame_data = &rob_chr->data.motion.frame_data;
            v12.data.step_data = &rob_chr->data.motion.step_data;
        }
        v12.data.field_38 = a8;
    }
    rob_chara_set_mouth_motion(rob_chr, &v12, 0, mot_db);
}

static void sub_140550B20(rob_chara* rob_chr, motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.mouth))
        sub_140554C40(rob_chr, -1, 0.0f, -1, 6.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_eyes_motion(rob_chara* rob_chr,
    RobEyesMotion* motion, int32_t type, motion_database* mot_db);

static void rob_chara_set_eyes_motion_id(rob_chara* rob_chr, int32_t motion_id, float_t frame,
    int32_t state, float_t duration, float_t a6, float_t step, int32_t a8, float_t offset, motion_database* mot_db) {
    RobEyesMotion v12;
    v12.data.duration = duration;
    v12.data.offset = offset;
    if (motion_id != -1) {
        v12.data.motion_id = motion_id;
        v12.data.state = state;
        v12.data.frame = frame;
        v12.data.play_frame_step = step;
        v12.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        v12.data.field_24 = a6;
        if (sub_14053F290(&v12)) {
            v12.data.frame_data = &rob_chr->data.motion.frame_data;
            v12.data.step_data = &rob_chr->data.motion.step_data;
        }
        v12.data.field_38 = a8;
    }
    rob_chara_set_eyes_motion(rob_chr, &v12, 0, mot_db);
}

static void sub_140550960(rob_chara* rob_chr, motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.eye))
        rob_chara_set_eyes_motion_id(rob_chr, -1, 0.0f, -1, 6.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_eyelid_motion(rob_chara* rob_chr,
    RobEyelidMotion* motion, int32_t type, motion_database* mot_db);

static void rob_chara_set_eyelid_motion_id(rob_chara* rob_chr, int32_t motion_id, float_t frame,
    int32_t state, float_t duration, float_t a6, float_t step, int32_t a8, float_t offset, motion_database* mot_db) {
    RobEyelidMotion v12;
    v12.data.duration = duration;
    v12.data.offset = offset;
    if (motion_id != -1) {
        v12.data.motion_id = motion_id;
        v12.data.state = state;
        v12.data.frame = frame;
        v12.data.play_frame_step = step;
        v12.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        v12.data.field_24 = a6;
        if (sub_14053F290(&v12)) {
            v12.data.frame_data = &rob_chr->data.motion.frame_data;
            v12.data.step_data = &rob_chr->data.motion.step_data;
        }
        v12.data.field_38 = a8;
    }
    rob_chara_set_eyelid_motion(rob_chr, &v12, 0, mot_db);
}

static void sub_1405508F0(rob_chara* rob_chr, motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.eyelid))
        rob_chara_set_eyelid_motion_id(rob_chr, -1, 0.0f, -1, 6.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static bool sub_1404190E0(rob_chara_bone_data* rob_bone_data) {
    return rob_bone_data->eyelid.blend.field_8 || rob_bone_data->eyelid.blend.field_9;
}

static void sub_140555F70(rob_chara* rob_chr, motion_database* mot_db) {
    if (sub_1404190E0(rob_chr->bone_data))
        return;

    if (rob_chr->data.motion.field_29 < 0)
        return;

    int32_t v3 = rob_chr->data.motion.field_150.field_1C0;
    if (v3 == 1) {
        if (rob_chr->data.motion.field_29 & 2) {
            rob_chr->set_eyelid_mottbl_motion_from_face(2, 3.0f, -1.0f, 1.0f, mot_db);
            rob_chr->data.motion.field_29 |= 0x02;
            return;
        }
        return;
    }
    else if (v3 == 2) {
        rob_chr->data.motion.field_150.field_1C0 = 0;
        rob_chr->data.motion.field_29 &= ~0x02;
    }
    else if (v3 != 0)
        return;

    sub_1405500F0(rob_chr);
    if (~rob_chr->data.motion.field_2A & 0x02 && rob_chr->data.motion.field_29 & 0x01) {
        rob_chr->data.motion.field_150.time += rand_state_array_get_float(1)
            * rob_chr->frame_speed * 2.0f * (float_t)(1.0 / 60.0);
        if (rob_chr->data.motion.field_150.time >= 5.0f) {
            rob_chr->set_eyelid_mottbl_motion_from_face(1, 3.0f, -1.0f, 1.0f, mot_db);
            rob_chr->data.motion.field_29 |= 0x02;
        }
    }
}

static void sub_14041CA70(rob_chara_bone_data* rob_bone_data, float_t a2) {
    rob_bone_data->motion_loaded.front()->field_4F8.field_C0 = a2;
}

static void sub_14041CAB0(rob_chara_bone_data* rob_bone_data, float_t a2) {
    rob_bone_data->motion_loaded.front()->field_4F8.field_C4 = a2;
}

static void sub_14041CA10(rob_chara_bone_data* rob_bone_data, vec3* a2) {
    rob_bone_data->motion_loaded.front()->field_4F8.field_C8 = *a2;
}

static void sub_140415400(mot_play_data* a1) {
    if (a1->frame_data.field_18 != 3)
        return;

    if (a1->field_40)
        a1->frame_data.frame = *a1->field_40;
    if (a1->field_48)
        a1->frame_data.step = *a1->field_48;
}

static void sub_14041DA00(rob_chara_bone_data* rob_bone_data) {
    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        sub_140415400(&i->mot_play_data);
}

static void sub_14053F2C0(RobPartialMotion* a1) {
    if (!sub_14053F290(a1))
        return;

    if (a1->data.frame_data)
        a1->data.frame = a1->data.frame_data->frame;

    if (a1->data.step_data)
        a1->data.play_frame_step = a1->data.step_data->frame;
}

static object_info sub_140550330(rob_chara* rob_chr) {
    if (rob_chr->data.motion.field_29 & 0x04)
        return rob_chr->data.motion.field_3B0.head_object;
    else
        return rob_chr->data.motion.field_150.head_object;
}

static void sub_140513950(
    rob_chara_item_equip* rob_itm_equip, item_id id, object_info object_info, bool a4,
    bone_database* bone_data, void* data, object_database* obj_db) {
    if ((uint32_t)(id - 1) > 13)
        object_info = ::object_info();
    rob_chara_item_equip_load_object_info(rob_itm_equip, object_info, id, a4, bone_data, data,  obj_db);
}

static void sub_140552310(rob_chara* rob_chr, item_id id,
    object_info object_info, bool a4, bone_database* bone_data, void* data, object_database* obj_db) {
    sub_140513950(rob_chr->item_equip,
        id, object_info, a4, bone_data, data, obj_db);
}

static object_info sub_140550350(rob_chara* rob_chr){
    if (rob_chr->data.motion.field_29 & 0x08)
        return rob_chr->data.motion.field_3B0.hand_l_object;
    else if (rob_chr->data.motion.field_2A & 0x04)
        return rob_chr->data.motion.hand_l_object;
    return rob_chr->data.motion.field_150.hand_l_object;
}

static object_info sub_140550380(rob_chara* rob_chr) {
    if (rob_chr->data.motion.field_29 & 0x10)
        return rob_chr->data.motion.field_3B0.hand_r_object;
    else if (rob_chr->data.motion.field_2A & 0x08)
        return rob_chr->data.motion.hand_r_object;
    return rob_chr->data.motion.field_150.hand_r_object;
}

static object_info sub_140550310(rob_chara* rob_chr) {
    if (rob_chr->data.motion.field_2A & 0x01)
        return rob_chr->data.motion.field_3B0.face_object;
    return rob_chr->data.motion.field_150.face_object;
}

static void sub_140412DA0(motion_blend_mot* a1, vec3* trans) {
    if (a1->bone_data.bones.size())
        *trans = a1->bone_data.bones.front().trans;
}

static void sub_140419800(rob_chara_bone_data* rob_bone_data, vec3* trans) {
    sub_140412DA0(rob_bone_data->motion_loaded.front(), trans);
}

static float_t sub_1405501F0(rob_chara* rob_chr) {
    if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
        return rob_chr->data.motion.field_138 / rob_chr->bone_data->ik_scale.ratio0;
    return rob_chr->data.motion.field_138;
}

static float_t sub_1405503B0(rob_chara* rob_chr) {
    if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
        return rob_chr->data.motion.field_13C / rob_chr->bone_data->ik_scale.ratio0;
    return rob_chr->data.motion.field_13C;
}

static void sub_140504F00(rob_chara* rob_chr) {
    if (rob_chr->data.field_1588.field_0.field_20.field_0 & 0x08
        && ~rob_chr->data.field_1588.field_0.field_20.field_8 & 0x10000000)
        return;

    vec3 v9 = vec3_null;
    sub_140419800(rob_chr->bone_data, &v9);
    float_t v3 = sub_1405503B0(rob_chr) * rob_chr->data.motion.step_data.frame;
    float_t v4 = sub_1405501F0(rob_chr) * rob_chr->data.motion.step_data.frame;
    v9.x -= v3 * rob_chr->data.field_1588.field_0.field_318.x;
    v9.y -= v4 * rob_chr->data.field_1588.field_0.field_318.y;
    v9.z -= v3 * rob_chr->data.field_1588.field_0.field_318.z;

    mat4 mat;
    mat4_rotate_y((float_t)((double_t)rob_chr->data.miku_rot.rot_y_int16 * M_PI * (1.0 / 32768.0)), &mat);
    mat4_mult_vec3(&mat, &v9, &v9);

    if (~rob_chr->data.field_1588.field_0.field_20.field_0 & 0x08) {
        rob_chr->data.field_1588.field_0.field_20.field_0 |= 0x08;
        rob_chr->data.miku_rot.position.x = rob_chr->data.miku_rot.field_24.x - v9.x;
        rob_chr->data.miku_rot.position.z = rob_chr->data.miku_rot.field_24.z - v9.z;
    }

    if (rob_chr->data.field_1588.field_0.field_20.field_8 & 0x10000000) {
        rob_chr->data.field_1588.field_0.field_20.field_8 &= ~0x10000000;
        rob_chr->data.miku_rot.position.y = rob_chr->data.miku_rot.field_24.y - v9.y;
    }
}

static void sub_140504710(rob_chara* rob_chr, motion_database* mot_db,
    bone_database* bone_data, void* data, object_database* obj_db) {
    sub_1405509D0(rob_chr, mot_db);
    sub_140550A40(rob_chr, mot_db);
    sub_140550AB0(rob_chr, mot_db);
    sub_140550B20(rob_chr, mot_db);
    sub_140550960(rob_chr, mot_db);
    sub_1405508F0(rob_chr, mot_db);
    sub_140555F70(rob_chr, mot_db);
    sub_14041C9D0(rob_chr->bone_data, !!(rob_chr->data.motion.field_28 & 0x08));
    sub_14041D2D0(rob_chr->bone_data, !!(rob_chr->data.motion.field_28 & 0x04));
    sub_14041C680(rob_chr->bone_data, !!(rob_chr->data.field_0 & 0x02));
    sub_14041D270(rob_chr->bone_data,
        (float_t)((double_t)rob_chr->data.field_1588.field_0.field_274 * M_PI * (1.0 / 32768.0)));
    sub_14041CA70(rob_chr->bone_data, rob_chr->data.motion.field_138);
    sub_14041CAB0(rob_chr->bone_data, rob_chr->data.motion.field_13C);
    sub_14041CA10(rob_chr->bone_data, &rob_chr->data.motion.field_140);
    sub_14041DA00(rob_chr->bone_data);

    struc_405* v2 = &rob_chr->data.motion.field_150;
    if ((rob_chr->data.motion.field_29 & 0x04))
        v2 = &rob_chr->data.motion.field_3B0;
    sub_14053F2C0(&v2->face);
    rob_chara_bone_data_set_face_frame(rob_chr->bone_data, v2->face.data.frame);
    rob_chara_bone_data_set_face_play_frame_step(rob_chr->bone_data, v2->face.data.play_frame_step);

    RobHandMotion* v3 = &rob_chr->data.motion.field_150.hand_l;
    if (rob_chr->data.motion.field_29 & 0x08)
        v3 = &rob_chr->data.motion.field_3B0.hand_l;
    else if (rob_chr->data.motion.field_2A & 0x04)
        v3 = &rob_chr->data.motion.hand_l;
    sub_14053F2C0(v3);
    rob_chara_bone_data_set_hand_l_frame(rob_chr->bone_data, v3->data.frame);
    rob_chara_bone_data_set_hand_l_play_frame_step(rob_chr->bone_data, v3->data.play_frame_step);

    RobHandMotion* v4 = &rob_chr->data.motion.field_150.hand_r;
    if (rob_chr->data.motion.field_29 & 0x10)
        v4 = &rob_chr->data.motion.field_3B0.hand_r;
    else if (rob_chr->data.motion.field_2A & 0x08)
        v4 = &rob_chr->data.motion.hand_r;
    sub_14053F2C0(v4);
    rob_chara_bone_data_set_hand_r_frame(rob_chr->bone_data, v4->data.frame);
    rob_chara_bone_data_set_hand_r_play_frame_step(rob_chr->bone_data, v4->data.play_frame_step);

    RobMouthMotion* v5 = &rob_chr->data.motion.field_150.mouth;
    if (rob_chr->data.motion.field_29 & 0x20)
        v5 = &rob_chr->data.motion.field_3B0.mouth;
    sub_14053F2C0(v5);
    rob_chara_bone_data_set_mouth_frame(rob_chr->bone_data, v5->data.frame);
    rob_chara_bone_data_set_mouth_play_frame_step(rob_chr->bone_data, v5->data.play_frame_step);

    RobEyesMotion* v6 = &rob_chr->data.motion.field_150.eye;
    if (rob_chr->data.motion.field_29 & 0x40)
        v6 = &rob_chr->data.motion.field_3B0.eye;
    sub_14053F2C0(v6);
    rob_chara_bone_data_set_eyes_frame(rob_chr->bone_data, v6->data.frame);
    rob_chara_bone_data_set_eyes_play_frame_step(rob_chr->bone_data, v6->data.play_frame_step);

    RobEyelidMotion* v7 = &rob_chr->data.motion.field_150.eyelid;
    if (rob_chr->data.motion.field_29 & 0x80)
        v7 = &rob_chr->data.motion.field_3B0.eyelid;
    sub_14053F2C0(v7);
    rob_chara_bone_data_set_eyelid_frame(rob_chr->bone_data, v7->data.frame);
    rob_chara_bone_data_set_eyelid_play_frame_step(rob_chr->bone_data, v7->data.play_frame_step);

    object_info v8 = sub_140550330(rob_chr);
    if (rob_chara_get_object_info(rob_chr, ITEM_ATAMA) != v8)
        sub_140552310(rob_chr, ITEM_ATAMA, v8, false, bone_data, data, obj_db);
    object_info v9 = rob_chara_get_head_object(rob_chr, 1);
    object_info v10 = rob_chara_get_head_object(rob_chr, 7);

    bool v11 = v8 == v9 || v8 == v10;
    rob_chr->bone_data->eyes.field_30 = v11;
    rob_chr->bone_data->field_788.field_91 = v11;

    object_info v12 = sub_140550350(rob_chr);
    if (rob_chara_get_object_info(rob_chr, ITEM_TE_L) != v12)
        sub_140552310(rob_chr, ITEM_TE_L, v12, true, bone_data, data, obj_db);

    object_info v13 = sub_140550380(rob_chr);
    if (rob_chara_get_object_info(rob_chr, ITEM_TE_R) != v13)
        sub_140552310(rob_chr, ITEM_TE_R, v13, true, bone_data, data, obj_db);

    object_info v14 = sub_140550310(rob_chr);
    if (rob_chara_get_object_info(rob_chr, ITEM_HARA) != v14)
        sub_140552310(rob_chr, ITEM_HARA, v14, false, bone_data, data, obj_db);

    rob_chara_bone_data_interpolate(rob_chr->bone_data);
    rob_chara_bone_data_update(rob_chr->bone_data, 0);
    sub_140504F00(rob_chr);

    rob_chr->data.motion.field_28 &= ~0x80;
}

static void sub_140504AC0(rob_chara* rob_chr) {
    vec3 v4 = rob_chr->data.miku_rot.field_30;
    vec3 v20 = vec3_null;
    if (rob_chr->data.field_0 & 0x02) {
        vec3_add(v4, rob_chr->data.miku_rot.field_3C, v20);
        vec3_add(v20, rob_chr->data.field_8.field_B8.field_10, v20);
        v4.y = rob_chr->data.field_8.field_B8.field_10.y
            - get_osage_gravity_const() * rob_chr->data.field_1588.field_0.field_200;
    }
    else if (rob_chr->data.field_1 >= 0) {
        vec3 v20 = rob_chr->data.miku_rot.field_3C;
        v20.x += v4.x + rob_chr->data.field_8.field_B8.field_10.x;
        v20.z += v4.z + rob_chr->data.field_8.field_B8.field_10.z;

        float_t v7 = 0.4f;
        if (rob_chr->data.field_1588.field_0.field_20.field_0 & 0xC00)
            v7 = 0.6f;

        float_t v13 = powf(v7, 1.0f);
        if (rob_chr->data.field_1588.field_330.field_18)
            v13 = 1.0f;

        v4.x *= v13;
        v4.y = 0.0f;
        v4.z *= v13;

        rob_chr->data.field_8.field_B8.field_10.x *= v13;
        rob_chr->data.field_8.field_B8.field_10.z *= v13;

        if (rob_chr->data.field_0 & 0x04
            && rob_chr->data.field_1588.field_0.field_1EC > rob_chr->data.motion.frame_data.frame) {
            v20.x += rob_chr->data.field_3DD4;
            v20.z += rob_chr->data.field_3DDC;
        }

        if (rob_chr->data.field_1588.field_330.field_18 == 3) {
            v20 = vec3_null;
            if (rob_chr->data.field_1588.field_330.field_1C >= rob_chr->data.motion.frame_data.frame)
                v20.y = rob_chr->data.field_1588.field_330.field_20.y;
            else
                rob_chr->data.field_1588.field_330.field_18 = 0;
        }
        else if (rob_chr->data.field_1588.field_330.field_18) {
            v20.x = 0.0f;
            v20.z = 0.0f;
            if (rob_chr->data.motion.frame_data.frame > rob_chr->data.field_1588.field_330.field_1C
                || rob_chr->data.field_1E68.field_1C && rob_chr->data.field_1588.field_330.field_18 == 1)
                rob_chr->data.field_1588.field_330.field_18 = 0;
            else {
                float_t v17 = rob_chr->data.field_1588.field_330.field_1C
                    - rob_chr->data.motion.frame_data.frame;
                if (v17 < 0.0f)
                    v17 = 0.0f;

                float_t v18 = 1.0f / (v17 + 1.0f);
                v20.x = v18 * (rob_chr->data.field_1588.field_330.field_20.x
                    - rob_chr->data.miku_rot.field_24.x);
                v20.z = v18 * (rob_chr->data.field_1588.field_330.field_20.z
                    - rob_chr->data.miku_rot.field_24.z);
            }
        }
    }

    if (rob_chr->data.field_1588.field_0.field_20.field_4 & 0x4000000
        && (rob_chr->data.field_3DA0 & 0x40 && rob_chr->data.field_3D9C & 0x20
            || rob_chr->data.field_3DA0 & 0x20 && rob_chr->data.field_3D9C & 0x40)) {
        v4.x = 0.0f;
        v4.z = 0.0f;
    }

    rob_chr->data.miku_rot.field_30 = v4;
    rob_chr->data.miku_rot.field_3C = vec3_null;
    vec3_add(v20, rob_chr->data.miku_rot.position, rob_chr->data.miku_rot.position);
}

static void rob_base_rob_chara_ctrl_thread_main(rob_chara* rob_chr) {
    data_struct* data = rctx_ptr->data;
    bone_database* bone_data = &data->data_ft.bone_data;
    motion_database* mot_db = &data->data_ft.mot_db;
    object_database* obj_db = &data->data_ft.obj_db;

    sub_140514520(rob_chr, bone_data, mot_db);
    sub_14054CC80(rob_chr);
    sub_1405077D0(rob_chr, bone_data, mot_db);
    sub_1405070E0(rob_chr, bone_data, mot_db);
    rob_chara_arm_adjust_ctrl(rob_chr);
    sub_140504710(rob_chr, mot_db, bone_data, data, obj_db);
    sub_140504AC0(rob_chr);
}

static void rob_base_rob_chara_free(rob_chara* rob_chr) {
    //sub_14054F370(&rob_chr->rob_touch);
    rob_chara_bone_data_motion_blend_mot_free(rob_chr->bone_data);
}

static void rob_disp_rob_chara_init(rob_chara* rob_chr,
    bone_database* bone_data, void* data, object_database* obj_db) {
    bone_node* v3 = rob_chara_bone_data_get_node(rob_chr->bone_data, 0);
    rob_chara_item_equip_reset_init_data(rob_chr->item_equip, v3);
    rob_chara_item_equip_set_item_equip_range(rob_chr->item_equip, rob_chr->module_index == 501);
    rob_chara_item_equip_load_object_info(rob_chr->item_equip,
        object_info(), ITEM_BODY, false, bone_data, data, obj_db);
    rob_chara_item_equip_load_object_info(rob_chr->item_equip,
        rob_chr->chara_init_data->field_7E0[ITEM_ATAMA], ITEM_ATAMA, false, bone_data, data, obj_db);
    rob_chara_item_equip_set_shadow_type(rob_chr->item_equip, rob_chr->chara_id);
    rob_chr->item_equip->field_A0 = 5;
    rob_chara_item_cos_data_reload_items(&rob_chr->item_cos_data,
        rob_chr->chara_id, bone_data, data, obj_db);
    if (rob_chara_item_cos_data_check_for_npr_flag(&rob_chr->item_cos_data))
        rob_chr->item_equip->npr_flag = true;

    /*if (rob_chara_check_for_ageageagain_module(rob_chrchara_index, rob_chr->module_index)) {
        sub_140542E30(rob_chr->chara_id, 1);
        sub_140542E30(rob_chr->chara_id, 2);
    }*/
}

static void rob_disp_rob_chara_ctrl(rob_chara* rob_chr) {
    if (!rob_chr->field_C)
        return;

    rob_chr->field_C = false;

    if (rob_chr->field_D) {
        rob_chr->field_D = false;
        return;
    }

    rob_chara_item_equip_ctrl_iterate_nodes(rob_chr->item_equip);

    /*TaskEffectSplash::Data* rob_impls1 = sub_1403649D0();
    if (rob_impls1)
        sub_140366A70(rob_impls1);*/

    /*TaskEffectFogRing::Data* v3 = sub_140348600();
    if (v3)
        sub_140349C30(v3);*/
}

static MotionBlendType motion_blend_mot_get_type(motion_blend_mot* a1) {
    MotionBlend* v1 = a1->blend;
    MotionBlendType v2 = MOTION_BLEND;
    if (v1 == &a1->cross)
        return MOTION_BLEND_CROSS;
    if (v1 == &a1->freeze)
        return MOTION_BLEND_FREEZE;
    if (v1 == &a1->combine)
        v2 = MOTION_BLEND_COMBINE;
    return v2;
}

static bool sub_140413810(motion_blend_mot* a1) {
    if (a1->blend && a1->blend->Field_30())
        return a1->blend->field_9;
    return false;
}

static void rob_disp_rob_chara_ctrl_thread_main(rob_chara* rob_chr) {
    if (rob_chara_bone_data_get_frame(rob_chr->bone_data) < 0.0f)
        rob_chara_bone_data_set_frame(rob_chr->bone_data, 0.0f);

    float_t frame_count = rob_chara_bone_data_get_frame_count(rob_chr->bone_data);
    if (rob_chara_bone_data_get_frame(rob_chr->bone_data) > frame_count)
        rob_chara_bone_data_set_frame(rob_chr->bone_data, frame_count);

    rob_chara_item_equip_set_opd_blend_data(rob_chr->item_equip, &rob_chr->bone_data->motion_loaded);

    vec3 trans = vec3_null;
    rob_chara_get_trans_scale(rob_chr, 0, &trans);
    rob_chr->item_equip->position = trans;
    rob_chara_item_equip_ctrl(rob_chr->item_equip);
    /*if (rob_chara_check_for_ageageagain_module(rob_chr->chara_index, rob_chr->module_index)) {
        sub_140543520(rob_chr->chara_id, 1, rob_chr->item_equip->step);
        sub_14054FC90(rob_chr, 1, "j_tail_l_006_wj");
        sub_140543520(rob_chr->chara_id, 2, rob_chr->item_equip->step);
        sub_14054FC90(rob_chr, 2, "j_tail_r_006_wj");
    }*/
}

static void rob_disp_rob_chara_disp(rob_chara* rob_chr) {
    rob_chr->item_equip->texture_color_coeff = vec4u_identity;
    rob_chr->item_equip->mat = rob_chr->data.adjust_data.mat;
    rob_chara_item_equip_disp(rob_chr->item_equip, rob_chr->chara_id, rctx_ptr);
}

static void rob_disp_rob_chara_free(rob_chara* rob_chr) {
    //sub_140541260(rob_chr->chara_id);
    rob_chr->item_equip->reset();
}

static bool sub_14041A010(rob_chara_bone_data* rob_bone_data) {
    float_t v1 = rob_bone_data->field_788.field_A0;
    if (v1 > 0.0f) {
        float_t v3 = rob_bone_data->field_788.field_A4;
        if (v1 > v3 && fabsf(v1 - v3) > 0.000001f)
            return true;
    }
    return rob_bone_data->field_788.field_8E;
}

static bool sub_140419F90(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->field_788.field_8E)
        return rob_bone_data->field_788.field_94 > 0.0f;
    return false;
}

static bool sub_140419F00(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->field_788.field_14C)
        return fabsf(rob_bone_data->field_788.field_150) > 0.000001f;
    return false;
}

static void sub_140409B70(struc_258* a1, vec3* a2, mat4u* a3, bool a4) {
    if (a2) {
        a1->field_190 = true;
        a1->field_191 = a4;
        a1->field_192 = a4;
        a1->field_193 = a4;
        a1->field_194 = a4;
        a1->field_195 = a4;

        if (a4 && a1->field_A0 > 0.0f && a1->field_A0 > a1->field_AC
            && fabsf(a1->field_A0 - a1->field_AC) > 0.000001f) {
            a1->field_AC = a1->field_AC + a1->field_A8;
            a1->field_198 = (a1->field_AC + 1.0f) / (a1->field_A0 + 1.0f);
        }
        else {
            a1->field_191 = false;
            a1->field_198 = 1.0f;
        }

        mat4 mat = *a3;
        mat4_inverse(&mat, &mat);

        vec3 v24;
        mat4_mult_vec3_trans(&mat, a2, &v24);

        float_t v12 = v24.x - 0.05f;
        if (a1->field_184 >= v12) {
            v12 = v24.x + 0.05f;
            if (a1->field_184 <= v12)
                v12 = a1->field_184;
        }
        a1->field_184 = v12;

        float_t v15 = v24.y - 0.05f;
        if (a1->field_188 >= v15) {
            v15 = v24.y + 0.05f;
            if (a1->field_188 <= v15)
                v15 = a1->field_188;
        }
        a1->field_188 = v15;

        float_t v18 = v24.z - 0.05f;
        if (a1->field_18C >= v18) {
            v18 = v24.z + 0.05f;
            if (a1->field_18C <= v18)
                v18 = a1->field_18C;
        }
        a1->field_18C = v18;
    }
    else {
        if (!a1->field_190 && !a1->field_191) {
            a4 = false;
            a1->field_198 = 0.0f;
        }
        else if (a4 && a1->field_A0 > 0.0f && a1->field_A0 > a1->field_AC
            && fabsf(a1->field_A0 - a1->field_AC) > 0.000001f) {
            a1->field_AC += a1->field_A8;
            a1->field_198 = 1.0f - (a1->field_AC + 1.0f) / (a1->field_A0 + 1.0f);
        }
        else {
            a1->field_191 = false;
            a1->field_198 = 0.0f;
        }

        a1->field_190 = false;
        a1->field_191 = a4;
        a1->field_192 = false;
        a1->field_193 = false;
        a1->field_194 = false;
        a1->field_195 = false;
    }
}

static void sub_14041C620(rob_chara_bone_data* rob_bone_data, vec3* a2, mat4u* a3, char a4) {
    sub_140409B70(&rob_bone_data->field_788, a2, a3, a4);
}

static void sub_1404189A0(rob_chara_bone_data* rob_bone_data) {
    //motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    //sub_140407280(&rob_bone_data->field_788, &v2->bone_data.bones,
    //    &v2->field_4F8.mat, v2->mot_play_data.frame_data.step);
}

static void sub_14041A160(rob_chara_bone_data* rob_bone_data, mat4u* a2) {
    //motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    //sub_140409170(&rob_bone_data->field_788, a2, &v2->bone_data.bones,
    //    &v2->field_4F8.mat, v2->mot_play_data.frame_data.step);
}

static void sub_14053BEE0(rob_chara* rob_chr) {
    if (rob_chr->type != ROB_CHARA_TYPE_2)
        return;

    bool v4 = true;
    if (rob_chr->data.field_2 & 0x80)
        v4 = false;
    rob_chr->data.field_2 &= ~0x80;

    rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;
    vec3* v3 = 0;
    if (sub_14041A010(rob_bone_data) || rob_bone_data->field_788.field_14C) {
        if (rob_bone_data->field_788.field_8C) {
            vec3 view_point;
            camera_get_view_point(rctx_ptr->camera, &view_point);
            rob_bone_data->field_788.field_B4 = view_point;
        }

        if (sub_140419F90(rob_chr->bone_data) || sub_140419F00(rob_bone_data))
            v3 = &rob_bone_data->field_788.field_B4;
    }

    sub_14041C620(rob_bone_data, v3, &rob_chr->data.adjust_data.mat, v4);
    sub_1404189A0(rob_bone_data);
    sub_14041A160(rob_bone_data, &rob_chr->data.adjust_data.mat);
}

static void rob_chara_set_hand_adjust(rob_chara* rob_chr,
    rob_chara_data_hand_adjust* adjust, rob_chara_data_hand_adjust* prev_adjust) {
    if (!adjust->enable)
        return;

    float_t chara_scale = rob_chr->data.adjust_data.scale;
    float_t opposite_chara_scale = rob_chara_array_get_data_adjust_scale(rob_chr->chara_id ? 0 : 1);
    bool chara_opposite_chara_same = fabsf(chara_scale - opposite_chara_scale) <= 0.000001f;

    switch (adjust->type) {
    case ROB_CHARA_DATA_HAND_ADJUST_NORMAL:
    case 15:
        adjust->scale = chara_size_table_get_value(1);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_SHORT:
        adjust->scale = chara_size_table_get_value(2);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_TALL:
        adjust->scale = chara_size_table_get_value(0);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_MIN:
        adjust->scale = min(opposite_chara_scale, chara_scale);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_MAX:
        adjust->scale = max(opposite_chara_scale, chara_scale);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_OPPOSITE_CHARA:
        adjust->scale = opposite_chara_scale;
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_1P:
        adjust->scale = rob_chara_array_get_data_adjust_scale(0);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_2P:
        adjust->scale = rob_chara_array_get_data_adjust_scale(1);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_3P:
        adjust->scale = rob_chara_array_get_data_adjust_scale(2);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_4P:
        adjust->scale = rob_chara_array_get_data_adjust_scale(3);
        break;
    }

    if (adjust->scale_select == 1 && adjust->enable_scale && chara_opposite_chara_same) {
        adjust->scale_select = 0;
        adjust->rotation_blend = prev_adjust->rotation_blend;
        adjust->disable_x = prev_adjust->disable_x;
        adjust->disable_y = prev_adjust->disable_y;
        adjust->disable_z = prev_adjust->disable_z;
        adjust->scale_blend = 1.0f;
    }

    float_t prev_scale;
    if (prev_adjust->enable || prev_adjust->scale_select == 1)
        prev_scale = prev_adjust->current_scale;
    else
        prev_scale = chara_scale;

    float_t scale;
    if (adjust->scale_select == 1)
        scale = adjust->scale;
    else
        scale = chara_scale;

    if (fabsf(prev_scale - scale * adjust->scale_blend) <= 0.000001f || adjust->duration <= adjust->current_time
        || fabsf(adjust->duration - adjust->current_time) <= 0.000001f) {
        adjust->current_scale = lerp(prev_scale, scale, adjust->scale_blend);
        if (fabsf(adjust->current_scale - chara_scale) <= 0.000001f) {
            adjust->current_scale = chara_scale;
            adjust->enable = false;
        }
    }
    else {
        float_t t = (adjust->current_time + 1.0f) / (adjust->duration + 1.0f);
        adjust->current_scale = lerp(prev_scale, scale, t * adjust->scale_blend);
        adjust->current_time += rob_chr->data.motion.step_data.frame;
    }
}

static void rob_chara_set_hands_adjust(rob_chara* rob_chr) {
    for (int32_t i = 0; i < 2; i++)
        rob_chara_set_hand_adjust(rob_chr, &rob_chr->data.motion.hand_adjust[i],
            &rob_chr->data.motion.hand_adjust_prev[i]);
}

static void sub_14040AE10(mat4* mat, vec3* a2) {
    vec3 v8;
    mat4_mult_vec3_inv_trans(mat, a2, &v8);
    float_t v3 = v8.x * v8.x + v8.y * v8.y;
    float_t v5 = sqrtf(v3);
    float_t v6 = sqrtf(v3 + (v8.z * v8.z));
    if (fabsf(v5) > 0.000001f && fabsf(v6) > 0.000001f) {
        float_t v7 = v5;
        v5 = 1.0f / v5;
        v6 = 1.0f / v6;
        mat4_rotate_z_mult_sin_cos(mat, v5 * v8.y, v5 * v8.x, mat);
        mat4_rotate_y_mult_sin_cos(mat, -v6 * v8.z, v6 * v7, mat);
    }
}

static void sub_140406A70(vec3* a1, std::vector<bone_data>* bones, mat4* a3, vec3* a4,
    const motion_bone_index* a5, float_t rotation_blend, float_t arm_length, bool solve_ik) {
    bone_data* v8 = bones->data();
    bone_data* v14 = &v8[a5[0]];

    if (v14->pole_target_mat) {
        vec3 v36;
        mat4_get_translation(v14->pole_target_mat, &v36);

        mat4 v38 = *v14->node->mat;
        sub_14040AE10(&v38, &v14->ik_target);
        mat4_mult_vec3_inv_trans(&v38, &v36, &v36);

        v38 = *v14->node->mat;
        sub_14040AE10(&v38, a4);

        vec3 v37;
        mat4_mult_vec3_trans(&v38, &v36, &v37);
        mat4_set_translation(v14->pole_target_mat, &v37);
    }

    v14->ik_target = *a4;
    v14->rot_mat[1] = mat4_identity;
    v14->rot_mat[2] = mat4_identity;
    bone_data_mult_ik(v14, 1);
    if (a5[1] == MOTION_BONE_NONE)
        return;

    bone_data* v25 = &v8[a5[1]];
    if (rotation_blend > 0.0f) {
        mat4 v38;
        mat4 v39;
        mat4_clear_trans(v25->parent_mat, &v38);
        mat4_clear_trans(v25->node->mat, &v39);
        mat4_transpose(&v38, &v38);
        mat4_mult(&v39, &v38, &v38);
        if (rotation_blend < 1.0) {
            mat4 rot_mat = v25->rot_mat[0];
            mat4_lerp_rotation(&rot_mat, &v38, &v38, rotation_blend);
        }
        v25->rot_mat[0] = v38;
    }

    if (solve_ik) {
        a5++;
        while (*a5 != MOTION_BONE_NONE)
            bone_data_mult_1(&v8[*a5++], a3, v8, true);
    }
    else
        bone_data_mult_1(v25, a3, v8, false);
}

static void sub_140418A00(rob_chara_bone_data* rob_bone_data, vec3* a2,
    const motion_bone_index* a3, float_t rotation_blend, float_t arm_length, bool solve_ik) {
    motion_blend_mot* v7 = rob_bone_data->motion_loaded.front();
    mat4 m = v7->field_4F8.mat;
    sub_140406A70(rob_bone_data->field_76C, &v7->bone_data.bones,
        &m, a2, a3, rotation_blend, arm_length, solve_ik);
}

static bool sub_14053ACA0(rob_chara* rob_chr, int32_t hand) {
    if (hand >= 2 || !rob_chr->data.motion.hand_adjust[hand].enable)
        return false;

    float_t rotation_blend = rob_chr->data.motion.hand_adjust[hand].rotation_blend;
    float_t chara_scale = rob_chr->data.adjust_data.scale;
    float_t adjust_scale = rob_chr->data.motion.hand_adjust[hand].current_scale;

    rob_chara_adjust_data v41 = rob_chr->data.adjust_data;
    v41.scale = adjust_scale / chara_scale;
    rob_chara_set_data_adjust_mat(rob_chr, &v41, 0);

    int32_t v15 = 0;
    bool solve_ik = true;
    if (rob_chr->data.motion.hand_adjust[hand].field_40 > 0
        && fabsf(rotation_blend - 1.0f) > 0.000001f
        && (fabsf(rob_chr->data.motion.hand_adjust[hand].offset.x) > 0.000001f
            || fabsf(rob_chr->data.motion.hand_adjust[hand].offset.y) > 0.000001f
            || fabsf(rob_chr->data.motion.hand_adjust[hand].offset.z) > 0.000001f)) {
        v15 = rob_chr->data.motion.hand_adjust[hand].field_40;
        solve_ik = false;
    }

    mat4 v40 = *rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, rob_kl_te_bones[hand]);
    mat4 v42 = v41.mat;
    mat4_mult(&v40, &v42, &v42);

    vec3 v37;
    vec3_mult_scalar(rob_chr->data.motion.hand_adjust[hand].offset, 1.0f - chara_scale / adjust_scale, v37);

    vec3 v38;
    mat4_mult_vec3_trans(&v42, &v37, &v38);

    vec3 v27 = vec3_null;
    if (rob_chr->data.motion.hand_adjust[hand].disable_x)
        v27.x = v40.row3.x - v38.x;
    if (rob_chr->data.motion.hand_adjust[hand].disable_y)
        v27.y = v40.row3.y - v38.y;
    if (rob_chr->data.motion.hand_adjust[hand].disable_z)
        v27.z = v40.row3.z - v38.z;

    vec3 v39;
    vec3_add(v38, v27, v39);

    vec3 v18 = vec3_null;
    if (v15 > 0) {
        mat4_mult_vec3_trans(&v42, &rob_chr->data.motion.hand_adjust[hand].offset, &v38);
        vec3_add(v38, v27, v18);
    }

    sub_140418A00(rob_chr->bone_data, &v39, off_140C9E020[hand], rotation_blend,
        rob_chr->data.motion.hand_adjust[hand].arm_length, solve_ik);

    while (v15 > 0) {
        mat4 v40 = *rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, rob_kl_te_bones[hand]);
        mat4_set_translation(&v40, &v18);

        vec3 v37;
        vec3_negate(rob_chr->data.motion.hand_adjust[hand].offset, v37);
        mat4_mult_vec3_trans(&v40, &v37, &v39);
        if (!--v15) {
            rotation_blend = 1.0f;
            solve_ik = true;
        }

        sub_140418A00(rob_chr->bone_data, &v39, off_140C9E020[hand], rotation_blend,
            rob_chr->data.motion.hand_adjust[hand].arm_length, solve_ik);
    }

    rob_chr->data.motion.hand_adjust[hand].field_30 = v39;
    return true;
}

static void sub_140412860(motion_blend_mot* mot, motion_bone_index index, mat4* mat) {
    if (index >= mot->bone_data.bones.size())
        return;

    bone_data* v4 = &mot->bone_data.bones.data()[index];
    if (!v4)
        return;

    switch (v4->type) {
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        *mat = *v4->node[2].mat;
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        *mat = *v4->node[3].mat;
        break;
    }
}

static void sub_140419290(rob_chara_bone_data* rob_bone_data, motion_bone_index index, mat4* mat) {
    sub_140412860(rob_bone_data->motion_loaded.front(), index, mat);
}

static bool sub_14053B580(rob_chara* rob_chr, int32_t a2) {
    if (a2 < 0 || a2 > 3)
        return 0;

    struc_306& v0 = rob_chr->data.field_1588.field_330.field_33C[a2];
    int16_t v7 = v0.field_C;
    if (v7 <= 0 || v7 >= 27 || !(v7 < 10 || rob_chr->field_20))
        return false;

    bool v2 = false;
    int32_t v8 = v0.field_0;
    float_t v9 = rob_chr->data.motion.frame_data.frame;
    if (v8 == 0) {
        if (v9
            < v0.frame)
            return false;
    }
    else if (v8 == 1) {
        if (v9
            < v0.frame
            || v9
            >= v0.field_8)
            return false;
    }
    else
        return false;

    mat4 mat;
    sub_140419290(rob_chr->bone_data, rob_ik_hand_leg_bones[a2], &mat);
    vec3 v13;
    mat4_get_translation(&mat, &v13);

    vec3 v45;
    switch (v0.field_C) {
    case 0:
        v45 = v0.field_10;
        break;
    case 16:
        vec3_add(v0.field_10, v13, v45);
        break;
    case 10: {
        float_t ratio0 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio0;
        vec3_mult_scalar(v13, ratio0, v45);
    } break;
    case 11: {
        float_t ratio1 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio1;
        vec3_mult_scalar(v13, ratio1, v45);
    } break;
    case 12: {
        float_t ratio3 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio3;
        vec3_mult_scalar(v13, ratio3, v45);
    } break;
    case 13: {
        float_t ratio0 = rob_chr->bone_data->ik_scale.ratio0;
        vec3_mult_scalar(v13, 1.0f / ratio0, v45);
    } break;
    case 14: {
        float_t ratio1 = rob_chr->bone_data->ik_scale.ratio1;
        vec3_mult_scalar(v13, 1.0f / ratio1, v45);
    } break;
    case 15: {
        float_t ratio3 = rob_chr->bone_data->ik_scale.ratio3;
        vec3_mult_scalar(v13, 1.0f / ratio3, v45);
    } break;
    default:
        if (v0.field_C < 16) {
            mat4* v26 = rob_chara_bone_data_get_mats_mat(
                (rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28), dword_140A2DDD0[v0.field_C]);
            mat4_get_translation(v26, &v45);
            vec3_add(v45, v0.field_10, v45);
        }
        else {
            mat4* v23 = rob_chara_bone_data_get_mats_mat(
                (rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28), dword_140A2DDD0[v0.field_C]);

            vec3& v24 = v0.field_10;

            mat4 v49;
            mat4_translate_mult(v23, v24.x, v24.y, v24.z, &v49);
            mat4_get_translation(&v49, &v45);
        }
        break;
    }

    float_t v29 = rob_chr->data.motion.frame_data.frame;
    float_t v30 = v0.frame;
    if (v29 < v30)
        v29 = v0.frame;
    else if (v29 > v0.field_8)
        v29 = v0.field_8;

    float_t v31 = 1.0f;
    float_t v32 = v0.field_8 - v30 + 1.0f;
    if (fabsf(v32) > 0.000001f) {
        v31 = (v0.field_8 - v29) / v32;
        v31 = clamp(v31, 0.0f, 1.0f);
    }

    int32_t v33 = v0.field_0;
    vec3 v48 = vec3_null;
    if (v33 == 0) {
        vec3_lerp_scalar(v45, v13, v48, v31);
    }
    else if (v33 == 1) {
        vec3_lerp_scalar(v13, v45, v48, v31);
    }

    switch (v0.field_E) {
    case 1:
        v48.y = v13.y;
        v48.z = v13.z;
        break;
    case 2:
        v48.x = v13.x;
        v48.z = v13.z;
        break;
    case 3:
        v48.x = v13.x;
        v48.y = v13.y;
        break;
    case 4:
        v48.z = v13.z;
        break;
    case 5:
        v48.y = v13.y;
        break;
    case 6:
        v48.x = v13.z;
        break;
    }

    vec3 v36;
    vec3_sub(v48, v13, v36);
    if (v0.field_1C.x != 0.0f && v36.x < v0.field_1C.x)
        v48.x = v0.field_1C.x + v13.x;
    else if (v0.field_28.x != 0.0f && v36.x > v0.field_28.x)
        v48.x = v0.field_28.x + v13.x;

    if (v0.field_1C.y != 0.0f && v36.y < v0.field_1C.y)
        v48.y = v0.field_1C.y + v13.y;
    else if (v0.field_28.y != 0.0f && v36.y > v0.field_28.y)
        v48.y = v0.field_28.y + v13.y;

    if (v0.field_1C.z != 0.0f && v36.z < v0.field_1C.z)
        v48.z = v0.field_1C.z + v13.z;
    else if (v0.field_28.z != 0.0f && v36.z > v0.field_28.z)
        v48.z = v0.field_28.z + v13.z;
    sub_140418A00(rob_chr->bone_data, &v48, off_140C9E000[a2], 0.0f, 0.0f, true);
    return true;
}

static void sub_1403F9A40(vec3* a1, vec3* a2, mat4* a3, float_t a4, float_t a5) {
    vec3 v10;
    mat4_mult_vec3_inv_trans(a3, a2, &v10);
    v10.y += a4;
    v10.z += a5;
    mat4_mult_vec3_trans(a3, &v10, a1);
}

static bool sub_14040AC40(vec3* a1, vec3* a2, float_t a3, vec3* a4) {
    vec3 v4;
    vec3_negate(*a2, v4);
    float_t v7;
    vec3_dot(*a1, v4, v7);
    float_t v8;
    vec3_dot(v4, v4, v8);
    v8 -= a3 * a3;
    if (v8 > 0.0f && v7 > 0.0f)
        return false;

    float_t v10 = v7 * v7 - v8;
    if (v10 < 0.0f)
        return false;

    vec3_mult_scalar(*a1, sqrtf(v10) - v7, *a4);
    return true;
}

static void sub_1403FA040(vec3* a1, float_t a2, mat4* a3, float_t a4,
    float_t a5, float_t a6, float_t a7, vec3* a8, float_t a9) {
    vec3 v16;
    mat4_mult_vec3_inv(a3, a1, &v16);
    v16.x = 0.0f;

    float_t v18;
    vec3_length_squared(v16, v18);
    if (fabsf(v18) <= 0.000001f)
        goto LABEL_66;

    vec3_normalize(v16, v16);

    vec3 v52;
    vec3 v53;
    float_t v23;
    if (a7 - a6 >= a5 - a4) {
        v23 = (a5 - a4) * 0.5f;

        v53.x = 0.0f;
        v53.y = a5 - v23;
        v53.z = a6 + v23;

        v52.x = 0.0f;
        v52.y = a5 - v23;
        v52.z = a7 - v23;
        if (fabsf(v16.y) <= 0.000001f) {
            if (fabsf(a4) <= 0.000001f || fabsf(a5) <= 0.000001f) {
                vec3_mult_scalar(v16, a2, v16);
                if (v16.z < a6 + v23) {
                    v16.z = a6 + v23;
                    goto LABEL_66;
                }
                else if (v16.z > a7 - v23) {
                    v16.z = a7 - v23;
                    goto LABEL_66;
                }
                goto LABEL_60;
            }
        }
        else if ((fabsf(a4) > 0.000001f || v16.y >= 0.0f) && (fabsf(a5) > 0.000001f || v16.y <= 0.0f)) {
            float_t v22 = a5;
            if (v16.y * a5 <= 0.0f)
                v22 = a4;

            float_t v31 = v22 * v16.z / v16.y;
            if (v31 >= a6 + v23 && v31 <= a7 - v23) {
                float_t v32 = sqrtf(v31 * v31 + v22 * v22);
                bool v17 = true;
                if (a2 < v32) {
                    v32 = a2;
                    v17 = false;
                }
                vec3_mult_scalar(v16, v32, v16);
                if (v17)
                    goto LABEL_66;
            }
        }
        else {
            v16.y = 0.0f;
            if (fabsf(v16.z) > 0.000001f) {
                vec3_normalize(v16, v16);
                vec3_mult_scalar(v16, a2, v16);
                v16.z = clamp(v16.z, a6, a7);
            }
            goto LABEL_66;
        }
    }
    else {
        v23 = (a7 - a6) * 0.5f;

        v53.x = 0.0f;
        v53.y = a5 - v23;
        v53.z = a7 - v23;

        v52.x = 0.0f;
        v52.y = 0.0f;
        v52.z = a7 - v23;
        if (fabsf(v16.z) <= 0.000001f) {
            if (fabsf(a6) <= 0.000001f || fabsf(a7) <= 0.000001f) {
                vec3_mult_scalar(v16, a2, v16);
                if (v16.y < a4 + v23) {
                    v16.y = a4 + v23;
                    goto LABEL_66;
                }
                else if (v16.y > a5 - v23) {
                    v16.y = a5 - v23;
                    goto LABEL_66;
                }
                goto LABEL_60;
            }
        }
        else if ((fabsf(a6) > 0.000001f || v16.z >= 0.0f) && (fabsf(a7) > 0.000001f || v16.z <= 0.0f)) {
            float_t v22 = a7;
            if (v16.z * a7 <= 0.0f)
                v22 = a6;

            float_t v31 = v22 * v16.y / v16.z;
            if (v31 >= a4 + v23 && v31 <= a5 - v23) {
                float_t v32 = sqrtf(v31 * v31 + v22 * v22);
                bool v17 = true;
                if (a2 < v32) {
                    v32 = a2;
                    v17 = false;
                }
                vec3_mult_scalar(v16, v32, v16);
                if (v17)
                    goto LABEL_66;
            }
        }
        else {
            v16.z = 0.0f;
            if (fabsf(v16.y) > 0.000001f) {
                vec3_normalize(v16, v16);
                vec3_mult_scalar(v16, a2, v16);
                v16.y = clamp(v16.y, a4, a5);
            }
            goto LABEL_66;
        }
    }

    {
        vec3 v51 = vec3_null;
        if (!sub_14040AC40(&v16, &v53, v23, &v51) && !sub_14040AC40(&v16, &v52, v23, &v51)) {
            v16 = vec3_null;
            goto LABEL_66;
        }

        float_t v40;
        vec3_length(v51, v40);
        bool v17 = true;
        if (a2 < v40) {
            v40 = a2;
            v17 = false;
        }
        vec3_mult_scalar(v16, v40, v16);
        if (v17)
            goto LABEL_66;
    }

LABEL_60:
    {
        float_t v20;
        vec3_dot(*a8, v16, v20);
        if (v20 != 0.0f) {
            float_t v43;
            vec3_length(*a8, v43);
            float_t v44 = v43 + (v43 * 0.97f - v43) * a9;

            float_t v45;
            vec3_length(v16, v45);
            if (v45 < v44) {
                if (v45 != 0.0f)
                    vec3_mult_scalar(v16, 1.0f / v45, v16);
                vec3_mult_scalar(v16, v44, v16);
            }
        }
    }

LABEL_66:
    mat4_mult_vec3(a3, &v16, a1);
    *a8 = v16;
}

static void sub_1403FA770(struc_312* a1) {
    mat4* v4 = a1->bones->data()[MOTION_BONE_C_KATA_L].node[3].mat;
    mat4* v5 = a1->bones->data()[MOTION_BONE_C_KATA_R].node[3].mat;

    vec3 v42;
    mat4_get_translation(v4, &v42);
    vec3 v43;
    mat4_get_translation(v5, &v43);

    vec3 v41;
    sub_1403F9A40(&v41, &v42, v4, a1->field_0.field_4, a1->field_0.field_8);
    vec3 v40;
    sub_1403F9A40(&v40, &v43, v5, a1->field_2C.field_4, a1->field_2C.field_8);

    vec3 v45;
    vec3_sub(v41, v40, v45);

    vec3 v44;
    vec3_sub(v40, v41, v44);

    float_t v20;
    vec3_distance(v41, v40, v20);

    float_t v21 = (a1->field_0.field_0 * a1->field_8C + a1->field_2C.field_0 - v20) * 0.5f;
    float_t v22 = (a1->field_2C.field_0 * a1->field_8C + a1->field_0.field_0 - v20) * 0.5f;

    bool v16 = true;
    bool v17 = false;
    if (v21 <= 0.0f) {
        v16 = false;
        vec3_length(a1->field_5C, v21);
        float_t v25 = (0.97f * v21 - v21) * a1->step + v21;
        if (v21 <= 0.001f || v25 <= 0.0f) {
            a1->field_5C = vec3_null;
            v45 = vec3_null;
        }
        else {
            vec3_mult_scalar(a1->field_5C, v25 / v21, a1->field_5C);
            mat4_mult_vec3(v4, &a1->field_5C, &v45);
            v17 = true;
        }
    }

    bool v19 = true;
    bool v18 = false;
    if (v22 <= 0.0f) {
        v19 = false;
        vec3_length(a1->field_68, v22);
        float_t v31 = (0.97f * v22 - v22) * a1->step + v22;
        if (v22 <= 0.001f || v31 <= 0.0f) {
            a1->field_68 = vec3_null;
            v44 = vec3_null;
        }
        else {
            vec3_mult_scalar(a1->field_68, v31 / v22, a1->field_68);
            mat4_mult_vec3(v5, &a1->field_68, &v44);
            v18 = true;
        }
    }

    if (v16)
        sub_1403FA040(&v45, v21, v4, a1->field_0.field_C, a1->field_0.field_10,
            a1->field_0.field_14, a1->field_0.field_18, &a1->field_5C, a1->step);

    if (v19)
        sub_1403FA040(&v44, v22, v5, a1->field_2C.field_C, a1->field_2C.field_10,
            a1->field_2C.field_14, a1->field_2C.field_18, &a1->field_68, a1->step);

    if (v16 || v17) {
        vec3_add(v42, v45, v42);
        sub_14040AE10(a1->bones->data()[MOTION_BONE_C_KATA_L].node[2].mat, &v42);
    }

    if (v19 || v18) {
        vec3_add(v43, v44, v43);
        sub_14040AE10(a1->bones->data()[MOTION_BONE_C_KATA_R].node[2].mat, &v43);
    }
}

static void sub_1403F9B20(struc_312* a1, motion_bone_index motion_bone_index) {
    float_t v3;
    float_t v4;
    float_t a5;
    vec3* v6;
    float_t v7;
    float_t v8;
    float_t v9;
    float_t v10;
    vec3 v11;
    float_t v14;
    if (motion_bone_index == MOTION_BONE_C_KATA_L) {
        v3 = a1->field_0.field_0;
        v4 = a1->field_0.field_4;
        a5 = a1->field_0.field_8;
        v7 = a1->field_0.field_C;
        v8 = a1->field_0.field_10;
        v9 = a1->field_0.field_14;
        v10 = a1->field_0.field_18;
        v11 = a1->field_0.field_1C;
        v14 = a1->field_0.field_28;
        v6 = &a1->field_74;
    }
    else if (motion_bone_index == MOTION_BONE_C_KATA_R) {
        v3 = a1->field_2C.field_0;
        v4 = a1->field_2C.field_4;
        a5 = a1->field_2C.field_8;
        v7 = a1->field_2C.field_C;
        v8 = a1->field_2C.field_10;
        v9 = a1->field_2C.field_14;
        v10 = a1->field_2C.field_18;
        v11 = a1->field_2C.field_1C;
        v14 = a1->field_2C.field_28;
        v6 = &a1->field_80;
    }
    else
        return;

    bone_data* v15 = a1->bones->data();
    bone_node* v18 = v15[motion_bone_index].node;
    mat4* v19 = v18[MOTION_BONE_KL_HARA_ETC].mat;

    vec3 v39;
    mat4_get_translation(v19, &v39);

    vec3 v37;
    sub_1403F9A40(&v37, &v39, v19, v4, a5);

    vec3 v40;
    mat4* v23 = v15[MOTION_BONE_KL_MUNE_B_WJ].node->mat;
    mat4_mult_vec3_trans(v23, &v11, &v40);

    vec3 v41;
    vec3_sub(v37, v40, v41);

    float_t v24;
    vec3_length(v41, v24);
    float_t v26 = v3 * a1->field_8C + v14 - v24;
    if (v26 > 0.0f)
        sub_1403FA040(&v41, v26, v19, v7, v8, v9, v10, v6, a1->step);
    else {
        float_t v31;
        vec3_length(*v6, v31);
        float_t v32 = (0.97f * v31 - v31) * a1->step + v31;
        if (v31 <= 0.001f || v32 <= 0.0f) {
            *v6 = vec3_null;
            return;
        }
        vec3_mult_scalar(*v6, v32 / v31, *v6);
        mat4_mult_vec3(v19, v6, &v41);
    }

    mat4* v42 = v15[motion_bone_index].node[2].mat;
    vec3_add(v39, v41, v39);
    sub_14040AE10(v42, &v39);
}

static void sub_1403FAF30(struc_312* a1, std::vector<bone_data>* bones, float_t step) {
    a1->step = step;
    a1->bones = bones;
    if (a1->field_58)
        sub_1403FA770(a1);

    if (a1->field_59) {
        sub_1403F9B20(a1, MOTION_BONE_C_KATA_L);
        sub_1403F9B20(a1, MOTION_BONE_C_KATA_R);
    }
}

static void sub_14041AB50(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    sub_1403FAF30(&rob_bone_data->field_958, &v2->bone_data.bones, v2->mot_play_data.frame_data.step);
}

static bool rob_chara_hands_adjust(rob_chara* rob_chr) {
    rob_chara_set_hands_adjust(rob_chr);
    bool v2 = false;
    if (sub_14053ACA0(rob_chr, 0) || sub_14053B580(rob_chr, 1))
        v2 = true;
    if (sub_14053ACA0(rob_chr, 1) || sub_14053B580(rob_chr, 0))
        v2 = true;
    sub_14041AB50(rob_chr->bone_data);
    return v2;
}

static bool sub_14053B530(rob_chara* rob_chr) {
    return sub_14053B580(rob_chr, 2) | sub_14053B580(rob_chr, 3);
}

static float_t sub_140406E90(bone_data* a1, bone_data* a2, float_t a3, vec3* a4) {
    vec3 v18 = vec3_null;
    mat4_mult_vec3_trans(a1->node[3].mat, &v18, &v18);
    *a4 = v18;

    vec3 v17;
    v17.x = 0.01f;
    v17.y = -0.05f;
    v17.z = 0.0;
    mat4_mult_vec3_trans(a2->node[0].mat, &v18, &v17);

    if (v18.y - a3 <= v17.y)
        return v18.y - a3;
    return  v17.y;
}

static float_t sub_14064AD10(vec3* a1, float_t a2) {
    task_stage_info v5;
    task_stage_get_current_stage_info(&v5);
    if (task_stage_has_stage_info(&v5)) {
        stage* stg = task_stage_get_stage(v5);
        if (!stg)
            return -1000.0f;

        stage_data* v3 = stg->stage_data;
        if (!v3)
            return -1000.0f;

        if (a1->x < v3->ring_rectangle_x - a2
            || a1->z < v3->ring_rectangle_y - a2
            || a1->x > v3->ring_rectangle_x + v3->ring_rectangle_width
            || a1->z > v3->ring_rectangle_y + v3->ring_rectangle_height)
            return v3->ring_out_height;
        return v3->ring_height;
    }
    return -1000.0f;
}

static void sub_140406920(vec3* a1, bone_data* a2, bone_data* a3, float_t heel_height, vec3* a5, float_t step) {
    vec3 v18;
    float_t v7 = sub_140406E90(a2, a3, heel_height, &v18);
    float_t v8 = sub_14064AD10(&v18, heel_height);
    if (a5->y != v8) {
        a5->x = a5->y;
        a5->z = 0.0f;
    }
    a5->y = v8;

    a5->z = step * 10.0f + a5->z;
    if (a5->z > 1.0f) {
        a5->x = v8;
        a5->z = 1.0f;
    }

    float_t v11 = v8 - v7;
    float_t v12 = a5->x - v7;
    if (v11 < 0.0f)
        v11 = 0.0f;
    if (v12 < 0.0f)
        v12 = 0.0f;

    a2->ik_target.x = v18.x;
    a2->ik_target.y = v18.y + lerp(v12, v11, a5->z);
    a2->ik_target.z = v18.z;
}

static void sub_1404065B0(vec3* a1, std::vector<bone_data>* a2, mat4* a3, float_t a4,
    bone_database_skeleton_type skeleton_type, const motion_bone_index* a6, const motion_bone_index* a7) {
    bone_database* bone_data = &rctx_ptr->data->data_ft.bone_data;
    const char* name = bone_database_skeleton_type_to_string(skeleton_type);
    float_t* base_heel_height = 0;
    float_t* heel_height = 0;
    if (!bone_data->get_skeleton_heel_height(name, &heel_height))
        return;

    sub_140406920(a1, &a2->data()[a6[0]], &a2->data()[MOTION_BONE_KL_TOE_L_WJ], *heel_height, &a1[0], a4);
    sub_140406920(a1, &a2->data()[a7[0]], &a2->data()[MOTION_BONE_KL_TOE_R_WJ], *heel_height, &a1[1], a4);

    ::bone_data* v7 = a2->data();
    ::bone_data* v17 = &v7[a6[0]];
    v17->rot_mat[1] = mat4_identity;
    v17->rot_mat[2] = mat4_identity;
    bone_data_mult_ik(v17, 1);

    ::bone_data* v20 = &v7[a7[0]];
    v20->rot_mat[1] = mat4_identity;
    v20->rot_mat[2] = mat4_identity;
    bone_data_mult_ik(v20, 1);

    motion_bone_index v21 = a6[1];

    a6++;
    while (*a6 != MOTION_BONE_NONE)
        bone_data_mult_1(&v7[*a6++], a3, v7, true);

    a7++;
    while (*a7 != MOTION_BONE_NONE)
        bone_data_mult_1(&v7[*a7++], a3, v7, true);
}

static void sub_140418810(rob_chara_bone_data* rob_bone_data, const motion_bone_index* a6, const motion_bone_index* a7) {
    motion_blend_mot* v3 = rob_bone_data->motion_loaded.front();
    mat4 m = v3->field_4F8.mat;
    sub_1404065B0(rob_bone_data->field_76C, &v3->bone_data.bones, &m,
        v3->mot_play_data.frame_data.step, rob_bone_data->skeleton_type, a6, a7);
}

static void sub_14053B260(rob_chara* rob_chr) {
    if (rob_chr->data.field_1588.field_0.field_20.field_C & 0x10000000)
        sub_140418810(rob_chr->bone_data, dword_140A2DDB0, dword_140A2DD90);
}

static void rob_chara_bone_data_left_hand_scale(rob_chara_bone_data* rob_bone_data, float_t scale) {
    mat4* kl_te_wj_mat = rob_chara_bone_data_get_mats_mat(rob_bone_data, ROB_BONE_KL_TE_L_WJ);
    if (!kl_te_wj_mat)
        return;

    vec3 trans;
    mat4_get_translation(kl_te_wj_mat, &trans);

    mat4 mat;
    mat4_scale(scale, scale, scale, &mat);

    vec3 temp;
    vec3_mult_scalar(trans, scale, temp);
    vec3_sub(trans, temp, trans);

    mat4_set_translation(&mat, &trans);

    for (int32_t i = ROB_BONE_KL_TE_L_WJ; i <= ROB_BONE_NL_OYA_C_L_WJ; i++) {
        mat4* m = rob_chara_bone_data_get_mats_mat(rob_bone_data, i);
        if (m)
            mat4_mult(m, &mat, m);
    }
    
    for (int32_t i = MOT_BONE_KL_TE_L_WJ; i <= MOT_BONE_NL_OYA_C_L_WJ; i++) {
        bone_node* node = rob_chara_bone_data_get_node(rob_bone_data, i);
        if (node)
            node->exp_data.scale = { scale, scale, scale };
    }
}

static void rob_chara_bone_data_right_hand_scale(rob_chara_bone_data* rob_bone_data, float_t scale) {
    mat4* kl_te_wj_mat = rob_chara_bone_data_get_mats_mat(rob_bone_data, ROB_BONE_KL_TE_R_WJ);
    if (!kl_te_wj_mat)
        return;

    vec3 trans;
    mat4_get_translation(kl_te_wj_mat, &trans);

    mat4 mat;
    mat4_scale(scale, scale, scale, &mat);

    vec3 temp;
    vec3_mult_scalar(trans, scale, temp);
    vec3_sub(trans, temp, trans);

    mat4_set_translation(&mat, &trans);

    for (int32_t i = ROB_BONE_KL_TE_R_WJ; i <= ROB_BONE_NL_OYA_C_R_WJ; i++) {
        mat4* m = rob_chara_bone_data_get_mats_mat(rob_bone_data, i);
        if (m)
            mat4_mult(m, &mat, m);
    }
    
    for (int32_t i = MOT_BONE_KL_TE_R_WJ; i <= MOT_BONE_NL_OYA_C_R_WJ; i++) {
        bone_node* node = rob_chara_bone_data_get_node(rob_bone_data, i);
        if (node)
            node->exp_data.scale = { scale, scale, scale };
    }
}

static void rob_motion_modifier_rob_chara_ctrl(rob_chara* rob_chr) {
    sub_14053BEE0(rob_chr);
    rob_chara_hands_adjust(rob_chr);
    if (!sub_14053B530(rob_chr))
        sub_14053B260(rob_chr);

    float_t left_hand_scale_default = rob_chr->data.adjust_data.left_hand_scale_default;
    float_t right_hand_scale_default = rob_chr->data.adjust_data.right_hand_scale_default;
    float_t left_hand_scale = rob_chr->data.adjust_data.left_hand_scale;
    float_t right_hand_scale = rob_chr->data.adjust_data.right_hand_scale;

    if (left_hand_scale_default > 0.0f) {
        if (left_hand_scale <= 0.0f)
            left_hand_scale = rob_chr->data.adjust_data.left_hand_scale_default;
        else
            left_hand_scale *= left_hand_scale_default;
    }

    if (right_hand_scale_default > 0.0f) {
        if (right_hand_scale <= 0.0f)
            right_hand_scale = rob_chr->data.adjust_data.right_hand_scale_default;
        else
            right_hand_scale *= right_hand_scale_default;
    }

    if (left_hand_scale > 0.0f)
        rob_chara_bone_data_left_hand_scale(rob_chr->bone_data, left_hand_scale);

    if (right_hand_scale > 0.0f)
        rob_chara_bone_data_right_hand_scale(rob_chr->bone_data, right_hand_scale);
}

static float_t rob_chara_array_get_data_adjust_scale(int32_t chara_id) {
    if (rob_chara_pv_data_array_check_chara_id(chara_id))
        return rob_chara_array[chara_id].data.adjust_data.scale;
    return 1.0f;
}

static rob_chara_item_equip* rob_chara_array_get_item_equip(int32_t chara_id) {
    return rob_chara_array[chara_id].item_equip;
}

static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    std::vector<bone_database_bone>* bones) {
    bone_database_bones_calculate_count(bones, &rob_bone_data->object_bone_count,
        &rob_bone_data->motion_bone_count, &rob_bone_data->total_bone_count,
        &rob_bone_data->ik_bone_count, &rob_bone_data->chain_pos);
}

static void rob_chara_bone_data_eyes_xrot_adjust(rob_chara_bone_data* rob_bone_data,
    const struc_241* a2, eyes_adjust* a3) {
    float_t v1_neg;
    float_t v1_pos;
    switch (a3->base_adjust) {
    case EYES_BASE_ADJUST_DIRECTION:
    default:
        v1_neg = a2->field_34 * -(float_t)(1.0 / 3.8);
        v1_pos = a2->field_38 * (float_t)(1.0 / 6.0);
        break;
    case EYES_BASE_ADJUST_CLEARANCE:
        v1_neg = a2->field_3C * -(float_t)(1.0 / 3.8);
        v1_pos = a2->field_40 * (float_t)(1.0 / 6.0);
        break;
    case EYES_BASE_ADJUST_OFF:
        v1_neg = 1.0f;
        v1_pos = 1.0f;
        break;
    }

    float_t v2_pos = 1.0f;
    float_t v2_neg = 1.0f;
    if (a3->xrot_adjust) {
        v2_neg = a2->field_2C;
        v2_pos = a2->field_30;
    }

    if (a3->neg >= 0.0f && a3->pos >= 0.0f) {
        v2_neg = a3->neg;
        v2_pos = a3->pos;
    }

    float_t eyes_xrot_adjust_neg = v1_neg * v2_neg;
    float_t eyes_xrot_adjust_pos = v1_pos * v2_pos;
    for (motion_blend_mot*& i : rob_bone_data->motions) {
        bone_data* data = i->bone_data.bones.data();
        data[MOTION_BONE_KL_EYE_L].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        data[MOTION_BONE_KL_EYE_L].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
        data[MOTION_BONE_KL_EYE_R].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        data[MOTION_BONE_KL_EYE_R].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
    }
}

static float_t rob_chara_bone_data_get_frame(rob_chara_bone_data* rob_bone_data) {
    return rob_bone_data->motion_loaded.front()->mot_play_data.frame_data.frame;
}

static float_t rob_chara_bone_data_get_frame_count(rob_chara_bone_data* rob_bone_data) {
    return (float_t)rob_bone_data->motion_loaded.front()->mot_key_data.mot.frame_count;
}

static void rob_chara_bone_data_get_ik_scale(
    rob_chara_bone_data* rob_bone_data, bone_database* bone_data) {
    if (rob_bone_data->motion_loaded.size() < 0)
        return;

    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    rob_chara_bone_data_ik_scale_calculate(&rob_bone_data->ik_scale, &v2->bone_data.bones,
        rob_bone_data->base_skeleton_type, rob_bone_data->skeleton_type, bone_data);
    float_t ratio0 = rob_bone_data->ik_scale.ratio0;
    v2->field_4F8.field_C0 = ratio0;
    v2->field_4F8.field_C4 = ratio0;
    v2->field_4F8.field_C8 = vec3_null;
}

static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index) {
    bone_node* node = rob_chara_bone_data_get_node(rob_bone_data, index);
    if (node)
        return node->mat;
    return 0;
}

static bone_node* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index) {
    if (index < rob_bone_data->nodes.size())
        return &rob_bone_data->nodes[index];
    return 0;
}

static void rob_chara_bone_data_ik_scale_calculate(
    rob_chara_bone_data_ik_scale* ik_scale, std::vector<bone_data>* bones,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data) {
    const char* base_name = bone_database_skeleton_type_to_string(base_skeleton_type);
    const char* name = bone_database_skeleton_type_to_string(skeleton_type);
    float_t* base_heel_height = 0;
    float_t* heel_height = 0;
    if (!bone_data->get_skeleton_heel_height(base_name, &base_heel_height)
        || !bone_data->get_skeleton_heel_height(name, &heel_height))
        return;

    ::bone_data* b_cl_mune = &bones->data()[MOTION_BONE_CL_MUNE];
    ::bone_data* b_kl_kubi = &bones->data()[MOTION_BONE_KL_KUBI];
    ::bone_data* b_c_kata_l = &bones->data()[MOTION_BONE_C_KATA_L];
    ::bone_data* b_cl_momo_l = &bones->data()[MOTION_BONE_CL_MOMO_L];

    float_t base_height = fabsf(b_cl_momo_l->base_translation[0].y) + b_cl_momo_l->ik_segment_length[0]
        + b_cl_momo_l->ik_2nd_segment_length[0] + *base_heel_height;
    float_t height = fabsf(b_cl_momo_l->base_translation[1].y) + b_cl_momo_l->ik_segment_length[1]
        + b_cl_momo_l->ik_2nd_segment_length[1] + *heel_height;
    ik_scale->ratio0 = height / base_height;
    ik_scale->ratio1 = (fabsf(b_kl_kubi->base_translation[1].y) + b_cl_mune->ik_segment_length[1])
        / (fabsf(b_kl_kubi->base_translation[0].y) + b_cl_mune->ik_segment_length[0]);
    ik_scale->ratio2 = (b_c_kata_l->ik_segment_length[1] + b_c_kata_l->ik_2nd_segment_length[1])
        / (b_c_kata_l->ik_segment_length[0] + b_c_kata_l->ik_2nd_segment_length[0]);
    ik_scale->ratio3 = (fabsf(b_kl_kubi->base_translation[1].y) + b_cl_mune->ik_segment_length[1] + height)
        / (fabsf(b_kl_kubi->base_translation[0].y) + b_cl_mune->ik_segment_length[0] + base_height);
}

static void mot_key_data_reserve_key_sets_by_skeleton_type(mot_key_data* a1,
    bone_database_skeleton_type type, bone_database* bone_data) {
    const char* name = bone_database_skeleton_type_to_string(type);
    std::vector<bone_database_bone>* bones = 0;
    if (!bone_data->get_skeleton_bones(name, &bones))
        return;

    mot_key_data_get_key_set_count_by_bone_database_bones(a1, bones);
    mot_key_data_reserve_key_sets(a1);
    a1->skeleton_type = type;
}

static void mot_blend_reserve_key_sets(mot_blend* blend, bone_database_skeleton_type type,
    bool(*bone_check_func)(motion_bone_index), size_t motion_bone_count, bone_database* bone_data) {
    mot_key_data_reserve_key_sets_by_skeleton_type(&blend->mot_key_data, type, bone_data);
    sub_140413350(&blend->field_0, bone_check_func, motion_bone_count);
}

static bool motion_blend_mot_check(motion_bone_index bone_index) {
    return 1;
}

static bool mot_blend_face_check(motion_bone_index bone_index) {
    return bone_index >= MOTION_BONE_FACE_ROOT && bone_index <= MOTION_BONE_TL_TOOTH_UPPER_WJ;
}

static bool mot_blend_hand_l_check(motion_bone_index bone_index) {
    return bone_index >= MOTION_BONE_N_HITO_L_EX && bone_index <= MOTION_BONE_NL_OYA_C_L_WJ;
}

static bool mot_blend_hand_r_check(motion_bone_index bone_index) {
    return bone_index >= MOTION_BONE_N_HITO_R_EX && bone_index <= MOTION_BONE_NL_OYA_C_R_WJ;
}

static bool mot_blend_mouth_check(motion_bone_index bone_index) {
    return bone_index >= MOTION_BONE_N_AGO && bone_index <= MOTION_BONE_TL_TOOTH_UNDER_WJ
        || bone_index >= MOTION_BONE_N_KUTI_D && bone_index <= MOTION_BONE_TL_KUTI_U_R_WJ
        || bone_index >= MOTION_BONE_N_TOOTH_UPPER && bone_index <= MOTION_BONE_TL_TOOTH_UPPER_WJ;
}

static bool mot_blend_eyes_check(motion_bone_index bone_index) {
    return bone_index >= MOTION_BONE_N_EYE_L && bone_index <= MOTION_BONE_KL_HIGHLIGHT_R_WJ;
}

static bool mot_blend_eyelid_check(motion_bone_index bone_index) {
    if (bone_index >= MOTION_BONE_N_MABU_L_D_A && bone_index <= MOTION_BONE_TL_MABU_R_U_C_WJ
        || bone_index >= MOTION_BONE_N_EYELID_L_A && bone_index <= MOTION_BONE_TL_EYELID_R_B_WJ)
        return true;
    return mot_blend_eyes_check(bone_index);
}

static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    bone_database_skeleton_type base_type,
    bone_database_skeleton_type type, bone_database* bone_data) {
    rob_chara_bone_data_init_skeleton(rob_bone_data, base_type, type, bone_data);
    for (motion_blend_mot*& i : rob_bone_data->motions)
        motion_blend_mot_load_bone_data(i, rob_bone_data,
            motion_blend_mot_check, bone_data);

    size_t motion_bone_count = rob_bone_data->motion_bone_count;
    mot_blend_reserve_key_sets(&rob_bone_data->face, base_type,
        mot_blend_face_check, motion_bone_count, bone_data);
    mot_blend_reserve_key_sets(&rob_bone_data->hand_l, base_type,
        mot_blend_hand_l_check, motion_bone_count, bone_data);
    mot_blend_reserve_key_sets(&rob_bone_data->hand_r, base_type,
        mot_blend_hand_r_check, motion_bone_count, bone_data);
    mot_blend_reserve_key_sets(&rob_bone_data->mouth, base_type,
        mot_blend_mouth_check, motion_bone_count, bone_data);
    mot_blend_reserve_key_sets(&rob_bone_data->eyes, base_type,
        mot_blend_eyes_check, motion_bone_count, bone_data);
    mot_blend_reserve_key_sets(&rob_bone_data->eyelid, base_type,
        mot_blend_eyelid_check, motion_bone_count, bone_data);
}

static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, bone_database* bone_data) {
    if (rob_bone_data->base_skeleton_type == base_skeleton_type
        && rob_bone_data->skeleton_type == skeleton_type)
        return;

    const char* name = bone_database_skeleton_type_to_string(base_skeleton_type);
    std::vector<bone_database_bone>* bones = 0;
    std::vector<uint16_t>* parent_indices = 0;
    std::vector<std::string>* motion_bones = 0;
    if (!bone_data->get_skeleton_bones(name, &bones)
        || !bone_data->get_skeleton_parent_indices(name, &parent_indices)
        || !bone_data->get_skeleton_motion_bones(name, &motion_bones))
        return;

    rob_chara_bone_data_calculate_bones(rob_bone_data, bones);
    rob_chara_bone_data_reserve(rob_bone_data);
    rob_chara_bone_data_set_mats(rob_bone_data, bones, motion_bones->data());
    rob_chara_bone_data_set_parent_mats(rob_bone_data, parent_indices->data());
    rob_bone_data->base_skeleton_type = base_skeleton_type;
    rob_bone_data->skeleton_type = skeleton_type;
}

static void sub_14041B4E0(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.data();
    for (uint32_t i = MOTION_BONE_FACE_ROOT; i <= MOTION_BONE_TL_TOOTH_UPPER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B580(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.data();
    for (uint32_t i = MOTION_BONE_FACE_ROOT; i <= MOTION_BONE_TL_TOOTH_UPPER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_EYE_L; i <= MOTION_BONE_KL_HIGHLIGHT_R_WJ; i++)
        bitfield[i >> 5] |= 1 << (i & 0x1F);
}

static void sub_14041B800(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.data();
    for (uint32_t i = MOTION_BONE_N_KUTI_D; i <= MOTION_BONE_TL_KUTI_U_R_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_AGO; i <= MOTION_BONE_TL_TOOTH_UNDER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_TOOTH_UPPER; i <= MOTION_BONE_TL_TOOTH_UPPER_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B440(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.data();
    for (uint32_t i = MOTION_BONE_N_EYE_L; i <= MOTION_BONE_KL_HIGHLIGHT_R_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B1C0(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.data();
    for (uint32_t i = MOTION_BONE_N_MABU_L_D_A; i <= MOTION_BONE_TL_MABU_R_U_C_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_EYELID_L_A; i <= MOTION_BONE_TL_EYELID_R_B_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B6B0(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.data();
    for (uint32_t i = MOTION_BONE_N_HITO_L_EX; i <= MOTION_BONE_NL_OYA_C_L_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B750(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.data();
    for (uint32_t i = MOTION_BONE_N_HITO_R_EX; i <= MOTION_BONE_NL_OYA_C_R_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_14041B300(struc_313* a1) {
    uint32_t* bitfield = a1->bitfield.data();
    for (uint32_t i = MOTION_BONE_N_MABU_L_D_A; i <= MOTION_BONE_TL_MABU_R_U_C_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));

    for (uint32_t i = MOTION_BONE_N_EYELID_L_A; i <= MOTION_BONE_TL_EYELID_R_B_WJ; i++)
        bitfield[i >> 5] &= ~(1 << (i & 0x1F));
}

static void sub_140413EA0(struc_240* a1, void(*a2)(struc_313*)) {
    a2(&a1->field_8);
}

static void sub_14041AD90(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->mouth.mot_key_data.key_sets_ready
        && rob_bone_data->mouth.mot_key_data.mot_data && !rob_bone_data->mouth.field_30)
        sub_140413EA0(&rob_bone_data->face.field_0, sub_14041B800);
    if (rob_bone_data->eyelid.mot_key_data.key_sets_ready
        && rob_bone_data->eyelid.mot_key_data.mot_data && !rob_bone_data->eyelid.field_30)
        sub_140413EA0(&rob_bone_data->face.field_0, sub_14041B1C0);
    else if (rob_bone_data->disable_eye_motion || rob_bone_data->eyes.mot_key_data.key_sets_ready
        && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.field_30)
        sub_140413EA0(&rob_bone_data->face.field_0, sub_14041B440);
}

static void sub_14041AD50(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->disable_eye_motion || rob_bone_data->eyes.mot_key_data.key_sets_ready
        && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.field_30)
        sub_140413EA0(&rob_bone_data->eyelid.field_0, sub_14041B440);
}

static void sub_14041ABA0(rob_chara_bone_data* a1) {
    for (motion_blend_mot*& i : a1->motion_loaded) {
        if (a1->face.mot_key_data.key_sets_ready
            && a1->face.mot_key_data.mot_data && !a1->face.field_30) {
            if (!a1->disable_eye_motion || a1->eyes.mot_key_data.key_sets_ready
                && a1->eyes.mot_key_data.mot_data && !a1->eyes.field_30)
                sub_140413EA0(&i->field_0, sub_14041B4E0);
            else
                sub_140413EA0(&i->field_0, sub_14041B580);
        }
        else {
            if (a1->mouth.mot_key_data.key_sets_ready
                && a1->mouth.mot_key_data.mot_data && !a1->mouth.field_30)
                sub_140413EA0(&i->field_0, sub_14041B800);
            if (a1->eyelid.mot_key_data.key_sets_ready
                && a1->eyelid.mot_key_data.mot_data && !a1->eyelid.field_30) {
                if (!a1->disable_eye_motion
                    || a1->eyes.mot_key_data.key_sets_ready
                    && a1->eyes.mot_key_data.mot_data && !a1->eyes.field_30)
                    sub_140413EA0(&i->field_0, sub_14041B1C0);
                else
                    sub_140413EA0(&i->field_0, sub_14041B300);
            }
            else if (a1->eyes.mot_key_data.key_sets_ready
                && a1->eyes.mot_key_data.mot_data && !a1->eyes.field_30)
                sub_140413EA0(&i->field_0, sub_14041B440);
        }

        if (a1->hand_l.mot_key_data.key_sets_ready
            && a1->hand_l.mot_key_data.mot_data && !a1->hand_l.field_30)
            sub_140413EA0(&i->field_0, sub_14041B6B0);
        if (a1->hand_r.mot_key_data.key_sets_ready
            && a1->hand_r.mot_key_data.mot_data && !a1->hand_r.field_30)
            sub_140413EA0(&i->field_0, sub_14041B750);
    }
    sub_14041AD90(a1);
    sub_14041AD50(a1);
}

static bool sub_1404136B0(motion_blend_mot* a1) {
    if (a1->blend)
        return a1->blend->Field_30();
    return false;
}

static bool sub_140413630(struc_308* a1) {
    return (a1->field_4 & 2) != 0;
}

static bool sub_140413790(struc_308* a1) {
    return (a1->field_0 & 2) != 0;
}

static bool sub_1404137A0(struc_308* a1) {
    return (a1->field_0 & 0x10) != 0;
}

static void sub_140415A10(motion_blend_mot* a1) {
    if (!a1->blend)
        return;

    struc_400 v7;
    v7.field_0 = false;
    v7.field_2 = false;
    v7.field_3 = false;
    v7.field_4 = false;
    if (!a1->field_4F8.field_BD)
        v7.field_0 = !sub_140413630(&a1->field_4F8);
    if (!a1->field_4F8.field_BC)
        v7.field_2 = !sub_140413790(&a1->field_4F8);
    v7.field_1 = sub_1404137A0(&a1->field_4F8);
    v7.frame = a1->mot_key_data.frame;
    v7.eyes_adjust = a1->field_4F8.eyes_adjust;
    v7.prev_eyes_adjust = a1->field_4F8.prev_eyes_adjust;
    a1->blend->Field_18(&v7);
}

static bool sub_140410A20(motion_blend_mot* a1) {
    return !a1->blend || !a1->blend->field_9;
}

static void sub_140415B30(mot_blend* a1) {
    struc_400 v3;
    v3.field_0 = false;
    v3.field_1 = false;
    v3.field_2 = false;
    v3.field_3 = false;
    v3.field_4 = false;
    v3.frame = 0.0f;
    v3.eyes_adjust = 0.0f;
    v3.prev_eyes_adjust = 0.0f;
    a1->blend.Field_18(&v3);
}

static void sub_14041DAC0(rob_chara_bone_data* a1) {
    size_t v2 = 0;
    bool v3 = 0;
    for (motion_blend_mot*& i : a1->motion_loaded) {
        if (!sub_1404136B0(i)) {
            if (v3) {
                rob_chara_bone_data_motion_blend_mot_list_free(a1, v2);
                break;
            }
            sub_140415A10(i);
            v3 = sub_140410A20(i);
        }
        else
            sub_140415A10(i);
        v2++;
    }

    sub_140415B30(&a1->face);
    sub_140415B30(&a1->hand_l);
    sub_140415B30(&a1->hand_r);
    sub_140415B30(&a1->mouth);
    sub_140415B30(&a1->eyes);
    sub_140415B30(&a1->eyelid);
}

static void rob_chara_bone_data_interpolate(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->motion_loaded.size() < 0)
        return;

    sub_14041ABA0(rob_bone_data);
    sub_14041DAC0(rob_bone_data);

    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        motion_blend_mot_interpolate(i);

    bone_database_skeleton_type skeleton_type = rob_bone_data->base_skeleton_type;
    motion_blend_mot* v5 = rob_bone_data->motion_loaded.front();
    std::vector<bone_data>* bones = &v5->bone_data.bones;
    std::vector<uint16_t>* bone_indices = &v5->bone_data.bone_indices;
    mot_blend_interpolate(&rob_bone_data->face, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->hand_l, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->hand_r, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->mouth, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->eyes, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&rob_bone_data->eyelid, bones, bone_indices, skeleton_type);
}

static void rob_chara_bone_data_motion_blend_mot_free(rob_chara_bone_data* rob_bone_data) {
    rob_bone_data->motion_indices.clear();
    for (motion_blend_mot*& i : rob_bone_data->motions)
        delete i;
    rob_bone_data->motions.clear();
}

static void rob_chara_bone_data_motion_blend_mot_init(rob_chara_bone_data* rob_bone_data) {
    for (int32_t i = 0; i < 3; i++) {
        rob_bone_data->motions.push_back(new motion_blend_mot());
        rob_bone_data->motion_indices.push_back(i);
    }
    rob_chara_bone_data_motion_blend_mot_list_init(rob_bone_data);
}

static void rob_chara_bone_data_motion_blend_mot_list_free(rob_chara_bone_data* rob_bone_data,
    size_t last_index) {
    for (size_t i = rob_bone_data->motion_loaded.size(); i > last_index; i--) {
        motion_blend_mot* v5 = rob_bone_data->motion_loaded.back();
        size_t v6 = rob_bone_data->motions.size();
        size_t v14 = rob_bone_data->motion_loaded_indices.back();
        if (v14 >= v6)
            delete v5;
        else {
            rob_bone_data->motion_indices.push_back(v14);
            sub_1404146F0(&v5->field_0);
        }
        rob_bone_data->motion_loaded.pop_back();
        rob_bone_data->motion_loaded_indices.pop_back();
    }
}

static void rob_chara_bone_data_motion_blend_mot_list_init(rob_chara_bone_data* rob_bone_data) {
    size_t free_index = rob_bone_data->motion_indices.front();
    rob_bone_data->motion_indices.pop_front();
    rob_bone_data->motion_loaded.push_front(rob_bone_data->motions[free_index]);
    rob_bone_data->motion_loaded_indices.push_front(free_index);
}

static void rob_chara_bone_data_reserve(rob_chara_bone_data* rob_bone_data) {
    rob_bone_data->mats.resize(rob_bone_data->object_bone_count);
    rob_bone_data->mats2.resize(rob_bone_data->total_bone_count - rob_bone_data->object_bone_count);
    rob_bone_data->nodes.resize(rob_bone_data->total_bone_count);
}

static void sub_1401EAD00(bone_data* a1, bone_data* a2) {
    if (a1->flags)
        return;

    switch (a1->type) {
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        a1->trans = a2->trans;
        a1->trans_prev[0] = a2->trans_prev[0];
        break;
    }

    switch (a1->type) {
    case BONE_DATABASE_BONE_ROTATION:
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        a1->rot_mat[0] = a2->rot_mat[0];
        a1->rot_mat_prev[0][0] = a2->rot_mat_prev[0][0];
        break;
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        a1->rot_mat[0] = a2->rot_mat[0];
        a1->rot_mat[1] = a2->rot_mat[1];
        a1->rot_mat_prev[0][0] = a2->rot_mat_prev[0][0];
        a1->rot_mat_prev[1][0] = a2->rot_mat_prev[1][0];
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        a1->rot_mat[0] = a2->rot_mat[0];
        a1->rot_mat[1] = a2->rot_mat[1];
        a1->rot_mat[2] = a2->rot_mat[2];
        a1->rot_mat_prev[0][0] = a2->rot_mat_prev[0][0];
        a1->rot_mat_prev[1][0] = a2->rot_mat_prev[1][0];
        a1->rot_mat_prev[2][0] = a2->rot_mat_prev[2][0];
        break;
    }
}

static void sub_140412BB0(motion_blend_mot* a1, std::vector<bone_data>* bones) {
    bone_data* bone = bones->data();
    for (bone_data& i : a1->bone_data.bones)
        sub_1401EAD00(&i, &bone[i.motion_bone_index]);
}

static void sub_14041AE40(rob_chara_bone_data* rob_bone_data) {
    struc_240* v3 = &rob_bone_data->motion_loaded.front()->field_0;
    if (!v3)
        return;

    if (rob_bone_data->face.mot_key_data.key_sets_ready
        && rob_bone_data->face.mot_key_data.mot_data && !rob_bone_data->face.field_30) {
        if (!rob_bone_data->disable_eye_motion
            || rob_bone_data->eyes.mot_key_data.key_sets_ready
            && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.field_30)
            sub_140413EA0(v3, sub_14041B4E0);
        else
            sub_140413EA0(v3, sub_14041B580);
    }
    else {
        if (rob_bone_data->mouth.mot_key_data.key_sets_ready
            && rob_bone_data->mouth.mot_key_data.mot_data && !rob_bone_data->mouth.field_30)
            sub_140413EA0(v3, sub_14041B800);
        if (rob_bone_data->eyelid.mot_key_data.key_sets_ready
            && rob_bone_data->eyelid.mot_key_data.mot_data && !rob_bone_data->eyelid.field_30) {
            if (!rob_bone_data->disable_eye_motion
                || rob_bone_data->eyes.mot_key_data.key_sets_ready
                && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.field_30)
                sub_140413EA0(v3, sub_14041B1C0);
            else
                sub_140413EA0(v3, sub_14041B300);
        }
        else
            if (rob_bone_data->eyes.mot_key_data.key_sets_ready
                && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.field_30)
                sub_140413EA0(v3, sub_14041B440);
    }

    if (rob_bone_data->hand_l.mot_key_data.key_sets_ready
        && rob_bone_data->hand_l.mot_key_data.mot_data && !rob_bone_data->hand_l.field_30)
        sub_140413EA0(v3, sub_14041B6B0);
    if (rob_bone_data->hand_r.mot_key_data.key_sets_ready
        && rob_bone_data->hand_r.mot_key_data.mot_data && !rob_bone_data->hand_r.field_30)
        sub_140413EA0(v3, sub_14041B750);
}

static void sub_14041BA60(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->motion_loaded.front())
        sub_1404146F0(&rob_bone_data->motion_loaded.front()->field_0);
}

static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data, int32_t motion_id,
    MotionBlendType blend_type, bone_database* bone_data, motion_database* mot_db) {
    if (!rob_bone_data->motion_loaded.size())
        return;

    if (blend_type == MOTION_BLEND_FREEZE) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.front(),
            motion_id, MOTION_BLEND_FREEZE, 1.0f, bone_data, mot_db);
        sub_14041BA60(rob_bone_data);
        return;
    }

    if (blend_type != MOTION_BLEND_CROSS) {
        if (blend_type == MOTION_BLEND_COMBINE)
            blend_type = MOTION_BLEND;
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.front(),
            motion_id, blend_type, 1.0f, bone_data, mot_db);
        return;
    }
    rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 2);

    if (rob_bone_data->motion_indices.size()) {
        rob_chara_bone_data_motion_blend_mot_list_init(rob_bone_data);
        sub_140412BB0(rob_bone_data->motion_loaded.front(),
            &rob_bone_data->motion_loaded.front()->bone_data.bones);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.front(),
            motion_id, MOTION_BLEND_CROSS, 1.0f, bone_data, mot_db);
        return;
    }

    motion_blend_mot* v16 = new motion_blend_mot();
    motion_blend_mot_load_bone_data(v16, rob_bone_data, v16->field_0.bone_check_func, bone_data);
    if (!v16) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        motion_blend_mot_load_file(rob_bone_data->motion_loaded.front(),
            motion_id, MOTION_BLEND_FREEZE, 1.0f, bone_data, mot_db);
        sub_14041BA60(rob_bone_data);
        return;
    }

    rob_bone_data->motion_loaded.push_back(v16);
    rob_bone_data->motion_loaded_indices.push_back(rob_bone_data->motions.size());
    sub_140412BB0(rob_bone_data->motion_loaded.front(),
        &rob_bone_data->motion_loaded.front()->bone_data.bones);
    motion_blend_mot_load_file(rob_bone_data->motion_loaded.front(),
        motion_id, MOTION_BLEND_CROSS, 1.0f, bone_data, mot_db);
}

static void sub_1404198D0(struc_258* a1) {
    a1->field_0 = 0;
    a1->field_8 = mat4u_identity;
    a1->field_48.field_0 = 0.0f;
    a1->field_48.field_4 = 0.0f;
    a1->field_48.field_8 = 0.0f;
    a1->field_48.field_C = 0.0f;
    a1->field_48.field_10 = 0.0f;
    a1->field_48.field_14 = 0.0f;
    a1->field_48.field_18 = 0.0f;
    a1->field_48.field_1C = 0.0f;
    a1->field_48.field_20 = 0.0f;
    a1->field_48.field_24 = 0.0f;
    a1->field_48.field_28 = 0.0f;
    a1->field_48.field_2C = 1.0f;
    a1->field_48.field_30 = 1.0f;
    a1->field_48.field_34 = -3.8f;
    a1->field_48.field_38 = 6.0f;
    a1->field_48.field_3C = -3.8f;
    a1->field_48.field_40 = 6.0f;
    a1->field_8C = false;
    a1->field_8D = false;
    a1->field_8E = false;
    a1->field_8F = false;
    a1->field_90 = false;
    a1->field_91 = false;
    a1->field_94 = 0.0f;
    a1->field_98 = 1.0f;
    a1->field_9C = 1.0f;
    a1->field_A0 = 0.0f;
    a1->field_A4 = 0.0f;
    a1->field_A8 = 0.0f;
    a1->field_AC = 0.0f;
    a1->field_B0 = 0.0f;
    a1->field_B4 = vec3_null;
    a1->field_C0 = vec3_null;
    a1->field_CC = mat4u_identity;
    a1->field_10C = mat4u_identity;
    a1->field_14C = 0;
    a1->field_150 = 0.0f;
    a1->field_154 = 0.0f;
    a1->field_158 = 0.0f;
    a1->field_15C.field_0 = 0.0f;
    a1->field_15C.field_4 = 0.0f;
    a1->field_15C.field_8 = 0.0f;
    a1->field_15C.field_C = 0.0f;
    a1->field_15C.field_10 = 0;
    a1->field_15C.field_14 = 0;
    a1->field_15C.field_18 = 0;
    a1->field_15C.field_1C = 0;
    a1->field_15C.field_20 = 0.0f;
    a1->field_15C.field_24 = 0.0f;
    a1->field_184 = 0.0f;
    a1->field_188 = 0.0f;
    a1->field_18C = 0.0f;
    a1->field_190 = false;
    a1->field_191 = false;
    a1->field_192 = false;
    a1->field_193 = false;
    a1->field_194 = false;
    a1->field_195 = false;
    a1->field_198 = 0.0f;
    a1->field_19C = 0.0f;
    a1->field_1A0 = 0.0f;
    a1->field_1A4 = 0;
    a1->field_1A8 = 0;
    a1->field_1AC = 0.0f;
    a1->field_1B0 = 0.0f;
    a1->field_1B4 = 0;
    a1->field_1B8 = 0;
    a1->field_1BC = vec2_identity;
    a1->field_1C4 = 0;
    a1->field_1C8 = 0;
}

static void sub_1403FAEF0(struc_312* a1) {
    a1->field_0.field_0 = 0.0f;
    a1->field_0.field_4 = 0.0f;
    a1->field_0.field_8 = 0.0f;
    a1->field_0.field_C = 0.0f;
    a1->field_0.field_10 = 0.0f;
    a1->field_0.field_14 = 0.0f;
    a1->field_0.field_18 = 0.0f;
    a1->field_0.field_1C = vec3_null;
    a1->field_0.field_28 = 0.0f;
    a1->field_2C.field_0 = 0.0f;
    a1->field_2C.field_4 = 0.0f;
    a1->field_2C.field_8 = 0.0f;
    a1->field_2C.field_C = 0.0f;
    a1->field_2C.field_10 = 0.0f;
    a1->field_2C.field_14 = 0.0f;
    a1->field_2C.field_18 = 0.0f;
    a1->field_2C.field_1C = vec3_null;
    a1->field_2C.field_28 = 0.0f;
    a1->field_58 = false;
    a1->field_59 = false;
    a1->field_8C = 1.0f;
    a1->bones = 0;
    a1->step = 1.0f;
    a1->field_5C = vec3_null;
    a1->field_68 = vec3_null;
    a1->field_74 = vec3_null;
    a1->field_80 = vec3_null;
}

static void rob_chara_bone_data_set_eyelid_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_duration(&rob_bone_data->eyelid, duration, step, offset);
}

static void rob_chara_bone_data_set_eyelid_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->eyelid, frame);
}

static void rob_chara_bone_data_set_eyelid_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->eyelid.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_eyes_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_duration(&rob_bone_data->eyes, duration, step, offset);
}

static void rob_chara_bone_data_set_eyes_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->eyes, frame);
}

static void rob_chara_bone_data_set_eyes_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->eyes.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_face_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_duration(&rob_bone_data->face, duration, step, offset);
}

static void rob_chara_bone_data_set_face_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->face, frame);
}

static void rob_chara_bone_data_set_face_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->face.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_play_frame_data_set_frame(&rob_bone_data->motion_loaded.front()->mot_play_data.frame_data, frame);
}

static void rob_chara_bone_data_set_hand_l_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_duration(&rob_bone_data->hand_l, duration, step, offset);
}

static void rob_chara_bone_data_set_hand_l_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->hand_l, frame);
}

static void rob_chara_bone_data_set_hand_l_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->hand_l.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_hand_r_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_duration(&rob_bone_data->hand_r, duration, step, offset);
}

static void rob_chara_bone_data_set_hand_r_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->hand_r, frame);
}

static void rob_chara_bone_data_set_hand_r_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->hand_r.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
    std::vector<bone_database_bone>* bones, std::string* motion_bones) {
    size_t chain_pos = 0;
    size_t ik_bone_count = 0;
    size_t total_bone_count = 0;

    mat4* mats = rob_bone_data->mats.data();
    mat4* mats2 = rob_bone_data->mats2.data();
    bone_node* nodes = rob_bone_data->nodes.data();
    for (bone_database_bone& i : *bones)
        switch (i.type) {
        case BONE_DATABASE_BONE_ROTATION:
        case BONE_DATABASE_BONE_TYPE_1:
        case BONE_DATABASE_BONE_POSITION:
            nodes->mat = mats++;
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            chain_pos++;
            total_bone_count++;
            break;
        case BONE_DATABASE_BONE_POSITION_ROTATION:
            nodes->mat = mats++;
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            chain_pos++;
            ik_bone_count++;
            total_bone_count++;
            break;
        case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
            nodes->mat = mats2++;
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat = mats++;
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat = mats2++;
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            chain_pos += 2;
            ik_bone_count++;
            total_bone_count += 3;
            break;
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            nodes->mat = mats++;
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat = mats++;
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat = mats++;
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat = mats2++;
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            chain_pos += 3;
            ik_bone_count++;
            total_bone_count += 4;
            break;
        }

    for (bone_node& i : rob_bone_data->nodes)
        i.ex_data_mat = i.mat;
}

static void rob_chara_bone_data_set_motion_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    motion_blend_mot_set_duration(rob_bone_data->motion_loaded.front(), duration, step, offset);
}

static void rob_chara_bone_data_set_mouth_anim_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_duration(&rob_bone_data->mouth, duration, step, offset);
}

static void rob_chara_bone_data_set_mouth_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->mouth, frame);
}

static void rob_chara_bone_data_set_mouth_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->mouth.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    uint16_t* parent_indices) {
    if (rob_bone_data->nodes.size() < 1)
        return;

    parent_indices++;
    bone_node* i_begin = rob_bone_data->nodes.data() + 1;
    bone_node* i_end = rob_bone_data->nodes.data() + rob_bone_data->nodes.size();
    for (bone_node* i = i_begin; i != i_end; i++)
        i->parent = &rob_bone_data->nodes[*parent_indices++];
}

static void rob_chara_bone_data_set_rotation_y(rob_chara_bone_data* rob_bone_data, float_t rotation_y) {
    rob_bone_data->motion_loaded.front()->bone_data.rotation_y = rotation_y;
}

static void sub_140413EB0(struc_308* a1) {
    a1->field_8C = false;
    a1->field_4C = a1->mat;
    a1->mat = mat4u_identity;
}

static void sub_1404117F0(motion_blend_mot* a1) {
    sub_140413EB0(&a1->field_4F8);
    int32_t skeleton_select = a1->mot_key_data.skeleton_select;
    for (bone_data& i : a1->bone_data.bones)
        bone_data_mult_0(&i, skeleton_select);
}

static void sub_14040FBF0(motion_blend_mot* a1, float_t a2) {
    bone_data* b_n_hara_cp = &a1->bone_data.bones.data()[MOTION_BONE_N_HARA_CP];
    bone_data* b_kg_hara_y = &a1->bone_data.bones.data()[MOTION_BONE_KG_HARA_Y];
    bone_data* b_kl_hara_xz = &a1->bone_data.bones.data()[MOTION_BONE_KL_HARA_XZ];
    bone_data* b_kl_hara_etc = &a1->bone_data.bones.data()[MOTION_BONE_KL_HARA_ETC];
    a1->field_4F8.field_90 = vec3_null;

    mat4 rot_mat = b_n_hara_cp->rot_mat[0];
    mat4 mat = b_kg_hara_y->rot_mat[0];
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat = b_kl_hara_xz->rot_mat[0];
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat = b_kl_hara_etc->rot_mat[0];
    mat4_mult(&mat, &rot_mat, &rot_mat);
    b_n_hara_cp->rot_mat[0] = rot_mat;
    b_kg_hara_y->rot_mat[0] = mat4u_identity;
    b_kl_hara_xz->rot_mat[0] = mat4u_identity;
    b_kl_hara_etc->rot_mat[0] = mat4u_identity;

    float_t v8 = a1->field_4F8.field_C0;
    float_t v9 = a1->field_4F8.field_C4;
    vec3 v10 = a1->field_4F8.field_C8;
    a1->field_4F8.field_A8 = b_n_hara_cp->trans;
    if (!a1->mot_key_data.skeleton_select) {
        if (a2 != v9) {
            b_n_hara_cp->trans.x = (b_n_hara_cp->trans.x - v10.x) * v9 + v10.x;
            b_n_hara_cp->trans.z = (b_n_hara_cp->trans.z - v10.z) * v9 + v10.z;
        }
        b_n_hara_cp->trans.y = ((b_n_hara_cp->trans.y - v10.y) * v8) + v10.y;
    }
    else {
        if (a2 != v9) {
            v9 /= a2;
            b_n_hara_cp->trans.x = (b_n_hara_cp->trans.x - v10.x) * v9 + v10.x;
            b_n_hara_cp->trans.z = (b_n_hara_cp->trans.z - v10.z) * v9 + v10.z;
        }

        if (a2 != v8) {
            v8 /= a2;
            b_n_hara_cp->trans.y = (b_n_hara_cp->trans.y - v10.y) * v8 + v10.y;
        }
    }
}

static void sub_140410A40(motion_blend_mot* a1, std::vector<bone_data>* a2, std::vector<bone_data>* a3) {
    MotionBlend* v4 = a1->blend;
    if (!v4 || !v4->field_9)
        return;

    v4->Field_20(a2, a3);

    for (bone_data& i : *a2) {
        if (!sub_140410250(&a1->field_0.field_8, i.motion_bone_index))
            continue;

        bone_data* v8 = 0;
        if (a3)
            v8 = &a3->data()[i.motion_bone_index];
        a1->blend->Blend(&a2->data()[i.motion_bone_index], v8);
    }
}

static void sub_140410B70(motion_blend_mot* a1, std::vector<bone_data>* a2) {
    MotionBlend* v3 = a1->blend;
    if (!v3 || !v3->field_9)
        return;

    v3->Field_20(&a1->bone_data.bones, a2);
    for (bone_data& i : a1->bone_data.bones) {
        if (!sub_140410250(&a1->field_0.field_8, i.motion_bone_index))
            continue;

        bone_data* v6 = 0;
        if (a2)
            v6 = &a2->data()[i.motion_bone_index];
        a1->blend->Blend(&a1->bone_data.bones.data()[i.motion_bone_index], v6);
    }
}

static void sub_140410CB0(mot_blend* a1, std::vector<bone_data>* a2) {
    if (!a1->blend.field_9)
        return;

    for (bone_data& i : *a2) {
        if (sub_140410250(&a1->field_0.field_8, i.motion_bone_index))
            a1->blend.Blend(&a2->data()[i.motion_bone_index], 0);
    }
}

static void sub_1404182B0(rob_chara_bone_data* rob_bone_data) {
    if (!rob_bone_data->motion_loaded.size())
        return;

    auto i_begin = rob_bone_data->motion_loaded.end();
    auto i_end = rob_bone_data->motion_loaded.begin();
    i_end++;
    auto i_end2 = rob_bone_data->motion_loaded.begin();
    for (auto i = i_begin; i != i_end;) {
        i--;
        if (!sub_1404136B0(*i))
            continue;

        auto v4 = i;
        v4++;
        sub_140410A40(*i, &(*v4)->bone_data.bones, &(*i)->bone_data.bones);
    }

    for (auto i = i_begin; i != i_end2;) {
        i--;
        if (!sub_1404136B0(*i))
            continue;

        auto v5 = i;
        v5++;
        if (i != i_end)
            sub_140410B70(*i, &(*v5)->bone_data.bones);
        else if (motion_blend_mot_get_type(*i) == MOTION_BLEND_FREEZE)
            sub_140410B70(*i, 0);
    }

    motion_blend_mot* v3 = rob_bone_data->motion_loaded.front();
    sub_140410CB0(&rob_bone_data->face, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->hand_l, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->hand_r, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->mouth, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->eyes, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->eyelid, &v3->bone_data.bones);

    bone_data* v7 = &v3->bone_data.bones.data()[0];
    v3->field_4F8.field_9C = v7->trans;
    if (sub_140413790(&v3->field_4F8)) { // WTF???
        v3->field_4F8.field_90 = v7->trans;
        vec3_sub(v7->trans, v7->trans, v7->trans);
    }
}

void sub_14041B9F0(rob_chara_bone_data* rob_bone_data) {
    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        sub_1404146F0(&i->field_0);

    sub_1404146F0(&rob_bone_data->face.field_0);
    sub_1404146F0(&rob_bone_data->eyelid.field_0);
}

static void rob_chara_bone_data_update(rob_chara_bone_data* rob_bone_data, mat4* mat) {
    if (!rob_bone_data->motion_loaded.size())
        return;

    for (motion_blend_mot*& i : rob_bone_data->motion_loaded) {
        sub_1404117F0(i);
        sub_14040FBF0(i, rob_bone_data->ik_scale.ratio0);
    }

    sub_1404182B0(rob_bone_data);
    sub_14041B9F0(rob_bone_data);
    motion_blend_mot_mult_mat(rob_bone_data->motion_loaded.front(), mat);
}

void sub_140506C20(struc_523* a1) {
    a1->field_0 = 0;
    a1->field_1 = 0;
    a1->field_4 = 0;
    a1->field_8 = 0;
    a1->field_C = 0;
    a1->field_10 = vec3_null;
    a1->field_1C = 0;
    a1->field_20 = 0;
    a1->field_24 = 0;
    a1->field_28 = 0;
    a1->field_2C = 0;
    a1->field_30 = 0;
    a1->field_34 = 0;
    a1->field_38 = 0;
    a1->field_3C = 0;
    a1->field_40 = 0;
    a1->field_44 = 0;
    a1->field_48 = 0;
    a1->field_4C = 0;
    a1->field_50 = 0;
    a1->field_58.clear();
    a1->field_68 = 0;
    a1->field_70 = 0;
    a1->field_78 = 0;
    a1->field_7C = 0;
    a1->field_80 = 0.0f;
}

static void sub_140505C90(struc_264* a1) {
    a1->field_0 = 0;
    a1->field_4 = { 0, {}, -1, 0 };
    a1->field_18 = { 0, {}, -1, 0 };
    a1->field_2C = { 0, {}, -1, 0 };
    a1->field_40 = { 0, {}, -1, 0 };
    a1->field_54 = { 0, {}, -1, 0 };
    a1->field_68 = 0;
    a1->field_6C = 0;
    a1->field_70 = 0;
    a1->field_74 = 0.0f;
    a1->field_78 = 0x7FFF;
    a1->field_7C = 0;
    a1->field_80 = 0;
    a1->field_84 = 0;
    a1->field_8C = 0;
    a1->field_94 = 0;
    a1->field_98 = 0;
    a1->field_9C = 0;
    a1->field_A0 = 0;
    a1->field_A4 = 0;
    a1->field_A5 = 0;
    a1->field_A8 = 0;
    a1->field_AC = 0;
    a1->field_B0 = 0;
    a1->eyes_adjust = 0;
    sub_140506C20(&a1->field_B8);
    a1->field_140 = 0;
    a1->field_144 = 0;
    a1->field_146 = 0;
    a1->field_148 = 0;
    a1->field_150 = 0;
    a1->field_152 = 0;
    a1->field_154 = 0;
    a1->field_158 = 0;
    a1->field_15C = 0;
    a1->field_160 = 0;
    a1->field_168 = -1;
    a1->field_16C = 0;
    a1->field_170 = 0;
    a1->field_174 = 0;
    a1->field_178 = 0;
    a1->field_17C = 0;
    a1->field_180 = 0;
    a1->field_184 = 0;
    a1->field_188 = 0;
    a1->field_18C = 999.0f;
    a1->field_190 = 0;
    a1->field_194 = 0;
    a1->field_198 = 0;
    a1->field_19C = 0;
    a1->field_1A0 = 0;
    a1->field_1A4 = 0;
    a1->field_1A8 = 0;
    a1->field_1B0 = 0;
    a1->field_1B4 = 0;
    a1->field_1B8 = 0;
    a1->field_1BC = -1;
    a1->field_1C0.field_0 = 0;
    a1->field_1C0.field_4 = 0;
    a1->field_1C8 = 0;
    a1->field_1C9 = 0;
    a1->field_1CA = 0;
    a1->field_1CC = 0;
    a1->field_1D0 = 0;
    a1->field_1D4 = 0;
}

static void sub_140536DD0(struc_223* a1) {
    a1->field_0.field_84 = 0;
    a1->field_0.field_88 = 0;
    a1->field_0.field_8C = 0;
    a1->field_0.field_90 = 0;
    a1->field_0.field_94 = 0;
    a1->field_0.field_96 = 0;
    a1->field_0.field_98 = -1;
    a1->field_0.field_9C = 0;
    a1->field_0.field_A0 = 0;
    a1->field_0.field_A4 = 0;
    a1->field_0.field_A8 = 0;
    a1->field_0.field_B0[0] = { 0, 1.2f, 1.0f };
    a1->field_0.field_B0[1] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[2] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[3] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[4] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[5] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[6] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[7] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[8] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[9] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[10] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[11] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[12] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[13] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[14] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[15] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[16] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[17] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[18] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[19] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[20] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[21] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[22] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[23] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[24] = { 0, -1.0f, -1.0f };
    a1->field_0.field_B0[25] = { 0, -1.0f, -1.0f };
    a1->field_0.field_260 = 0;
    a1->field_0.field_270 = 0;
    a1->field_0.field_2B0 = 0;
    a1->field_0.field_2D8 = a1->field_0.field_2D0;
    a1->field_0.field_310 = 3;
    a1->field_0.field_314 = 0;
}

static void sub_140537110(struc_223* a1) {
    a1->field_0.motion_id = -1;
    a1->field_0.frame_count = 0.0f;
    a1->field_0.field_8 = 0.0f;
    a1->field_0.field_C = 0;
    sub_140536DD0(a1);
    a1->field_0.field_50 = 0;
    a1->field_0.field_52 = 0;
    a1->field_0.field_54 = 0;
    a1->field_0.field_58 = -1;
    a1->field_0.field_5C = -1;
    a1->field_0.field_60 = 0;
    a1->field_0.field_64 = -1;
    a1->field_0.field_68 = 0;
    a1->field_0.field_6C = -1;
    a1->field_0.field_70 = 0.0f;
    a1->field_0.field_74 = -1.0f;
    a1->field_0.field_78 = 0.0f;
    a1->field_0.field_7C = 0.0f;
    a1->field_0.field_80 = 0.0f;
    a1->field_0.field_1E8 = 0;
    a1->field_0.field_1EC = 0.0f;
    a1->field_0.field_1F0 = 0.0f;
    a1->field_0.field_1F4 = 0.0f;
    a1->field_0.field_1F8 = -1.0f;
    a1->field_0.field_1FC = 0.0f;
    a1->field_0.field_200 = 1.0f;
    a1->field_0.field_204 = -1;
    a1->field_0.field_208 = 0;
    a1->field_0.field_20C = -1;
    a1->field_0.field_210 = 0;
    a1->field_0.field_218 = -1.0f;
    a1->field_0.field_21C = -1.0f;
    a1->field_0.field_220 = 0;
    a1->field_0.field_228.clear();
    a1->field_0.field_238 = -1;
    a1->field_0.field_23C = 0.0f;
    a1->field_0.field_240 = 0;
    a1->field_0.field_244 = 0;
    a1->field_0.field_248 = 0;
    a1->field_0.field_250 = 0;
    a1->field_0.field_258 = -1.0f;
    a1->field_0.field_25C = 0;
    a1->field_0.field_268 = 0;
    a1->field_0.field_274 = 0;
    a1->field_0.field_278 = 0;
    a1->field_0.field_27C = -1;
    a1->field_0.field_280 = 0;
    a1->field_0.field_284 = 0;
    a1->field_0.field_288 = 0;
    a1->field_0.field_290 = 0;
    a1->field_0.field_294 = 0;
    a1->field_0.field_298 = 0;
    a1->field_0.field_29C = 0.0f;
    a1->field_0.field_2A0 = 0;
    a1->field_0.field_2A4 = 0.0f;
    a1->field_0.field_2A8 = 0.0f;
    a1->field_0.field_2AC = 0.0f;
    a1->field_0.field_2B8 = 0;
    a1->field_0.field_2BC = 0;
    a1->field_0.field_2C0 = -1.0f;
    a1->field_0.field_2C4 = -1.0f;
    a1->field_0.field_2C8 = 0;
    a1->field_0.field_2E8 = -1;
    a1->field_0.field_2EC = 0;
    a1->field_0.field_2F0 = 0;
    a1->field_0.field_2F4 = 0;
    a1->field_0.field_2FC = -1;
    a1->field_0.field_300 = 100;
    a1->field_0.field_304 = 1;
    a1->field_0.field_308 = 0;
    a1->field_0.field_30C = 1.0f;
    a1->field_0.field_318 = vec3_null;
    a1->field_0.field_324 = 0.05f;
    a1->field_0.field_328 = -1.0f;
    a1->field_0.iterations = -1;
}

static void sub_1405335C0(struc_223* a1) {
    a1->field_330.field_0.field_0 = 0;
    a1->field_330.field_0.field_8 = 0;
    a1->field_330.field_10 = 0;
    a1->field_330.field_14 = 0;
    a1->field_330.field_18 = 0;
    a1->field_330.field_1C = 0.0f;
    a1->field_330.field_20 = vec3_null;

    for (struc_226& i : a1->field_330.field_2C)
        for (int8_t& j : i.field_0)
            j = 0;

    for (struc_225& i : a1->field_330.field_80)
        for (float_t& j : i.field_0)
            j = 1.0f;

    for (struc_224& i : a1->field_330.field_1C4)
        for (int32_t& j : i.field_0)
            j = 0;

    a1->field_330.field_308 = 0;
    a1->field_330.field_310 = -1.0f;
    a1->field_330.field_314 = -1.0f;
    a1->field_330.field_318 = 0;
    a1->field_330.field_31C = 0;
    a1->field_330.field_320 = 1.0f;
    a1->field_330.field_324 = 1.0f;
    a1->field_330.field_32C = 0;
    a1->field_330.field_330 = -1.0f;
    a1->field_330.field_334 = -1.0f;
    a1->field_330.field_338 = 0;
    a1->field_330.field_33C[0] = { -1, -1.0f, -1.0f };
    a1->field_330.field_33C[1] = { -1, -1.0f, -1.0f };
    a1->field_330.field_33C[2] = { -1, -1.0f, -1.0f };
    a1->field_330.field_33C[3] = { -1, -1.0f, -1.0f };
}

static void sub_140539750(struc_223* a1) {
    a1->field_0.field_10 = {};
    a1->field_0.field_20 = {};
    sub_140537110(a1);
    sub_1405335C0(a1);
    a1->field_7A0 = 0;
    a1->motion_set_id = -1;
}

static void sub_140505FB0(struc_209* a1) {
    a1->field_0 = 0;
    a1->field_4 = 0;
    a1->field_8 = 0;
    a1->field_C = 0;
    a1->field_10 = 0;
    a1->field_14 = 0;
    a1->field_18 = 0;
    a1->field_1C = 0;
    a1->field_20 = 0;
    a1->field_24 = 0;
    a1->field_28 = 0;
    a1->field_2C = 0;
    a1->field_30 = 0;
    a1->field_34 = 0;
    a1->field_38 = 0;
    a1->field_3C = 0;
    a1->field_40 = 0;
    a1->field_44 = 0;
    a1->field_48 = 0;
    a1->field_4C = 0;
    a1->field_50 = 0;
    a1->field_54 = 0;
    a1->field_58 = 0;
    a1->field_5C = -1;
    a1->field_60 = 0;
    a1->field_64 = 0;
    a1->field_68 = -1;
    a1->field_6C = 0;
    a1->field_70 = 0;
    a1->field_74 = 0;

    for (mat4u& i : a1->field_78)
        i = mat4u_identity;

    for (mat4u& i : a1->field_738)
        i = mat4u_identity;

    for (struc_195& i : a1->field_DF8)
        i = {};

    for (struc_195& i : a1->field_1230)
        i = {};

    for (struc_195& i : a1->field_1668)
        i = {};

    for (struc_210& i : a1->field_1AA0)
        i = {};

    for (float_t& i : a1->field_1BE4)
        i = 0.0f;

    a1->field_1C50.clear();
    a1->field_1C50.resize(27);

    size_t v3 = 0;
    for (std::pair<int64_t, float_t>& i : a1->field_1C50)
        i = { v3++, 0.0f };

    a1->field_1C68 = 27;
    a1->field_1C70 = 27;
    a1->field_1C78 = 27;
    a1->field_1C80 = 27;

    for (int32_t& i : a1->field_1C88)
        i = 0;

    for (int32_t& i : a1->field_1CF4)
        i = 0;

    for (int32_t& i : a1->field_1D60)
        i = 0;

    a1->field_1DCC = 0;
    a1->field_1DD0 = 0;
    a1->field_1DD4 = 0;
    a1->field_1DD8 = 0;
    a1->field_1DDC = 0;
    a1->field_1DE0 = 0;
    a1->field_1DE4 = 0.0f;
    a1->field_1DE8 = -1;
    a1->field_1DEC = -1;
    a1->field_1DF0 = 0;
    a1->field_1DF4 = 0;
    a1->field_1DF8 = -1;
    a1->field_1DFC = -1;

    a1->field_1E00 = 0;
    a1->field_1E04 = 0;
    a1->field_1E08 = 0;
    a1->field_1E0C = 0;
    a1->field_1E10 = 0;
    a1->field_1E14 = 0;
    a1->field_1E18 = 0;
    a1->field_1E1C = 0;
    a1->field_1E20 = 0;
    a1->field_1E24 = 0;
    a1->field_1E28 = 0;
    a1->field_1E2C = 0;
    a1->field_1E30 = 0;
    a1->field_1E34 = 0;
    a1->field_1E38 = 0;
    a1->field_1E3C = 0;
    a1->field_1E40 = 0;
    a1->field_1E44 = 0;
    a1->field_1E48 = 0;
    a1->field_1E4C = 0;
    a1->field_1E50 = 0;
    a1->field_1E54 = 0;
    a1->field_1E58 = 0;
    a1->field_1E5C = 0;
    a1->field_1E60 = 0;
    a1->field_1E64 = 0;
    a1->field_1E68 = 0;
    a1->field_1E6C = 0;
    a1->field_1E70 = 0;
    a1->field_1E74 = 0;
    a1->field_1E78 = 0;
    a1->field_1E7C = 0;
    a1->field_1E80 = 0;
    a1->field_1E84 = 0;
    a1->field_1E88 = 0;
    a1->field_1E8C = 0;
    a1->field_1E90 = 0;
    a1->field_1E94 = 0;
    a1->field_1E98 = 0;
    a1->field_1E9C = 0;
    a1->field_1EA0 = 0;
    a1->field_1EA4 = 0;
    a1->field_1EA8 = 0;
    a1->field_1EAC = 0;
    a1->field_1EB0 = 0;

    for (struc_267& i : a1->field_1EB4)
        i = { 1000.0f, 0x145F, 0, -1.0f, 0 };

    for (int32_t& i : a1->field_1EF4)
        i = 0;

    a1->field_1F04 = 1;
    a1->field_1F08 = 0;
    a1->field_1F0C = 0;
    a1->field_1F10 = 0;
    a1->field_1F14 = 0;
    a1->field_1F18 = 0;
    a1->field_1F1C = 0;
    a1->field_1F20 = 0;
    a1->field_1F21 = 0;
    a1->field_1F22 = 0;
    a1->field_1F23 = 0;
    a1->field_1F24 = 0;
    a1->field_1F25 = 0;
    a1->field_1F26 = 0;
}

static bool rob_chara_item_cos_data_check_for_npr_flag(rob_chara_item_cos_data* itm_cos_data) {
    int32_t* arr = itm_cos_data->cos.arr;
    for (int32_t i = ITEM_SUB_ZUJO; i < ITEM_SUB_MAX; i++) {
        item_table_item* item = item_table_array_get_item(itm_cos_data->chara_index, *arr++);
        if (item && item->npr_flag)
            return true;
    }
    return false;
}

static object_info rob_chara_item_cos_data_get_head_object_replace(
    rob_chara_item_cos_data* item_cos_data, int32_t head_object_id) {
    auto elem = item_cos_data->head_replace.find(head_object_id);
    if (elem != item_cos_data->head_replace.end())
        return elem->second;
    return object_info();
}

static int32_t item_cos_tex_chg_counter = 0;

static texture_id item_cos_tex_chg_counter_get(texture_id id) {
    if (id != texture_id(0x30, 0))
        return id;

    texture_id tex_id = texture_id(0x30, item_cos_tex_chg_counter);
    if (item_cos_tex_chg_counter < 0xFFFFFFF)
        item_cos_tex_chg_counter++;
    else
        item_cos_tex_chg_counter = 0;
    return tex_id;
}

static void sub_140526FD0(rob_chara_item_cos_data* item_cos_data, int32_t item_no, item_table_item* item) {
    if (!(item->attr & 0x0C))
        return;

    if (!(item->attr & 0x08)) {
        std::vector<item_cos_texture_change_tex> tex_chg_vec;
        for (item_table_item_data_tex& i : item->data.tex) {
            item_cos_texture_change_tex chg_tex;
            chg_tex.org = texture_storage_get_texture(i.org);
            chg_tex.chg = texture_storage_get_texture(i.chg);
            chg_tex.changed = false;
            tex_chg_vec.push_back(chg_tex);
        }
        item_cos_data->texture_change.insert({ item_no, tex_chg_vec });
        return;
    }
    else if (item->data.col.size() <= 0)
        return;

    std::vector<int32_t> chg_tex_ids;
    if (item->attr & 0x04)
        for (item_table_item_data_tex& i : item->data.tex)
            chg_tex_ids.push_back(i.chg);
    else
        for (item_table_item_data_col& i : item->data.col)
            chg_tex_ids.push_back(i.tex_id);

    std::vector<item_cos_texture_change_tex> tex_chg_vec;
    size_t index = 0;
    for (int32_t& i : chg_tex_ids) {
        size_t j = &i - chg_tex_ids.data();
        texture* tex = texture_storage_get_texture(i);
        if (!tex) {
            index++;
            continue;
        }

        bool changed = false;
        if (item->data.col[j].flag & 0x01) {
            tex = texture_copy(item_cos_tex_chg_counter_get(texture_id(0x30, 0)), tex);
            texture_apply_color_tone(tex, tex, &item->data.col[j].col_tone);
            changed = true;
        }

        item_cos_texture_change_tex chg_tex;
        chg_tex.org = texture_storage_get_texture(item->data.col[j].tex_id);
        chg_tex.chg = tex;
        chg_tex.changed = changed;
        tex_chg_vec.push_back(chg_tex);
    }
    item_cos_data->texture_change.insert({ item_no, tex_chg_vec });
}

static void sub_140527280(rob_chara_item_cos_data* item_cos_data, int32_t item_no, item_table_item* item) {
    if (!(item->attr & 0x20) || !(item->attr & 0xC))
        return;

    for (item_table_item_data_obj& i : item->data.obj) {
        if (i.rpk == ITEM_NONE)
            continue;

        auto elem = item_cos_data->item_change.find(i.rpk);
        if (elem == item_cos_data->item_change.end()) {
            std::vector<uint32_t> vec;
            vec.push_back(item_no);
            item_cos_data->item_change.insert({ i.rpk, vec });
        }
        else {
            std::vector<uint32_t> vec = elem->second;
            vec.push_back(item_no);
            item_cos_data->item_change.insert_or_assign(i.rpk, vec);
        }
    }
}

static void sub_140522990(rob_chara_item_cos_data* item_cos_data) {
    rob_chara_item_cos_data_texture_change_clear(item_cos_data);
    rob_chara_item_cos_data_texture_pattern_clear(item_cos_data);
    for (int32_t sub_id = ITEM_SUB_ZUJO; sub_id < ITEM_SUB_MAX; sub_id++) {
        int32_t item_no = item_cos_data->cos.arr[sub_id];
        if (!item_no || sub_id == ITEM_SUB_KUTSU)
            continue;

        item_table_item* item = item_table_array_get_item(item_cos_data->chara_index, item_no);
        if (!item)
            continue;

        sub_140526FD0(item_cos_data, item_no, item);
        sub_140527280(item_cos_data, item_no, item);
    }
}

static void sub_14052B4C0(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip, item_id id, bool object) {
    if (object)
        rob_chara_item_equip_set_texture_pattern(rob_itm_equip, 0, 0);
    else
        rob_chara_item_equip_set_object_texture_pattern(rob_itm_equip, id, 0, 0);
}

static void sub_14052CCC0(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    std::vector<uint32_t>& item_nos, item_id id, bool a5) {
    if (id < ITEM_BODY || id >= ITEM_MAX)
        return;

    for (uint32_t& i : item_nos) {
        auto elem = item_cos_data->texture_change.find(i);
        if (elem == item_cos_data->texture_change.end())
            continue;

        for (item_cos_texture_change_tex& j : elem->second)
            if (j.org && j.chg)
                item_cos_data->texture_pattern[id].push_back({ j.org->id, j.chg->id });
    }

    if (a5)
        rob_chara_item_equip_set_texture_pattern(rob_itm_equip,
            item_cos_data->texture_pattern[id].data(), item_cos_data->texture_pattern[id].size());
    else
        rob_chara_item_equip_set_object_texture_pattern(rob_itm_equip, id,
            item_cos_data->texture_pattern[id].data(), item_cos_data->texture_pattern[id].size());
}

static void sub_140513C40(rob_chara_item_equip* rob_itm_equip, item_id id, object_info obj_info,
    bone_database* bone_data, void* data, object_database* obj_db) {
    if ((uint32_t)(id - 15) > 0xF)
        obj_info = object_info();
    rob_chara_item_equip_load_object_info(rob_itm_equip, obj_info, id, 0, bone_data, data, obj_db);
}

static void sub_140522A30(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    bone_database* bone_data, void* data, object_database* obj_db) {
    const chara_init_data* chr_init_data = chara_init_data_get(item_cos_data->chara_index);
    for (int32_t i = ITEM_ATAMA; i < ITEM_KAMI; i++) {
        sub_140513950(rob_itm_equip, (item_id)i, chr_init_data->field_7E0[i], false, bone_data, data, obj_db);
        auto elem = item_cos_data->item_change.find(i);
        if (elem == item_cos_data->item_change.end())
            sub_14052B4C0(item_cos_data, rob_itm_equip, (item_id)i, false);
        else
            sub_14052CCC0(item_cos_data, rob_itm_equip, elem->second, (item_id)i, false);
    }

    for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++) {
        sub_140513C40(rob_itm_equip, (item_id)i, object_info(), bone_data, data, obj_db);
        sub_14052B4C0(item_cos_data, rob_itm_equip, (item_id)i, false);
    }
    sub_14052B4C0(item_cos_data, rob_itm_equip, ITEM_NONE, true);

    item_cos_data->field_F0.clear();
    item_cos_data->field_100.clear();
    item_cos_data->head_replace.clear();
}

static item_id sub_140525B90(rob_chara_item_cos_data* item_cos_data, item_sub_id sub_id) {
    const item_id v3[] = {
        ITEM_ZUJO, ITEM_KAMI, ITEM_NONE, ITEM_NONE,
        ITEM_MEGANE, ITEM_NONE, ITEM_KUCHI, ITEM_NONE,
        ITEM_KUBI, ITEM_NONE, ITEM_OUTER, ITEM_NONE,
        ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE,
        ITEM_JOHA_USHIRO, ITEM_NONE, ITEM_NONE, ITEM_PANTS,
        ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE,
        ITEM_NONE,
    };

    return v3[sub_id];
}

static void rob_chara_item_cos_data_set_texture_pattern(rob_chara_item_cos_data* item_cos_data,
    rob_chara_item_equip* rob_itm_equip, uint32_t item_no, item_id id, bool tex_pat_for_all) {
    if (id < ITEM_BODY || id >= ITEM_MAX)
        return;

    auto elem = item_cos_data->texture_change.find(item_no);
    if (elem == item_cos_data->texture_change.end())
        return;

    for (item_cos_texture_change_tex& i : elem->second)
        if (i.org && i.chg)
            item_cos_data->texture_pattern[id].push_back({ i.org->id, i.chg->id });

    if (tex_pat_for_all)
        rob_chara_item_equip_set_texture_pattern(rob_itm_equip,
            item_cos_data->texture_pattern[id].data(), item_cos_data->texture_pattern[id].size());
    else
        rob_chara_item_equip_set_object_texture_pattern(rob_itm_equip, id,
            item_cos_data->texture_pattern[id].data(), item_cos_data->texture_pattern[id].size());
}

static item_id sub_140512EF0(rob_chara_item_equip* rob_itm_eq, item_id id) {
    int32_t v2 = id;
    if (id < ITEM_BODY || id >= ITEM_ITEM16)
        return ITEM_NONE;

    for (rob_chara_item_equip_object* i = &rob_itm_eq->item_equip_object[id];
        i->obj_info.not_null(); i++) {
        if (++v2 >= ITEM_ITEM16)
            return ITEM_NONE;
    }
    return (item_id)v2;
}

static void sub_140513B90(rob_chara_item_equip* rob_itm_equip, item_id id, object_info a3, object_info a4,
    bone_database* bone_data, void* data, object_database* obj_db) {
    if ((uint32_t)(id - 15) > 0xF) {
        a4 = object_info();
        a3 = object_info();
    }
    rob_itm_equip->field_D0 = a4;
    rob_itm_equip->field_D4 = id;
    rob_chara_item_equip_load_object_info(rob_itm_equip, a3, id, false, bone_data, data, obj_db);
    if (id >= ITEM_BODY && id < ITEM_MAX)
        rob_itm_equip->item_equip_object[id].disp = false;
}

static void sub_14052C560(rob_chara_item_cos_data* item_sub_data,
    rob_chara_item_equip* rob_itm_equip, int32_t item_no, item_table_item* item, item_sub_id sub_id,
    bone_database* bone_data, void* data, object_database* obj_db) {
    for (item_table_item_data_obj& i : item->data.obj) {
        if (i.obj_info.is_null())
            break;

        if (item->type == 1) {
            if (i.rpk != ITEM_NONE) {
                sub_140513950(rob_itm_equip, i.rpk, i.obj_info, false, bone_data, data, obj_db);
                if (item->attr & 0xC)
                    rob_chara_item_cos_data_set_texture_pattern(item_sub_data,
                        rob_itm_equip, item_no, i.rpk, false);
            }
            continue;
        }

        else if (item->type)
            continue;

        item_id id = sub_140525B90(item_sub_data, sub_id);
        if (id != ITEM_NONE) {
            sub_140513C40(rob_itm_equip, id, i.obj_info, bone_data, data, obj_db);
            item_sub_data->field_F0.insert({ i.obj_info, id });
            if (item->attr & 0xC)
                rob_chara_item_cos_data_set_texture_pattern(item_sub_data,
                    rob_itm_equip, item_no, id, false);
        }
        else {
            id = sub_140512EF0(rob_itm_equip, ITEM_ITEM09);
            if (id != ITEM_NONE && ((item->attr & 0x10) == 0 || &i - item->data.obj.data())) {
                bool v8 = false;
                if (item->attr & 0x10 && &i - item->data.obj.data() == 1) {
                    sub_140513B90(rob_itm_equip, id, item->data.obj[1].obj_info,
                        item->data.obj[0].obj_info, bone_data, data, obj_db);
                    v8 = true;
                }
                else
                    sub_140513C40(rob_itm_equip, id, i.obj_info, bone_data, data, obj_db);
                item_sub_data->field_F0.insert({ i.obj_info, id });
                if (item->attr & 0xC)
                    rob_chara_item_cos_data_set_texture_pattern(item_sub_data,
                        rob_itm_equip, item_no, id, v8);
            }
        }
    }

    item_sub_data->head_replace.clear();
    if (!(item->attr & 0x800))
        return;

    const char* chara_name = chara_index_get_chara_name(item_sub_data->chara_index);
    char buf[0x100];
    for (int32_t i = 0; i < 9; i++) {
        sprintf_s(buf, sizeof(buf), "%sITM%03d_ATAM_HEAD_%02d_SP__DIVSKN", chara_name, item_no, i);
        object_info obj_info = obj_db->get_object_info(buf);
        if (obj_info.not_null())
            item_sub_data->head_replace.insert({ i, obj_info });
    }
}

static void sub_140522C60(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    bone_database* bone_data, void* data, object_database* obj_db) {
    if (!item_table_array_get_table(item_cos_data->chara_index))
        return;

    const item_sub_id dword_140A2BB70[] = {
        ITEM_SUB_ZUJO, ITEM_SUB_KAMI, ITEM_SUB_HITAI, ITEM_SUB_ME, ITEM_SUB_MEGANE,
        ITEM_SUB_MIMI, ITEM_SUB_KUCHI, ITEM_SUB_MAKI, ITEM_SUB_KUBI, ITEM_SUB_INNER,
        ITEM_SUB_JOHA_MAE, ITEM_SUB_JOHA_USHIRO, ITEM_SUB_HADA, ITEM_SUB_U_UDE, ITEM_SUB_L_UDE,
        ITEM_SUB_TE, ITEM_SUB_BELT, ITEM_SUB_ASI, ITEM_SUB_SUNE, ITEM_SUB_HEAD, ITEM_SUB_MAX,
    };

    const item_sub_id* v6 = dword_140A2BB70;
    while (*v6 != ITEM_SUB_MAX) {
        int32_t item_no = item_cos_data->cos.arr[*v6];
        if (item_no) {
            item_table_item* item = item_table_array_get_item(item_cos_data->chara_index, item_no);
            if (item && item->type != 3 && item->attr & 0x01)
                sub_14052C560(item_cos_data, rob_itm_equip, item_no, item, *v6, bone_data, data, obj_db);
        }
        v6++;
    }
}

static void sub_14052C8C0(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    int32_t item_no, item_table_item* item, bone_database* bone_data, void* data, object_database* obj_db) {
    for (item_table_item_data_obj& i : item->data.obj) {
        object_info v11 = i.obj_info;
        item_id id = i.rpk;
        if (item->type == 1) {
            if (v11.is_null()) {
                if (id == ITEM_NONE)
                    continue;
            }
            else if (id == ITEM_NONE) {
            }
            else if (i.rpk < 100 || i.rpk > 105) {
                sub_140513950(rob_itm_equip, id, v11, false, bone_data, data, obj_db);
            }
            else {
                item_id v13 = sub_140512EF0(rob_itm_equip, ITEM_KAMI);
                if (v13 == ITEM_NONE)
                    continue;

                sub_140513C40(rob_itm_equip, v13, i.obj_info, bone_data, data, obj_db);
                item_cos_data->field_F0.insert({ i.obj_info, v13 });
                item_cos_data->field_100.insert({ id, v13 });
                id = v13;
            }
        }
        else if (item->type == 0) {
            id = sub_140512EF0(rob_itm_equip, ITEM_ITEM09);
            if (id != ITEM_NONE) {
                sub_140513C40(rob_itm_equip, id, i.obj_info, bone_data, data, obj_db);
                item_cos_data->field_F0.insert({ i.obj_info, id });
                continue;
            }
        }
        else
            continue;

        if (item->attr & 0xC)
            rob_chara_item_cos_data_set_texture_pattern(item_cos_data, rob_itm_equip, item_no, id, false);
    }
}

static void sub_140522D00(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    bone_database* bone_data, void* data, object_database* obj_db) {
    if (!item_table_array_get_table(item_cos_data->chara_index))
        return;

    const item_sub_id dword_140A2BBC8[] = {
        ITEM_SUB_KATA, ITEM_SUB_PANTS, ITEM_SUB_MAX,
    };

    const item_sub_id* v6 = dword_140A2BBC8;
    while (*v6 != ITEM_SUB_MAX) {
        int32_t item_no = item_cos_data->cos.arr[*v6];
        if (item_no) {
            item_table_item* item = item_table_array_get_item(item_cos_data->chara_index, item_no);
            if (item && item->type != 3 && item->attr & 0x01)
                sub_14052C8C0(item_cos_data, rob_itm_equip, item_no, item, bone_data, data, obj_db);
        }
        v6++;
    }
}

static void sub_140525D90(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    int32_t a3, bone_database* bone_data, void* data, object_database* obj_db) {
    auto elem = item_cos_data->field_100.find(a3);
    if (elem != item_cos_data->field_100.end())
        sub_140513C40(rob_itm_equip, (item_id)elem->second, object_info(), bone_data, data, obj_db);
}

static void sub_140522D90(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    bone_database* bone_data, void* data, object_database* obj_db) {
    if (!item_table_array_get_table(item_cos_data->chara_index))
        return;

    const item_sub_id dword_140A2BBD8[] = {
        ITEM_SUB_COSI, ITEM_SUB_OUTER, ITEM_SUB_MAX,
    };

    const item_sub_id* v6 = dword_140A2BBD8;
    while (*v6 != ITEM_SUB_MAX) {
        int32_t item_no = item_cos_data->cos.arr[*v6];
        if (!item_no) {
            v6++;
            continue;
        }

        item_table_item* item = item_table_array_get_item(item_cos_data->chara_index, item_no);
        if (!item || item->type != 1)
            continue;

        for (item_table_item_data_obj& i : item->data.obj) {
            if (i.obj_info.is_null())
                continue;

            item_id id = sub_140525B90(item_cos_data, *v6);
            sub_140513C40(rob_itm_equip, id, i.obj_info, bone_data, data, obj_db);
            item_cos_data->field_F0.insert({ i.obj_info, id });
            if (item->attr & 0xC)
                rob_chara_item_cos_data_set_texture_pattern(item_cos_data, rob_itm_equip, item_no, id, false);

            if (i.rpk == ITEM_NONE)
                continue;

            sub_140513950(rob_itm_equip, i.rpk, object_info(), false, bone_data, data, obj_db);
            switch (i.rpk) {
            case ITEM_UDE_R:
                sub_140525D90(item_cos_data, rob_itm_equip, 102, bone_data, data, obj_db);
                break;
            case ITEM_UDE_L:
                sub_140525D90(item_cos_data, rob_itm_equip, 100, bone_data, data, obj_db);
                break;
            case ITEM_TE_R:
                sub_140525D90(item_cos_data, rob_itm_equip, 103, bone_data, data, obj_db);
                break;
            case ITEM_TE_L:
                sub_140525D90(item_cos_data, rob_itm_equip, 101, bone_data, data, obj_db);
                break;
            case ITEM_SUNE:
                sub_140525D90(item_cos_data, rob_itm_equip, 104, bone_data, data, obj_db);
                break;
            case ITEM_ASI:
                sub_140525D90(item_cos_data, rob_itm_equip, 105, bone_data, data, obj_db);
                break;
            }
        }
        v6++;
    }
}

static void sub_140522F90(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip) {
    if (!item_table_array_get_table(item_cos_data->chara_index))
        return;

    int32_t item_no = item_cos_data->cos.arr[ITEM_SUB_KUTSU];
    if (!item_no)
        return;

    item_table_item* item = item_table_array_get_item(item_cos_data->chara_index, item_no);
    if (!item || item->data.col.size())
        return;

    vec3 texture_color_coeff = item->data.col[0].col_tone.blend;
    vec3 texture_color_offset = item->data.col[0].col_tone.offset;
    vec3 texture_specular_coeff = vec3_identity;
    vec3 texture_specular_offset = vec3_null;
    if (item->data.col.size() > 1) {
        texture_specular_coeff = item->data.col[1].col_tone.blend;
        texture_specular_offset = item->data.col[1].col_tone.offset;
    }

    for (int32_t i = ITEM_ATAMA; i <= ITEM_ITEM16; i++) {
        if (rob_chara_item_equip_get_object_info(rob_itm_equip, (item_id)i).is_null())
            continue;

        rob_chara_item_equip_object* itm_eq_obj = &rob_itm_equip->item_equip_object[i];
        itm_eq_obj->texture_data.field_0 = false;
        itm_eq_obj->texture_data.texture_color_coeff = texture_color_coeff;
        itm_eq_obj->texture_data.texture_color_offset = texture_color_offset;
        itm_eq_obj->texture_data.texture_specular_coeff = texture_specular_coeff;
        itm_eq_obj->texture_data.texture_specular_offset = texture_specular_offset;
    }
}

static void sub_140523230(rob_chara_item_cos_data* item_cos_data,
    rob_chara_item_equip* rob_itm_equip, int32_t item_no) {
    if (!item_no)
        return;

    item_table_item* item = item_table_array_get_item(item_cos_data->chara_index, item_no);
    if (!item || !((item->type + 1) & ~0x04))
        return;

    if (item->attr & 0x02)
        for (item_table_item_data_ofs& i : item->data.ofs) {
            if (item_cos_data->cos.arr[i.sub_id] != i.no)
                continue;

            for (item_table_item_data_obj& j : item->data.obj) {
                if (j.obj_info.is_null())
                    break;

                auto elem = item_cos_data->field_F0.find(j.obj_info);
                if (elem != item_cos_data->field_F0.end())
                    rob_chara_item_equip_set_null_blocks_expression_data(
                        rob_itm_equip, elem->second, &i.position, &i.rotation, &i.scale);
            }
            break;
        }

    for (item_table_item_data_obj& i : item->data.obj) {
        if (i.obj_info.is_null())
            break;

        auto elem = item_cos_data->field_F0.find(i.obj_info);
        if (elem != item_cos_data->field_F0.end()) {
            item_id id = elem->second;
            sub_140514130(rob_itm_equip, id, !(item->attr & 0x80));
            sub_140514110(rob_itm_equip, id, !(item->attr & 0x100));
            sub_140513C60(rob_itm_equip, id, !(item->attr & 0x400));
        }
    }
}

static void sub_1405231D0(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip) {
    if (!item_table_array_get_table(item_cos_data->chara_index))
        return;

    for (int32_t i = ITEM_SUB_ZUJO; i < ITEM_SUB_MAX; i++)
        sub_140523230(item_cos_data, rob_itm_equip, item_cos_data->cos.arr[i]);
}

static void sub_1405234E0(rob_chara_item_cos_data* item_cos_data) {
    item_cos_data->item_change.clear();
    item_cos_data->field_F0.clear();
    item_cos_data->field_100.clear();
}

static void rob_chara_item_cos_data_reload_items(rob_chara_item_cos_data* itm_cos_data,
    int32_t chara_id, bone_database* bone_data, void* data, object_database* obj_db) {
    rob_chara_item_equip* rob_itm_equip = rob_chara_array_get_item_equip(chara_id);
    sub_140522990(itm_cos_data);
    sub_140522A30(itm_cos_data, rob_itm_equip, bone_data, data, obj_db);
    sub_140522C60(itm_cos_data, rob_itm_equip, bone_data, data, obj_db);
    sub_140522D00(itm_cos_data, rob_itm_equip, bone_data, data, obj_db);
    sub_140522D90(itm_cos_data, rob_itm_equip, bone_data, data, obj_db);
    sub_140522F90(itm_cos_data, rob_itm_equip);
    sub_1405231D0(itm_cos_data, rob_itm_equip);
    sub_1405234E0(itm_cos_data);
}

static void rob_chara_item_cos_data_set_chara_index(
    rob_chara_item_cos_data* item_cos_data, chara_index chara_index) {
    item_cos_data->chara_index = chara_index;
}

static void rob_chara_item_cos_data_set_chara_index_item_nos(
    rob_chara_item_cos_data* item_cos_data, chara_index chara_index, int32_t* items) {
    rob_chara_item_cos_data_set_chara_index(item_cos_data, chara_index);
    rob_chara_item_cos_data_set_item_nos(item_cos_data, items);
}

static void rob_chara_item_cos_data_set_customize_item(rob_chara_item_cos_data* item_cos_data, int32_t item_no) {
    if (!item_no)
        return;

    item_table_item* item = item_table_array_get_item(item_cos_data->chara_index, item_no);
    if (item)
        item_cos_data->cos.arr[item->sub_id] = item_no;
}

static void rob_chara_item_cos_data_set_customize_items(rob_chara_item_cos_data* item_cos_data,
    rob_chara_pv_data_customize_items* customize_items) {
    for (int32_t i = 0; i < 4; i++)
        rob_chara_item_cos_data_set_customize_item(item_cos_data, customize_items->arr[i]);
}

static void rob_chara_item_cos_data_set_item(
    rob_chara_item_cos_data* item_cos_data, item_sub_id sub_id, int32_t item_no) {
    item_cos_data->cos.arr[sub_id] = item_no;
}

static void rob_chara_item_cos_data_set_item_nos(
    rob_chara_item_cos_data* item_cos_data, int32_t* item_nos) {
    for (int32_t i = ITEM_SUB_ZUJO; i < ITEM_SUB_MAX; i++)
        rob_chara_item_cos_data_set_item(item_cos_data, (item_sub_id)i, item_nos[i]);
}

static void rob_chara_item_cos_data_texture_change_clear(rob_chara_item_cos_data* item_cos_data) {
    for (auto i : item_cos_data->texture_change)
        for (item_cos_texture_change_tex& j : i.second)
            if (j.changed) {
                texture_free(j.chg);
                j.chg = 0;
            }
    item_cos_data->texture_change.clear();
}

static void rob_chara_item_cos_data_texture_pattern_clear(rob_chara_item_cos_data* item_cos_data) {
    for (std::vector<texture_pattern_struct>& i : item_cos_data->texture_pattern)
        i.clear();
}

static float_t sub_140512F60(rob_chara_item_equip* rob_itm_equip) {
    float_t v26;
    float_t v27;
    float_t v28;

    mat4* mats = rob_itm_equip->matrices;
    float_t v25 = 1.0f;
    float_t* v2 = &v27;
    float_t v3 = mats->row3.y;
    v26 = mats->row3.x;
    if (v3 >= 1.0f)
        v2 = &v25;
    v28 = mats->row3.z;
    float_t v4 = mats[175].row3.x;
    v27 = v3;
    float_t v5 = *v2;
    float_t v6 = mats[175].row3.y;
    v26 = v4;
    float_t* v7 = &v27;
    float_t v8 = mats[175].row3.z;
    v27 = v6;
    if (v5 <= v6)
        v7 = &v25;
    float_t v9 = mats[181].row3.y;
    v28 = v8;
    float_t v10 = mats[181].row3.x;
    v25 = v5;
    float_t v11 = *v7;
    v26 = v10;
    float_t* v12 = &v27;
    float_t v13 = mats[181].row3.z;
    v27 = v9;
    if (v11 <= v9)
        v12 = &v25;
    float_t v14 = mats[105].row3.y;
    v28 = v13;
    float_t v15 = mats[105].row3.x;
    v25 = v11;
    float_t v16 = *v12;
    v26 = v15;
    float_t*v17 = &v27;
    float_t v18 = mats[105].row3.z;
    v27 = v14;
    if (v16 <= v14)
        v17 = &v25;
    float_t v19 = mats[140].row3.y;
    v28 = v18;
    float_t v20 = mats[140].row3.x;
    v25 = v16;
    float_t v21 = *v17;
    v26 = v20;
    float_t* v22 = &v27;
    float_t v23 = mats[140].row3.z;
    v25 = v21;
    if (v21 <= v19)
        v22 = &v25;
    v27 = v19;
    v28 = v23;
    return *v22;
}

static void sub_140512C20(rob_chara_item_equip* rob_itm_equip, render_context* rctx) {
    if (rob_itm_equip->field_D0.is_null() || rob_itm_equip->field_D4 == ITEM_NONE)
        return;

    mat4 mat = mat4_identity;
    const char* name;
    if (rob_itm_equip->field_DC == 1) {
        name = "kl_te_r_wj";
        mat4_translate(0.0f, 0.0f, 0.082f, &mat);
        mat4_rotate_mult(&mat, (float_t)(-90.9 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    if (rob_itm_equip->field_DC == 2) {
        name = "kl_te_l_wj";
        mat4_translate(0.0f, 0.0015f, -0.0812f, &mat);
        mat4_rotate_mult(&mat, (float_t)(-34.5 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    else
        name = "j_1_hyoutan_000wj";

    bone_database* bone_data = &rctx->data->data_ft.bone_data;
    bone_node* node = rob_chara_item_equip_object_get_bone_node_by_name(
        &rob_itm_equip->item_equip_object[rob_itm_equip->field_D4], name, bone_data);
    if (!node || !node->mat)
        return;

    mat4_mult(&mat, node->mat, &mat);
    glPushMatrix();
    glMultTransposeMatrixf(&mat.row0.x);
    int32_t tex_pat_count = (int32_t)rob_itm_equip->texture_pattern.size();
    if (tex_pat_count)
        rctx->object_data.set_texture_pattern(tex_pat_count, rob_itm_equip->texture_pattern.data());
    draw_task_add_draw_object_by_object_info(rctx, 0, rob_itm_equip->field_D0, 0, 0, 0, 0, 0, false);
    if (tex_pat_count)
        rctx->object_data.set_texture_pattern();
    glPopMatrix();
}

static void rob_chara_item_equip_add_motion_reset_data(rob_chara_item_equip* rob_item_equip,
    int32_t motion_id, float_t frame, int32_t iterations) {
    for (int32_t i = rob_item_equip->first_item_equip_object; i < rob_item_equip->max_item_equip_object; i++)
        rob_chara_item_equip_object_add_motion_reset_data(
            &rob_item_equip->item_equip_object[i], motion_id, frame, iterations);
}

static void rob_chara_item_equip_disp(
    rob_chara_item_equip* rob_itm_equip, int32_t chara_id, render_context* rctx) {
    draw_task_flags v2 = (draw_task_flags)0;
    if (rctx->chara_reflect)
        enum_or(v2, DRAW_TASK_CHARA_REFLECT);
    if (rctx->chara_refract)
        enum_or(v2, DRAW_TASK_REFRACT);

    object_data* object_data = &rctx->object_data;
    shadow* shad = rctx->draw_pass.shadow_ptr;
    if (rob_itm_equip->shadow_type != -1) {
        if (rob_itm_equip->field_A0 & 4) {
            shadow_type_enum shadow_type = rob_itm_equip->shadow_type;
            vec3 pos = rob_itm_equip->position;
            pos.y -= 0.2f;
            shad->field_1D0[shadow_type].push_back(pos);

            float_t v9;
            if (sub_140512F60(rob_itm_equip) <= -0.2f)
                v9 = -0.5f;
            else
                v9 = 0.05f;
            rctx->draw_pass.shadow_ptr->field_1C0[shadow_type] = v9;
            object_data->set_shadow_type(shadow_type);
            enum_or(v2, DRAW_TASK_SHADOW);
        }

        if (rob_itm_equip->field_A0 & 1)
            enum_or(v2, DRAW_TASK_4);
    }
    object_data->set_draw_task_flags(v2);
    object_data->set_chara_color(rob_itm_equip->chara_color);

    vec4 v23;
    object_data->get_texture_color_coeff(&v23);

    vec4 texture_color_coeff = rob_itm_equip->texture_color_coeff;
    object_data->set_texture_color_coeff(&texture_color_coeff);
    object_data->set_wet_param(rob_itm_equip->wet);
    //rctx->draw_pass.field_31C |= rob_itm_equip->field_F9;
    sub_140512C20(rob_itm_equip, rctx);
    //sub_1405421D0(chara_id, rctx->chara_reflect, rob_itm_equip->chara_color);

    mat4 mat = rob_itm_equip->mat;
    if (rob_itm_equip->item_equip_range)
        for (int32_t i = rob_itm_equip->first_item_equip_object;
            i < rob_itm_equip->max_item_equip_object; i++)
            rob_chara_item_equip_object_disp(&rob_itm_equip->item_equip_object[i], &mat, rctx);
    else {
        for (int32_t i = ITEM_ATAMA; i < ITEM_MAX; i++) {
            draw_task_flags v18 = (draw_task_flags)0;
            if (rob_itm_equip->field_18[i] == 0) {
                if (rctx->chara_reflect)
                    enum_or(v18, DRAW_TASK_CHARA_REFLECT);
                if (rctx->chara_refract)
                    enum_or(v18, DRAW_TASK_REFRACT);
            }

            draw_task_flags v19 = (draw_task_flags)(DRAW_TASK_4 | DRAW_TASK_SHADOW);
            if (i == 8)
                v19 = (draw_task_flags)0;

            if (~rob_itm_equip->field_A0 & 0x04)
                enum_and(v19, ~DRAW_TASK_SHADOW);

            object_data->set_draw_task_flags( (draw_task_flags)(v18 | v19 | DRAW_TASK_SSS));
            rob_chara_item_equip_object_disp(&rob_itm_equip->item_equip_object[i], &mat, rctx);
        }
    }
    object_data->set_texture_color_coeff(&v23);
    object_data->set_wet_param();
    object_data->set_chara_color();
    object_data->set_draw_task_flags();
    object_data->set_shadow_type();
}

static object_info rob_chara_item_equip_get_object_info(
    rob_chara_item_equip* rob_itm_equip, item_id id) {
    if (id >= ITEM_BODY && id <= ITEM_ITEM16)
        return rob_itm_equip->item_equip_object[id].obj_info;
    return object_info();
}

static void rob_chara_item_equip_get_parent_bone_nodes(
    rob_chara_item_equip* rob_itm_equip, bone_node* bone_nodes, bone_database* bone_data) {
    rob_itm_equip->bone_nodes = bone_nodes;
    rob_itm_equip->matrices = bone_nodes->mat;
    for (int32_t i = rob_itm_equip->first_item_equip_object;
        i < rob_itm_equip->max_item_equip_object; i++)
        rob_chara_item_equip_object_get_parent_bone_nodes(
            &rob_itm_equip->item_equip_object[i], rob_itm_equip->bone_nodes, bone_data);
}

static void rob_chara_item_equip_load_object_info(
    rob_chara_item_equip* rob_itm_equip, object_info object_info, item_id id,
    bool osage_reset, bone_database* bone_data, void* data, object_database* obj_db) {
    if (id < ITEM_BODY || id > ITEM_ITEM16 || !rob_itm_equip->bone_nodes)
        return;

    rob_chara_item_equip_object_load_object_info_ex_data(
        &rob_itm_equip->item_equip_object[id], object_info,
        rob_itm_equip->bone_nodes, osage_reset, bone_data, data, obj_db);
    rob_itm_equip->item_equip_object[id].disp = true;
}

static void rob_chara_item_equip_object_add_motion_reset_data(rob_chara_item_equip_object* itm_eq_obj,
    int32_t motion_id, float_t frame, int32_t osage_iterations) {
    if (osage_iterations > 0)
        rob_chara_item_equip_object_ctrl_iterate_nodes(itm_eq_obj, osage_iterations);

    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
        i->AddMotionResetData(motion_id, frame);

    for (ExClothBlock*& i : itm_eq_obj->cloth_blocks)
        i->AddMotionResetData(motion_id, frame);
}

static void rob_chara_item_equip_object_check_no_opd(
    rob_chara_item_equip_object* itm_eq_obj, std::vector<opd_blend_data>* opd_blend_data) {
    itm_eq_obj->use_opd = true;
    if (!itm_eq_obj->osage_blocks.size() && !itm_eq_obj->cloth_blocks.size() || !opd_blend_data->size())
        return;

    for (::opd_blend_data& i : *opd_blend_data) {
        float_t* opd_data = 0;
        uint32_t opd_count = 0;
        osage_play_data_manager_get_opd_file_data(itm_eq_obj->obj_info,
            i.motion_id, &opd_data, &opd_count);
        if (!opd_data) {
            itm_eq_obj->use_opd = false;
            break;
        }
    }
}

static void rob_chara_item_equip_object_clear_ex_data(rob_chara_item_equip_object* itm_eq_obj) {
    for (ExNullBlock*& i : itm_eq_obj->null_blocks) {
        i->Reset();
        delete i;
    }
    itm_eq_obj->null_blocks.clear();

    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks) {
        i->Reset();
        delete i;
    }
    itm_eq_obj->osage_blocks.clear();

    for (ExConstraintBlock*& i : itm_eq_obj->constraint_blocks) {
        i->Reset();
        delete i;
    }
    itm_eq_obj->constraint_blocks.clear();

    for (ExExpressionBlock*& i : itm_eq_obj->expression_blocks) {
        i->Reset();
        delete i;
    }
    itm_eq_obj->expression_blocks.clear();

    for (ExClothBlock*& i : itm_eq_obj->cloth_blocks) {
        i->Reset();
        delete i;
    }
    itm_eq_obj->cloth_blocks.clear();

    itm_eq_obj->node_blocks.clear();
}

static void rob_chara_item_equip_object_disp(
    rob_chara_item_equip_object* itm_eq_obj, mat4* mat, render_context* rctx) {
    if (itm_eq_obj->obj_info.is_null())
        return;

    draw_task_flags v2 = rctx->object_data.get_draw_task_flags();
    draw_task_flags v4 = v2;
    if (fabsf(itm_eq_obj->alpha - 1.0f) > 0.000001f)
        enum_or(v4, itm_eq_obj->draw_task_flags);
    else
        enum_and(v4, ~(DRAW_TASK_40000 | DRAW_TASK_20000 | DRAW_TASK_10000));
    rctx->object_data.set_draw_task_flags(v4);
    if (itm_eq_obj->disp) {
        draw_task_add_draw_object_by_object_info_object_skin(rctx,
            itm_eq_obj->obj_info,
            &itm_eq_obj->texture_pattern,
            &itm_eq_obj->texture_data,
            itm_eq_obj->alpha,
            itm_eq_obj->mats,
            itm_eq_obj->ex_data_block_mats.data(),
            itm_eq_obj->mat,
            mat);

        for (ExNodeBlock*& i : itm_eq_obj->node_blocks)
            i->Disp();
    }
    rctx->object_data.set_draw_task_flags(v2);
}

static int32_t rob_chara_item_equip_object_get_bone_index(
    rob_chara_item_equip_object* itm_eq_obj, const char* name, bone_database* bone_data) {
    int32_t bone_index = bone_data->get_skeleton_motion_bone_index(
        bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), name);
    if (bone_index == -1)
        for (ex_data_name_bone_index& i : itm_eq_obj->ex_bones)
            if (!str_utils_compare(name, i.name))
                return 0x8000 | i.bone_index;
    return bone_index;
}

static bone_node* rob_chara_item_equip_object_get_bone_node(
    rob_chara_item_equip_object* itm_eq_obj, int32_t bone_index) {
    if (~bone_index & 0x8000)
        return &itm_eq_obj->bone_nodes[bone_index & 0x7FFF];
    else if ((bone_index & 0x7FFF) < itm_eq_obj->ex_data_bone_nodes.size())
        return &itm_eq_obj->ex_data_bone_nodes[bone_index & 0x7FFF];
    return 0;
}

static bone_node* rob_chara_item_equip_object_get_bone_node_by_name(
    rob_chara_item_equip_object* itm_eq_obj, const char* name, bone_database* bone_data) {
    return rob_chara_item_equip_object_get_bone_node(itm_eq_obj,
        rob_chara_item_equip_object_get_bone_index(itm_eq_obj, name, bone_data));
}

static void rob_chara_item_equip_object_get_parent_bone_nodes(
    rob_chara_item_equip_object* itm_eq_obj, bone_node* bone_nodes, bone_database* bone_data) {
    itm_eq_obj->bone_nodes = bone_nodes;
    itm_eq_obj->mats = bone_nodes->mat;
    for (ExNodeBlock*& i : itm_eq_obj->node_blocks)
        i->parent_bone_node = rob_chara_item_equip_object_get_bone_node_by_name(
            itm_eq_obj, i->parent_name.c_str(), bone_data);
}

static void rob_chara_item_equip_object_init_ex_data_bone_nodes(
    rob_chara_item_equip_object* itm_eq_obj, obj_skin_ex_data* ex_data) {
    itm_eq_obj->ex_data_bone_nodes.clear();
    itm_eq_obj->ex_data_mats.clear();
    itm_eq_obj->ex_data_block_mats.clear();

    int32_t count = ex_data->bone_names_count;
    itm_eq_obj->ex_data_bone_nodes.resize(count);
    itm_eq_obj->ex_data_mats.resize(count);
    itm_eq_obj->ex_data_block_mats.resize(count);

    bone_node* v4 = itm_eq_obj->ex_data_bone_nodes.data();
    mat4* v5 = itm_eq_obj->ex_data_mats.data();
    mat4* v8 = itm_eq_obj->ex_data_block_mats.data();
    for (int32_t v10 = 0; v10 < count; v10++)
        v4[v10].mat = &v5[v10];

    if (ex_data->bone_names) {
        itm_eq_obj->ex_bones.clear();
        for (int32_t i = 0; i < count; i++) {
            bone_node* node = &v4[i];
            node->name = ex_data->bone_names[i];
            node->mat = &v5[i];
            *node->mat = mat4_identity;
            node->ex_data_mat = &v8[i];
            *node->ex_data_mat = mat4_identity;

            itm_eq_obj->ex_bones.push_back({ ex_data->bone_names[i], i});
        }
    }
}

static void rob_chara_item_equip_object_init_members(rob_chara_item_equip_object* itm_eq_obj, size_t index) {
    itm_eq_obj->index = index;
    itm_eq_obj->obj_info = object_info();
    itm_eq_obj->field_14 = -1;
    itm_eq_obj->mats = 0;
    itm_eq_obj->texture_pattern.clear();
    itm_eq_obj->texture_data.field_0 = -1;
    itm_eq_obj->texture_data.texture_color_coeff = vec3_identity;
    itm_eq_obj->texture_data.texture_color_offset = vec3_null;
    itm_eq_obj->texture_data.texture_specular_coeff = vec3_identity;
    itm_eq_obj->texture_data.texture_specular_offset = vec3_null;
    itm_eq_obj->alpha = 1.0f;
    itm_eq_obj->draw_task_flags = DRAW_TASK_10000;
    itm_eq_obj->null_blocks_data_set = 0;
    itm_eq_obj->disp = true;
    itm_eq_obj->field_A4 = 0;
    itm_eq_obj->mat = 0;
    itm_eq_obj->bone_nodes = 0;
    rob_chara_item_equip_object_clear_ex_data(itm_eq_obj);
    itm_eq_obj->ex_data_bone_nodes.clear();
    itm_eq_obj->ex_data_mats.clear();
    itm_eq_obj->ex_data_block_mats.clear();
    itm_eq_obj->ex_bones.clear();
    itm_eq_obj->field_1B8 = 0;
    itm_eq_obj->use_opd = false;
    itm_eq_obj->field_1C0 = 0;
}

static void rob_chara_item_equip_object_load_ex_data(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_ex_data* ex_data, bone_database* bone_data, void* data, object_database* obj_db) {
    if (!ex_data->blocks)
        return;

    std::vector<std::pair<uint32_t, rob_osage_node*>> v129;
    std::map<const char*, ExNodeBlock*> v138;
    rob_chara_item_equip_object_clear_ex_data(itm_eq_obj);

    size_t constraint_count = 0;
    size_t expression_count = 0;
    size_t osage_count = 0;
    size_t cloth_count = 0;
    size_t null_count = 0;
    char** bone_names_ptr = ex_data->bone_names;
    obj_skin_block* v4 = ex_data->blocks;
    for (uint32_t i = 0; i < ex_data->blocks_count; i++, v4++) {
        ExNodeBlock* node;
        if (v4->type == OBJ_SKIN_BLOCK_CLOTH) {
            if (cloth_count >= 0x08)
                continue;

            ExClothBlock* cls = new ExClothBlock;
            node = cls;
            itm_eq_obj->cloth_blocks.push_back(cls);
            cls->index = cloth_count + osage_count;
            cloth_count++;
            continue;
        }
        else if (v4->type == OBJ_SKIN_BLOCK_CONSTRAINT) {
            if (constraint_count >= 0x40)
                continue;

            ExConstraintBlock* cns = new ExConstraintBlock;
            node = cns;
            itm_eq_obj->constraint_blocks.push_back(cns);
            cns->InitData(itm_eq_obj, &v4->constraint,
                bone_names_ptr[v4->constraint.name_index & 0x7FFF], bone_data);
            constraint_count++;
        }
        else if (v4->type == OBJ_SKIN_BLOCK_EXPRESSION) {
            if (expression_count >= 0x50)
                continue;

            ExExpressionBlock* exp = new ExExpressionBlock;
            node = exp;
            itm_eq_obj->expression_blocks.push_back(exp);
            exp->InitData(itm_eq_obj, &v4->expression,
                bone_names_ptr[v4->expression.name_index & 0x7FFF],
                itm_eq_obj->obj_info, itm_eq_obj->index, bone_data);
            expression_count++;
        }
        else if (v4->type == OBJ_SKIN_BLOCK_OSAGE) {
            if (osage_count >= 0x100)
                continue;

            ExOsageBlock* osg = new ExOsageBlock;
            node = osg;
            itm_eq_obj->osage_blocks.push_back(osg);
            osg->InitData(itm_eq_obj, &v4->osage,
                bone_names_ptr[v4->osage.external_name_index & 0x7FFF],
                &itm_eq_obj->skin_ex_data->osage_nodes[v4->osage.start_index],
                itm_eq_obj->bone_nodes, itm_eq_obj->ex_data_bone_nodes.data(), itm_eq_obj->skin);
            osg->sub_1405F3E10(&v4->osage,
                &itm_eq_obj->skin_ex_data->osage_nodes[v4->osage.start_index], &v129, &v138);
            osg->index = osage_count;
            osage_count++;
        }
        else {
            if (null_count >= 0x4)
                continue;

            ExNullBlock* null = new ExNullBlock;
            node = null;
            itm_eq_obj->null_blocks.push_back(null);
            null->InitData(itm_eq_obj, &v4->constraint,
                bone_names_ptr[v4->constraint.name_index & 0x7FFF], bone_data);
            null_count++;
        }

        obj_skin_block_node* v54 = &v4->base;
        bone_node_expression_data exp_data;
        exp_data.position = v54->position;
        exp_data.rotation = v54->rotation;
        exp_data.scale = v54->scale;
        exp_data.parent_scale = vec3_identity;
        node->parent_name = std::string(string_data(&v54->parent_name), v54->parent_name.length);

        bone_node* parent_bone_node = rob_chara_item_equip_object_get_bone_node_by_name(itm_eq_obj,
            string_data(&v54->parent_name), bone_data);
        node->parent_bone_node = parent_bone_node;
        if (node->bone_node_ptr) {
            node->bone_node_ptr->exp_data = exp_data;
            node->bone_node_ptr->parent = parent_bone_node;
        }

        auto v75 = v138.find(parent_bone_node->name);
        if (v75 != v138.end())
            node->parent_node = v75->second;

        v138.insert({ node->name, node });
        itm_eq_obj->node_blocks.push_back(node);
    }

    for (ExNodeBlock*& i : itm_eq_obj->node_blocks) {
        ExNodeBlock* v86 = i->parent_node;
        if (v86) {
            v86->has_children_node = true;
            if ((v86->type & ~3u) || v86->type == EX_OSAGE || !v86->field_58)
                continue;
        }
        i->field_58 = true;
    }

    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks) {
        ExOsageBlock* osg = i;
        ExNodeBlock* parent_node = osg->parent_node;
        bone_node* parent_bone_node = 0;
        if (!parent_node || osg->field_58)
            parent_bone_node = osg->parent_bone_node;
        else {
            while (!parent_node->field_58) {
                parent_node = parent_node->parent_node;
                if (!parent_node)
                    break;
            }

            if (parent_node)
                parent_bone_node = parent_node->parent_bone_node;
        }

        if (parent_bone_node && parent_bone_node->ex_data_mat) {
            osg->rob.parent_mat_ptr = parent_bone_node->ex_data_mat;
            osg->rob.parent_mat = *parent_bone_node->ex_data_mat;
        }
    }

    if (ex_data->osage_sibling_infos)
        for (uint32_t i = 0; i < ex_data->osage_sibling_infos_count; i++) {
            obj_skin_osage_sibling_info* sibling_info = &ex_data->osage_sibling_infos[i];
            uint32_t name_index = sibling_info->name_index;
            uint32_t sibling_name_index = sibling_info->sibling_name_index;

            auto node = v129.begin();
            for (; node != v129.end(); node++)
                if (node->first == name_index)
                    break;

            auto sibling_node = v129.begin();
            for (; sibling_node != v129.end(); sibling_node++)
                if (sibling_node->first == sibling_name_index)
                    break;

            if (node != v129.end() && sibling_node != v129.end()) {
                rob_osage_node* v102 = node->second;
                v102->sibling_node = sibling_node->second;
                v102->max_distance = sibling_info->max_distance;
            }
        }

    if (osage_count || cloth_count)
        rob_chara_item_equip_object_skp_load_file(itm_eq_obj, data, bone_data, obj_db);

    size_t v103 = 0;
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
        v103 += i->rob.nodes.size() - 1;
    //for (ExClothBlock*& i : itm_eq_obj->cloth_blocks.begin)
    //    v103 += i->rob.base.field_20.size() - i->rob.base.field_10;
    itm_eq_obj->field_1C0 = v103;
}

static void rob_chara_item_equip_object_load_object_info_ex_data(
    rob_chara_item_equip_object* itm_eq_obj, object_info object_info, bone_node* bone_nodes,
    bool osage_reset, bone_database* bone_data, void* data, object_database* obj_db) {
    itm_eq_obj->obj_info = object_info;
    itm_eq_obj->bone_nodes = bone_nodes;
    itm_eq_obj->mats = bone_nodes->mat;
    itm_eq_obj->ex_data_bone_nodes.clear();
    itm_eq_obj->ex_bones.clear();
    itm_eq_obj->ex_data_mats.clear();
    rob_chara_item_equip_object_clear_ex_data(itm_eq_obj);
    obj_skin* skin = object_storage_get_obj_skin(itm_eq_obj->obj_info);
    if (!skin || !skin->ex_data_init)
        return;

    itm_eq_obj->skin = skin;
    itm_eq_obj->skin_ex_data = &skin->ex_data;
    rob_chara_item_equip_object_init_ex_data_bone_nodes(itm_eq_obj, &skin->ex_data);
    rob_chara_item_equip_object_load_ex_data(itm_eq_obj, &skin->ex_data, bone_data, data, obj_db);

    if (osage_reset && itm_eq_obj->osage_blocks.size())
        itm_eq_obj->osage_iterations = 60;
}

static bool rob_chara_item_equip_object_set_boc(rob_chara_item_equip_object* itm_eq_obj,
    skin_param_osage_root* skp_root, ExOsageBlock* osg) {
    rob_osage* rob_osg = &osg->rob;
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++)
        i->data_ptr->boc.clear();

    bool v6 = false;
    for (skin_param_osage_root_boc& i : skp_root->boc) {
        for (ExOsageBlock*& j : itm_eq_obj->osage_blocks) {
            if (str_utils_compare(j->name, i.ed_root.c_str())
                || i.ed_node + 1ULL >= rob_osg->nodes.size()
                || i.st_node + 1ULL >= j->rob.nodes.size())
                continue;

            rob_osage_node* v17 = rob_osage_get_node(rob_osg, i.ed_node + 1LL);
            rob_osage_node* v18 = rob_osage_get_node(&j->rob, i.st_node + 1LL);
            v17->data_ptr->boc.push_back(v18);
            v6 = true;
            break;
        }
    }
    return v6;
}

static void rob_chara_item_equip_object_set_collision_target_osage(
    rob_chara_item_equip_object* itm_eq_obj, skin_param_osage_root* skp_root, skin_param* skp) {
    if (!skp_root->colli_tgt_osg.size())
        return;

    const char* colli_tgt_osg = skp_root->colli_tgt_osg.c_str();
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
        if (!str_utils_compare(colli_tgt_osg, i->name)) {
            skp->colli_tgt_osg = &i->rob.nodes;
            break;
        }
}

static void rob_chara_item_equip_object_set_motion_reset_data(
    rob_chara_item_equip_object* itm_eq_obj, int32_t motion_id, float_t frame) {
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
        i->SetMotionResetData(motion_id, frame);

    for (ExClothBlock*& i : itm_eq_obj->cloth_blocks)
        i->SetMotionResetData(motion_id, frame);
}

static void rob_chara_item_equip_object_set_motion_skin_param(
    rob_chara_item_equip_object* itm_eq_obj, int8_t chara_id, int32_t motion_id, int32_t frame) {
    if (itm_eq_obj->obj_info.is_null())
        return;

    struc_571** v7 = 0;//sub_14060B470(chara_id, itm_eq_obj->obj_info, motion_id, frame);
    if (!v7)
        return;

    struc_571* v9 = *v7;
    itm_eq_obj->field_1B8 = 0;
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks) {
        itm_eq_obj->field_1B8 |= v9->field_88;
        i->SetSkinParam(v9++);
    }

    for (ExClothBlock*& i : itm_eq_obj->cloth_blocks) {
        itm_eq_obj->field_1B8 |= v9->field_88;
        i->SetSkinParam(v9++);
    }
}

static void rob_chara_item_equip_object_set_null_blocks_expression_data(
    rob_chara_item_equip_object* rob_itm_equip, vec3* position, vec3* rotation, vec3* scale) {
    vec3 pos = *position;
    vec3 rot;
    vec3_mult_scalar(*rotation, DEG_TO_RAD_FLOAT, rot);
    vec3 sc = *scale;

    rob_itm_equip->null_blocks_data_set = true;
    for (ExNullBlock*& i : rob_itm_equip->null_blocks) {
        if (!i || !i->bone_node_ptr)
            continue;

        bone_node_expression_data* node_exp_data = &i->bone_node_ptr->exp_data;
        node_exp_data->position = pos;
        node_exp_data->rotation = rot;
        node_exp_data->scale = sc;
    }
}

static void rob_chara_item_equip_object_set_osage_reset(
    rob_chara_item_equip_object* itm_eq_obj) {
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
        i->SetOsageReset();

    for (ExClothBlock*& i : itm_eq_obj->cloth_blocks)
        i->SetOsageReset();
}

static void rob_chara_item_equip_object_set_osage_move_cancel(
    rob_chara_item_equip_object* itm_eq_obj, float_t value) {
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
        i->rob.move_cancel = value;
    //for (ExClothBlock*& i : itm_eq_obj->cloth_blocks)
    //    i->rob.move_cancel = value;
}

static void rob_chara_item_equip_object_set_texture_pattern(
    rob_chara_item_equip_object* itm_eq_obj, texture_pattern_struct* tex_pat, size_t count) {
    itm_eq_obj->texture_pattern.clear();
    if (count && tex_pat)
        for (size_t i = 0; i < count; i++)
            itm_eq_obj->texture_pattern.push_back(tex_pat[i]);
}

static void rob_chara_item_equip_object_skp_load(
    rob_chara_item_equip_object* itm_eq_obj, key_val* kv, bone_database* bone_data) {
    if (kv->key_hash.size() < 1)
        return;

    itm_eq_obj->field_1B8 = 0;
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks) {
        ExOsageBlock* osg = i;
        skin_param_osage_root root;
        rob_osage_load_skin_param(&osg->rob, kv, osg->name,
            &root, &itm_eq_obj->obj_info, bone_data);
        rob_chara_item_equip_object_set_collision_target_osage(itm_eq_obj, &root, osg->rob.skin_param_ptr);
        itm_eq_obj->field_1B8 |= rob_chara_item_equip_object_set_boc(itm_eq_obj, &root, osg);
        itm_eq_obj->field_1B8 |= root.coli_type != 0;
        //itm_eq_obj->field_1B8 |= sub_1405FAA30(itm_eq_obj, &root, 0);
    }
    for (ExClothBlock*& i : itm_eq_obj->cloth_blocks) {
        ExClothBlock* cls = i;
        skin_param_osage_root root;
        skin_param_osage_root_parse(kv, cls->name, &root, bone_data);
        //rob_cloth_load_skin_param(&cls->rob, &root);
    }
}

static void rob_chara_item_equip_object_skp_load_file(
    rob_chara_item_equip_object* itm_eq_obj, void* data,
    bone_database* bone_data, object_database* obj_db) {
    key_val* kv_ptr = 0;//sub_14060B4B0(itm_eq_obj->object_info);
    if (kv_ptr) {
        rob_chara_item_equip_object_skp_load(itm_eq_obj, kv_ptr, bone_data);
        return;
    }

    char buf[0x200];
    const char* name = obj_db->get_object_name(itm_eq_obj->obj_info);
    sprintf_s(buf, sizeof(buf), "ext_skp_%s.txt", name);

    for (int32_t i = 0; buf[i]; i++)
        if (buf[i] > 0x40 && buf[i] < 0x5B)
            buf[i] += 0x20;

    key_val kv;
    ((data_struct*)data)->load_file(&kv, "rom/skin_param/", buf, key_val::load_file);
    rob_chara_item_equip_object_skp_load(itm_eq_obj, &kv, bone_data);
}

static void rob_chara_item_equip_reset_init_data(rob_chara_item_equip* rob_itm_equip,
    bone_node* bone_nodes) {
    rob_itm_equip->reset();
    rob_itm_equip->bone_nodes = bone_nodes;
    rob_itm_equip->matrices = bone_nodes->mat;

    mat4u* v7 = rob_itm_equip->field_13C;
    int32_t* v8 = rob_itm_equip->field_18;
    for (int32_t v6 = ITEM_BODY; v6 < ITEM_MAX; v6++) {
        rob_chara_item_equip_object_init_members(&rob_itm_equip->item_equip_object[v6], v6);
        *v8++ = 0;
        *v7++ = mat4u_identity;
    }

    rob_itm_equip->step = 1.0f;
    rob_itm_equip->wet = 0.0f;
    rob_itm_equip->wind_strength = 1.0f;
    rob_itm_equip->chara_color = true;
    rob_itm_equip->npr_flag = false;
    rob_itm_equip->mat = mat4u_identity;
}

static void rob_chara_item_equip_set_item_equip_range(rob_chara_item_equip* rob_itm_equip, bool value) {
    rob_itm_equip->item_equip_range = value;
    if (value) {
        rob_itm_equip->first_item_equip_object = ITEM_BODY;
        rob_itm_equip->max_item_equip_object = ITEM_ATAMA;
    }
    else {
        rob_itm_equip->first_item_equip_object = ITEM_ATAMA;
        rob_itm_equip->max_item_equip_object = ITEM_ITEM16;
    }
}

static void rob_chara_item_equip_set_motion_reset_data(
    rob_chara_item_equip* rob_itm_equip, int32_t motion_id, float_t frame) {
    for (int32_t i = rob_itm_equip->first_item_equip_object; i < rob_itm_equip->max_item_equip_object; i++)
        rob_chara_item_equip_object_set_motion_reset_data(&rob_itm_equip->item_equip_object[i], motion_id, frame);

    task_wind.ptr->reset();
}

static void rob_chara_item_equip_set_motion_skin_param(
    rob_chara_item_equip* rob_itm_equip, int8_t chara_id, int32_t motion_id, int32_t frame) {
    for (int32_t i = ITEM_KAMI; i < ITEM_ITEM16; i++)
        rob_chara_item_equip_object_set_motion_skin_param(&rob_itm_equip->item_equip_object[i], chara_id, motion_id, frame);
}

static void rob_chara_item_equip_set_null_blocks_expression_data(
    rob_chara_item_equip* rob_itm_equip, item_id id, vec3* position, vec3* rotation, vec3* scale) {
    rob_chara_item_equip_object_set_null_blocks_expression_data(
        &rob_itm_equip->item_equip_object[id],
        position,
        rotation,
        scale);
}

static void rob_chara_item_equip_set_object_texture_pattern(rob_chara_item_equip* rob_itm_equip,
    item_id id, texture_pattern_struct* tex_pat, size_t count) {
    rob_chara_item_equip_object_set_texture_pattern(
        &rob_itm_equip->item_equip_object[id], tex_pat, count);
}

static void rob_chara_item_equip_set_opd_blend_data(
    rob_chara_item_equip* rob_itm_equip, std::list<motion_blend_mot*>* a2) {
    rob_itm_equip->opd_blend_data.clear();
    if (!a2)
        return;

    for (motion_blend_mot*& i : *a2) {
        opd_blend_data v11;
        v11.motion_id = i->mot_key_data.motion_id;
        v11.frame = i->mot_play_data.frame_data.frame;
        v11.frame_count = (float_t)i->mot_key_data.mot.frame_count - 1.0f;
        v11.field_C = false;
        v11.type = MOTION_BLEND_NONE;
        v11.blend = 0.0f;

        if (sub_140413810(i)) {
            v11.type = motion_blend_mot_get_type(i);
            v11.field_C = true;
            v11.blend = i->blend->blend;
        }
        rob_itm_equip->opd_blend_data.push_back(v11);
    }

    if (rob_itm_equip->opd_blend_data.size()) {
        opd_blend_data* v9 = &rob_itm_equip->opd_blend_data.back();
        if (v9->field_C && v9->type != MOTION_BLEND_FREEZE)
            v9->field_C = false;
    }

    if (rob_itm_equip->use_opd)
        for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++)
            rob_chara_item_equip_object_check_no_opd(&rob_itm_equip->item_equip_object[i], &rob_itm_equip->opd_blend_data);
}

static void rob_chara_item_equip_set_osage_reset(rob_chara_item_equip* rob_itm_equip) {
    for (int32_t i = rob_itm_equip->first_item_equip_object; i < rob_itm_equip->max_item_equip_object; i++)
        rob_chara_item_equip_object_set_osage_reset(&rob_itm_equip->item_equip_object[i]);

    task_wind.ptr->reset();
}

static void rob_chara_item_equip_set_osage_move_cancel(rob_chara_item_equip* rob_itm_equip,
    uint8_t id, float_t value) {
    if (id == 0)
        for (int32_t i = rob_itm_equip->first_item_equip_object; i < rob_itm_equip->max_item_equip_object; ++i)
            rob_chara_item_equip_object_set_osage_move_cancel(&rob_itm_equip->item_equip_object[i], value);
    else if (id == 1)
        rob_chara_item_equip_object_set_osage_move_cancel(&rob_itm_equip->item_equip_object[ITEM_KAMI], value);
    else if (id == 2)
        rob_chara_item_equip_object_set_osage_move_cancel(&rob_itm_equip->item_equip_object[ITEM_OUTER], value);
}

static void rob_chara_item_equip_set_osage_step(rob_chara_item_equip* rob_itm_equip, float_t value) {
    rob_itm_equip->step = value;
}

static void rob_chara_item_equip_set_shadow_type(rob_chara_item_equip* rob_itm_equip, int32_t chara_id) {
    if (chara_id == 0)
        rob_itm_equip->shadow_type = SHADOW_CHARA;
    else
        rob_itm_equip->shadow_type = SHADOW_STAGE;
}

static void rob_chara_item_equip_set_texture_pattern(rob_chara_item_equip* rob_itm_equip,
    texture_pattern_struct* tex_pat, size_t count) {
    rob_itm_equip->texture_pattern.clear();
    if (count && tex_pat)
        for (size_t i = 0; i < count; i++)
            rob_itm_equip->texture_pattern.push_back(tex_pat[i]);
}

static void rob_chara_add_motion_reset_data(rob_chara* rob_chr,
    int32_t motion_id, float_t frame, int32_t iterations) {
    if (motion_id != -1)
        rob_chara_item_equip_add_motion_reset_data(rob_chr->item_equip,
            motion_id, frame, iterations);
}

static bool rob_chara_check_for_ageageagain_module(chara_index chara_index, int32_t module_index) {
    return chara_index == CHARA_MIKU && module_index == 148;
}

static object_info rob_chara_get_head_object(rob_chara* rob_chr, int32_t head_object_id) {
    if (head_object_id < 0 || head_object_id > 8)
        return {};
    object_info obj_info = rob_chara_item_cos_data_get_head_object_replace(
        &rob_chr->item_cos_data, head_object_id);
    if (obj_info.not_null())
        return obj_info;
    return rob_chr->chara_init_data->head_objects[head_object_id];
}

static object_info rob_chara_get_object_info(rob_chara* rob_chr, item_id id) {
    return rob_chara_item_equip_get_object_info(rob_chr->item_equip, id);
}
static void rob_chara_load_default_motion(rob_chara* rob_chr,
    bone_database* bone_data, motion_database* mot_db) {
    int32_t motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
    rob_chara_load_default_motion_sub(rob_chr, 1, motion_id, bone_data, mot_db);
}

static void rob_chara_bone_data_load_face_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db) {
    sub_14041AD90(rob_bone_data);
    sub_140412F20(&rob_bone_data->face, &rob_bone_data->motion_loaded.front()->bone_data.bones);
    sub_1404146F0(&rob_bone_data->face.field_0);
    mot_blend_load_motion(&rob_bone_data->face, motion_id, mot_db);
}

static void rob_chara_bone_data_load_hand_l_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db) {
    sub_140412F20(&rob_bone_data->hand_l, &rob_bone_data->motion_loaded.front()->bone_data.bones);
    mot_blend_load_motion(&rob_bone_data->hand_l, motion_id, mot_db);
}

static void rob_chara_bone_data_load_hand_r_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db) {
    sub_140412F20(&rob_bone_data->hand_r, &rob_bone_data->motion_loaded.front()->bone_data.bones);
    mot_blend_load_motion(&rob_bone_data->hand_r, motion_id, mot_db);
}

static void rob_chara_bone_data_load_mouth_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db) {
    sub_140412F20(&rob_bone_data->mouth, &rob_bone_data->motion_loaded.front()->bone_data.bones);
    mot_blend_load_motion(&rob_bone_data->mouth, motion_id, mot_db);
}

static void rob_chara_bone_data_load_eyes_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db) {
    sub_140412F20(&rob_bone_data->eyes, &rob_bone_data->motion_loaded.front()->bone_data.bones);
    mot_blend_load_motion(&rob_bone_data->eyes, motion_id, mot_db);
}

static void rob_chara_bone_data_load_eyelid_motion(rob_chara_bone_data* rob_bone_data,
    int32_t motion_id, motion_database* mot_db) {
    sub_14041AD50(rob_bone_data);
    sub_140412F20(&rob_bone_data->eyelid, &rob_bone_data->motion_loaded.front()->bone_data.bones);
    sub_1404146F0(&rob_bone_data->eyelid.field_0);
    mot_blend_load_motion(&rob_bone_data->eyelid, motion_id, mot_db);
}

static void sub_140419820(rob_chara_bone_data* rob_bone_data, int32_t skeleton_select) {
    sub_140412E10(rob_bone_data->motion_loaded.front(), skeleton_select);
}

static void rob_chara_load_default_motion_sub(rob_chara* rob_chr, int32_t skeleton_select,
    int32_t motion_id, bone_database* bone_data, motion_database* mot_db) {
    rob_chara_bone_data_load_face_motion(rob_chr->bone_data, -1, mot_db);
    rob_chara_bone_data_load_hand_l_motion(rob_chr->bone_data, -1, mot_db);
    rob_chara_bone_data_load_hand_r_motion(rob_chr->bone_data, -1, mot_db);
    rob_chara_bone_data_load_mouth_motion(rob_chr->bone_data, -1, mot_db);
    rob_chara_bone_data_load_eyes_motion(rob_chr->bone_data, -1, mot_db);
    rob_chara_bone_data_load_eyelid_motion(rob_chr->bone_data, -1, mot_db);
    rob_chara_bone_data_motion_load(rob_chr->bone_data, motion_id, MOTION_BLEND_FREEZE, bone_data, mot_db);
    rob_chara_bone_data_set_frame(rob_chr->bone_data, 0.0f);
    sub_14041C680(rob_chr->bone_data, false);
    sub_14041C9D0(rob_chr->bone_data, false);
    sub_14041D2D0(rob_chr->bone_data, false);
    sub_14041BC40(rob_chr->bone_data, false);
    sub_14041D270(rob_chr->bone_data, 0.0f);
    sub_14041D2A0(rob_chr->bone_data, 0.0f);
    rob_chara_bone_data_set_rotation_y(rob_chr->bone_data, 0.0f);
    rob_chara_bone_data_set_motion_duration(rob_chr->bone_data, 0.0f, 1.0f, 1.0f);
    sub_14041D310(rob_chr->bone_data, 0.0f, 0.0f, 2);
    rob_chara_bone_data_interpolate(rob_chr->bone_data);
    rob_chara_bone_data_update(rob_chr->bone_data, 0);
    sub_140509D30(rob_chr);
    sub_140419820(rob_chr->bone_data, skeleton_select);
}

static bool rob_chara_data_adjust_compare(rob_chara_data_adjust* left, rob_chara_data_adjust* right) {
    return right->motion_id == left->motion_id
        && left->set_frame == 0.0f
        && right->type == left->type
        && right->cycle_type == left->cycle_type
        && right->ignore_gravity == left->ignore_gravity
        && left->external_force.x == right->external_force.x
        && left->external_force.y == right->external_force.y
        && left->external_force.z == right->external_force.z
        && left->external_force_cycle_strength.x == right->external_force_cycle_strength.x
        && left->external_force_cycle_strength.y == right->external_force_cycle_strength.y
        && left->external_force_cycle_strength.z == right->external_force_cycle_strength.z
        && left->external_force_cycle.x == right->external_force_cycle.x
        && left->external_force_cycle.y == right->external_force_cycle.y
        && left->external_force_cycle.z == right->external_force_cycle.z
        && left->cycle == right->cycle
        && left->phase == right->phase;
}

static void rob_chara_set_adjust(rob_chara* rob_chr, rob_chara_data_adjust* adjust_new,
    rob_chara_data_adjust* adjust, rob_chara_data_adjust* adjust_prev) {
    if (rob_chara_data_adjust_compare(adjust_new, adjust)) {
        adjust_new->frame = adjust->frame;
        adjust_new->transition_frame = adjust->transition_frame;
    }
    *adjust_prev = *adjust;
    *adjust = *adjust_new;
}

static void rob_chara_set_pv_data(rob_chara* rob_chr, int8_t chara_id,
    chara_index chara_index, int32_t module_index, rob_chara_pv_data* pv_data) {
    rob_chr->chara_id = chara_id;
    rob_chr->chara_index = chara_index;
    rob_chr->module_index = module_index;
    rob_chr->pv_data = *pv_data;
    rob_chr->chara_init_data = chara_init_data_get(chara_index);
    if (!check_module_index_is_501(module_index)) {
        item_cos_data* cos_data = item_table_array_get_item_cos_data_by_module_index(chara_index, module_index);
        if (cos_data)
            rob_chara_item_cos_data_set_chara_index_item_nos(&rob_chr->item_cos_data, chara_index, cos_data->arr);
    }
    rob_chara_item_cos_data_set_customize_items(&rob_chr->item_cos_data, &pv_data->customize_items);
}

static void rob_cmn_mottbl_read(void* a1, void* data, size_t size) {
    free(rob_cmn_mottbl_data);

    farc f;
    f.read(data, size, true);
    if (f.files.size()) {
        farc_file* ff = f.read_file("rob_cmn_mottbl.bin");
        if (ff && ff->data) {
            rob_cmn_mottbl_data = (rob_cmn_mottbl_header*)ff->data;
            ff->data = 0;
        }
    }
}

static rob_manager_rob_impl* rob_manager_rob_impls1_get(TaskRobManager* rob_mgr) {
    if (!rob_manager_rob_impls1_init) {
        rob_manager_rob_impls1_init = true;
        rob_manager_rob_impls1[0].task = &rob_mgr->prepare_control;
        rob_manager_rob_impls1[0].name = "ROB_PREPARE_CONTROL";
        rob_manager_rob_impls1[1].task = 0;
        rob_manager_rob_impls1[1].name = "";
    }
    return rob_manager_rob_impls1;
}

static rob_manager_rob_impl* rob_manager_rob_impls2_get(TaskRobManager* rob_mgr) {
    if (!rob_manager_rob_impls2_init) {
        rob_manager_rob_impls2_init = true;
        rob_manager_rob_impls2[0].task = &rob_mgr->prepare_action;
        rob_manager_rob_impls2[0].name = "ROB_PREPARE_ACTION";
        rob_manager_rob_impls2[1].task = &rob_mgr->base;
        rob_manager_rob_impls2[1].name = "ROB_BASE";
        rob_manager_rob_impls2[2].task = &rob_mgr->motion_modifier;
        rob_manager_rob_impls2[2].name = "ROB_MOTION_MODIFIER";
        rob_manager_rob_impls2[3].task = &rob_mgr->collision;
        rob_manager_rob_impls2[3].name = "ROB_COLLISION";
        rob_manager_rob_impls2[4].task = &rob_mgr->info;
        rob_manager_rob_impls2[4].name = "ROB_INFO";
        rob_manager_rob_impls2[5].task = &rob_mgr->disp;
        rob_manager_rob_impls2[5].name = "ROB_DISP";
        rob_manager_rob_impls2[6].task = 0;
        rob_manager_rob_impls2[6].name = "";
    }
    return rob_manager_rob_impls2;
}

static rob_osage_node* rob_osage_get_node(rob_osage* rob_osg,
    size_t index) {
    if (index < rob_osg->nodes.size())
        return &rob_osg->nodes[index];
    else
        return &rob_osg->node;
}

static float_t* rob_osage_load_opd_data(rob_osage* rob_osg,
    size_t node_index, float_t* opd_data, size_t opd_count) {
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++) {
        if (node_index >= i->opd_data.size())
            i->opd_data.resize(node_index + 1);

        opd_vec3_data* opd = &i->opd_data[node_index];
        opd->x = opd_data;
        opd_data += opd_count;
        opd->y = opd_data;
        opd_data += opd_count;
        opd->z = opd_data;
        opd_data += opd_count;
    }
    return opd_data;
}

static void rob_osage_load_skin_param(rob_osage* rob_osg, key_val* kv, const char* name,
    skin_param_osage_root* skp_root, object_info* obj_info, bone_database* bone_data) {
    rob_osg->skin_param_ptr = &rob_osg->skin_param;
    for (rob_osage_node& i : rob_osg->nodes)
        i.data_ptr = &i.data;

    skin_param_osage_root_parse(kv, name, skp_root, bone_data);
    rob_osage_set_skin_param_osage_root(rob_osg, skp_root);

    const osage_setting_osg_cat* osage_setting = osage_setting_data_get_cat_value(obj_info, name);
    if (osage_setting)
        rob_osg->osage_setting = *osage_setting;

    std::vector<skin_param_osage_node> vec;
    vec.resize(rob_osg->nodes.size() - 1);
    skin_param_osage_node_parse(kv, name, &vec, skp_root);
    rob_osage_set_skp_osg_nodes(rob_osg, &vec);
}

static void rob_osage_set_air_res(rob_osage* rob_osg, float_t air_res) {
    rob_osg->skin_param_ptr->air_res = air_res;
}

static void rob_osage_coli_set(rob_osage* rob_osg, mat4* mats) {
    if (rob_osg->skin_param_ptr->coli.size())
        osage_coli_set(rob_osg->coli, rob_osg->skin_param_ptr->coli.data(), mats);
    osage_coli_ring_set(rob_osg->coli_ring, &rob_osg->ring.skp_root_coli, mats);
}

static void rob_osage_set_coli_r(rob_osage* rob_osg, float_t coli_r) {
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++)
        i->data_ptr->skp_osg_node.coli_r = coli_r;
}

static void rob_osage_set_force(rob_osage* rob_osg, float_t force, float_t force_gain) {
    rob_osg->skin_param_ptr->force = force;
    rob_osg->skin_param_ptr->force_gain = force_gain;
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++) {
        i->data_ptr->force = force;
        force = force * force_gain;
    }
}

static void rob_osage_set_hinge(rob_osage* rob_osg, float_t hinge_y, float_t hinge_z) {
    hinge_y = min(hinge_y, 179.0f);
    hinge_z = min(hinge_z, 179.0f);
    hinge_y = hinge_y * DEG_TO_RAD_FLOAT;
    hinge_z = hinge_z * DEG_TO_RAD_FLOAT;
    rob_osage_node* i_begin = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (rob_osage_node* i = i_begin; i != i_end; i++) {
        rob_osage_node_data* data = i->data_ptr;
        data->skp_osg_node.hinge.ymin = -hinge_y;
        data->skp_osg_node.hinge.ymax = hinge_y;
        data->skp_osg_node.hinge.zmin = -hinge_z;
        data->skp_osg_node.hinge.zmax = hinge_z;
    }
}

static void rob_osage_set_init_rot(rob_osage* rob_osg, float_t init_rot_y, float_t init_rot_z) {
    rob_osg->skin_param_ptr->init_rot.y = init_rot_y * DEG_TO_RAD_FLOAT;
    rob_osg->skin_param_ptr->init_rot.z = init_rot_z * DEG_TO_RAD_FLOAT;
}

static void rob_osage_set_motion_reset_data(rob_osage* rob_osg, int32_t motion_id, float_t frame) {
    rob_osg->osage_reset = true;
    auto v6 = rob_osg->motion_reset_data.find({ motion_id, (int32_t)roundf(frame * 1000.0f) });
    if (v6 != rob_osg->motion_reset_data.end() && v6->second.size() + 1 == rob_osg->nodes.size())
        rob_osg->reset_data_list = &v6->second;
}

static void rob_osage_set_rot(rob_osage* rob_osg, float_t rotation_y, float_t rotation_z) {
    rob_osg->skin_param_ptr->rot.y = rotation_y * DEG_TO_RAD_FLOAT;
    rob_osg->skin_param_ptr->rot.z = rotation_z * DEG_TO_RAD_FLOAT;
}

static void rob_osage_set_skin_param_osage_root(rob_osage* rob_osg, skin_param_osage_root* skp_root) {
    rob_osg->skin_param_ptr->reset();
    rob_osage_set_force(rob_osg, skp_root->force, skp_root->force_gain);
    rob_osage_set_air_res(rob_osg, skp_root->air_res);
    rob_osage_set_rot(rob_osg, skp_root->rot_y, skp_root->rot_z);
    rob_osage_set_init_rot(rob_osg, skp_root->init_rot_y, skp_root->init_rot_z);
    rob_osage_set_yz_order(rob_osg, skp_root->yz_order);
    rob_osg->skin_param_ptr->friction = skp_root->friction;
    rob_osg->skin_param_ptr->wind_afc = skp_root->wind_afc;
    rob_osage_set_coli_r(rob_osg, skp_root->coli_r);
    rob_osage_set_hinge(rob_osg, skp_root->hinge_y, skp_root->hinge_z);
    rob_osg->skin_param_ptr->coli = skp_root->coli;
    rob_osg->skin_param_ptr->coli_type = skp_root->coli_type;
    rob_osg->skin_param_ptr->stiffness = skp_root->stiffness;
    rob_osg->skin_param_ptr->move_cancel = skp_root->move_cancel;
}

static void rob_osage_set_skp_osg_nodes(rob_osage* rob_osg,
    std::vector<skin_param_osage_node>* skp_osg_nodes) {
    if (!skp_osg_nodes)
        return;

    if (rob_osg->nodes.size() - 1 != skp_osg_nodes->size())
        return;

    rob_osage_node* i = rob_osg->nodes.data() + 1;
    rob_osage_node* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    skin_param_osage_node* j = skp_osg_nodes->data();
    skin_param_osage_node* j_end = skp_osg_nodes->data() + skp_osg_nodes->size();
    while (i != i_end && j != j_end) {
        i->data_ptr->skp_osg_node = *j++;
        i->data_ptr->skp_osg_node.hinge.limit();
        i++;
    }
}

static void rob_osage_set_yz_order(rob_osage* rob_osg, int32_t yz_order) {
    rob_osg->yz_order = yz_order;
}

static void rob_osage_free(rob_osage* rob_osg) {
    rob_osg->reset();
}

static void skin_param_osage_node_parse(key_val* kv, const char* name,
    std::vector<skin_param_osage_node>* vec, skin_param_osage_root* skp_root) {
    char buf[SKP_TEXT_BUF_SIZE];
    int32_t count;
    size_t len;
    size_t len1;
    size_t len2;
    size_t off;

    key_val lkv;
    if (!kv->get_local_key_val(name, &lkv))
        return;

    len = utf8_length(name);
    memcpy(buf, name, len);
    off = len;

    len1 = 5;
    memcpy(&buf[len], ".node", 5);
    off = len + len1;

    buf[off] = 0;
    key_val sub_local_key_val;
    if (lkv.get_local_key_val(buf, &sub_local_key_val)) {
        if (lkv.read_int32_t(buf, off, ".length", 8, &count)) {
            size_t c = vec->size();
            if (c < count)
                vec->resize(count);
        }
        else
            vec->clear();

        int32_t j = 0;
        for (auto i = vec->begin(); i != vec->end(); i++, j++) {
            len2 = sprintf_s(buf + len + len1,
                SKP_TEXT_BUF_SIZE - len - len1, ".%d", j);
            off = len + len1 + len2;

            *i = skin_param_osage_node();

            float_t coli_r = 0;
            if (sub_local_key_val.read_float_t(
                buf, off, ".coli_r", 8, &coli_r))
                i->coli_r = coli_r;

            float_t weight = 0;
            if (sub_local_key_val.read_float_t(
                buf, off, ".weight", 8, &weight))
                i->weight = weight;

            float_t inertial_cancel = 0;
            if (sub_local_key_val.read_float_t(
                buf, off, ".inertial_cancel", 17, &inertial_cancel))
                i->inertial_cancel = inertial_cancel;

            i->hinge.ymin = -skp_root->hinge_y;
            i->hinge.ymax = skp_root->hinge_y;
            i->hinge.zmin = -skp_root->hinge_z;
            i->hinge.zmax = skp_root->hinge_z;

            float_t hinge_ymax = 0.0f;
            if (sub_local_key_val.read_float_t(
                buf, off, ".hinge_ymax", 12, &hinge_ymax))
                i->hinge.ymax = hinge_ymax;

            float_t hinge_ymin = 0.0f;
            if (sub_local_key_val.read_float_t(
                buf, off, ".hinge_ymin", 12, &hinge_ymin))
                i->hinge.ymin = hinge_ymin;

            float_t hinge_zmax = 0.0f;
            if (sub_local_key_val.read_float_t(
                buf, off, ".hinge_zmax", 12, &hinge_zmax))
                i->hinge.zmax = hinge_zmax;

            float_t hinge_zmin = 0.0f;
            if (sub_local_key_val.read_float_t(
                buf, off, ".hinge_zmin", 12, &hinge_zmin))
                i->hinge.zmin = hinge_zmin;
        }
    }
}

static void skin_param_osage_root_parse(key_val* kv, const char* name,
    skin_param_osage_root* skp_root, bone_database* bone_data) {
    char buf[SKP_TEXT_BUF_SIZE];
    int32_t count;
    size_t len;
    size_t len1;
    size_t len2;
    size_t len3;
    size_t off;

    key_val lkv;
    if (!kv->get_local_key_val(name, &lkv))
        return;

    len = utf8_length(name);
    memcpy(buf, name, len);
    off = len;

    int32_t node_length = 0;
    lkv.read_int32_t(
        buf, off, ".node.length", 13, &node_length);

    len1 = 5;
    memcpy(&buf[len], ".root", 5);
    off = len + len1;

    float_t force = 0.0f;
    float_t force_gain = 0.0f;
    if (lkv.read_float_t(
        buf, off, ".force", 7, &force)
        && lkv.read_float_t(
            buf, off, ".force_gain", 12, &force_gain)) {
        skp_root->force = force;
        skp_root->force_gain = force_gain;
    }

    float_t air_res = 0.0f;
    if (lkv.read_float_t(
        buf, off, ".air_res", 9, &air_res))
        skp_root->air_res = min(air_res, 0.9f);

    float_t rot_y = 0.0f;
    float_t rot_z = 0.0f;
    if (lkv.read_float_t(
        buf, off, ".rot_y", 7, &rot_y)
        && lkv.read_float_t(
            buf, off, ".rot_z", 7, &rot_z)) {
        skp_root->rot_y = rot_y;
        skp_root->rot_z = rot_z;
    }

    float_t friction = 0.0f;
    if (lkv.read_float_t(
        buf, off, ".friction", 10, &friction))
        skp_root->friction = friction;

    float_t wind_afc = 0;
    if (lkv.read_float_t(
        buf, off, ".wind_afc", 10, &wind_afc)) {
        skp_root->wind_afc = wind_afc;
        if (!str_utils_compare_length(name, utf8_length(name), "c_opa", 5))
            skp_root->wind_afc = 0.0f;
    }

    int32_t coli_type = 0;
    if (lkv.read_int32_t(
        buf, off, ".coli_type", 11, &coli_type))
        skp_root->coli_type = coli_type;

    float_t init_rot_y = 0.0f;
    float_t init_rot_z = 0.0f;
    if (lkv.read_float_t(
        buf, off, ".init_rot_y", 12, &init_rot_y)
        && lkv.read_float_t(
            buf, off, ".init_rot_z", 12, &init_rot_z)) {
        skp_root->init_rot_y = init_rot_y;
        skp_root->init_rot_z = init_rot_z;
    }

    float_t hinge_y = 0.0f;
    float_t hinge_z = 0.0f;
    if (lkv.read_float_t(
        buf, off, ".hinge_y", 9, &hinge_y)
        && lkv.read_float_t(
            buf, off, ".hinge_z", 9, &hinge_z)) {
        skp_root->hinge_y = hinge_y;
        skp_root->hinge_z = hinge_z;
    }

    float_t coli_r = 0.0f;
    if (lkv.read_float_t(
        buf, off, ".coli_r", 8, &coli_r))
        skp_root->coli_r = coli_r;

    float_t stiffness = 0.0f;
    if (lkv.read_float_t(
        buf, off, ".stiffness", 11, &stiffness))
        skp_root->stiffness = stiffness;

    float_t move_cancel = 0.0f;
    if (lkv.read_float_t(
        buf, off, ".move_cancel", 13, &move_cancel))
        skp_root->move_cancel = move_cancel;

    len2 = 5;
    memcpy(buf + len + len1, ".coli", 5);
    off = len + len1 + len2;

    buf[off] = 0;
    key_val sub_local_key_val;
    if (lkv.read_int32_t(buf, off, ".length", 8, &count)
        && lkv.get_local_key_val(buf, &sub_local_key_val)) {
        std::vector<skin_param_osage_root_coli>& vc = skp_root->coli;
        for (int32_t i = 0; i < count; i++) {
            skin_param_osage_root_coli* c = &vc[i];
            len3 = sprintf_s(buf + len + len1 + len2,
                SKP_TEXT_BUF_SIZE - len - len1 - len2, ".%d", i);
            off = len + len1 + len2 + len3;

            skin_param_osage_root_coli_type type = SKIN_PARAM_OSAGE_ROOT_COLI_TYPE_NONE;
            if (!sub_local_key_val.read_int32_t(
                buf, off, ".type", 6, (int32_t*)&type))
                break;
            c->type = type;

            float_t radius = 0;
            if (!sub_local_key_val.read_float_t(
                buf, off, ".radius", 8, &radius))
                break;
            c->radius = radius;

            const char* bone0_name = 0;
            if (!sub_local_key_val.read_string(
                buf, off, ".bone.0.name", 13, &bone0_name))
                break;

            c->bone0_index = bone_data->get_skeleton_object_bone_index(
                bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone0_name);

            vec3 bone0_pos = vec3_null;
            if (!sub_local_key_val.read_float_t(
                buf, off, ".bone.0.posx", 13, &bone0_pos.x))
                break;

            if (!sub_local_key_val.read_float_t(
                buf, off, ".bone.0.posy", 13, &bone0_pos.y))
                break;

            if (!sub_local_key_val.read_float_t(
                buf, off, ".bone.0.posz", 13, &bone0_pos.z))
                break;

            c->bone0_pos = bone0_pos;

            const char* bone1_name = 0;
            if (sub_local_key_val.read_string(
                buf, off, ".bone.1.name", 13, &bone1_name)) {

                c->bone1_index = bone_data->get_skeleton_object_bone_index(
                    bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), bone1_name);

                vec3 bone1_pos = vec3_null;
                if (!sub_local_key_val.read_float_t(
                    buf, off, ".bone.1.posx", 13, &bone1_pos.x))
                    break;

                if (!sub_local_key_val.read_float_t(
                    buf, off, ".bone.1.posy", 13, &bone1_pos.y))
                    break;

                if (!sub_local_key_val.read_float_t(
                    buf, off, ".bone.1.posz", 13, &bone1_pos.z))
                    break;

                c->bone1_pos = bone1_pos;

            }
        }
    }

    len2 = 4;
    memcpy(buf + len + len1, ".boc", 4);
    off = len + len1 + len2;

    buf[off] = 0;
    if (lkv.read_int32_t(buf, off, ".length", 8, &count)
        && lkv.get_local_key_val(buf, &sub_local_key_val)) {
        std::vector<skin_param_osage_root_boc>* vb = &skp_root->boc;

        vb->resize(count);
        for (int32_t i = 0; i < count; i++) {
            len3 = sprintf_s(buf + len + len1 + len2,
                SKP_TEXT_BUF_SIZE - len - len1 - len2, ".%d", i);
            off = len + len1 + len2 + len3;

            int32_t st_node = 0;
            std::string ed_root;
            int32_t ed_node = 0;
            if (sub_local_key_val.read_int32_t(
                buf, off, ".st_node", 9, &st_node)
                && st_node < node_length
                && sub_local_key_val.read_string(
                    buf, off, ".ed_root", 9, &ed_root)
                && sub_local_key_val.read_int32_t(
                    buf, off, ".ed_node", 9, &ed_node)
                && ed_node < node_length) {
                skin_param_osage_root_boc b;
                b.st_node = st_node;
                b.ed_root = ed_root;
                b.ed_node = ed_node;
                vb->push_back(b);
            }
        }
    }

    off = len + len1;
    std::string colli_tgt_osg;
    if (lkv.read_string(
        buf, off, ".colli_tgt_osg", 15, &colli_tgt_osg))
        skp_root->colli_tgt_osg = colli_tgt_osg;

    len2 = 11;
    memcpy(buf + len + len1, ".normal_ref", 11);
    off = len + len1 + len2;

    buf[off] = 0;
    if (lkv.read_int32_t(buf, off, ".length", 8, &count)
        && lkv.get_local_key_val(buf, &sub_local_key_val)) {
        std::vector<skin_param_osage_root_normal_ref>* vnr = &skp_root->normal_ref;

        vnr->resize(count);
        for (int32_t i = 0; i < count; i++) {
            len3 = sprintf_s(buf + len + len1 + len2,
                SKP_TEXT_BUF_SIZE - len - len1 - len2, ".%d", i);
            off = len + len1 + len2 + len3;

            std::string n;
            if (sub_local_key_val.read_string(
                buf, off, ".N", 3, &n)) {
                skin_param_osage_root_normal_ref nr;
                nr.n = n;
                sub_local_key_val.read_string(
                    buf, off, ".U", 3, &nr.u);
                sub_local_key_val.read_string(
                    buf, off, ".D", 3, &nr.d);
                sub_local_key_val.read_string(
                    buf, off, ".L", 3, &nr.l);
                sub_local_key_val.read_string(
                    buf, off, ".R", 3, &nr.r);
                vnr->push_back(nr);
            }
        }
    }
}

static bool task_rob_load_check_load_req_data() {
    if (task_rob_load.field_F0 == 2 && !task_rob_load.load_req_data_obj.size())
        return !!task_rob_load.load_req_data.size();
    return true;
}

rob_chara* rob_chara_array_get(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return 0;

    if (rob_chara_pv_data_array_check_chara_id(chara_id))
        return &rob_chara_array[chara_id];
    return 0;
}

int32_t rob_chara_array_init_chara_index(chara_index chara_index,
    rob_chara_pv_data* pv_data, int32_t module_index, bool can_set_default) {
    if (!TaskWork::CheckTaskReady(&task_rob_manager)
        || pv_data->type < ROB_CHARA_TYPE_0|| pv_data->type > ROB_CHARA_TYPE_3)
        return -1;

    int32_t chara_id = 0;
    while (rob_chara_pv_data_array[chara_id].type != ROB_CHARA_TYPE_NONE) {
        chara_id++;
        if (chara_id >= ROB_CHARA_COUNT)
            return -1;
    }

    if (can_set_default && (module_index < 0 || module_index > 498))
        module_index = 0;
    rob_chara_pv_data_array[chara_id] = *pv_data;
    rob_chara_set_pv_data(&rob_chara_array[chara_id], chara_id, chara_index, module_index, pv_data);
    task_rob_manager.AppendInitCharaList(&rob_chara_array[chara_id]);
    return chara_id;
}

void rob_chara_array_free_chara_id(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    task_rob_manager.AppendFreeCharaList(&rob_chara_array[chara_id]);
    rob_chara_pv_data_array[chara_id].type = ROB_CHARA_TYPE_NONE;
}

bool pv_osage_manager_array_ptr_get_disp() {
    bool disp = false;
    for (int32_t i = 0; i < CHARA_MAX; i++)
        disp |= pv_osage_manager_array_ptr_get(i)->GetDisp();
    return disp;
}

int32_t rob_chara_array_reset_pv_data(int32_t chara_id) {
    rob_chara_pv_data_array[chara_id].type = ROB_CHARA_TYPE_NONE;
    return chara_id;
}

inline bool rob_chara_pv_data_array_check_chara_id(int32_t chara_id) {
    return rob_chara_pv_data_array[chara_id].type != ROB_CHARA_TYPE_NONE;
}

int32_t rob_chara::get_rob_cmn_mottbl_motion_id(int32_t id) {
    if (id >= 214 && id <= 223)
        return pv_data.motion_face_ids[id - 214];
    else if (id == 224)
        return data.motion.motion_id;
    else if (id >= 226 && id <= 235)
        return pv_data.motion_face_ids[id - 226];

    int32_t v2 = data.field_8.field_1A4;
    if (rob_cmn_mottbl_data
        && chara_index >= CHARA_MIKU
        && chara_index >= 0 && chara_index < rob_cmn_mottbl_data->chara_count
        && id >= 0 && id < rob_cmn_mottbl_data->mottbl_indices_count) {
        rob_cmn_mottbl_sub_header* v4 = (rob_cmn_mottbl_sub_header*)((uint8_t*)rob_cmn_mottbl_data
            + rob_cmn_mottbl_data->subheaders_offset);
        if (v2 >= 0 && v2 < v4[chara_index].field_4)
            return ((int32_t*)((uint8_t*)rob_cmn_mottbl_data
                + *(int32_t*)((uint8_t*)rob_cmn_mottbl_data
                    + v2 * sizeof(int32_t) + v4[chara_index].data_offset)))[id];
    }
    return -1;
}

void task_rob_manager_append_task() {
    TaskWork::AppendTask(&task_rob_manager, "ROB_MANAGER TASK");
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        rob_chara_pv_data_array[i].type = ROB_CHARA_TYPE_NONE;
}

bool task_rob_manager_check_chara_loaded(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return false;
    return task_rob_manager.CheckCharaLoaded(&rob_chara_array[chara_id]);
}

void task_rob_manager_free_task() {
    task_rob_manager.SetDest();
}

bone_node_expression_data::bone_node_expression_data() {
    position = vec3_null;
    rotation = vec3_null;
    scale = vec3_identity;
    parent_scale = vec3_identity;
}

bone_node::bone_node() : name(), mat(), parent(), ex_data_mat() {

}

mot_key_data::mot_key_data() : key_sets_ready(), key_set_count(), key_set(),
mot(), key_set_data(), mot_data(), skeleton_type(), skeleton_select(), field_68() {
    motion_id = -1;
    frame = -1.0f;
}

mot_key_data::~mot_key_data() {

}

bone_data::bone_data() : type(), has_parent(), motion_bone_index(), mirror(), parent(),
flags(), key_set_offset(), key_set_count(), frame(), base_translation(), rotation(),
ik_target(), trans(), rot_mat(), trans_prev(), rot_mat_prev(), pole_target_mat(),
parent_mat(), node(), ik_segment_length(), ik_2nd_segment_length(), arm_length() {
    eyes_xrot_adjust_neg = 1.0f;
    eyes_xrot_adjust_pos = 1.0f;
}

bone_data_parent::bone_data_parent() : rob_bone_data(),
motion_bone_count(), ik_bone_count(), chain_pos(), global_trans(),
global_rotation(), bone_key_set_count(), global_key_set_count(), rotation_y() {

}

bone_data_parent::~bone_data_parent() {

}

void mot_play_frame_data::reset() {
    frame = 0.0f;
    step = 1.0f;
    step_prev = 1.0f;
    frame_count = 0;
    last_frame = -1.0f;
    field_14 = -1.0f;
    field_18 = -1;
    field_1C = -1;
    field_20 = 0;
    field_24 = -1.0f;
    field_28 = 0;
    field_2C = -1;
}

void mot_play_data::reset() {
    frame_data.reset();
    field_30 = 0;
    field_34 = 0;
    field_38 = 0;
    field_40 = 0;
    field_48 = 0;
}

MotionBlend::MotionBlend() : field_8(), field_9(),
duration(), frame(), step(1.0f), offset(), blend() {

}

MotionBlend::~MotionBlend() {

}

void MotionBlend::Reset() {
    field_8 = false;
    field_9 = false;
    duration = 0.0f;
    frame = 0.0f;
    step = 1.0f;
    offset = 0.0f;
    blend = 0.0f;
}

bool MotionBlend::Field_30() {
    return false;
}

void MotionBlend::SetDuration(float_t duration, float_t step, float_t offset) {
    this->step = step;
    this->offset = offset;
    if (duration < 0.0f) {
        this->duration = 0.0f;
        field_8 = false;
    }
    else {
        this->duration = duration;
        field_8 = true;
    }
    frame = 0.0f;
}

MotionBlendCross::MotionBlendCross() : field_20(), field_21(),
field_24(mat4u_identity), field_64(mat4u_identity), field_A4(mat4u_identity), field_E4(mat4u_identity) {

}

MotionBlendCross::~MotionBlendCross() {

}

void MotionBlendCross::Reset() {
    MotionBlend::Reset();
    field_20 = false;
    field_21 = false;
    field_24 = mat4u_identity;
    field_64 = mat4u_identity;
    field_A4 = mat4u_identity;
    field_E4 = mat4u_identity;
}

void MotionBlendCross::Field_10(float_t a2, float_t a3, int32_t a4) {

}

void MotionBlendCross::Field_18(struc_400* a2) {
    field_20 = a2->field_0;
    field_21 = true;
    if (duration <= 0.0f || duration <= frame || fabsf(duration - frame) <= 0.000001f) {
        field_8 = false;
        field_9 = false;
    }
    else {
        field_9 = true;
        blend = (frame + offset) / (duration + offset);
        frame += step;
    }

    if (field_9) {
        mat4 mat;
        mat4_rotate_y(-a2->prev_eyes_adjust, &mat);
        field_24 = mat;
    }
    else
        field_24 = mat4u_identity;
}

void MotionBlendCross::Field_20(std::vector<bone_data>* a2, std::vector<bone_data>* a3) {
    if (!a2 || !a3)
        return;

    bone_data* v4 = a2->data();
    bone_data* v7 = a3->data();

    mat4_mult(&v4[4].rot_mat[0], &v4[0].rot_mat[0], &field_64);
    mat4_mult(&v7[0].rot_mat[0], &field_24, &field_A4);
    mat4_blend_rotation(&v4[0].rot_mat[0], &field_A4, &v4[0].rot_mat[0], blend);
    mat4_mult(&v7[4].rot_mat[0], &field_A4, &field_A4);
    mat4_blend_rotation(&v4[4].rot_mat[0], &v7[4].rot_mat[0], &v4[4].rot_mat[0], blend);
    mat4_mult(&v4[4].rot_mat[0], &v4[0].rot_mat[0], &field_E4);
    mat4_transpose(&field_E4, &field_E4);
}

void MotionBlendCross::Blend(bone_data* a2, bone_data* a3) {
    if (!a2 || !a3 || a2->flags)
        return;

    mat4u v15;
    mat4u v16;

    switch (a2->type) {
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
        vec3_lerp_scalar(a3->trans, a2->trans, a2->trans, blend);
        break;
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        if (field_21) {
            a2->trans.x = lerp(a3->trans.x, a2->trans.x, blend);
            a2->trans.z = lerp(a3->trans.z, a2->trans.z, blend);
        }
        if (field_20)
            a2->trans.y = lerp(a3->trans.y, a2->trans.y, blend);
        break;
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        if (a2->motion_bone_index == MOTION_BONE_CL_MUNE) {
            mat4_mult(&a3->rot_mat[0], &field_A4, &v15);
            mat4_mult(&a3->rot_mat[1], &v15, &v15);
            mat4_mult(&a2->rot_mat[0], &field_64, &v16);
            mat4_mult(&a2->rot_mat[1], &v16, &v16);
            mat4_blend_rotation(&v16, &v15, &v16, blend);
            mat4_mult(&v16, &field_E4, &v16);
        }
        else {
            mat4_mult(&a3->rot_mat[1], &a3->rot_mat[0], &v15);
            mat4_mult(&a2->rot_mat[1], &a2->rot_mat[0], &v16);
            mat4_blend_rotation(&v16, &v15, &v16, blend);
        }
        mat4_transpose(&a2->rot_mat[0], &v15);
        mat4_mult(&v16, &v15, &a2->rot_mat[1]);
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        mat4_blend_rotation(&a2->rot_mat[2], &a3->rot_mat[2], &a2->rot_mat[2], blend);
        mat4_mult(&a3->rot_mat[1], &a3->rot_mat[0], &v15);
        mat4_mult(&a2->rot_mat[1], &a2->rot_mat[0], &v16);
        mat4_blend_rotation(&v16, &v15, &v16, blend);
        mat4_transpose(&a2->rot_mat[0], &v15);
        mat4_mult(&v16, &v15, &a2->rot_mat[1]);
        break;
    default:
        if (a2->motion_bone_index == MOTION_BONE_KL_KOSI_Y) {
            mat4_mult(&a3->rot_mat[0], &field_A4, &v16);
            mat4_mult(&a2->rot_mat[0], &field_64, &v15);
            mat4_blend_rotation(&v15, &v16, &v15, blend);
            mat4_mult(&v15, &field_E4, &a2->rot_mat[0]);
        }
        else if (a2->motion_bone_index > MOTION_BONE_N_HARA)
            mat4_blend_rotation(&a2->rot_mat[0], &a3->rot_mat[0], &a2->rot_mat[0], blend);
        break;
    }
}

MotionBlendCombine::MotionBlendCombine() {

}

MotionBlendCombine::~MotionBlendCombine() {

}

void MotionBlendCombine::Field_18(struc_400* a2) {
    field_20 = a2->field_0;
    field_21 = true;
    if (field_9) {
        mat4 mat;
        mat4_rotate_y(-a2->prev_eyes_adjust, &mat);
        field_24 = mat;
    }
    else
        field_24 = mat4u_identity;
}

bool MotionBlendCombine::Field_30() {
    return true;
}

MotionBlendFreeze::MotionBlendFreeze() : field_20(),field_21(), field_24(), field_28(), field_2C(), field_30(),
field_34(mat4u_identity), field_74(mat4u_identity), field_B4(mat4u_identity), field_F4(mat4u_identity) {

}

MotionBlendFreeze::~MotionBlendFreeze() {

}

void MotionBlendFreeze::Reset() {
    MotionBlend::Reset();
    field_20 = false;
    field_21 = false;
    field_24 = 2;
    field_28 = 0.0f;
    field_2C = 0.0f;
    field_30 = 2;
    field_34 = mat4u_identity;
    field_74 = mat4u_identity;
    field_B4 = mat4u_identity;
    field_F4 = mat4u_identity;
}

void MotionBlendFreeze::Field_10(float_t a2, float_t a3, int32_t a4) {
    field_2C = a3;
    field_30 = a4;
    if (a2 < 0.0f)
        field_28 = 0.0f;
    else
        field_28 = a2;
}

void MotionBlendFreeze::Field_18(struc_400* a2) {
    field_24 = 2;
    if (duration <= 0.0f || duration <= frame || fabsf(duration - frame) <= 0.000001f) {
        float_t v7 = a2->frame;
        float_t v8 = field_2C;
        float_t v9 = v7 - v8;
        if (v7 < v8 || field_28 <= 0.0f || field_28 <= v9 || fabsf(field_28 - v9) <= 0.000001f) {
            field_8 = false;
            field_9 = false;
        }
        else {
            field_9 = true;
            field_24 = field_30;
            blend = 1.0f - (v9 + offset) / (field_28 + offset);
            if (a2->field_4)
                field_24 = field_30 + 1;
            field_20 = a2->field_2;
            field_21 = a2->field_3;
        }
    }
    else {
        field_9 = true;
        field_24 = 0;
        blend = (frame + offset) / (duration + offset);
        frame += step;
        field_20 = a2->field_0;
        field_21 = a2->field_1;
    }

    if (field_9) {
        mat4 mat;
        if (field_24)
            mat4_rotate_y(a2->eyes_adjust, &mat);
        else
            mat4_rotate_y(-a2->prev_eyes_adjust, &mat);
        field_34 = mat;
    }
    else
        field_34 = mat4u_identity;
}

void MotionBlendFreeze::Field_20(std::vector<bone_data>* a2, std::vector<bone_data>* a3) {
    if (!a2)
        return;

    bone_data* v2 = a2->data();
    mat4_mult(&v2[4].rot_mat[0], &v2->rot_mat[0], &field_74);
    mat4_mult(&v2->rot_mat_prev[0][field_24], &field_34, &field_B4);
    mat4_blend_rotation(v2->rot_mat, &field_B4, v2->rot_mat, blend);
    mat4_mult(&v2[4].rot_mat_prev[0][field_24], &field_B4, &field_B4);
    mat4_blend_rotation(&v2[4].rot_mat[0], &v2[4].rot_mat_prev[0][field_24], &v2[4].rot_mat[0], blend);
    mat4_mult(&v2[4].rot_mat[0], &v2->rot_mat[0], &field_F4);
    mat4_transpose(&field_F4, &field_F4);
}

void MotionBlendFreeze::Blend(bone_data* a2, bone_data* a3) {
    if (!a2 || a2->flags)
        return;

    mat4u v15;
    mat4u v16;
    switch (a2->type) {
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
        vec3_lerp_scalar(a2->trans_prev[field_24], a2->trans, a2->trans, blend);
        break;
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        if (field_21) {
            a2->trans.x = lerp(a2->trans_prev[field_24].x, a2->trans.x, blend);
            a2->trans.z = lerp(a2->trans_prev[field_24].z, a2->trans.z, blend);
        }
        if (field_20)
            a2->trans.y = lerp(a2->trans_prev[field_24].y, a2->trans.y, blend);
        break;
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        if (a2->motion_bone_index == MOTION_BONE_CL_MUNE) {
            mat4_mult(&a2->rot_mat_prev[0][field_24], &field_B4, &v15);
            mat4_mult(&a2->rot_mat_prev[1][field_24], &v15, &v15);
            mat4_mult(&a2->rot_mat[0], &field_74, &v16);
            mat4_mult(&a2->rot_mat[1], &v16, &v16);
            mat4_blend_rotation(&v16, &v15, &v16, blend);
            mat4_mult(&v16, &field_F4, &v16);
        }
        else {
            mat4_mult(&a2->rot_mat_prev[1][field_24], &a2->rot_mat_prev[0][field_24], &v15);
            mat4_mult(&a2->rot_mat[1], &a2->rot_mat[0], &v16);
            mat4_blend_rotation(&v16, &v15, &v16, blend);
        }
        mat4_transpose(&a2->rot_mat[0], &v15);
        mat4_mult(&v16, &v15, &a2->rot_mat[1]);
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        mat4_blend_rotation(&a2->rot_mat[2], &a2->rot_mat_prev[2][field_24], &a2->rot_mat[2], blend);
        mat4_mult(&a2->rot_mat_prev[1][field_24], &a2->rot_mat_prev[0][field_24], &v15);
        mat4_mult(&a2->rot_mat[1], &a2->rot_mat[0], &v16);
        mat4_blend_rotation(&v16, &v15, &v16, blend);
        mat4_transpose(&a2->rot_mat[0], &v15);
        mat4_mult(&v16, &v15, &a2->rot_mat[1]);
        break;
    default:
        if (a2->motion_bone_index == MOTION_BONE_KL_KOSI_Y) {
            mat4_mult(&a2->rot_mat_prev[0][field_24], &field_B4, &v16);
            mat4_mult(&a2->rot_mat[0], &field_74, &v15);
            mat4_blend_rotation(&v15, &v16, &v15, blend);
            mat4_mult(&v15, &field_F4, &a2->rot_mat[0]);
        }
        else if (a2->motion_bone_index > MOTION_BONE_N_HARA)
            mat4_blend_rotation(a2->rot_mat, &a2->rot_mat_prev[0][field_24], a2->rot_mat, blend);
        break;
    }
}

PartialMotionBlendFreeze::PartialMotionBlendFreeze() {

}

PartialMotionBlendFreeze::~PartialMotionBlendFreeze() {

}

void PartialMotionBlendFreeze::Reset() {
    MotionBlendFreeze::Reset();
}

void PartialMotionBlendFreeze::Field_10(float_t a2, float_t a3, int32_t a4) {

}

void PartialMotionBlendFreeze::Field_18(struc_400* a1) {
    if (duration <= 0.0f || duration <= frame || fabsf(duration - frame) <= 0.000001f) {
        field_8 = false;
        field_9 = false;
    }
    else {
        field_9 = true;
        frame += step;
        blend = (offset + frame) / (offset + duration);
    }
}

void PartialMotionBlendFreeze::Field_20(std::vector<bone_data>* a1, std::vector<bone_data>* a2) {

}

void PartialMotionBlendFreeze::Blend(bone_data* a2, bone_data* a3) {
    if (!a2 || a2->flags)
        return;

    float_t inv_blend = 1.0f - blend;
    switch (a2->type) {
    case BONE_DATABASE_BONE_POSITION:
    case BONE_DATABASE_BONE_TYPE_1:
        vec3_lerp_scalar(a2->trans_prev[0], a2->trans, a2->trans, blend);
        break;
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        mat4_lerp_rotation(&a2->rot_mat_prev[0][0], &a2->rot_mat[0], &a2->rot_mat[0], blend);
        vec3_lerp_scalar(a2->trans_prev[0], a2->trans, a2->trans, blend);
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        mat4_lerp_rotation(&a2->rot_mat[2], &a2->rot_mat_prev[2][0], &a2->rot_mat[2], blend);
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION: {
        mat4u v15;
        mat4u v16;
        mat4u v17;
        mat4_mult(&a2->rot_mat_prev[1][0], &a2->rot_mat_prev[0][0], &v16);
        mat4_mult(&a2->rot_mat[1], &a2->rot_mat[0], &v15);
        mat4_lerp_rotation(&v16, &v15, &v15, blend);
        mat4_transpose(&a2->rot_mat[0], &v17);
        mat4_mult(&a2->rot_mat[1], &v17, &v15);
    } break;
    default:
        mat4_lerp_rotation(&a2->rot_mat_prev[0][0], &a2->rot_mat[0], &a2->rot_mat[0], blend);
        break;
    }
}

struc_313::struc_313() : motion_bone_count() {
    bitfield.reserve(16);
}

struc_313::~struc_313() {

}

struc_240::struc_240() : bone_check_func(), motion_bone_count() {

}

struc_240::~struc_240() {

}

motion_blend_mot::motion_blend_mot() : bone_data(),
mot_key_data(), mot_play_data(), field_4F8(), field_5CC(), blend() {
    mot_play_data.reset();
    field_4F8.field_C0 = 1.0f;
    field_4F8.field_C4 = 1.0f;
}

motion_blend_mot::~motion_blend_mot() {

}

void motion_blend_mot::reset() {
    mot_play_data.reset();
    field_4F8.field_0 = 0;
    field_4F8.field_8 = 0;
    field_4F8.mat = mat4u_identity;
    field_4F8.field_4C = mat4u_identity;
    field_4F8.field_8C = false;
    field_4F8.eyes_adjust = 0.0f;
    field_4F8.prev_eyes_adjust = 0.0f;
    field_4F8.field_90 = vec3_identity;
    field_4F8.field_9C = vec3_identity;
    field_4F8.field_A8 = vec3_identity;
    field_4F8.field_BC = 0;
    field_4F8.field_BD = 0;
    field_4F8.field_C0 = 1.0f;
    field_4F8.field_C4 = 1.0f;
    field_4F8.field_C8 = vec3_null;
}

rob_chara_bone_data_ik_scale::rob_chara_bone_data_ik_scale() {
    ratio0 = 1.0f;
    ratio1 = 1.0f;
    ratio2 = 1.0f;
    ratio3 = 1.0f;
}

mot_blend::mot_blend() : field_30(), mot_key_data(), mot_play_data() {

}

mot_blend::~mot_blend() {

}

void mot_blend::reset() {
    mot_play_data.reset();
    blend.Reset();
    field_0.bone_check_func = 0;
    sub_1404119A0(&field_0.field_8);
    field_0.motion_bone_count = 0;
    field_30 = 0;
}

rob_chara_bone_data::rob_chara_bone_data() : field_0(), field_1(), object_bone_count(),
total_bone_count(), motion_bone_count(), ik_bone_count(), chain_pos(),
disable_eye_motion(), field_76C(), field_784(), field_788(), field_958() {
    base_skeleton_type = BONE_DATABASE_SKELETON_NONE;
    skeleton_type = BONE_DATABASE_SKELETON_NONE;
}

rob_chara_bone_data::~rob_chara_bone_data() {
    for (motion_blend_mot*& i : motions) {
        delete i;
        i = 0;
    }
}

void rob_chara_bone_data::reset() {
    object_bone_count = 0;
    motion_bone_count = 0;
    total_bone_count = 0;
    ik_bone_count = 0;
    chain_pos = 0;
    mats.clear();
    mats2.clear();
    nodes.clear();
    base_skeleton_type = BONE_DATABASE_SKELETON_NONE;
    skeleton_type = BONE_DATABASE_SKELETON_NONE;

    if (motions.size() == 3) {
        rob_chara_bone_data_motion_blend_mot_list_free(this, 0);
        for (motion_blend_mot*& i : motions)
            i->reset();
        rob_chara_bone_data_motion_blend_mot_list_init(this);
    }
    else {
        rob_chara_bone_data_motion_blend_mot_free(this);
        rob_chara_bone_data_motion_blend_mot_init(this);
    }

    face.reset();
    hand_l.reset();
    hand_r.reset();
    mouth.reset();
    eyes.reset();
    eyelid.reset();
    disable_eye_motion = false;
    ik_scale = rob_chara_bone_data_ik_scale();
    field_76C[0] = vec3_null;
    field_76C[0] = vec3_null;
    sub_1404198D0(&field_788);
    sub_1403FAEF0(&field_958);
}

rob_chara_pv_data::rob_chara_pv_data() {
    type = ROB_CHARA_TYPE_2;
    field_4 = true;
    field_5 = false;
    field_6 = false;
    field_8 = vec3_null;
    rot_y_int16 = 0x00;
    field_16 = 0xC9;
    field_18 = {};
    field_44 = {};
    field_70 = 0;
    motion_face_ids[0] = -1;
    motion_face_ids[1] = -1;
    motion_face_ids[2] = -1;
    motion_face_ids[3] = -1;
    motion_face_ids[4] = -1;
    motion_face_ids[5] = -1;
    motion_face_ids[6] = -1;
    motion_face_ids[7] = -1;
    motion_face_ids[8] = -1;
    motion_face_ids[9] = -1;
    chara_size_index = 1;
    height_adjust = false;
    customize_items = {};
    eyes_adjust.base_adjust = EYES_BASE_ADJUST_DIRECTION;
    eyes_adjust.neg = -1.0f;
    eyes_adjust.pos = -1.0f;
}

rob_chara_pv_data::~rob_chara_pv_data() {

}

ExNodeBlock::ExNodeBlock() : bone_node_ptr(), type(), name(), parent_bone_node(),
parent_name(), parent_node(), item_equip_object(), field_58(), field_59(), has_children_node() {

}

ExNodeBlock::~ExNodeBlock() {

}

void ExNodeBlock::Field_10() {
    field_59 = false;
}

void ExNodeBlock::Reset() {
    bone_node_ptr = 0;
}

void ExNodeBlock::Field_58() {
    field_59 = false;
}

void ExNodeBlock::InitData(bone_node* bone_node, ex_node_type type,
    const char* name, rob_chara_item_equip_object* itm_eq_obj) {
    bone_node_ptr = bone_node;
    this->type = type;
    if (name)
        this->name = name;
    else
        this->name = "(null)";
    parent_bone_node = 0;
    item_equip_object = itm_eq_obj;
}

ExNullBlock::ExNullBlock() {
    Init();
}

ExNullBlock::~ExNullBlock() {

}

void ExNullBlock::Init() {
    bone_node_ptr = 0;
    type = EX_NONE;
    name = 0;
    parent_bone_node = 0;
    item_equip_object = 0;
    cns_data = 0;
}

void ExNullBlock::Field_10() {
    field_59 = false;
}

void ExNullBlock::Field_18(int32_t stage, bool a3) {

}

void ExNullBlock::Field_20() {
    if (!bone_node_ptr)
        return;

    mat4 mat;
    mat4 ex_data_mat = *parent_bone_node->ex_data_mat;
    bone_node_expression_data_mat_set(&bone_node_ptr->exp_data,
        &parent_bone_node->exp_data.parent_scale, &ex_data_mat, &mat);
    *bone_node_ptr->ex_data_mat = ex_data_mat;
    *bone_node_ptr->mat = mat;
}

void ExNullBlock::SetOsagePlayData() {
    Field_20();
}

void ExNullBlock::Disp() {

}

void ExNullBlock::Reset() {
    bone_node_ptr = 0;
}

void ExNullBlock::Field_40() {

}

void ExNullBlock::Field_48() {
    Field_20();
}

void ExNullBlock::Field_50() {

}

void ExNullBlock::Field_58() {
    field_59 = false;
}

void ExNullBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_constraint* cns_data, const char* cns_data_name, bone_database* bone_data) {
    bone_node* node = rob_chara_item_equip_object_get_bone_node_by_name(
        itm_eq_obj, cns_data_name, bone_data);
    type = EX_NONE;
    bone_node_ptr = node;
    this->cns_data = cns_data;
    name = node->name;
    item_equip_object = itm_eq_obj;
}

ExConstraintBlock::ExConstraintBlock() : constraint_type(),
source_node_bone_node(), direction_up_vector_bone_node(), cns_data() {
    Init();
}

ExConstraintBlock::~ExConstraintBlock() {

}

void ExConstraintBlock::Init() {
    constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
    source_node_bone_node = 0;
    direction_up_vector_bone_node = 0;
    cns_data = 0;
    field_80 = 0;
}

void ExConstraintBlock::Field_10() {
    if (bone_node_ptr) {
        bone_node_expression_data* node_exp_data = &bone_node_ptr->exp_data;
        node_exp_data->position = cns_data->base.position;
        node_exp_data->rotation = cns_data->base.rotation;
        node_exp_data->scale = cns_data->base.scale;
    }
    field_59 = false;
}

void ExConstraintBlock::Field_18(int32_t stage, bool a3) {
    if (field_59)
        return;

    if (stage == 0) {
        if (field_58)
            Field_20();
    }
    else if (stage == 2) {
        if (has_children_node)
            DataSet();
    }
    else if (stage == 5)
        Field_20();
}

void ExConstraintBlock::Field_20() {
    if (!parent_bone_node)
        return;

    if (field_59) {
        field_59 = false;
        return;
    }

    Calc();
    DataSet();
    field_59 = true;
}

void ExConstraintBlock::SetOsagePlayData() {
    Field_20();
}

void ExConstraintBlock::Disp() {

}

void ExConstraintBlock::Reset() {
    bone_node_ptr = 0;
}

void ExConstraintBlock::Field_40() {

}

void ExConstraintBlock::Field_48() {
    Field_20();
}

void ExConstraintBlock::Field_50() {
    Field_20();
}

void ExConstraintBlock::Field_58() {
    field_59 = false;
}

void ExConstraintBlock::Calc() {
    bone_node* node = bone_node_ptr;
    if (!node)
        return;

    vec3 pos;
    vec3_mult(parent_bone_node->exp_data.parent_scale, node->exp_data.position, pos);

    mat4 mat;
    mat4_translate_mult(parent_bone_node->ex_data_mat, pos.x, pos.y, pos.z, &mat);

    switch (constraint_type) {
    case OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION: {
        vec3 trans;
        mat4_get_translation(&mat, &trans);

        mat3 rot;
        mat3_from_mat4(source_node_bone_node->mat, &rot);
        mat3_normalize_rotation(&rot, &rot);

        vec3 offset = cns_data->orientation.offset;
        mat3_rotate_mult(&rot, offset.x, offset.y, offset.z, &rot);

        mat4_from_mat3(&rot, node->mat);
        mat4_set_translation(node->mat, &trans);
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION: {
        vec3 align_axis = cns_data->direction.align_axis;
        vec3 target_offset = cns_data->direction.target_offset;
        mat4_mult_vec3_trans(source_node_bone_node->mat, &target_offset, &target_offset);
        mat4_mult_vec3_inv_trans(&mat, &target_offset, &target_offset);
        float_t target_offset_length;
        vec3_length_squared(target_offset, target_offset_length);
        if (target_offset_length <= 0.000001f)
            break;

        mat4 v59;
        sub_1401EB410(&v59, &align_axis, &target_offset);
        if (direction_up_vector_bone_node) {
            vec3 affected_axis = cns_data->direction.up_vector.affected_axis;
            mat4 v56;
            mat4_mult(&v59, &mat, &v56);
            mat4_mult_vec3(&v56, &affected_axis, &affected_axis);
            mat4* v20 = direction_up_vector_bone_node->mat;

            vec3 v23;
            vec3 v24;
            mat4_get_translation(v20, &v23);
            mat4_get_translation(&v56, &v24);
            vec3_sub(v23, v24, v23);

            vec3 v50;
            mat4_mult_vec3(&mat, &target_offset, &v50);

            vec3 v25;
            vec3 v29;
            vec3_cross(v50, affected_axis, v25);
            vec3_cross(v50, v23, v29);
            vec3_normalize(v25, v25);
            vec3_normalize(v29, v29);

            vec3 v35;
            vec3_cross(v25, v29, v35);

            float_t v39;
            float_t v36;
            vec3_dot(v29, v25, v39);
            vec3_dot(v35, v50, v36);

            float_t v40;
            vec3_length(v35, v40);
            if (v36 >= 0.0f)
                v40 = -v40;

            sub_1405F10D0(&v56, &target_offset, v39, v40);
            mat4_mult(&v59, &v56, &v59);
        }
        mat4_mult(&v59, &mat, node->mat);
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_POSITION: {
        vec3 constraining_offset = cns_data->position.constraining_object.offset;
        vec3 constrained_offset = cns_data->position.constrained_object.offset;
        if (cns_data->position.constraining_object.affected_by_orientation)
            mat4_mult_vec3(source_node_bone_node->mat, &constraining_offset, &constraining_offset);

        vec3 source_node_trans;
        mat4_get_translation(source_node_bone_node->mat, &source_node_trans);
        vec3_add(constraining_offset, source_node_trans, source_node_trans);
        mat4_set_translation(&mat, &source_node_trans);
        if (direction_up_vector_bone_node) {
            vec3 up_vector_trans;
            mat4_get_translation(direction_up_vector_bone_node->mat, &up_vector_trans);
            mat4_mult_vec3_inv_trans(&mat, &up_vector_trans, &up_vector_trans);

            mat4 v26;
            sub_1401EB410(&v26, &cns_data->position.up_vector.affected_axis, &up_vector_trans);
            mat4_mult(&v26, &mat, &mat);
        }
        if (cns_data->position.constrained_object.affected_by_orientation)
            mat4_mult_vec3(&mat, &constrained_offset, &constrained_offset);

        mat4 constrained_offset_mat;
        mat4_translate(constrained_offset.x, constrained_offset.y,
            constrained_offset.z, &constrained_offset_mat);
        mat4_mult(&mat, &constrained_offset_mat, node->mat);
    } break;
    case OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE:
    default:
        *node->mat = mat;
        break;
    }
}

void ExConstraintBlock::DataSet() {
    if (!bone_node_ptr)
        return;

    bone_node_expression_data* exp_data = &bone_node_ptr->exp_data;
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;

    mat4 mat;
    mat4_inverse_normalized(parent_bone_node->ex_data_mat, &mat);
    mat4_mult(bone_node_ptr->mat, &mat, &mat);
    mat4_get_rotation(&mat, &bone_node_ptr->exp_data.rotation);
    mat4_get_translation(&mat, &exp_data->position);
    if (fabsf(parent_scale.x) > 0.000001f)
        exp_data->position.x /= parent_scale.x;
    if (fabsf(parent_scale.y) > 0.000001f)
        exp_data->position.y /= parent_scale.z;
    if (fabsf(parent_scale.z) > 0.000001f)
        exp_data->position.z /= parent_scale.z;
    *bone_node_ptr->ex_data_mat = *bone_node_ptr->mat;
    mat4_scale_rot(bone_node_ptr->mat, parent_scale.x, parent_scale.y, parent_scale.z, bone_node_ptr->mat);
    vec3_mult(exp_data->scale, parent_scale, exp_data->parent_scale);
}

void ExConstraintBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_constraint* cns_data, const char* cns_data_name, bone_database* bone_data) {
    bone_node* node = rob_chara_item_equip_object_get_bone_node_by_name(
        itm_eq_obj, cns_data_name, bone_data);
    type = EX_CONSTRAINT;
    bone_node_ptr = node;
    this->cns_data = cns_data;
    name = node->name;
    item_equip_object = itm_eq_obj;

    source_node_bone_node = rob_chara_item_equip_object_get_bone_node_by_name(
        itm_eq_obj, string_data(&cns_data->source_node_name), bone_data);

    obj_skin_block_constraint_type type = cns_data->type;
    char* up_vector_name;
    if (type == OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION) {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_DIRECTION;
        up_vector_name = string_data(&cns_data->direction.up_vector.name);
    }
    else if (type == OBJ_SKIN_BLOCK_CONSTRAINT_POSITION) {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_POSITION;
        up_vector_name = string_data(&cns_data->position.up_vector.name);
    }
    else if (type == OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE) {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_DISTANCE;
        up_vector_name = string_data(&cns_data->distance.up_vector.name);
    }
    else if (type == OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION) {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_ORIENTATION;
        return;
    }
    else {
        constraint_type = OBJ_SKIN_BLOCK_CONSTRAINT_NONE;
        return;
    }

    if (up_vector_name)
        direction_up_vector_bone_node = rob_chara_item_equip_object_get_bone_node_by_name(
            itm_eq_obj, up_vector_name, bone_data);
}

void ExConstraintBlock::sub_1405F10D0(mat4* mat, vec3* a2, float_t a3, float_t a4) {
    vec3 v5;
    vec3 v6;
    vec3 v7;
    vec3 v8;
    vec3 v9;
    vec3_normalize(*a2, v5);
    vec3_mult_scalar(v5, 1.0f - a3, v9);
    vec3_mult_scalar(v5, v9.x, v6);
    vec3_mult_scalar(v5, v9.y, v7);
    vec3_mult_scalar(v5, v9.z, v8);
    mat->row0.x = v6.x + a3;
    mat->row0.y = v6.y - v5.z * a4;
    mat->row0.z = v6.z + v5.y * a4;
    mat->row0.w = 0.0f;
    mat->row1.x = v7.x + v5.z * a4;
    mat->row1.y = v7.y + a3;
    mat->row1.z = v7.z - v5.x * a4;
    mat->row1.w = 0.0f;
    mat->row2.x = v8.x - v5.y * a4;
    mat->row2.y = v8.y + v5.x * a4;
    mat->row2.z = v8.z + a3;
    mat->row2.w = 0.0f;
    mat->row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
}

void ExConstraintBlock::sub_1401EB410(mat4* mat, vec3* a2, vec3* target_offset) {
    vec3 v3;
    vec3 v4;
    vec3_normalize(*a2, v3);
    vec3_normalize(*target_offset, v4);
    vec3 v13;
    vec3_cross(v4, v3, v13);

    float_t v18;
    vec3_dot(v3, v4, v18);
    v18 = clamp(v18, -1.0f, 1.0f);
    float_t v19 = 1.0f - v18;

    float_t v20 = 1.0f - v18 * v18;
    v20 = sqrtf(clamp(v20, 0.0f, 1.0f));

    sub_1405F10D0(mat, &v13, v18, v20);
}

ExExpressionBlock::ExExpressionBlock() : values(), types(), expressions(),
stack_data(), exp_data(), field_3D20(), field_3D28(), frame(), step(true) {

}

ExExpressionBlock::~ExExpressionBlock() {

}

void ExExpressionBlock::Init() {
    frame = 0.0f;
}

void ExExpressionBlock::Field_10() {
    bone_node_expression_data* node_exp_data = &bone_node_ptr->exp_data;
    node_exp_data->position = exp_data->base.position;
    node_exp_data->rotation = exp_data->base.rotation;
    node_exp_data->scale = exp_data->base.scale;
    field_59 = false;
}

void ExExpressionBlock::Field_18(int32_t stage, bool a3) {
    if (field_59)
        return;

    if (stage == 0) {
        if (field_58)
            Field_20();
    }
    else if (stage == 2) {
        if (has_children_node)
            DataSet();
    }
    else if (stage == 5)
        Field_20();
}

void ExExpressionBlock::Field_20() {
    if (!parent_bone_node)
        return;

    if (field_59) {
        field_59 = false;
        return;
    }

    Calc();
    DataSet();
    field_59 = true;
}

void ExExpressionBlock::SetOsagePlayData() {
    Field_20();
}

void ExExpressionBlock::Disp() {

}

void ExExpressionBlock::Reset() {
    bone_node_ptr = 0;
}

void ExExpressionBlock::Field_40() {

}

void ExExpressionBlock::Field_48() {
    Field_20();
}

void ExExpressionBlock::Field_50() {
    Field_20();
}

void ExExpressionBlock::Field_58() {
    field_59 = false;
}

void ExExpressionBlock::Calc() {
    float_t delta_frame = get_delta_frame();
    if (step)
        delta_frame *= item_equip_object->item_equip->step;
    float_t frame = this->frame + delta_frame;
    this->frame = frame >= 65535.0f ? frame - 65535.0f : frame;

    if (field_3D28) {
        field_3D28(&bone_node_ptr->exp_data);
        return;
    }

    for (int32_t i = 0; i < 9; i++) {
        if (!expressions[i])
            break;

        float_t value = ex_expression_block_stack_get_value(expressions[i]);
        if (types[i] == EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN)
            value *= DEG_TO_RAD_FLOAT;
        *values[i] = value;
    }
}

void ExExpressionBlock::DataSet() {
    bone_node_expression_data* data = &bone_node_ptr->exp_data;
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    mat4 ex_data_mat = *parent_bone_node->ex_data_mat;
    mat4 mat = mat4_identity;
    bone_node_expression_data_mat_set(data, &parent_scale, &ex_data_mat, &mat);
    *bone_node_ptr->mat = mat;
    *bone_node_ptr->ex_data_mat = ex_data_mat;
}

void ExExpressionBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_expression* exp_data, const char* exp_data_name, object_info a4,
    size_t index, bone_database* bone_data) {
    ex_expression_block_stack* stack_buf[28];
    ex_expression_block_stack** stack_buf_val = stack_buf;

    bone_node* node = rob_chara_item_equip_object_get_bone_node_by_name(
        itm_eq_obj, exp_data_name, bone_data);
    type = EX_EXPRESSION;
    bone_node_ptr = node;
    this->exp_data = exp_data;
    name = node->name;
    item_equip_object = itm_eq_obj;

    node->exp_data.position = exp_data->base.position;
    node->exp_data.rotation = exp_data->base.rotation;
    node->exp_data.scale = exp_data->base.scale;
    field_3D28 = 0;

    ex_expression_block_stack* stack_val = stack_data;
    for (int32_t i = 0; i < 9; i++) {
        values[i] = 0;
        expressions[i] = 0;
    }

    for (int32_t i = 0; i < 9; i++) {
        const char* expression = string_data(&exp_data->expressions[i]);
        if (!expression || str_utils_compare_length(expression, exp_data->expressions[i].length, "= ", 2))
            break;

        expression += 2;

        int32_t index = 0;
        expression = str_utils_get_next_int32_t(expression, &index, ' ');
        values[i] = bone_node_get_exp_data_component(bone_node_ptr, index, &types[i]);

        while (expression) {
            string v73;
            expression = str_utils_get_next_string(expression, &v73, ' ');
            if (!v73.length || !memcmp(string_data(&v73), "error", min(v73.length, 5)) && v73.length == 5) {
                string_free(&v73);
                break;
            }

            if (*string_data(&v73) == 'n') {
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_NUMBER;
                expression = str_utils_get_next_float_t(expression, &stack_val->number.value, ' ');
                *stack_buf_val++ = stack_val;
            }
            else if (*string_data(&v73) == 'v') {
                string func_str;
                expression = str_utils_get_next_string(expression, &func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
                int32_t v40 = *string_data(&func_str) - '0';
                if (v40 < 9) {
                    bone_node* v42 = rob_chara_item_equip_object_get_bone_node_by_name(itm_eq_obj,
                        string_data(&func_str) + 2, bone_data);
                    if (v42)
                        stack_val->var.value = bone_node_get_exp_data_component(v42, v40, &stack_val->type);
                    else {
                        stack_val->type = EX_EXPRESSION_BLOCK_STACK_NUMBER;
                        stack_val->number.value = 0.0f;
                    }
                }
                else
                    stack_val->var.value = &frame;
                *stack_buf_val++ = stack_val;
                string_free(&func_str);
            }
            else if (*string_data(&v73) == 'f') {
                string func_str;
                expression = str_utils_get_next_string(expression, &func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_OP1;
                stack_val->op1.v1 = stack_buf_val[-1];
                stack_val->op1.func = exp_func_op1_find_func(&func_str, exp_func_op1_array)->func;
                stack_buf_val[-1] = stack_val;
                string_free(&func_str);
            }
            else if (*string_data(&v73) == 'g') {
                string func_str;
                expression = str_utils_get_next_string(expression, &func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_OP2;
                stack_val->op2.v1 = stack_buf_val[-2];
                stack_val->op2.v2 = stack_buf_val[-1];
                stack_val->op2.func = exp_func_op2_find_func(&func_str, exp_func_op2_array)->func;
                stack_buf_val[-2] = stack_val;
                stack_buf_val--;
                string_free(&func_str);
            }
            else if (*string_data(&v73) == 'h') {
                string func_str;
                expression = str_utils_get_next_string(expression, &func_str, ' ');
                stack_val->type = EX_EXPRESSION_BLOCK_STACK_OP3;
                stack_val->op3.v1 = stack_buf_val[-3];
                stack_val->op3.v2 = stack_buf_val[-2];
                stack_val->op3.v3 = stack_buf_val[-1];
                stack_val->op3.func = exp_func_op3_find_func(&func_str, exp_func_op3_array)->func;
                stack_buf_val[-3] = stack_val;
                stack_buf_val -= 2;
                string_free(&func_str);
            }
            string_free(&v73);
            expressions[i] = stack_buf_val[-1];
            stack_val++;
        }
    }

    step = !((ssize_t)index - ITEM_TE_R <= 1);
}

ExOsageBlock::ExOsageBlock() : rob(), mats(), step(1.0f) {
    Reset();
}

ExOsageBlock::~ExOsageBlock() {
    Reset();
    rob_osage_free(&rob);
}

void ExOsageBlock::Init() {
    Reset();
}

void ExOsageBlock::Field_10() {
    field_59 = false;
}

void ExOsageBlock::Field_18(int32_t stage, bool a3) {
    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data[0].field_C)
        step = 1.0f;

    mat4* parent_node_mat = parent_bone_node->ex_data_mat;
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    switch (stage) {
    case 0:
        sub_1404803B0(&rob, parent_node_mat, &parent_scale, has_children_node);
        break;
    case 1:
    case 2:
        if ((stage == 1 && field_58) || (stage == 2 && rob.field_2A0)) {
            SetWindDirection();
            sub_14047C800(&rob, parent_node_mat, &parent_scale, step, a3, true, has_children_node);
        }
        break;
    case 3:
        rob_osage_coli_set(&rob, mats);
        sub_14047ECA0(&rob, step);
        break;
    case 4:
        sub_14047D620(&rob, step);
        break;
    case 5: {
        sub_14047D8C0(&rob, parent_node_mat, &parent_scale, step, false);
        sub_140480260(&rob, parent_node_mat, &parent_scale, step, a3);
        field_59 = true;
    } break;
    }
}

void ExOsageBlock::Field_20() {
    field_1FF8 &= ~2;
    if (field_59) {
        field_59 = false;
        return;
    }

    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data[0].field_C)
        step = 1.0f;

    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    vec3 scale = parent_bone_node->exp_data.scale;

    mat4 mat = *parent_bone_node->ex_data_mat;
    if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f) {
        vec3_div(vec3_identity, scale, scale);
        mat4_scale_rot(&mat, scale.x, scale.y, scale.z, &mat);
    }
    SetWindDirection();
    rob_osage_coli_set(&rob, mats);
    sub_14047C750(&rob, &mat, &parent_scale, step);
}

void ExOsageBlock::SetOsagePlayData() {
    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    sub_14047E240(&rob, parent_bone_node->ex_data_mat, &parent_scale, &rob_itm_equip->opd_blend_data);
}

void ExOsageBlock::Disp() {

}

void ExOsageBlock::Reset() {
    index = 0;
    rob.reset();
    field_1FF8 &= ~3;
    mats = 0;
    step = 1.0f;
    ExNodeBlock::Reset();
}

void ExOsageBlock::Field_40() {

}

void ExOsageBlock::Field_48() {
    step = 4.0f;
    SetWindDirection();
    rob_osage_coli_set(&rob, mats);
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    sub_14047F990(&rob, parent_bone_node->ex_data_mat, &parent_scale, 0);
    field_1FF8 &= ~2;
}

void ExOsageBlock::Field_50() {
    if (field_59) {
        field_59 = false;
        return;
    }

    SetWindDirection();

    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    rob_osage_coli_set(&rob, mats);
    sub_14047C770(&rob, parent_bone_node->ex_data_mat, &parent_scale, step, 1);
    float_t step = 0.5f * this->step;
    this->step = max(step, 1.0f);
}

void ExOsageBlock::Field_58() {
    vec3 parent_scale = parent_bone_node->exp_data.parent_scale;
    sub_14047E1C0(&rob, &parent_scale);
    field_59 = false;
}

void ExOsageBlock::AddMotionResetData(int32_t motion_id, float_t frame) {
    rob_osage_add_motion_reset_data(&rob, motion_id, frame);
}

void ExOsageBlock::InitData(rob_chara_item_equip_object* itm_eq_obj,
    obj_skin_block_osage* osg_data, const char* osg_data_name, obj_skin_osage_node* osg_nodes,
    bone_node* bone_nodes, bone_node* ex_data_bone_nodes, obj_skin* skin) {
    ExNodeBlock::InitData(&ex_data_bone_nodes[osg_data->external_name_index & 0x7FFF],
        EX_OSAGE, osg_data_name, itm_eq_obj);
    rob_osage_init_data(&rob, osg_data, osg_nodes, ex_data_bone_nodes, skin);
    field_1FF8 &= ~2;
    mats = bone_nodes->mat;
}

float_t* ExOsageBlock::LoadOpdData(size_t node_index, float_t* opd_data, size_t opd_count) {
    return rob_osage_load_opd_data(&rob, node_index, opd_data, opd_count);
}

void ExOsageBlock::SetMotionResetData(int32_t motion_id, float_t frame) {
    rob_osage_set_motion_reset_data(&rob, motion_id, frame);
}

void ExOsageBlock::SetOsageReset() {
    rob.osage_reset = true;
}

void ExOsageBlock::SetSkinParam(struc_571* skp) {
    if (skp->nodes_data.size() == rob.nodes.size() - 1) {
        size_t node_index = 0;
        rob_osage_node* i_begin = rob.nodes.data() + 1;
        rob_osage_node* i_end = rob.nodes.data() + rob.nodes.size();
        for (rob_osage_node* i = i_begin; i != i_end; i++)
            i->data_ptr = &skp->nodes_data[node_index++];
    }
    else {
        rob.skin_param_ptr = &rob.skin_param;
        for (rob_osage_node& i : rob.nodes)
            i.data_ptr = &i.data;
    }
}

void ExOsageBlock::SetWindDirection() {
    vec3_mult_scalar(task_wind.ptr->wind_direction,
        item_equip_object->item_equip->wind_strength,
        rob.wind_direction);
}

void ExOsageBlock::sub_1405F3E10(obj_skin_block_osage* osg_data,
    obj_skin_osage_node* osg_nodes, std::vector<std::pair<uint32_t, rob_osage_node*>>* a4,
    std::map<const char*, ExNodeBlock*>* a5) {
    rob_osage_node* v9 = rob_osage_get_node(&rob, 0);
    a4->push_back({ osg_data->external_name_index, v9 });

    for (uint32_t i = 0; i < osg_data->count; i++) {
        v9 = rob_osage_get_node(&rob, i);
        a4->push_back({ osg_nodes[i].name_index, v9 });

        if (v9->bone_node_ptr && v9->bone_node_ptr->name)
            a5->insert({ v9->bone_node_ptr->name, this });
    }

    v9 = rob_osage_get_node(&rob, osg_data->count + 1ULL);
    a4->push_back({ osg_data->name_index, v9 });

    if (v9->bone_node_ptr && v9->bone_node_ptr->name)
        a5->insert({ v9->bone_node_ptr->name, this });
}

ExClothBlock::ExClothBlock() {
    Init();
}

ExClothBlock::~ExClothBlock() {

}

void ExClothBlock::Init() {
    //rob.base.field_48(&cls->rob);
    cls_data = 0;
    field_2428 = 0;
    index = 0;
}

void ExClothBlock::Field_10() {
    field_59 = false;
}

void ExClothBlock::Field_18(int32_t stage, bool a3) {

}

void ExClothBlock::Field_20() {
    //sub_14021FD00(&rob, field_2428);
    //rob.__vftable->set_wind_direction(&rob, wind_task_struct_get_wind_direction());
    rob_chara_item_equip* rob_itm_equip = item_equip_object->item_equip;
    float_t step = get_delta_frame() * rob_itm_equip->step;
    if (rob_itm_equip->opd_blend_data.size() && rob_itm_equip->opd_blend_data[0].field_C)
        step = 1.0f;
    //sub_140218560(&rob, step, 0);
}

void ExClothBlock::SetOsagePlayData() {
    //sub_140218E40(&cls->rob, &cls->base.item_equip_object->item_equip->opd_blend_data);
}

void ExClothBlock::Disp() {

}

void ExClothBlock::Reset() {
    bone_node_ptr = 0;
}

void ExClothBlock::Field_40() {

}

void ExClothBlock::Field_48() {
    //sub_14021FD00(&rob, field_2428);
    //rob.__vftable->set_wind_direction(&rob, wind_task_struct_get_wind_direction());
    //sub_14021D480(&rob);
}

void ExClothBlock::Field_50() {
    //sub_14021FD00(&rob, field_2428);
    //rob.__vftable->set_wind_direction(&rob, wind_task_struct_get_wind_direction());
    //sub_14021D840(&rob);
}

void ExClothBlock::Field_58() {
    field_59 = false;
}

void ExClothBlock::AddMotionResetData(int32_t motion_id, float_t frame) {
    //rob_cloth_add_motion_reset_data(&rob, motion_id, frame);
}

float_t* ExClothBlock::LoadOpdData(size_t node_index, float_t* opd_data, size_t opd_count) {
    return opd_data;//rob_cloth_load_opd_data(&rob, node_index, opd_data, opd_count);
}

void ExClothBlock::SetMotionResetData(int32_t motion_id, float_t frame) {
    //rob_cloth::set_motion_reset_data(&rob, motion_id, frame);
}

void ExClothBlock::SetOsageReset() {
    //rob.osage_reset = true;
}

void ExClothBlock::SetSkinParam(struc_571* skp) {
    //rob.skin_param_ptr = &skp->skin_param;
}

skin_param_hinge::skin_param_hinge() {
    ymin = -90.0f;
    ymax = 90.0f;
    zmin = -90.0f;
    zmax = 90.0f;
}

skin_param_hinge::~skin_param_hinge() {

}

void skin_param_hinge::limit() {
    ymin = max(ymin, -179.0f) * DEG_TO_RAD_FLOAT;
    ymax = min(ymax, 179.0f) * DEG_TO_RAD_FLOAT;
    zmin = max(zmin, -179.0f) * DEG_TO_RAD_FLOAT;
    zmax = min(zmax, 179.0f) * DEG_TO_RAD_FLOAT;
}

skin_param_osage_node::skin_param_osage_node() : coli_r(0.0f), weight(1.0f), inertial_cancel(0.0f) {
    hinge.limit();
}

skin_param_osage_node::~skin_param_osage_node() {

}

rob_osage_node_data::rob_osage_node_data() : force(), normal_ref() {

}

rob_osage_node_data::~rob_osage_node_data() {

}

void rob_osage_node_data::reset() {
    force = 0.0f;
    boc.clear();
    normal_ref.field_0 = 0;
    normal_ref.n = 0;
    normal_ref.u = 0;
    normal_ref.d = 0;
    normal_ref.l = 0;
    normal_ref.r = 0;
    normal_ref.mat = mat4u_identity;
    skp_osg_node.weight = 1.0f;
    skp_osg_node.inertial_cancel = 0.0f;
    skp_osg_node.coli_r = 0.0f;
    skp_osg_node.hinge = skin_param_hinge();
    skp_osg_node.hinge.limit();
}

rob_osage_node::rob_osage_node() : length(), trans(), trans_orig(), trans_diff(), field_28(),
child_length(), bone_node_ptr(), bone_node_mat(), sibling_node(), max_distance(), field_94(),
reset_data(), field_C8(), field_CC(1.0f), external_force(), force(1.0f), mat(), field_1B0() {
    data_ptr = &data;
    opd_data.resize(3);
}

rob_osage_node::~rob_osage_node() {

}

void rob_osage_node::reset() {
    length = 0.0f;
    trans = vec3_null;
    trans_orig = vec3_null;
    trans_diff = vec3_null;
    field_28 = vec3_null;
    child_length = 0.0f;
    bone_node_ptr = 0;
    bone_node_mat = 0;
    sibling_node = 0;
    max_distance = 0.0f;
    field_94 = vec3_null;
    reset_data.trans = vec3_null;
    reset_data.trans_diff = vec3_null;
    reset_data.rotation = vec3_null;
    reset_data.length = 0.0f;
    field_C8 = 0.0f;
    field_CC = 1.0f;
    external_force = vec3_null;
    force = 1.0f;
    data.reset();
    data_ptr = &data;
    opd_data.clear();
    opd_data.resize(3);
    mat = mat4u_null;
}

skin_param_osage_root_coli::skin_param_osage_root_coli() : type(),
bone0_index(), bone1_index(), radius(0.2f), bone0_pos(), bone1_pos() {

}

skin_param_osage_root_coli::~skin_param_osage_root_coli() {

}

skin_param_osage_root_normal_ref::skin_param_osage_root_normal_ref() {

}

skin_param_osage_root_normal_ref::~skin_param_osage_root_normal_ref() {

}

skin_param_osage_root_boc::skin_param_osage_root_boc() : ed_node(), st_node() {

}

skin_param_osage_root_boc::~skin_param_osage_root_boc() {

}

skin_param_osage_root::skin_param_osage_root() : field_0(), force(), force_gain(), air_res(0.5f),
rot_y(), rot_z(), init_rot_y(), init_rot_z(), hinge_y(90.0f), hinge_z(90.0f), name("NO-PARAM"),
coli_r(), friction(1.0f), wind_afc(0.5f), yz_order(), coli_type(), stiffness(), move_cancel(-0.01f) {
    coli.resize(13);
}

skin_param_osage_root::~skin_param_osage_root() {

}

skin_param::skin_param() : friction(1.0f), wind_afc(), air_res(1.0f), rot(), init_rot(),
coli_type(), stiffness(), move_cancel(-0.01f), coli_r(), force(), force_gain(), colli_tgt_osg() {
    hinge.limit();
}

skin_param::~skin_param() {

}

void skin_param::reset() {
    coli.clear();
    friction = 1.0f;
    wind_afc = 0.0f;
    air_res = 1.0f;
    rot = vec3_null;
    init_rot = vec3_null;
    coli_type = 0;
    stiffness = 0.0f;
    move_cancel = -0.01f;
    coli_r = 0.0f;
    hinge = skin_param_hinge();
    hinge.limit();
    force = 0.0f;
    force_gain = 0.0f;
    colli_tgt_osg = 0;
}

struc_571::struc_571() : field_88() {

}

struc_571::~struc_571() {

}

osage_coli::osage_coli() : type(), radius(), bone0_pos(), bone1_pos(),
bone_pos_diff(), bone_pos_diff_length(), bone_pos_diff_length_squared(), field_34() {

}

osage_coli::~osage_coli() {

}

osage_ring_data::osage_ring_data() : ring_rectangle_x(0.0f), ring_rectangle_y(0.0f), ring_rectangle_width(0.0f),
ring_rectangle_height(0.0f), ring_height(-1000.0f), ring_out_height(-1000.0f), field_18() {

}

osage_ring_data::~osage_ring_data() {

}

osage_setting_osg_cat::osage_setting_osg_cat() : exf() {
    parts = ROB_OSAGE_PARTS_NONE;
}

rob_osage::rob_osage() : skin_param_ptr(), osage_setting(), field_2A0(), field_2A1(), field_2A4(),
wind_direction(), field_1EB4(), yz_order(), field_1EBC(), parent_mat_ptr(), parent_mat(), move_cancel(),
field_1F0C(), osage_reset(), field_1F0E(), field_1F0F(), set_external_force(), external_force(), reset_data_list() {

}

rob_osage::~rob_osage() {

}

void rob_osage::reset() {
    nodes.clear();
    node.reset();
    wind_direction = vec3_null;
    field_1EB4 = 0.0f;
    yz_order = 0;
    field_2A0 = true;
    motion_reset_data.clear();
    move_cancel = 0.0f;
    field_1F0C = false;
    osage_reset = false;
    field_1F0E = false;
    ring = osage_ring_data();
    field_1F0F = 0;
    skin_param.reset();
    skin_param_ptr = &skin_param;
    osage_setting = osage_setting_osg_cat();
    reset_data_list = 0;
    field_2A4 = 0.0f;
    field_2A1 = false;
    set_external_force = false;
    external_force = vec3_null;
    parent_mat_ptr = 0;
    parent_mat = mat4u_null;
}

rob_chara_item_equip_object::rob_chara_item_equip_object() : index(), mats(),
obj_info(), field_14(), texture_data(), null_blocks_data_set(), alpha(),
draw_task_flags(), disp(), field_A4(), mat(), osage_iterations(), bone_nodes(),
field_138(), field_1B8(), field_1C0(), use_opd(), skin_ex_data(), skin(), item_equip() {
    rob_chara_item_equip_object_init_members(this, 0x12345678);
}

rob_chara_item_equip_object::~rob_chara_item_equip_object() {
    rob_chara_item_equip_object_init_members(this, 0xDDDDDDDD);
}

rob_chara_item_equip::rob_chara_item_equip() : bone_nodes(), matrices(), item_equip_object(), field_18(),
item_equip_range(), first_item_equip_object(), max_item_equip_object(), field_A0(), shadow_type(), position(),
eyes_adjust(), field_D4(), disable_update(), field_DC(), texture_color_coeff(), wet(), wind_strength(),
chara_color(), npr_flag(), mat(), field_13C(), field_8BC(), field_8C0(), field_8C4(), field_8C8(), field_8CC(),
field_8D0(), field_8D4(), field_8D8(), field_8DC(), field_8E0(), field_8E4(), field_8E8(), field_8EC(),
field_8F0(), field_8F4(), field_8F8(), field_8FC(), field_900(), field_908(), field_910(), field_918(),
field_920(), field_928(), field_930(), step(1.0f), use_opd(), parts_short(), parts_append(), parts_white_one_l() {
    item_equip_object = new rob_chara_item_equip_object[ITEM_MAX];
    wind_strength = 1.0f;
    chara_color = true;
    for (int32_t i = ITEM_BODY; i < ITEM_MAX; i++)
        item_equip_object[i].item_equip = this;
}

rob_chara_item_equip::~rob_chara_item_equip() {
    reset();

    if (item_equip_object) {
        delete[] item_equip_object;
        item_equip_object = 0;
    }
}

void rob_chara_item_equip::reset() {
    bone_nodes = 0;
    matrices = 0;
    for (int32_t i = ITEM_BODY; i < ITEM_MAX; i++)
        rob_chara_item_equip_object_init_members(&item_equip_object[i]);
    item_equip_range = false;
    shadow_type = (shadow_type_enum)-1;
    field_A0 = 5;
    texture_pattern.clear();
    field_D0 = object_info();
    field_D4 = -1;
    disable_update = false;
    parts_short = false;
    parts_append = false;
    parts_white_one_l = false;
    step = 1.0f;
    use_opd = false;
    first_item_equip_object = ITEM_ATAMA;
    max_item_equip_object = ITEM_ITEM16;
}

item_cos_texture_change_tex::item_cos_texture_change_tex() : org(), chg(), changed() {

}

item_cos_texture_change_tex::~item_cos_texture_change_tex() {

}

rob_chara_item_cos_data::rob_chara_item_cos_data() : chara_index(), chara_index_2nd(), cos(), cos_2nd() {

}

rob_chara_item_cos_data::~rob_chara_item_cos_data() {

}

struc_523::struc_523() : field_0(), field_1(), field_4(), field_8(), field_C(),
field_10(), field_1C(), field_20(), field_24(), field_28(), field_2C(), field_30(),
field_34(), field_38(), field_3C(), field_40(), field_44(), field_48(), field_4C(),
field_50(), field_54(), field_68(), field_70(), field_78(), field_7C(), field_80() {

}

struc_523::~struc_523() {

}

struc_264::struc_264() : field_0(), field_4(), field_18(), field_2C(), field_40(), field_54(), field_68(),
field_6C(), field_70(), field_74(), field_78(), field_7C(), field_80(), field_84(), field_88(), field_8C(),
field_90(), field_94(), field_98(), field_9C(), field_A0(), field_A4(), field_A5(), field_A8(), field_AC(),
field_B0(), eyes_adjust(), field_140(), field_144(), field_145(), field_146(), field_148(), field_150(),
field_152(), field_154(), field_158(), field_15C(), field_160(), field_164(), field_168(), field_16C(),
field_170(), field_174(), field_178(), field_17C(), field_180(), field_184(), field_188(), field_18C(),
field_190(), field_194(), field_198(), field_19C(), field_1A0(), field_1A4(), field_1A8(), field_1B0(), field_1B4(),
field_1B8(), field_1BC(), field_1C0(), field_1C8(), field_1C9(), field_1CA(), field_1CC(), field_1D0(), field_1D4() {

}

struc_264::~struc_264() {

}

SubActParam::SubActParam(SubActParamType type) {
    this->type = type;
}

SubActParam::~SubActParam() {

}

SubActParamAngry::SubActParamAngry() : SubActParam(SUB_ACTION_PARAM_ANGRY) {

}

SubActParamAngry::~SubActParamAngry() {

}

SubActParamCountNum::SubActParamCountNum() : SubActParam(SUB_ACTION_PARAM_COUNT_NUM) {
    field_10 = 0;
}

SubActParamCountNum::~SubActParamCountNum() {

}

SubActParamCry::SubActParamCry() : SubActParam(SUB_ACTION_PARAM_CRY) {

}

SubActParamCry::~SubActParamCry() {

}

SubActParamEmbarrassed::SubActParamEmbarrassed() : SubActParam(SUB_ACTION_PARAM_EMBARRASSED) {

}

SubActParamEmbarrassed::~SubActParamEmbarrassed() {

}

SubActParamLaugh::SubActParamLaugh() : SubActParam(SUB_ACTION_PARAM_LAUGH) {

}

SubActParamLaugh::~SubActParamLaugh() {

}

SubActParamShakeHand::SubActParamShakeHand() : SubActParam(SUB_ACTION_PARAM_SHAKE_HAND) {
    field_10 = 0;
}

SubActParamShakeHand::~SubActParamShakeHand() {

}

SubActExec::SubActExec(SubActExecType type) {
    this->type = type;
    field_C = 0;
};

SubActExec::~SubActExec() {

};

SubActExecAngry::SubActExecAngry() : SubActExec(SUB_ACTION_EXECUTE_ANGRY) {

};

SubActExecAngry::~SubActExecAngry() {

};

void SubActExecAngry::Field_8() {
    type = SUB_ACTION_EXECUTE_ANGRY;
    field_C = 0;
};

void SubActExecAngry::Field_10(SubActParam* param) {
    Field_8();
};

void SubActExecAngry::Field_18(rob_chara* rob_chr) {
    rob_chr->data.rob_sub_action.data.field_8 = this;
};

void SubActExecAngry::Field_20(rob_chara* rob_chr) {

};

SubActExecCountNum::SubActExecCountNum() : SubActExec(SUB_ACTION_EXECUTE_COUNT_NUM) {
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
};

SubActExecCountNum::~SubActExecCountNum() {

};

void SubActExecCountNum::Field_8() {
    type = SUB_ACTION_EXECUTE_COUNT_NUM;
    field_C = 0;
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
};

void SubActExecCountNum::Field_10(SubActParam* param) {
    Field_8();
    if (param) {
        SubActParamCountNum* param_count_num = dynamic_cast<SubActParamCountNum*>(param);
        if (param_count_num)
            field_10 = param_count_num->field_10;
    }
};

void SubActExecCountNum::Field_18(rob_chara* rob_chr) {
    rob_chr->data.rob_sub_action.data.field_8 = this;
};

void SubActExecCountNum::Field_20(rob_chara* rob_chr) {

};

SubActExecCry::SubActExecCry() : SubActExec(SUB_ACTION_EXECUTE_CRY) {

};

SubActExecCry::~SubActExecCry() {

};

void SubActExecCry::Field_8() {
    type = SUB_ACTION_EXECUTE_CRY;
    field_C = 0;
};

void SubActExecCry::Field_10(SubActParam* param) {
    Field_8();
};

void SubActExecCry::Field_18(rob_chara* rob_chr) {
    rob_chr->data.rob_sub_action.data.field_8 = this;
};

void SubActExecCry::Field_20(rob_chara* rob_chr) {

};

SubActExecEmbarrassed::SubActExecEmbarrassed() : SubActExec(SUB_ACTION_EXECUTE_EMBARRASSED) {
    field_10 = 0;
};

SubActExecEmbarrassed::~SubActExecEmbarrassed() {

};

void SubActExecEmbarrassed::Field_8() {
    type = SUB_ACTION_EXECUTE_EMBARRASSED;
    field_C = 0;
    field_10 = 0;
};

void SubActExecEmbarrassed::Field_10(SubActParam* param) {
    Field_8();
};

void SubActExecEmbarrassed::Field_18(rob_chara* rob_chr) {
    rob_chr->data.rob_sub_action.data.field_8 = this;
};

void SubActExecEmbarrassed::Field_20(rob_chara* rob_chr) {

};

SubActExecLaugh::SubActExecLaugh() : SubActExec(SUB_ACTION_EXECUTE_LAUGH) {

};

SubActExecLaugh::~SubActExecLaugh() {

};

void SubActExecLaugh::Field_8() {
    type = SUB_ACTION_EXECUTE_LAUGH;
    field_C = 0;
};

void SubActExecLaugh::Field_10(SubActParam* param) {
    Field_8();
};

void SubActExecLaugh::Field_18(rob_chara* rob_chr) {
    rob_chr->data.rob_sub_action.data.field_8 = this;
};

void SubActExecLaugh::Field_20(rob_chara* rob_chr) {

};

SubActExecShakeHand::SubActExecShakeHand() : SubActExec(SUB_ACTION_EXECUTE_SHAKE_HAND) {
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
};

SubActExecShakeHand::~SubActExecShakeHand() {

};

void SubActExecShakeHand::Field_8() {
    type = SUB_ACTION_EXECUTE_SHAKE_HAND;
    field_C = 0;
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
};

void SubActExecShakeHand::Field_10(SubActParam* param) {
    Field_8();
    if (param) {
        SubActParamShakeHand* param_shake_hand = dynamic_cast<SubActParamShakeHand*>(param);
        if (param_shake_hand)
            field_10 = param_shake_hand->field_10;
    }
};

void SubActExecShakeHand::Field_18(rob_chara* rob_chr) {
    rob_chr->data.rob_sub_action.data.field_8 = this;
};

void SubActExecShakeHand::Field_20(rob_chara* rob_chr) {

};

RobSubAction::Data::Data() : field_0(), field_8(), field_10(), field_18() {

}

RobSubAction::Data::~Data() {

}

RobSubAction::RobSubAction() {
    Reset();
}

RobSubAction::~RobSubAction() {
    Reset();
}

void RobSubAction::Reset() {
    data.field_0 = 0;
    data.field_8 = 0;
    data.field_10 = 0;
    data.field_18 = 0;
    data.cry.Field_8();
    data.shake_hand.Field_8();
    data.embarrassed.Field_8();
    data.angry.Field_8();
    data.laugh.Field_8();
}

RobPartialMotion::Data::Data() : motion_id(-1), mottbl_index(6), state(-1), frame(), play_frame_step(1.0f),
frame_count(), duration(), step(1.0f), offset(), field_24(), frame_data(), step_data(), field_38() {

}

RobPartialMotion::Data::~Data() {

}

RobPartialMotion::RobPartialMotion()  {

}

RobPartialMotion::~RobPartialMotion() {

}

RobFaceMotion::RobFaceMotion() {
    Reset();
}

RobFaceMotion::~RobFaceMotion() {

}

void RobFaceMotion::Reset() {
    data.motion_id = -1;
    data.mottbl_index = 6;
    data.state = -1;
    data.frame = 0.0f;
    data.play_frame_step = 1.0f;
    data.frame_count = 0.0f;
    data.duration = 0.0f;
    data.step = 1.0f;
    data.offset = 1.0f;
    data.field_24 = 0.0f;
    data.frame_data = 0;
    data.step_data = 0;
    data.field_38 = -1;
}

RobHandMotion::RobHandMotion() {
    Reset();
}

RobHandMotion::~RobHandMotion() {

}

void RobHandMotion::Reset() {
    data.motion_id = -1;
    data.mottbl_index = 6;
    data.state = -1;
    data.frame = 0.0f;
    data.play_frame_step = 1.0f;
    data.frame_count = 0.0f;
    data.duration = 0.0f;
    data.step = 1.0f;
    data.offset = 1.0f;
    data.field_24 = 0.0f;
    data.frame_data = 0;
    data.step_data = 0;
    data.field_38 = -1;
}

RobMouthMotion::RobMouthMotion() {
    Reset();
}

RobMouthMotion::~RobMouthMotion() {

}

void RobMouthMotion::Reset() {
    data.motion_id = -1;
    data.mottbl_index = 6;
    data.state = -1;
    data.frame = 0.0f;
    data.play_frame_step = 1.0f;
    data.frame_count = 0.0f;
    data.duration = 0.0f;
    data.step = 1.0f;
    data.offset = 1.0f;
    data.field_24 = 0.0f;
    data.frame_data = 0;
    data.step_data = 0;
    data.field_38 = -1;
}

RobEyesMotion::RobEyesMotion() {
    Reset();
}

RobEyesMotion::~RobEyesMotion() {

}

void RobEyesMotion::Reset() {
    data.motion_id = -1;
    data.mottbl_index = 6;
    data.state = -1;
    data.frame = 0.0f;
    data.play_frame_step = 1.0f;
    data.frame_count = 0.0f;
    data.duration = 0.0f;
    data.step = 1.0f;
    data.offset = 1.0f;
    data.field_24 = 0.0f;
    data.frame_data = 0;
    data.step_data = 0;
    data.field_38 = -1;
}

RobEyelidMotion::RobEyelidMotion() {
    Reset();
}

RobEyelidMotion::~RobEyelidMotion() {

}

void RobEyelidMotion::Reset() {
    data.motion_id = -1;
    data.mottbl_index = 6;
    data.state = -1;
    data.frame = 0.0f;
    data.play_frame_step = 1.0f;
    data.frame_count = 0.0f;
    data.duration = 0.0f;
    data.step = 1.0f;
    data.offset = 1.0f;
    data.field_24 = 0.0f;
    data.frame_data = 0;
    data.step_data = 0;
    data.field_38 = -1;
}

struc_405::struc_405() : field_1C0(), time() {

}

struc_405::~struc_405() {

}

void struc_405::Reset() {
    face.Reset();
    hand_l.Reset();
    hand_r.Reset();
    mouth.Reset();
    eye.Reset();
    eyelid.Reset();
    head_object = object_info();
    hand_l_object = object_info();
    hand_r_object = object_info();
    face_object = object_info();
    field_1C0 = 0;
    time = 0.0f;
}

void rob_chara_data_adjust::reset() {
    enable = false;
    frame = 0.0f;
    transition_frame = 0.0f;
    curr_external_force = vec3_null;
    curr_force = 1.0f;
    curr_strength = 1.0f;
    motion_id = -1;
    set_frame = 0.0f;
    force_duration = 0.0f;
    type = 6;
    cycle_type = 0;
    ignore_gravity = true;
    external_force = vec3_null;
    external_force_cycle_strength = vec3_null;
    external_force_cycle = vec3_null;
    cycle = 1.0f;
    phase = 0.0f;
    force = 1.0f;
    strength = 1.0f;
    strength_transition = 0.0f;
}

void rob_chara_data_hand_adjust::reset() {
    enable = false;
    scale_select = 0;
    type = ROB_CHARA_DATA_HAND_ADJUST_NONE;
    current_scale = 1.0f;
    scale = 1.0f;
    duration = 0.0f;
    current_time = 0.0f;
    rotation_blend = 1.0f;
    scale_blend = 1.0f;
    enable_scale = false;
    disable_x = false;
    disable_y = false;
    disable_z = false;
    offset = vec3_null;
    field_30 = vec3_null;
    arm_length = 0.0f;
    field_40 = 0;
}

void rob_chara_data_arm_adjust::reset() {
    enable = false;
    value = 0.0f;
    prev_value = 0.0f;
    next_value = 0.0f;
    duration = 0.0f;
    frame = 0.0f;
}

rob_chara_motion::rob_chara_motion() : motion_id(-1), prev_motion_id(-1), frame_data(), step_data(), step(1.0f),
field_24(), field_28(), field_29(), field_2A(), field_2C(), field_30(), field_34(), field_36(), field_38(),
field_B8(), field_138(), field_13C(), field_140(),  field_14C(), field_314(), parts_adjust(),
parts_adjust_prev(), adjust_global(), adjust_global_prev(), hand_adjust(), hand_adjust_prev(), arm_adjust() {

}

rob_chara_motion::~rob_chara_motion() {

}

void rob_chara_motion::reset() {
    motion_id = -1;
    prev_motion_id = -1;
    frame_data.frame = 0.0f;
    frame_data.prev_frame = 0.0f;
    frame_data.last_set_frame = 0.0f;
    step_data.frame = 0.0f;
    step_data.field_4 = 0.0f;
    step_data.step = -1.0f;
    step = -1.0f;
    field_24 = 0;
    field_28 = 0;
    field_29 = 0;
    field_2A = 0;
    field_2C = 0;
    field_30 = 0;
    field_34 = 0;
    field_36 = 0;

    for (int32_t& i : field_38)
        i = 0;
    for (int32_t& i : field_B8)
        i = 0;

    field_138 = 1.0f;
    field_13C = 1.0f;
    field_140 = vec3_null;
    field_150.Reset();
    hand_l.Reset();
    hand_r.Reset();
    hand_l_object = object_info();
    hand_r_object = object_info();
    field_3B0.Reset();

    for (rob_chara_data_adjust& i : parts_adjust)
        i.reset();

    for (rob_chara_data_adjust& i : parts_adjust_prev)
        i.reset();

    adjust_global.reset();
    adjust_global_prev.reset();

    for (rob_chara_data_arm_adjust& i : arm_adjust)
        i.reset();

    for (rob_chara_data_hand_adjust& i : hand_adjust)
        i.reset();

    for (rob_chara_data_hand_adjust& i : hand_adjust_prev)
        i.reset();
}

mothead_mot::mothead_mot() {
    field_0.field_0 = 19;
    field_0.field_4 = 0;
    field_0.field_8 = 0;
    field_0.field_C = 0;
    field_10 = 0x04;
    field_12 = 0x04;
}

mothead_mot::~mothead_mot() {

}

mothead::mothead() {
    mot_set_id = -1;
    first_mot_id = 0;
    last_mot_id = 0;
}

mothead::~mothead() {
    for (mothead_mot*& i : mots)
        if (i) {
            delete i;
            i = 0;
        }
}

struc_652::struc_652() : motion_id(), frame_count(), field_8(), field_C(), field_10(), field_20(),
field_30(), field_40(), field_50(), field_52(), field_54(), field_58(), field_5C(), field_60(),
field_64(), field_68(), field_6C(), field_70(), field_74(), field_78(), field_7C(), field_80(),
field_84(), field_88(), field_8C(), field_90(), field_94(), field_96(), field_98(), field_9C(),
field_A0(), field_A4(), field_A8(), field_B0(), field_1E8(), field_1EC(), field_1F0(), field_1F4(),
field_1F8(), field_1FC(), field_200(), field_204(), field_208(), field_20C(), field_210(), field_218(),
field_21C(), field_220(), field_238(), field_23C(), field_240(), field_244(), field_248(), field_250(),
field_258(), field_25C(), field_260(), field_268(), field_270(), field_274(), field_276(), field_278(),
field_27C(), field_280(), field_284(), field_288(), field_290(), field_294(), field_298(), field_29C(),
field_2A0(), field_2A4(), field_2A8(), field_2AC(), field_2B0(), field_2B8(), field_2BC(), field_2C0(),
field_2C4(), field_2C8(), field_2CC(), field_2D0(), field_2D8(), field_2E0(), field_2E8(), field_2EC(),
field_2F0(), field_2F4(), field_2F8(), field_2FC(), field_300(), field_304(), field_308(), field_30C(),
field_310(), field_314(), field_318(), field_324(), field_328(), iterations() {

}

struc_652::~struc_652() {

}

struc_223::struc_223() : field_330(), field_7A0(), motion_set_id() {

}

struc_223::~struc_223() {

}

void rob_chara_data_miku_rot::reset() {
    rot_y_int16 = 0;
    field_2 = 0;
    field_4 = 0;
    field_6 = 0;
    field_8 = 0;
    field_C = 0;
    field_10 = 0.0f;
    field_14 = 0;
    position = vec3_null;
    field_24 = vec3_null;
    field_30 = vec3_null;
    field_3C = vec3_null;
    field_48 = vec3_null;
    field_54 = vec3_null;
    field_60 = 0;
    field_64 = 0;
    field_68 = 0;
    field_6C = mat4u_identity;
}

void rob_chara_adjust_data::reset() {
    scale = 1.0f;
    height_adjust = false;
    pos_adjust_y = 0.0f;
    pos_adjust = vec3_null;
    offset = vec3_null;
    offset_x = true;
    offset_y = false;
    offset_z = true;
    get_global_trans = false;
    mat4 mat;
    mat4_translate(trans.x, trans.y, trans.z, &mat);
    this->mat = mat;
    left_hand_scale = -1.0f;
    right_hand_scale = -1.0f;
    left_hand_scale_default = -1.0f;
    right_hand_scale_default = -1.0f;
}

struc_209::struc_209() : field_0(), field_4(), field_8(), field_C(), field_10(), field_14(), field_18(),
field_1C(), field_20(), field_24(), field_28(), field_2C(), field_30(), field_34(), field_38(), field_3C(),
field_40(), field_44(), field_48(), field_4C(), field_50(), field_54(), field_58(), field_5C(), field_60(),
field_64(), field_68(), field_6C(), field_70(), field_74(), field_78(), field_738(), field_DF8(), field_1230(),
field_1668(), field_1AA0(), field_1BE4(), field_1C68(), field_1C70(), field_1C78(), field_1C80(), field_1C88(),
field_1CF4(), field_1D60(), field_1DCC(), field_1DD0(), field_1DD4(), field_1DD8(), field_1DDC(), field_1DE0(),
field_1DE4(), field_1DE8(), field_1DEC(), field_1DF0(), field_1DF4(), field_1DF8(), field_1DFC(), field_1E00(),
field_1E04(), field_1E08(), field_1E0C(), field_1E10(), field_1E14(), field_1E18(), field_1E1C(), field_1E20(),
field_1E24(), field_1E28(), field_1E2C(), field_1E30(), field_1E34(), field_1E38(), field_1E3C(), field_1E40(),
field_1E44(), field_1E48(), field_1E4C(), field_1E50(), field_1E54(), field_1E58(), field_1E5C(), field_1E60(),
field_1E64(), field_1E68(), field_1E6C(), field_1E70(), field_1E74(), field_1E78(), field_1E7C(), field_1E80(),
field_1E84(), field_1E88(), field_1E8C(), field_1E90(), field_1E94(), field_1E98(), field_1E9C(),
field_1EA0(), field_1EA4(), field_1EA8(), field_1EAC(), field_1EB0(), field_1EB4(), field_1EF4(),
field_1F04(), field_1F08(), field_1F0C(), field_1F10(), field_1F14(), field_1F18(), field_1F1C(),
field_1F20(), field_1F21(), field_1F22(), field_1F23(), field_1F24(), field_1F25(), field_1F26() {

}

struc_209::~struc_209() {

}

rob_chara_data::rob_chara_data() : field_0(), field_1(), field_2(), field_3(), field_4(), miku_rot(),
adjust_data(), field_3D90(), field_3D94(), field_3D98(), field_3D9A(), field_3D9C(), field_3D9D(), field_3DA0(),
field_3DA4(), field_3DA8(), field_3DB0(), field_3DB8(), field_3DBC(), field_3DC0(), field_3DC4(),
field_3DC8(), field_3DCC(), field_3DD0(), field_3DD4(), field_3DD8(), field_3DDC(), field_3DE0() {

}

rob_chara_data::~rob_chara_data() {

}

void rob_chara_data::reset() {
    sub_140505C90(&field_8);
    rob_sub_action.Reset();
    motion.reset();
    sub_140539750(&field_1588);
    miku_rot.reset();
    adjust_data.reset();
    sub_140505FB0(&field_1E68);
    field_3D90 = 0;
    field_3D94 = 0;
    field_3D98 = 0;
    field_3D9C = 0;
    field_3DA0 = 0;
    field_3DA8 = 0;
    field_3DB0 = 0;
    field_3DB8 = 0;
    field_3DBC = 0;
    field_3DA4 = 0;
    field_3DC8 = 0;
    field_3DD4 = 0.0f;
    field_3DCC = 0;
    field_3DC0 = 0;
    field_3DC4 = 0;
    field_3DD8 = 0;
    field_3DD0 = 0;
    field_3DDC = 0.0f;
    field_3DE0 = 0;
}

rob_chara::rob_chara() : chara_id(), type(), field_C(), field_D(),
chara_index(), module_index(), chara_init_data(), field_20() {
    frame_speed = 1.0f;
    bone_data = new rob_chara_bone_data();
    item_equip = new rob_chara_item_equip();
}

rob_chara::~rob_chara() {
    delete bone_data;
    bone_data = 0;
    delete item_equip;
    item_equip = 0;
}

inline void motion_storage_init() {
    motion_storage_data = {};
}

inline bool motion_storage_check_mot_file_not_ready(uint32_t set_id) {
    auto elem = motion_storage_data.find(set_id);
    if (elem != motion_storage_data.end())
        return elem->second.CheckNotReady();
    return false;
}

inline const mot_data* motion_storage_get_mot_data(uint32_t motion_id, motion_database* mot_db) {
    uint32_t set_id = -1;
    size_t motion_index = -1;
    for (motion_set_info& i : mot_db->motion_set) {
        for (motion_info& j : i.motion)
            if (j.id == motion_id) {
                set_id = i.id;
                motion_index = &j - i.motion.data();

                break;
            }

        if (set_id != -1)
            break;
    }

    if (set_id == -1)
        return 0;

    auto elem = motion_storage_data.find(set_id);
    if (elem != motion_storage_data.end())
        return &elem->second.mot_set->vec[motion_index];
    return 0;
}

inline float_t motion_storage_get_mot_data_frame_count(uint32_t motion_id, motion_database* mot_db) {
    const mot_data* mot = motion_storage_get_mot_data(motion_id, mot_db);
    if (mot)
        return mot->frame_count;
    return 0.0f;
}

inline const mot_set* motion_storage_get_motion_set(uint32_t set_id) {
    auto elem = motion_storage_data.find(set_id);
    if (elem != motion_storage_data.end())
        return elem->second.mot_set;
    return 0;
}

inline void motion_storage_free() {
    motion_storage_data.clear();
}

inline void mothead_storage_init() {
    mothead_storage_data = {};
}

inline bool mothead_storage_check_mhd_file_not_ready(uint32_t set_id) {
    auto elem = mothead_storage_data.find(set_id);
    if (elem != mothead_storage_data.end())
        return elem->second.CheckNotReady();
    return false;
}

inline const mothead_mot* mothead_storage_get_mot_by_motion_id(uint32_t motion_id, motion_database* mot_db) {
    uint32_t set_id = mot_db->get_motion_set_id_by_motion_id(motion_id);
    if (set_id == -1)
        return &mothead_mot_null;

    auto elem = mothead_storage_data.find(set_id);
    if (elem == mothead_storage_data.end() || !elem->second.data)
        return &mothead_mot_null;

    mothead* mhd = elem->second.data;
    if (mhd->first_mot_id > (int32_t)motion_id || mhd->last_mot_id < (int32_t)motion_id)
        return &mothead_mot_null;

    mothead_mot* sub = mhd->mots[(ssize_t)(int32_t)motion_id - mhd->first_mot_id];
    if (!sub)
        return &mothead_mot_null;
    return sub;
}

inline void mothead_storage_free() {
    mothead_storage_data.clear();
}

inline void osage_setting_data_init() {
    osage_setting_data.cat = {};
    osage_setting_data.obj = {};
}

inline bool osage_setting_data_load_file(void* data, const char* path, const char* file, uint32_t hash) {
    key_val kv;
    if (key_val::load_file(&kv, path, file, hash)) {
        osage_setting_data.parse(&kv);
        return true;
    }
    return false;
}

inline void osage_setting_data_free() {
    osage_setting_data.cat.clear();
    osage_setting_data.obj.clear();
}

MotFile::MotFile() : mot_set_info(), mot_set(), load_count() {
    FreeData();
}

MotFile:: ~MotFile() {
    FreeData();
}

bool MotFile::CheckNotReady() {
    return file_handler.check_not_ready();
}

void MotFile::FreeData() {
    mot_set_info = 0;
    if (mot_set) {
        delete mot_set;
        mot_set = 0;
    }
    file_handler.free_data();
    load_count = 0;
}

void MotFile::LoadFile(std::string* mdata_dir, uint32_t set) {
    if (load_count > 0) {
        load_count++;
        return;
    }

    std::string rom_dir = "rom/";
    std::string rob_dir = "rob/";
    std::string farc_file = "mot_" + mot_set_info->name + ".farc";
    std::string mot_file = "mot_" + mot_set_info->name + ".bin";
    if (mdata_dir && mdata_dir->size())
        if (rctx_ptr->data->check_file_exists(mdata_dir->c_str(), farc_file.c_str())) {
            rom_dir = *mdata_dir;
            rob_dir.clear();
        }

    std::string dir = rom_dir + rob_dir;
    if (rctx_ptr->data->check_file_exists(dir.c_str(), farc_file.c_str())) {
        if (file_handler.read_file(rctx_ptr->data,
            dir.c_str(), farc_file.c_str(), mot_file.c_str(), false))
            file_handler.set_callback_data(0, ParseFileParent, this);
    }
    else {
        if (file_handler.read_file(rctx_ptr->data, rom_dir.c_str(), mot_file.c_str()))
            file_handler.set_callback_data(0, ParseFileParent, this);
    }
    load_count = 1;
}

void MotFile::ParseFile(void* data, size_t size) {
    mot_set = new ::mot_set;
    mot_set->unpack_file(data, size, false);
}

bool MotFile::Unload() {
    if (--load_count < 0) {
        load_count = 0;
        return true;
    }
    else if (load_count <= 0) {
        FreeData();
        return true;
    }
    else
        return false;
}

void MotFile::ParseFileParent(void* data, void* file_data, size_t size) {
    ((MotFile*)data)->ParseFile(file_data, size);
}

MhdFile::MhdFile() : data(), set(), load_count() {
    FreeData();
}

MhdFile::~MhdFile() {
    FreeData();
}

bool MhdFile::CheckNotReady() {
    return file_handler.check_not_ready();
}

void MhdFile::FreeData() {
    if (data) {
        delete data;
        data = 0;
    }
    set = -1;
    file_path.clear();
    file_handler.free_data();
    load_count = 0;
}

void MhdFile::LoadFile(const char* path, const char* file, uint32_t set) {
    if (load_count > 0) {
        load_count++;
        return;
    }

    this->set = set;
    this->file_path = file_path;
    if (file_handler.read_file(rctx_ptr->data, path, file))
        file_handler.set_callback_data(0, ParseFileParent, this);
    load_count = 1;
}

void MhdFile::ParseFile(void* data) {
    mothead_file* mhdf = (mothead_file*)((size_t)data + ((uint32_t*)data)[1]);
    this->data = MhdFile::ParseMothead(mhdf, (size_t)mhdf);
}

mothead* MhdFile::ParseMothead(mothead_file* mhdf, size_t data) {
    mothead* mhd = new mothead;
    mhd->mot_set_id = mhdf->mot_set_id;
    mhd->first_mot_id = mhdf->first_mot_id;
    mhd->last_mot_id = mhdf->last_mot_id;
    uint32_t* mot_offsets = (uint32_t*)(data + mhdf->mot_offsets_offset);
    size_t mot_count = (size_t)mhdf->last_mot_id - mhdf->first_mot_id + 1;
    mhd->mots.resize(mot_count);
    mothead_mot** mots = mhd->mots.data();
    for (; mot_count; mot_count--) {
        *mots = 0;
        if (*mot_offsets)
            *mots = ParseMotheadMot((mothead_mot_file*)(data + *mot_offsets), data);
        mots++;
        mot_offsets++;
    }
    return mhd;
}

mothead_mot* MhdFile::ParseMotheadMot(mothead_mot_file* mhdsf, size_t data) {
    struct struc_335 {
        int32_t type;
        uint32_t offset;
    };

    struct struc_337 {
        mothead_data_type type;
        int32_t frame;
        uint32_t offset;
    };

    mothead_mot* mhdm = new mothead_mot;
    mhdm->field_0.field_0 = mhdsf->field_0;
    mhdm->field_0.field_4 = mhdsf->field_4;
    mhdm->field_0.field_8 = mhdsf->field_8;
    mhdm->field_0.field_C = mhdsf->field_C;
    mhdm->field_10 = mhdsf->field_10;
    mhdm->field_12 = mhdsf->field_12;

    if (mhdsf->field_14) {
        struc_335* v11 = (struc_335*)(data + mhdsf->field_14);
        struc_335* v12 = v11;
        while ((v12++)->type >= 0);

        size_t count = v12 - v11;
        mhdm->field_18.resize(count);
        mothead_data2* v15 = mhdm->field_18.data();
        for (size_t i = count; i; i--) {
            v15->type = v11->type;
            v15->data = (void*)(data + v11->offset);
            v11++;
            v15++;
        }
    }

    if (mhdsf->field_18) {
        struc_337* v20 = (struc_337*)(data + mhdsf->field_18);
        struc_337* v21 = v20;
        while ((v21++)->type >= 0);

        size_t count = v21 - v20;
        mhdm->data.resize(count);
        mothead_data* v24 = mhdm->data.data();
        for (size_t i = count; i; i--) {
            v24->type = v20->type;
            v24->frame = v20->frame;
            v24->data = (void*)(data + v20->offset);
            v20++;
            v24++;
        }
    }

    if (mhdsf->field_1C) {
        uint32_t* v29 = (uint32_t*)(data + mhdsf->field_1C);
        uint32_t* v30 = v29;
        while (*v30++);

        size_t count = v30 - v29;
        mhdm->field_28.resize(count);
        int64_t* v33 = mhdm->field_28.data();
        for (size_t i = count; i; i--)
            *v33++ = data + *v29++;
    }
    return mhdm;
}

bool MhdFile::Unload() {
    if (--load_count < 0) {
        load_count = 0;
        return true;
    }
    else if (load_count <= 0) {
        FreeData();
        return true;
    }
    else
        return false;
}

void MhdFile::ParseFileParent(void* data, void* file_data, size_t size) {
    ((MhdFile*)data)->ParseFile(file_data);
}

osage_setting::osage_setting() {

}

osage_setting::~osage_setting() {

}

void osage_setting::parse(key_val* kv) {
    char buf[OSG_SET_TEXT_BUF_SIZE];
    int32_t count;
    int32_t count1;
    size_t len;
    size_t len1;
    size_t len2;
    size_t len3;
    size_t off;

    key_val lkv;
    if (kv->get_local_key_val("cat", &lkv)) {
        len = 3;
        memcpy(buf, "cat", 3);
        off = len;

        buf[off] = 0;
        key_val slkv;
        if (lkv.read_int32_t(buf, off, ".length", 8, &count)
            && lkv.get_local_key_val(buf, &slkv))
            for (int32_t i = 0; i < count; i++) {
                len1 = sprintf_s(buf + len,
                    OSG_SET_TEXT_BUF_SIZE - len, ".%d", i);
                off = len + len1;

                std::string name;
                lkv.read_string(
                    buf, off, ".name", 6, &name);

                len2 = 4;
                memcpy(buf + len + len1, ".osg", 4);
                off = len + len1 + len2;

                buf[min(off, OSG_SET_TEXT_BUF_SIZE - 1)] = 0;
                key_val sslkv;
                if (lkv.read_int32_t(buf, off, ".length", 8, &count1)
                    && lkv.get_local_key_val(buf, &sslkv))
                    for (int32_t j = 0; j < count1; j++) {
                        len3 = sprintf_s(buf + len + len1 + len2,
                            OSG_SET_TEXT_BUF_SIZE - len - len1 - len2, ".%d", j);
                        off = len + len1 + len2 + len3;

                        int32_t exf = 0;
                        lkv.read_int32_t(
                            buf, off, ".exf", 5, &exf);

                        std::string parts;
                        lkv.read_string(
                            buf, off, ".parts", 7, &parts);

                        std::string root;
                        lkv.read_string(
                            buf, off, ".root", 6, &root);

                        std::pair<std::string, std::string> key = { name, root };
                        auto elem = cat.find(key);
                        if (elem == cat.end())
                            elem = cat.insert({ key, {} }).first;

                        elem->second.exf = exf;
                        elem->second.parts = parse_parts_string(&parts);
                    }
            }
    }

    data_struct* data = rctx_ptr->data;
    object_database* obj_db = &data->data_ft.obj_db;

    if (kv->get_local_key_val("obj", &lkv)) {
        len = 3;
        memcpy(buf, "obj", 3);
        off = len;

        buf[off] = 0;
        key_val slkv;
        if (lkv.read_int32_t(buf, off, ".length", 8, &count)
            && lkv.get_local_key_val(buf, &slkv)) {
            for (int32_t i = 0; i < count; i++) {
                len1 = sprintf_s(buf + len,
                    OSG_SET_TEXT_BUF_SIZE - len, ".%d", i);
                off = len + len1;

                std::string cat;
                lkv.read_string(
                    buf, off, ".cat", 5, &cat);

                std::string name;
                lkv.read_string(
                    buf, off, ".name", 6, &name);

                obj.insert_or_assign(obj_db->get_object_info(name.c_str()), cat);
            }
        }
    }
}

rob_osage_parts osage_setting::parse_parts_string(std::string* s) {
    static const char* parts_string_array[] = {
        "LEFT", "RIGHT", "CENTER", "LONG_C", "SHORT_L", "SHORT_R",
        "APPEND_L", "APPEND_R", "MUFFLER", "WHITE_ONE_L", "PONY", "ANGEL_L", "ANGEL_R",
    };

    for (const char*& i : parts_string_array)
        if (!s->compare(i))
            return (rob_osage_parts)(&i - parts_string_array);
    return ROB_OSAGE_PARTS_NONE;
}

ReqData::ReqData() : chara_index(), count() {

}

ReqData::ReqData(::chara_index chara_index, int32_t count) {
    this->chara_index = chara_index;
    this->count = count;
}

ReqData::~ReqData() {

}

void ReqData::Reset() {
    chara_index = CHARA_MAX;
    count = 0;
}

ReqDataObj::ReqDataObj() : ReqData(chara_index, count), cos() {

}

ReqDataObj::ReqDataObj(::chara_index chara_index, int32_t count) : ReqData(chara_index, count), cos() {

}

ReqDataObj::~ReqDataObj() {

}

void ReqDataObj::Reset() {
    ReqData::Reset();
    cos = {};
}

osage_set_motion::osage_set_motion() : motion_id() {

}

osage_set_motion::~osage_set_motion() {

}

pv_data_set_motion::pv_data_set_motion() : motion_id() {

}

pv_data_set_motion::~pv_data_set_motion() {

}

PvOsageManager::PvOsageManager() : state(), chara_id(), reset(), field_74(),
motion_index(), pv(), thread(), disp(), not_reset(), exit(), field_D4() {
    Reset();
    disp_mtx.lock();
    disp = false;
    disp_mtx.unlock();
    not_reset_mtx.lock();
    not_reset = false;
    not_reset_mtx.unlock();
    exit_mtx.lock();
    exit = false;
    exit_mtx.unlock();
    thread = new std::thread(PvOsageManager::ThreadMain, this);
}

PvOsageManager::~PvOsageManager() {
    exit_mtx.lock();
    exit = true;
    exit_mtx.unlock();
    cnd.notify_one();
    thread->join();
    delete thread;
    thread = 0;
}

bool PvOsageManager::Init() {
    state = 0;
    return true;
}

bool PvOsageManager::Ctrl() {
    if (!state) {
        //sub_1404F8AA0(this);
        if (sub_1404F7AF0())
            return false;
        state = 1;
    }
    else if (state != 1)
        return false;
    sub_1404F7BD0(false);
    return true;
}

bool PvOsageManager::Dest() {
    return true;
}

void PvOsageManager::Disp() {

}

bool PvOsageManager::GetDisp() {
    std::unique_lock<std::mutex> u_disp_lock(disp_mtx);
    bool disp = this->disp;
    u_disp_lock.unlock();
    return disp;
}

void PvOsageManager::SetNotResetTrue() {
    std::unique_lock<std::mutex> u_not_reset_lock(not_reset_mtx);
    not_reset = true;
    u_not_reset_lock.unlock();
}

bool PvOsageManager::sub_1404F7AF0() {
    if (osage_set_motion.size() > 1) {
        osage_set_motion.pop_back();
        return true;
    }
    return false;
}

void PvOsageManager::sub_1404F7BD0(bool not_reset) {
    if (reset && !not_reset) {
        data_struct* data = rctx_ptr->data;
        bone_database* bone_data = &data->data_ft.bone_data;
        motion_database* mot_db = &data->data_ft.mot_db;
        rob_chara* rob_chr = rob_chara_array_get(chara_id);
        rob_chr->set_motion_id(rob_chr->get_rob_cmn_mottbl_motion_id(0),
            0.0f, 0.0f, 1, 0, MOTION_BLEND_CROSS, bone_data, mot_db);
        rob_chr->set_face_mottbl_motion(0, 6, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, true, mot_db);
        rob_chr->set_hand_l_mottbl_motion(0, 192, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
        rob_chr->set_hand_r_mottbl_motion(0, 192, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
        rob_chr->set_mouth_mottbl_motion(0, 131, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
        rob_chr->set_eyes_mottbl_motion(0, 165, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
        rob_chr->set_eyelid_mottbl_motion_from_face(0, 0.0f, -1.0f, 0.0f, mot_db);
    }
    Reset();
}

void PvOsageManager::Reset() {
    state = 1;
    chara_id = -1;
    reset = true;
    pv = 0;
    reset_frames_list.clear();
    osage_set_motion.clear();
    pv_set_motion.clear();
}

void PvOsageManager::sub_1404F82F0() {
    do {
        std::unique_lock<std::mutex> u_not_reset_lock(not_reset_mtx);
        bool not_reset = this->not_reset;
        u_not_reset_lock.unlock();
        if (not_reset)
            break;

        //sub_1404F8AA0();
    } while (sub_1404F7AF0());

    std::unique_lock<std::mutex> u_not_reset_lock(not_reset_mtx);
    bool not_reset = this->not_reset;
    u_not_reset_lock.unlock();
    sub_1404F7BD0(not_reset);
}

void PvOsageManager::ThreadMain(PvOsageManager* pv_osg_mgr) {
    std::unique_lock<std::mutex> u_lock(pv_osg_mgr->mtx);
    while (true) {
        pv_osg_mgr->cnd.wait(u_lock);
        std::unique_lock<std::mutex> u_exit_lock(pv_osg_mgr->exit_mtx);
        bool exit = pv_osg_mgr->exit;
        u_exit_lock.unlock();
        if (exit)
            break;

        pv_osg_mgr->sub_1404F82F0();

        std::unique_lock<std::mutex> u_not_reset_lock(pv_osg_mgr->not_reset_mtx);
        u_not_reset_lock.unlock();

        std::unique_lock<std::mutex> u_disp_lock(pv_osg_mgr->disp_mtx);
        pv_osg_mgr->disp = false;
        u_disp_lock.unlock();
    }

    std::unique_lock<std::mutex> u_disp_lock(pv_osg_mgr->disp_mtx);
    pv_osg_mgr->disp = false;
    u_disp_lock.unlock();
    u_lock.unlock();
}

RobThreadParent::RobThreadParent() : exit(), thread() {
    thread = new std::thread(RobThreadParent::ThreadMain, this);
}

RobThreadParent::~RobThreadParent() {
    mtx.lock();
    exit = true;
    mtx.unlock();
    cnd.notify_one();
    thread->join();
    delete thread;
    thread = 0;
}

void RobThreadParent::AppendRobCharaFunc(rob_chara* rob_chr, void(*rob_chr_func)(rob_chara*)) {
    std::unique_lock<std::mutex> u_lock(mtx);
    RobThread thrd;
    thrd.data = rob_chr;
    thrd.func = rob_chr_func;
    AppendRobThread(&thrd);
    cnd.notify_one();
    u_lock.unlock();
}

void RobThreadHandler::sub_14054E3F0() {
    for (RobThreadParent*& i : arr)
        i->sub_14054E370();
}

void RobThreadParent::AppendRobThread(RobThread* thread) {
    std::unique_lock<std::mutex> u_lock(threads_mtx);
    threads.push_back(*thread);
    u_lock.unlock();
}

bool RobThreadParent::CheckThreadsNotNull() {
    std::unique_lock<std::mutex> u_lock(threads_mtx);
    bool not_null = !!threads.size();
    u_lock.unlock();
    return not_null;
}

void RobThreadParent::sub_14054E0D0() {
    std::unique_lock<std::mutex> u_lock(threads_mtx);
    threads.pop_front();
    bool v5 = this->threads.size() == 0;
    u_lock.unlock();

    if (v5) {
        std::unique_lock<std::mutex> u_lock(field_28);
        field_30.notify_one();
        u_lock.unlock();
    }
}

void RobThreadParent::sub_14054E370() {
    std::unique_lock<std::mutex> u_lock(field_28);
    if (CheckThreadsNotNull())
        field_30.wait(u_lock);
    u_lock.unlock();
}

void RobThreadParent::ThreadMain(RobThreadParent* rob_thrd_parent) {
    std::unique_lock<std::mutex> u_lock(rob_thrd_parent->mtx);
    while (!rob_thrd_parent->exit) {
        if (rob_thrd_parent->threads.size()) {
            RobThread* thread = &rob_thrd_parent->threads.front();
            if (thread->func)
                thread->func(thread->data);
            rob_thrd_parent->sub_14054E0D0();
        }
        else
            rob_thrd_parent->cnd.wait(u_lock);
    }
    u_lock.unlock();
}

RobThreadHandler::RobThreadHandler() {
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        arr[i] = new RobThreadParent;
}

RobThreadHandler::~RobThreadHandler() {
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++) {
        delete arr[i];
        arr[i] = 0;
    }
}

void RobThreadHandler::AppendRobCharaFunc(int32_t chara_id,
    rob_chara* rob_chr, void(*rob_chr_func)(rob_chara*)) {
    arr[chara_id]->AppendRobCharaFunc(rob_chr, rob_chr_func);
}

RobImplTask::RobImplTask() {

}

RobImplTask::~RobImplTask() {

}

bool RobImplTask::Init() {
    return true;
}

bool RobImplTask::Ctrl() {
    return false;
}

bool RobImplTask::Dest() {
    return true;
}

void RobImplTask::Disp() {

}

void RobImplTask::AppendList(rob_chara* rob_chr, std::list<rob_chara*>* list) {
    if (!rob_chr || !list || rob_chr->chara_id >= ROB_CHARA_COUNT || list->size() >= ROB_CHARA_COUNT)
        return;

    bool found = false;
    for (rob_chara*& i : *list)
        if (i->chara_id == rob_chr->chara_id) {
            found = true;
            break;
        }

    if (!found)
        list->push_back(rob_chr);
}

void RobImplTask::AppendCtrlCharaList(rob_chara* rob_chr) {
    AppendList(rob_chr, &ctrl_chara);
}

void RobImplTask::AppendFreeCharaList(rob_chara* rob_chr) {
    AppendList(rob_chr, &free_chara);
}

void RobImplTask::AppendInitCharaList(rob_chara* rob_chr) {
    AppendList(rob_chr, &init_chara);
}

void RobImplTask::FreeList(int8_t* chara_id, std::list<rob_chara*>* list) {
    if (!chara_id || !list || *chara_id >= ROB_CHARA_COUNT || !list->size())
        return;

    for (auto i = list->begin(); i != list->end();) {
        rob_chara* rob_chr = i._Ptr->_Myval;
        if (rob_chr->chara_id == *chara_id)
            i = list->erase(i);
        else
            i++;
    }
}

void RobImplTask::FreeCharaLists() {
    init_chara.clear();
    ctrl_chara.clear();
    free_chara.clear();
}

void RobImplTask::FreeCtrlCharaList(int8_t* chara_id) {
    FreeList(chara_id, &ctrl_chara);
}

void RobImplTask::FreeFreeCharaList(int8_t* chara_id) {
    FreeList(chara_id, &free_chara);
}

void RobImplTask::FreeInitCharaList(int8_t* chara_id) {
    FreeList(chara_id, &init_chara);
}

struc_462::struc_462() : rob_chr() {
    pv_id = -1;
    motion_id = -1;
    frame = -1;
}

struc_462::~struc_462() {

}

static uint8_t skin_param_manager_index = 0;

SkinParamManager::SkinParamManager() {
    index = skin_param_manager_index++;
    state = 1;
}

SkinParamManager::~SkinParamManager() {
    state = 1;
    Reset();
}

bool SkinParamManager::Init() {
    state = 0;
    sub_14060FBD0();
    return true;
}

bool SkinParamManager::Ctrl() {
    if (state == 0) {
        if (sub_1406112F0())
            return false;
        state = 1;
    }
    else if (state != 1)
        return false;
    return true;
}

bool SkinParamManager::Dest() {
    return true;
}

void SkinParamManager::Disp() {

}

void SkinParamManager::Reset() {
    field_70.clear();

    for (auto i : field_88) {
        if (!i.second)
            continue;

        i.second->clear();
        i.second->shrink_to_fit();
        delete i.second;
        i.second = 0;
    }
    field_88.clear();

    for (auto i : field_98) {
        if (!i.second)
            continue;

        i.second->clear();
        i.second->shrink_to_fit();
        delete i.second;
        i.second = 0;
    }
    field_98.clear();

    for (auto i : field_A8) {
        if (!i.second)
            continue;

        i.second->clear();
        i.second->shrink_to_fit();
        delete i.second;
        i.second = 0;
    }
    field_A8.clear();

    for (struc_461*& i : field_B8) {
        if (i->file_handler) {
            delete i->file_handler;
            i->file_handler = 0;
        }
        delete i;
    }
    field_B8.clear();

    Task::SetDest();
}

void SkinParamManager::sub_14060FBD0() {
    if (!field_70.size())
        return;

    data_struct* data = rctx_ptr->data;
    object_database* obj_db = &data->data_ft.obj_db;

    field_B8.clear();
    for (struc_462& i : field_70) {
        rob_chara_item_equip* rob_itm_equip = i.rob_chr->item_equip;
        for (int32_t j = ITEM_KAMI; j < ITEM_MAX; j++) {
            object_info v9 = rob_chara_item_equip_get_object_info(rob_itm_equip, (item_id)j);
            if (!v9.is_null())
                continue;

            const char* v11 = obj_db->get_object_name(v9);
            if (!v11)
                continue;

            obj_skin* skin = object_storage_get_obj_skin(v9);
            if (!skin || !skin->ex_data_init)
                return;

            obj_skin_ex_data* ex_data = &skin->ex_data;
            int32_t osage_count = 0;
            int32_t cloth_count = 0;

            obj_skin_block* b = ex_data->blocks;
            for (int32_t k = ex_data->blocks_count; k; k--, b++)
                if (b->type == OBJ_SKIN_BLOCK_OSAGE)
                    osage_count++;
                else if (b->type == OBJ_SKIN_BLOCK_CLOTH)
                    cloth_count++;

            if (!ex_data || !cloth_count + osage_count)
                continue;

            std::string v64 = "ext_skp_" + std::string(v11) + ".txt";
            struc_461* v19 = new struc_461;
            if (!v19)
                continue;

            v19->id = (item_id)j;
            v19->rob_chr = i.rob_chr;
            v19->field_C = -1;

            bool v17 = false;
            std::string v61;
            std::string v62;
            if (i.name.size())
                //v19->field_C = sub_140610A90(this, &i, &v64, &i.name, &v61);

            if (v19->field_C == -1) {
                std::string v63;
                //v19->field_C = sub_14060B720(sp_skp_db_data_get(), &i, &v9, &v62, &v63);
                if (v19->field_C == -1) {
                    delete v19;
                    continue;
                }

                if (v19->field_C == 0 || v19->field_C == 1) {
                    v17 = true;
                    v61 = "rom/skin_param/" + v63;
                }
                else if (v19->field_C == 2) {
                    v61 = "rom/skin_param/" + v62;
                }
            }

            std::vector<struc_571>* v31 = 0;
            if (v19->field_C == 0) {
                std::pair<object_info, std::pair<int32_t, int32_t>> v49 = { v9, { i.motion_id, i.frame } };
                auto elem = field_88.find(v49);
                if (elem != field_88.end()) {
                    delete v19;
                    continue;
                }

                v31 = new std::vector<struc_571>;
                field_88.insert({ v49, v31 });
            }
            else if (v19->field_C == 1) {
                std::pair<object_info, std::pair<int32_t, int32_t>> v49 = { v9, { i.motion_id, -1 } };
                auto elem = field_98.find(v49);
                if (elem != field_98.end()) {
                    delete v19;
                    continue;
                }

                v31 = new std::vector<struc_571>;
                field_98.insert({ v49, v31 });
            }
            else if (v19->field_C == 2) {
                std::pair<object_info, std::pair<int32_t, int32_t>> v49 = { v9, { -1, -1 } };
                auto elem = field_A8.find(v49);
                if (elem != field_A8.end()) {
                    delete v19;
                    continue;
                }

                v31 = new std::vector<struc_571>;
                field_A8.insert({ v49, v31 });
            }

            if (!v31) {
                delete v19;
                continue;
            }

            v31->resize((size_t)cloth_count + osage_count);
            v19->field_10 = v31;

            p_file_handler* v39 = new p_file_handler;
            if (v17)
                v39->read_file(rctx_ptr->data, 0, v61.c_str(), v62.c_str(), false);
            else
                v39->read_file(rctx_ptr->data, 0, v61.c_str());
            v19->file_handler = v39;
            field_B8.push_back(v19);
        }
    }
}

bool SkinParamManager::sub_1406112F0() {
    return false;
}

TaskRobBase::TaskRobBase() {

}

TaskRobBase::~TaskRobBase() {

}

bool TaskRobBase::Init() {
    return true;
}

bool TaskRobBase::Ctrl() {
    if (pv_osage_manager_array_ptr_get_disp())
        return false;

    for (rob_chara*& i : init_chara) {
        rob_base_rob_chara_init(i);
        AppendCtrlCharaList(i);
    }
    init_chara.clear();

    for (rob_chara*& i : ctrl_chara)
        if (i)
            rob_thread_handler->AppendRobCharaFunc(i->chara_id,
                i, rob_base_rob_chara_ctrl_thread_main);

    rob_thread_handler->sub_14054E3F0();

    for (rob_chara*& i : ctrl_chara)
        if (i && i->data.field_3 > -1)
            rob_base_rob_chara_ctrl(i);

    for (rob_chara*& i : free_chara)
        if (i)
            rob_base_rob_chara_free(i);
    free_chara.clear();
    return false;
}

bool TaskRobBase::Dest() {
    if (pv_osage_manager_array_ptr_get_disp()) {
        pv_osage_manager_array_ptr_set_not_reset_true();
        return false;
    }

    init_chara.clear();

    for (rob_chara*& i : ctrl_chara)
        AppendFreeCharaList(i);
    ctrl_chara.clear();

    for (rob_chara*& i : free_chara)
        rob_base_rob_chara_free(i);
    free_chara.clear();

    FreeCharaLists();
    return true;
}

bool TaskRobBase::CheckType(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobBase::IsFrameDependent() {
    return true;
}

TaskRobCollision::TaskRobCollision() {

}

TaskRobCollision::~TaskRobCollision() {

}

bool TaskRobCollision::Init() {
    return true;
}

bool TaskRobCollision::Ctrl() {
    for (rob_chara*& i : init_chara)
        AppendCtrlCharaList(i);
    init_chara.clear();

    /*if (sub_140227DD0())*/ {
        for (rob_chara*& i : ctrl_chara) {
            sub_140509D30(i);
            //sub_14050F4D0(i);
            //sub_14050E930(i);
        }

        /*for (rob_chara*& i : ctrl_chara)
            if (i && i->data.field_3 > -1)
                sub_14050EA90(v13);*/
    }

    free_chara.clear();
    return false;
}

bool TaskRobCollision::Dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobCollision::CheckType(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobCollision::IsFrameDependent() {
    return true;
}

TaskRobDisp::TaskRobDisp() {

}

TaskRobDisp::~TaskRobDisp() {

}

bool TaskRobDisp::Init() {
    return true;
}

bool TaskRobDisp::Ctrl() {
    if (pv_osage_manager_array_ptr_get_disp())
        return false;

    data_struct* data = rctx_ptr->data;
    bone_database* bone_data = &data->data_ft.bone_data;
    object_database* obj_db = &data->data_ft.obj_db;
    for (rob_chara*& i : init_chara) {
        rob_disp_rob_chara_init(i, bone_data, data, obj_db);
        AppendCtrlCharaList(i);
    }
    init_chara.clear();

    for (rob_chara*& i : ctrl_chara)
        if (i && i->data.field_3 > -1)
            rob_disp_rob_chara_ctrl(i);

    for (rob_chara*& i : ctrl_chara)
        if (i && i->data.field_3 > -1)
            rob_thread_handler->AppendRobCharaFunc(i->chara_id, i,
                rob_disp_rob_chara_ctrl_thread_main);

    rob_thread_handler->sub_14054E3F0();

    for (rob_chara*& i : free_chara)
        rob_disp_rob_chara_free(i);
    free_chara.clear();
    return false;
}

bool TaskRobDisp::Dest() {
    if (pv_osage_manager_array_ptr_get_disp()) {
        pv_osage_manager_array_ptr_set_not_reset_true();
        return false;
    }
    /*else if (sub_14047A430()) {
        sub_140475B30();
        return false;
    }*/

    init_chara.clear();

    for (rob_chara*& i : ctrl_chara)
        AppendFreeCharaList(i);
    ctrl_chara.clear();

    for (rob_chara*& i : free_chara)
        rob_disp_rob_chara_free(i);
    free_chara.clear();

    FreeCharaLists();
    return true;
}

void TaskRobDisp::Disp() {
    for (rob_chara*& i : ctrl_chara) {
        if (!i)
            continue;

        int32_t chara_id = i->chara_id;
        if (pv_osage_manager_array_get_disp(&chara_id))
            continue;

        if (i->data.field_0 & 1 && i->data.field_3 > -1)
            rob_disp_rob_chara_disp(i);
    }
}

bool TaskRobDisp::CheckType(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_3;
}

bool TaskRobDisp::IsFrameDependent() {
    return false;
}

TaskRobInfo::TaskRobInfo() {

}

TaskRobInfo::~TaskRobInfo() {

}

bool TaskRobInfo::Init() {
    return true;
}

bool TaskRobInfo::Ctrl() {
    for (rob_chara*& i : init_chara)
        AppendCtrlCharaList(i);
    init_chara.clear();

    /*for (rob_chara*& i : ctrl_chara)
        if (i && i->data.field_3 > -1)
            sub_140518190(i);*/

    free_chara.clear();
    return false;
}

bool TaskRobInfo::Dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobInfo::CheckType(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobInfo::IsFrameDependent() {
    return true;
}

TaskRobLoad::TaskRobLoad() : field_68(), field_6C(), field_F0() {

}

TaskRobLoad::~TaskRobLoad() {

}

bool TaskRobLoad::Init() {
    field_F0 = 0;
    return true;
}

bool TaskRobLoad::Ctrl() {
    int32_t v0 = field_F0;
    while (v0 != -1) {
        field_F0 = v0;
        switch (v0) {
        case 0:
            v0 = CtrlFunc1();
            break;
        case 1:
            v0 = CtrlFunc2();
            break;
        case 2:
            v0 = CtrlFunc3();
            break;
        case 3:
            v0 = CtrlFunc4();
            break;
        }
    }
    return false;
}

bool TaskRobLoad::Dest() {
    if (pv_osage_manager_array_ptr_get_disp()) {
        pv_osage_manager_array_ptr_set_not_reset_true();
        return false;
    }
    /*else if (sub_14047A430()) {
        sub_140475B30();
        return false;
    }*/

    if (field_68)
        field_68 = 0;

    UnloadLoadedChara();
    ResetReqDataObj();
    ResetReqData();
    field_F0 = -1;
    return true;
}

bool TaskRobLoad::AppendFreeReqData(chara_index chara_index) {
    if (chara_index < CHARA_MIKU || chara_index > CHARA_TETO)
        return false;

    for (std::list<ReqData>::iterator i = load_req_data.end(); i != load_req_data.begin(); ) {
        i--;
        if (i->chara_index == chara_index) {
            i = load_req_data.erase(i);
            return true;
        }
    }

    ReqData value;
    value.chara_index = chara_index;
    free_req_data.push_back(value);
    return true;
}

bool TaskRobLoad::AppendFreeReqDataObj(chara_index chara_index, item_cos_data* cos) {
    if (chara_index < CHARA_MIKU || chara_index > CHARA_TETO)
        return false;

    for (std::list<ReqDataObj>::iterator i = load_req_data_obj.end(); i != load_req_data_obj.begin(); ) {
        i--;
        if (i->chara_index == chara_index
            && !memcmp(&i->cos, cos, sizeof(item_cos_data))) {
            i = load_req_data_obj.erase(i);
            return true;
        }
    }

    ReqDataObj value;
    value.chara_index = chara_index;
    value.cos = *cos;
    free_req_data_obj.push_back(value);
    return true;
}

bool TaskRobLoad::AppendLoadReqData(chara_index chara_index) {
    if (chara_index < CHARA_MIKU || chara_index > CHARA_TETO)
        return false;

    for (std::list<ReqData>::iterator i = free_req_data.end(); i != free_req_data.begin(); ) {
        i--;
        if (i->chara_index == chara_index) {
            i = free_req_data.erase(i);
            return true;
        }
    }

    ReqData value;
    value.chara_index = chara_index;
    value.count = 1;
    load_req_data.push_back(value);
    return true;
}

bool TaskRobLoad::AppendLoadReqDataObj(chara_index chara_index, item_cos_data* cos) {
    if (chara_index < CHARA_MIKU || chara_index > CHARA_TETO)
        return false;

    for (std::list<ReqDataObj>::iterator i = free_req_data_obj.end(); i != free_req_data_obj.begin(); ) {
        i--;
        if (i->chara_index == chara_index
            && !memcmp(&i->cos, cos, sizeof(item_cos_data))) {
            i = free_req_data_obj.erase(i);
            return true;
        }
    }

    ReqDataObj value;
    value.chara_index = chara_index;
    value.cos = *cos;
    value.count = 1;
    load_req_data_obj.push_back(value);
    return true;
}

void TaskRobLoad::AppendLoadedReqData(ReqData* req_data) {
    for (ReqData& i : loaded_req_data)
        if (i.chara_index == req_data->chara_index) {
            if (i.count != 0x7FFFFFFF)
                i.count++;
            return;
        }

    loaded_req_data.push_back(*req_data);
}

void TaskRobLoad::AppendLoadedReqDataObj(ReqDataObj* req_data_obj) {
    for (ReqDataObj& i : loaded_req_data_obj)
        if (i.chara_index == req_data_obj->chara_index
            && !memcmp(&i.cos, &req_data_obj->cos, sizeof(item_cos_data))) {
            if (i.count != 0x7FFFFFFF)
                i.count++;
            return;
        }

    loaded_req_data_obj.push_back(*req_data_obj);
}

int32_t TaskRobLoad::CtrlFunc1() {
    field_68 = true;
    return 1;
}

int32_t TaskRobLoad::CtrlFunc2() {
    return 2;
}

int32_t TaskRobLoad::CtrlFunc3() {
    int32_t ret = -1;
    if (free_req_data_obj.size()) {
        for (ReqDataObj& i : free_req_data_obj) {
            UnloadCharaItemsParent(i.chara_index, &i.cos);
            FreeLoadedReqDataObj(&i);
        }
        free_req_data_obj.clear();
    }

    if (free_req_data.size()) {
        for (ReqData& i : free_req_data) {
            UnloadCharaObjSetMotionSet(i.chara_index);
            FreeLoadedReqData(&i);
        }
        free_req_data.clear();
    }

    data_struct* data = rctx_ptr->data;
    motion_database* mot_db = &data->data_ft.mot_db;
    object_database* obj_db = &data->data_ft.obj_db;

    if (load_req_data_obj.size()) {
        for (ReqDataObj& i : load_req_data_obj) {
            AppendLoadedReqDataObj(&i);
            LoadCharaItemsParent(i.chara_index, &i.cos, data, obj_db);
            load_item_req_data_obj.push_back(i);
        }
        load_req_data_obj.clear();
        ret = 3;
    }

    if (load_req_data.size()) {
        for (ReqData& i : load_req_data) {
            AppendLoadedReqData(&i);
            LoadCharaObjSetMotionSet(i.chara_index, data, obj_db, mot_db);
            load_item_req_data.push_back(i);
        }
        load_req_data.clear();
        ret = 3;
    }
    return ret;
}

int32_t TaskRobLoad::CtrlFunc4() {
    if (load_item_req_data_obj.size()) {
        for (ReqDataObj& i : load_item_req_data_obj)
            if (LoadCharaItemsCheckNotReadParent(i.chara_index, &i.cos))
                return -1;
        load_item_req_data_obj.clear();
    }
    if (!load_item_req_data.size())
        return 2;

    if (load_item_req_data.size()) {
        for (ReqData& i : load_item_req_data)
            if (LoadCharaObjSetMotionSetCheck(i.chara_index))
                return -1;
        load_item_req_data.clear();
    }
    if (!load_item_req_data.size())
        return 2;

    return -1;
}

void TaskRobLoad::FreeLoadedReqData(ReqData* req_data) {
    for (std::list<ReqData>::iterator i = loaded_req_data.begin(); i != loaded_req_data.end(); i++)
        if (i->chara_index == req_data->chara_index) {
            if (i->count > 0)
                i->count--;

            if (!i->count)
                i = loaded_req_data.erase(i);
            return;
        }
}

void TaskRobLoad::FreeLoadedReqDataObj(ReqDataObj* req_data_obj) {
    for (std::list<ReqDataObj>::iterator i = loaded_req_data_obj.begin(); i != loaded_req_data_obj.end(); i++)
        if (i->chara_index == req_data_obj->chara_index
            && !memcmp(&i->cos, &req_data_obj->cos, sizeof(item_cos_data))) {
            if (i->count > 0)
                i->count--;

            if (!i->count)
                i = loaded_req_data_obj.erase(i);
            return;
        }
}

void TaskRobLoad::LoadCharaItem(chara_index chara_index,
    int32_t item_no, void* data, object_database* obj_db) {
    if (!item_no)
        return;

    std::vector<uint32_t>* item_objset = 0;
    item_table_array_get_item_objset(chara_index, item_no, &item_objset);
    if (!item_objset)
        return;

    for (uint32_t& i : *item_objset)
        if (i != (uint32_t)-1)
            object_storage_load_set(data, obj_db, i);
}

bool TaskRobLoad::LoadCharaItemCheckNotRead(chara_index chara_index, int32_t item_no) {
    if (!item_no)
        return false;

    std::vector<uint32_t>* item_objset = 0;
    item_table_array_get_item_objset(chara_index, item_no, &item_objset);
    if (!item_objset)
        return true;

    for (uint32_t& i : *item_objset)
        if (i != (uint32_t)-1 && object_storage_load_obj_set_check_not_read(i))
            return true;
    return false;
}

void TaskRobLoad::LoadCharaItems(chara_index chara_index,
    item_cos_data* cos, void* data, object_database* obj_db) {
    for (int32_t i = ITEM_SUB_ZUJO; i < ITEM_SUB_MAX; i++)
        LoadCharaItem(chara_index, cos->arr[i], data, obj_db);
}

bool TaskRobLoad::LoadCharaItemsCheckNotRead(chara_index chara_index, item_cos_data* cos) {
    for (int32_t& i : cos->arr)
        if (LoadCharaItemCheckNotRead(chara_index, i))
            return true;
    return false;
}

bool TaskRobLoad::LoadCharaItemsCheckNotReadParent(chara_index chara_index, item_cos_data* cos) {
    return TaskRobLoad::LoadCharaItemsCheckNotRead(chara_index, cos)
        || cos->data.kami == 649 && object_storage_load_obj_set_check_not_read(3291);
}

void TaskRobLoad::LoadCharaItemsParent(chara_index chara_index,
    item_cos_data* cos, void* data, object_database* obj_db) {
    LoadCharaItems(chara_index, cos, data, obj_db);
    if (cos->data.kami == 649)
        object_storage_load_set(rctx_ptr->data, obj_db, 3291);
}

void TaskRobLoad::LoadCharaObjSetMotionSet(chara_index chara_index,
    void* data, object_database* obj_db, motion_database* mot_db) {
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    object_storage_load_set(data, obj_db, chr_init_data->object_set);
    motion_set_load_motion(2, 0, mot_db);
    motion_set_load_mothead(2, 0, mot_db);
    motion_set_load_motion(chr_init_data->motion_set, 0, mot_db);
}

bool TaskRobLoad::LoadCharaObjSetMotionSetCheck(chara_index chara_index) {
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    if (object_storage_load_obj_set_check_not_read(chr_init_data->object_set)
        || motion_storage_check_mot_file_not_ready(2)
        || mothead_storage_check_mhd_file_not_ready(2))
        return true;
    return motion_storage_check_mot_file_not_ready(chr_init_data->motion_set);
}

void TaskRobLoad::ResetReqData() {
    load_req_data.clear();
    free_req_data.clear();
    load_item_req_data.clear();
    loaded_req_data.clear();
}

void TaskRobLoad::ResetReqDataObj() {
    load_req_data_obj.clear();
    free_req_data_obj.clear();
    load_item_req_data_obj.clear();
    loaded_req_data_obj.clear();
}

void TaskRobLoad::UnloadCharaItem(chara_index chara_index, int32_t item_no) {
    if (!item_no)
        return;

    std::vector<uint32_t>* item_objset = 0;
    item_table_array_get_item_objset(chara_index, item_no, &item_objset);
    if (!item_objset)
        return;

    for (uint32_t& i : *item_objset)
        if (i != (uint32_t)-1)
            object_storage_unload_set(i);
}

void TaskRobLoad::UnloadCharaItems(chara_index chara_index, item_cos_data* cos) {
    for (int32_t i = ITEM_SUB_ZUJO; i < ITEM_SUB_MAX; i++)
        UnloadCharaItem(chara_index, cos->arr[i]);
}

void TaskRobLoad::UnloadCharaItemsParent(chara_index chara_index, item_cos_data* cos) {
    if (cos->data.kami == 649)
        object_storage_unload_set(3291);
    UnloadCharaItems(chara_index, cos);
}

void TaskRobLoad::UnloadCharaObjSetMotionSet(chara_index chara_index) {
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    object_storage_unload_set(chr_init_data->object_set);
    motion_set_unload_motion(2);
    motion_set_unload_mothead(2);
    motion_set_unload_motion(chr_init_data->motion_set);
}

void TaskRobLoad::UnloadLoadedChara() {
    for (ReqDataObj& i : loaded_req_data_obj)
        for (int32_t j = i.count; j; j--)
            UnloadCharaItemsParent(i.chara_index, &i.cos);

    loaded_req_data_obj.clear();

    for (ReqData& i : loaded_req_data)
        for (int32_t j = i.count; j; j--)
            UnloadCharaObjSetMotionSet(i.chara_index);

    loaded_req_data.clear();
}

TaskRobManager::TaskRobManager() : ctrl_state(), dest_state() {

}

TaskRobManager::~TaskRobManager() {

}

bool TaskRobManager::Init() {
    TaskWork::AppendTask(&task_rob_load, 0, "ROB_LOAD", 0);
    rob_manager_rob_impl* rob_impls1 = rob_manager_rob_impls1_get(this);
    for (; rob_impls1->task; rob_impls1++) {
        RobImplTask* task = rob_impls1->task;
        TaskWork::AppendTask(task, rob_impls1->name);
        task->is_frame_dependent = task->IsFrameDependent();
        task->FreeCharaLists();
    }

    rob_manager_rob_impl* rob_impls2 = rob_manager_rob_impls2_get(this);
    for (; rob_impls2->task; rob_impls2++) {
        RobImplTask* task = rob_impls2->task;
        TaskWork::AppendTask(task, rob_impls2->name);
        task->is_frame_dependent = task->IsFrameDependent();
        task->FreeCharaLists();
    }

    ctrl_state = 0;
    dest_state = 0;
    return true;
}

bool TaskRobManager::Ctrl() {
    if (!ctrl_state) {
        if (task_rob_load_check_load_req_data())
            return false;
        if (load_chara.size()) {
            data_struct* data = rctx_ptr->data;
            bone_database* bone_data = &data->data_ft.bone_data;
            motion_database* mot_db = &data->data_ft.mot_db;

            for (rob_chara*& i : load_chara) {
                i->reset_data(&i->pv_data, bone_data, mot_db);
                AppendLoadedCharaList(i);
            }
            CheckTypeAppendInitCharaLists(&load_chara);
            load_chara.clear();
        }
        ctrl_state = 1;
    }
    else if (ctrl_state != 1)
        return false;

    if (free_chara.size()) {
        for (rob_chara*& i : free_chara) {
            chara_index chara_index = i->chara_index;
            task_rob_load.AppendFreeReqData(chara_index);
            task_rob_load.AppendFreeReqDataObj(chara_index, &i->item_cos_data.cos);
            FreeLoadedCharaList(&i->chara_id);
        }

        for (rob_chara*& i : free_chara) {
            rob_chara_type type = i->type;
            for (rob_manager_rob_impl* j = rob_manager_rob_impls1_get(this); j->task; j++) {
                if (!j->task->CheckType(type))
                    continue;

                j->task->FreeInitCharaList(&i->chara_id);
                j->task->FreeCtrlCharaList(&i->chara_id);
                j->task->AppendFreeCharaList(i);
            }

            for (rob_manager_rob_impl* j = rob_manager_rob_impls2_get(this); j->task; j++) {
                if (!j->task->CheckType(type))
                    continue;

                j->task->FreeInitCharaList(&i->chara_id);
                j->task->FreeCtrlCharaList(&i->chara_id);
                j->task->AppendFreeCharaList(i);
            }
        }
        free_chara.clear();
    }

    if (init_chara.size()) {
        for (rob_chara*& i : init_chara) {
            if (CheckHasRobCharaLoad(i))
                continue;

            load_chara.push_back(i);
            chara_index chara_index = i->chara_index;
            task_rob_load.AppendLoadReqData(chara_index);
            task_rob_load.AppendLoadReqDataObj(chara_index, &i->item_cos_data.cos);
        }
        init_chara.clear();
        ctrl_state = 0;
    }
    return false;
}

bool TaskRobManager::Dest() {
    if (dest_state == 0) {
        rob_manager_rob_impl* rob_impls1 = rob_manager_rob_impls1_get(this);
        for (; rob_impls1->task; rob_impls1++)
            rob_impls1->task->SetDest();

        rob_manager_rob_impl* rob_impls2 = rob_manager_rob_impls2_get(this);
        for (; rob_impls2->task; rob_impls2++)
            rob_impls2->task->SetDest();

        init_chara.clear();
        load_chara.clear();
        free_chara.clear();
        loaded_chara.clear();
        dest_state = 1;
    }
    else if (dest_state != 1)
        return false;

    task_rob_load.SetDest();
    dest_state = 2;
    return true;
}

void TaskRobManager::Disp() {

}

void TaskRobManager::AppendFreeCharaList(rob_chara* rob_chr) {
    if (!rob_chr || rob_chr->chara_id >= ROB_CHARA_COUNT || free_chara.size() >= ROB_CHARA_COUNT)
        return;

    int32_t chara_id = rob_chr->chara_id;
    for (std::list<rob_chara*>::iterator i = init_chara.begin(); i != init_chara.end();)
        if (i._Ptr->_Myval->chara_id == chara_id) {
            i = init_chara.erase(i);
            return;
        }
        else
            i++;

    bool loaded_chara_found = false;
    for (rob_chara*& i : loaded_chara)
        if (i->chara_id == chara_id) {
            loaded_chara_found = true;
            break;
        }

    bool load_chara_found = false;
    for (rob_chara*& i : load_chara)
        if (i->chara_id == chara_id) {
            load_chara_found = true;
            break;
        }

    if (!loaded_chara_found && !load_chara_found)
        return;

    bool free_chara_found = false;
    for (rob_chara*& i : free_chara)
        if (i->chara_id == chara_id) {
            free_chara_found = true;
            break;
        }

    if (!free_chara_found)
        free_chara.push_back(rob_chr);
}

void TaskRobManager::AppendInitCharaList(rob_chara* rob_chr) {
    if (!rob_chr || rob_chr->chara_id >= ROB_CHARA_COUNT || init_chara.size() >= ROB_CHARA_COUNT)
        return;

    int32_t chara_id = rob_chr->chara_id;
    bool loaded_chara_found = false;
    for (rob_chara*& i : loaded_chara)
        if (i->chara_id == chara_id) {
            loaded_chara_found = true;
            break;
        }

    bool load_chara_found = false;
    for (rob_chara*& i : load_chara)
        if (i->chara_id == chara_id) {
            load_chara_found = true;
            break;
        }

    if (loaded_chara_found || load_chara_found)
        return;

    bool init_chara_found = false;
    for (rob_chara*& i : init_chara)
        if (i->chara_id == chara_id) {
            init_chara_found = true;
            break;
        }

    if (!init_chara_found)
        init_chara.push_back(rob_chr);
}

void TaskRobManager::AppendLoadedCharaList(rob_chara* rob_chr) {
    if (!rob_chr || rob_chr->chara_id >= ROB_CHARA_COUNT || loaded_chara.size() >= ROB_CHARA_COUNT)
        return;

    int32_t chara_id = rob_chr->chara_id;
    bool loaded_chara_found = false;
    for (rob_chara*& i : loaded_chara)
        if (i->chara_id == chara_id) {
            loaded_chara_found = true;
            break;
        }

    if (!loaded_chara_found)
        loaded_chara.push_back(rob_chr);
}

bool TaskRobManager::CheckCharaLoaded(rob_chara* rob_chr) {
    if (!TaskWork::CheckTaskReady(this)
        || rob_chr->chara_id < 0 || rob_chr->chara_id >= ROB_CHARA_COUNT)
        return false;

    int8_t chara_id  = rob_chr->chara_id;
    for (rob_chara*& i : free_chara)
        if (i->chara_id == chara_id)
            return false;

    for (rob_chara*& i : loaded_chara)
        if (i->chara_id == chara_id)
            return true;
    return false;
}

bool TaskRobManager::CheckHasRobCharaLoad(rob_chara* rob_chr) {
    if (!rob_chr || rob_chr->chara_id >= ROB_CHARA_COUNT || !load_chara.size())
        return false;

    int32_t chara_id = rob_chr->chara_id;
    for (rob_chara*& i : load_chara)
        if (i->chara_id == chara_id)
            return true;

    return false;
}

void TaskRobManager::CheckTypeAppendInitCharaLists(std::list<rob_chara*>* rob_chr_list) {
    for (rob_chara*& i : *rob_chr_list) {
        rob_chara_type type = i->type;
        rob_manager_rob_impl* rob_impls1 = rob_manager_rob_impls1_get(this);
        for (; rob_impls1->task; rob_impls1++) {
            RobImplTask* task = rob_impls1->task;
            if (task->CheckType(type))
                task->AppendInitCharaList(i);
        }

        rob_manager_rob_impl* rob_impls2 = rob_manager_rob_impls2_get(this);
        for (; rob_impls2->task; rob_impls2++) {
            RobImplTask* task = rob_impls2->task;
            if (task->CheckType(type))
                task->AppendInitCharaList(i);
        }
    }
}

void TaskRobManager::FreeLoadedCharaList(int8_t* chara_id) {
    if (!chara_id || *chara_id >= ROB_CHARA_COUNT || !loaded_chara.size())
        return;

    for (std::list<rob_chara*>::iterator i = loaded_chara.begin(); i != loaded_chara.end();)
        if (i._Ptr->_Myval->chara_id == *chara_id) {
            i = loaded_chara.erase(i);
            return;
        }
        else
            i++;
}

TaskRobMotionModifier::TaskRobMotionModifier() {

}

TaskRobMotionModifier::~TaskRobMotionModifier() {

}

bool TaskRobMotionModifier::Init() {
    return true;
}

bool TaskRobMotionModifier::Ctrl() {
    if (pv_osage_manager_array_ptr_get_disp())
        return false;

    for (rob_chara*& i : init_chara)
        if (i)
            AppendCtrlCharaList(i);
    init_chara.clear();

    /*for (rob_chara*& i : ctrl_chara)
        if (i)
            sub_1405484A0(i);*/

    for (rob_chara*& i : ctrl_chara)
        if (i)
            rob_motion_modifier_rob_chara_ctrl(i);

    free_chara.clear();
    return false;
}

bool TaskRobMotionModifier::Dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobMotionModifier::CheckType(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobMotionModifier::IsFrameDependent() {
    return true;
}

TaskRobPrepareAction::TaskRobPrepareAction() {

}

TaskRobPrepareAction::~TaskRobPrepareAction() {

}

bool TaskRobPrepareAction::Init() {
    return true;
}

bool TaskRobPrepareAction::Ctrl() {
    return false;
}

bool TaskRobPrepareAction::Dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobPrepareAction::CheckType(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobPrepareAction::IsFrameDependent() {
    return true;
}

TaskRobPrepareControl::TaskRobPrepareControl() {

}

TaskRobPrepareControl::~TaskRobPrepareControl() {

}

bool TaskRobPrepareControl::Init() {
    return true;
}

static void sub_1404156B0(mot_play_data* a1) {
    mot_play_frame_data* frame_data = &a1->frame_data;
    frame_data->field_28 = 0;
    if (a1->field_34 && a1->field_38 >= 0.0f)
        a1->field_38 = a1->field_38 - frame_data->step;

    switch (frame_data->field_18) {
    case 1: {
        frame_data->frame += frame_data->step;
        if (frame_data->field_14 >= 0.0f && frame_data->frame > frame_data->field_14)
            frame_data->frame = frame_data->field_14;

        float_t last_frame = frame_data->last_frame;
        switch (frame_data->field_1C) {
        case 0:
        default:
            frame_data->frame = min(frame_data->frame, frame_data->frame_count);
            break;
        case 1: {
            if (frame_data->field_24 >= 0.0f && frame_data->field_24 <= last_frame)
                last_frame = frame_data->field_24;

            if (frame_data->frame >= last_frame) {
                if (frame_data->field_2C > 0 && frame_data->field_2C <= a1->field_30) {
                    frame_data->field_1C = -1;
                    return;
                }

                float_t v21 = frame_data->frame - last_frame;
                float_t v22 = last_frame - frame_data->field_20;
                if (v21 >= v22 && v22 > 0.0f)
                    v21 = fmodf(v21, v22);
                frame_data->frame = v21 + frame_data->field_20;
                frame_data->field_28 = true;
                a1->field_30++;
            }
        } break;
        case 2: {
            if (frame_data->field_24 >= 0.0f && frame_data->field_24 <= last_frame)
                last_frame = frame_data->field_24;
            if (last_frame < frame_data->frame) {
                if (frame_data->field_2C > 0 && frame_data->field_2C <= a1->field_30) {
                    frame_data->field_1C = -1;
                    return;
                }

                frame_data->frame = frame_data->field_20;
                frame_data->field_28 = true;
                a1->field_30++;
            }
        } break;
        case 3: {
            if (frame_data->frame <= last_frame)
                return;

            if (frame_data->field_2C > 0 && a1->field_30 <= frame_data->field_2C) {
                frame_data->field_1C = -1;
                return;
            }

            float_t frame = frame_data->frame - last_frame;
            if (frame >= last_frame && last_frame > 0.0f)
                frame = fmodf(frame, last_frame);
            frame_data->frame = last_frame - frame;
            frame_data->field_18 = 2;
            frame_data->field_28 = true;
            a1->field_30++;
        } break;
        }
    } break;
    case 2: {
        float_t last_frame = frame_data->last_frame;
        frame_data->frame -= frame_data->step;
        float_t v24 = frame_data->frame;
        switch (frame_data->field_1C) {
        case 0:
        default: {
            if (v24 < 0.0f)
                frame_data->frame = 0.0f;
        } break;
        case 1: {
            float_t v23 = 0.0f;
            if (frame_data->field_20 >= 0.0f)
                v23 = frame_data->field_20;
            if (v24 <= v23) {
                if (frame_data->field_2C > 0 && frame_data->field_2C <= a1->field_30) {
                    frame_data->field_1C = -1;
                    return;
                }

                float_t v36 = last_frame;
                if (frame_data->field_24 >= 0.0f && frame_data->field_24 <= v36)
                    v36 = frame_data->field_24;

                float_t v37 = v36 - v23;
                float_t v38 = v23 - v24;
                if (v38 >= v37 && v37 > 0.0f)
                    v38 = fmodf(v38, v37);
                frame_data->frame = v36 - v38;
                frame_data->field_28 = true;
                a1->field_30++;
            }
        } break;
        case 2: {
            float_t v23 = 0.0f;
            if (frame_data->field_20 >= 0.0f)
                v23 = frame_data->field_20;
            if (v24 < v23) {
                if (frame_data->field_2C > 0 && frame_data->field_2C <= a1->field_30) {
                    frame_data->field_1C = -1;
                    return;
                }

                float_t v33 = last_frame;
                if (frame_data->field_24 >= 0.0f && frame_data->field_24 <= last_frame)
                    v33 = frame_data->field_24;
                frame_data->frame = v33;
                frame_data->field_28 = true;
                a1->field_30++;
            }
        } break;
        case 3: {
            if (v24 >= 0.0f)
                return;

            if (frame_data->field_2C > 0 && frame_data->field_2C <= a1->field_30) {
                frame_data->field_1C = -1;
                return;
            }

            float_t v29 = -v24;
            if (last_frame > 0.0f && v29 >= last_frame)
                v29 = fmodf(v29, last_frame);
            frame_data->frame = v29;
            frame_data->field_18 = 1;
            frame_data->field_28 = true;
            a1->field_30++;
        } break;
        }
    } break;
    case 3: {
        if (a1->field_40)
            frame_data->frame = *a1->field_40;
        if (a1->field_48)
            frame_data->step = *a1->field_48;
    } break;
    }
}

static void sub_14041DBA0(rob_chara_bone_data* rob_bone_data) {
    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        sub_1404156B0(&i->mot_play_data);
}

static void sub_14053F300(RobPartialMotion* a1) {
    if (!sub_14053F2B0(a1))
        return;

    if (sub_14053F280(a1)) {
        float_t frame = a1->data.frame + a1->data.play_frame_step;
        float_t last_frame = a1->data.frame_count - 1.0f;
        switch (a1->data.field_38) {
        case 0:
            if (frame > last_frame)
                frame = last_frame;
            break;
        case 1:
            if (frame >= last_frame)
                frame = fmodf(frame - last_frame, last_frame);
            break;
        case 2:
            if (frame > last_frame)
                frame = 0.0f;
            break;
        case 3:
            if (frame > last_frame) {
                a1->data.state = 4;
                frame = last_frame - fmodf(frame - last_frame, last_frame);
            }
            break;
        }
        a1->data.frame = frame;
    }
    else if (sub_14053F270(a1)) {
        float_t frame = a1->data.frame - a1->data.play_frame_step;
        float_t last_frame = a1->data.frame_count - 1.0f;
        switch (a1->data.field_38) {
        case 0:
            if (frame < 0.0f)
                frame = 0.0f;
            break;
        case 1:
            if (frame <= 0.0f)
                frame = last_frame - fmodf(-frame, last_frame);
            break;
        case 2:
            if (frame < 0.0f)
                frame = last_frame;
            break;
        case 3:
            if (frame < 0.0f) {
                a1->data.state = 2;
                frame = fmodf(-frame, last_frame);
            }
            break;
        }
        a1->data.frame = frame;
    }
    else if (sub_14053F2A0(a1)) {
        if (a1->data.field_24 >= 0.0f)
            a1->data.field_24 -= a1->data.play_frame_step;
    }
    else if (sub_14053F290(a1)) {
        if (a1->data.frame_data)
            a1->data.frame = a1->data.frame_data->frame;
        if (a1->data.step_data)
            a1->data.play_frame_step = a1->data.step_data->frame;
    }
}

static void sub_140548660(rob_chara* rob_chr) {
    rob_chr->data.motion.frame_data.prev_frame = rob_chr->data.motion.frame_data.frame;
    sub_14041DBA0(rob_chr->bone_data);
    if (rob_chr->data.motion.field_28 < 0)
        rob_chara_bone_data_set_frame(rob_chr->bone_data, rob_chr->data.motion.frame_data.last_set_frame);
    rob_chr->data.motion.frame_data.frame = rob_chara_bone_data_get_frame(rob_chr->bone_data);
    sub_14053F300(&rob_chr->data.motion.field_150.face);
    sub_14053F300(&rob_chr->data.motion.field_150.hand_l);
    sub_14053F300(&rob_chr->data.motion.field_150.hand_r);
    sub_14053F300(&rob_chr->data.motion.field_150.mouth);
    sub_14053F300(&rob_chr->data.motion.field_150.eye);
    sub_14053F300(&rob_chr->data.motion.field_150.eyelid);
    sub_14053F300(&rob_chr->data.motion.field_3B0.face);
    sub_14053F300(&rob_chr->data.motion.field_3B0.hand_l);
    sub_14053F300(&rob_chr->data.motion.field_3B0.hand_r);
    sub_14053F300(&rob_chr->data.motion.field_3B0.mouth);
    sub_14053F300(&rob_chr->data.motion.field_3B0.eye);
    sub_14053F300(&rob_chr->data.motion.field_3B0.eyelid);
}

static void sub_140548460(rob_chara* rob_chr) {
    rob_chr->data.field_2 ^= (rob_chr->data.field_2 ^ (2 * rob_chr->data.field_2)) & 0x10;
    if (rob_chr->data.field_1588.field_0.field_20.field_0 & 0x01)
        rob_chr->data.field_2 |= 0x08;
    else
        rob_chr->data.field_2 &= ~0x08;
}

bool TaskRobPrepareControl::Ctrl() {
    if (pv_osage_manager_array_ptr_get_disp())
        return false;

    for (rob_chara*& i : init_chara)
        if (i)
            AppendCtrlCharaList(i);
    init_chara.clear();

    /*for (rob_chara*& i : ctrl_chara)
        if (i)
            sub_1405484A0(i);*/

    for (rob_chara*& i : ctrl_chara)
        if (i) {
            sub_140548660(i);
            sub_140548460(i);
        }

    free_chara.clear();
    return false;
}

bool TaskRobPrepareControl::Dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobPrepareControl::CheckType(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobPrepareControl::IsFrameDependent() {
    return true;
}
