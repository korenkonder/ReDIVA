/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob.hpp"
#include "../../KKdLib/io/file_stream.hpp"
#include "../../KKdLib/io/json.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/prj/algorithm.hpp"
#include "../../KKdLib/key_val.hpp"
#include "../../KKdLib/msgpack.hpp"
#include "../../KKdLib/sort.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../KKdLib/waitable_timer.hpp"
#include "../data.hpp"
#include "../mdata_manager.hpp"
#include "../pv_db.hpp"
#include "../random.hpp"
#include "../pv_expression.hpp"
#include "../stage.hpp"
#include "motion.hpp"
#include "skin_param.hpp"

enum rob_sleeve_type : uint8_t {
    ROB_SLEEVE_L  = 0x01,
    ROB_SLEEVE_R  = 0x02,

    ROB_SLEEVE_LR = 0x03,
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
    void ParseFile(const void* data);
    mothead* ParseMothead(mothead_file* mhdsf, size_t data);
    mothead_mot* ParseMotheadMot(mothead_mot_file* mhdsf, size_t data);
    bool Unload();

    static void ParseFileParent(MhdFile* mhd, const void* file_data, size_t size);
};

struct OpdMaker {
    struct Data {
        bool empty;
        std::map<std::pair<object_info, int32_t>, p_file_handler*> opdi_files;

        Data();
        ~Data();

        bool CheckOpdiFilesNotReady();
        const void* GetOpdiFileData(object_info obj_info, uint32_t motion_id);
        bool IsValidOpdiFile(rob_chara* rob_chr, uint32_t motion_id);
        void Reset();
    };

    rob_chara* rob_chr;
    std::vector<int32_t>* motion_ids;
    bool waiting;
    uint32_t index;
    bool field_18;
    OpdMaker::Data* data;
    std::thread* thread;
    std::mutex waiting_mtx;
    std::mutex field_38;
    std::mutex field_40;

    OpdMaker();
    virtual ~OpdMaker();

    void Ctrl();
    bool InitThread(rob_chara* rob_chr, std::vector<int32_t>* motion_ids, OpdMaker::Data* data);
    bool IsWaiting();
    void Reset();
    bool SetOsagePlayInitData(uint32_t motion_id);
    void SetWaiting(bool value);

    void sub_140475AE0();
    bool sub_140478330();

    static void ThreadMain(OpdMaker* opd_maker);
};

struct OpdMakeWorker : public app::Task {
    int32_t state;
    int32_t chara_id;
    int32_t field_70[25];
    bool field_D4;
    OpdMaker::Data data;

    OpdMakeWorker();
    virtual ~OpdMakeWorker() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    bool AddTask(bool a2);
};

struct osage_play_data_init_header {
    uint32_t signature;
    std::pair<uint16_t, uint16_t> obj_info;
    uint32_t motion_id;
    uint16_t nodes_count;
};

struct osage_play_data_init_node {
    vec3 trans;
    vec3 trans_diff;
};

struct osage_play_data_header {
    uint32_t signature;
    std::pair<uint16_t, uint16_t> obj_info;
    uint32_t motion_id;
    uint32_t frame_count;
    uint16_t nodes_count;

    osage_play_data_header();
};

struct osage_play_data_node_header {
    uint16_t count;
    int16_t shift;
#pragma warning(suppress: 4200)
    int16_t data[];
};

struct opd_file_data {
    osage_play_data_header head;
    float_t* data;
    int32_t load_count;

    void unload();
};

class OsagePlayDataManager : public app::Task {
public:
    int32_t state;
    prj::vector_pair<object_info, uint32_t> opd_req_data;
    std::list<p_file_handler*> file_handlers;
    std::map<std::pair<object_info, uint32_t>, opd_file_data> opd_file_data;

    OsagePlayDataManager();
    virtual ~OsagePlayDataManager() override;

    virtual bool Init() override;
    virtual bool Ctrl() override;
    virtual bool Dest() override;
    virtual void Disp() override;

    bool AddTask();
    void AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids);
    bool CheckTaskReady();
    void GetOpdFileData(object_info obj_info,
        uint32_t motion_id, const float_t*& data, uint32_t& count);
    void LoadOpdFile(p_file_handler* pfhndl);
    void LoadOpdFileList();
    void Reset();
};

struct struc_528 {
    bool field_0;
    mothead_data_type type;
    const mothead_data* data;
};

struct struc_527 {
    rob_chara* rob_chr;
    uint32_t motion_id;
    float_t frame;
    float_t last_frame;
    struc_528* type_62;
    const mothead_data* type_62_data;
    struc_528* type_75;
    const mothead_data* type_75_data;
    struc_528* type_67;
    const mothead_data* type_67_data;
    struc_528* type_74;
    const mothead_data* type_74_data;
    struc_528* type_79;
    const mothead_data* type_79_data;
    int32_t field_68;

    struc_527(rob_chara* rob_chr, int32_t stage_index, uint32_t motion_id,
        float_t frame, const bone_database* bone_data, const motion_database* mot_db);
    ~struc_527();
};

struct struc_567 {
    uint16_t year;
    uint16_t month;
    uint16_t day;
};

struct struc_568 {
    struc_567 field_0;
    int64_t field_10;
};

struct struc_656 {
    std::vector<uint32_t> field_0[25];

    struc_656();
    ~struc_656();
};

struct struc_655 {
    size_t count;
    uint32_t index;
    struc_656 field_10[10];

    struc_655();
    ~struc_655();
};

struct struc_657 {
    uint32_t mode;
    uint32_t progress;
    chara_index chara;
    std::vector<uint32_t> field_10;

    struc_657();
    ~struc_657();
};

struct struc_691 {
    uint32_t mode;
    uint32_t count;
    uint32_t index;
    std::vector<struc_657> field_10;

    struc_691();
    ~struc_691();
};

struct OpdMakeManager : app::Task {
    int32_t mode;
    struc_655 field_88;
    chara_index chara;
    std::vector<int32_t> motion_ids;
    std::vector<uint32_t> motion_set_ids;
    OpdMakeWorker* workers[4];
    struc_691 field_1860;
    bool field_1888;
    bool field_1889;

    OpdMakeManager();
    virtual ~OpdMakeManager() override;

    bool Init() override;
    bool Ctrl() override;
    bool Dest() override;
    void Disp() override;

    bool DelTask();
};

struct struc_380 {
    rob_chara* rob_chara;
    rob_chara_data* rob_chara_data;
    struc_223* field_10;
};

struct mothead_func_data {
    rob_chara* rob_chr;
    rob_chara_data* rob_chr_data;
    rob_chara_data* field_10;
    rob_chara* field_18;
    rob_chara_data* field_20;
    struc_223* field_28;
    bool is_x;
};

typedef void(*mothead_func)(mothead_func_data*, void*,
    const mothead_data*, int32_t, const motion_database*);

struct mothead_func_struct {
    mothead_func func;
    int32_t flags;
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

struct rob_cmn_mottbl_sub_header {
    int32_t data_offset;
    int32_t field_4;
};

struct rob_cmn_mottbl_header {
    int32_t chara_count;
    int32_t mottbl_indices_count;
    int32_t subheaders_offset;
};

struct rob_chara_age_age_init_data {
    vec3 trans;
    float_t rot_x;
    float_t offset;
    float_t gravity;
    float_t scale;
    float_t rot_speed;
    float_t rot_y;
    float_t appear;
    float_t life_time;
};

struct rob_chara_age_age_data {
    int32_t index;
    int32_t part_id;
    vec3 trans;
    float_t field_14;
    float_t rot_z;
    int32_t field_1C;
    float_t rot_speed;
    float_t gravity;
    float_t scale;
    float_t alpha;
    mat4 mat_scale;
    mat4 mat;
    mat4 prev_parent_mat;
    float_t remaining;
    bool alive;

    rob_chara_age_age_data();

    void reset();
};

struct rob_chara_age_age_object_vertex {
    vec3 position;
    vec3 normal;
    vec4 tangent;
    vec2 texcoord;
};

struct rob_chara_age_age_object {
    obj_set_handler* obj_set_handler;
    int32_t obj_index;
    int32_t field_C;
    int32_t num_vertex;
    int32_t num_index;
    ::obj obj;
    obj_mesh mesh;
    obj_sub_mesh sub_mesh;
    obj_material_data material[2];
    obj_axis_aligned_bounding_box axis_aligned_bounding_box;
    rob_chara_age_age_object_vertex* vertex_data;
    int32_t vertex_data_size;
    int32_t vertex_array_size;
    obj_mesh_vertex_buffer obj_vert_buf;
    obj_mesh_index_buffer obj_index_buf;
    vec3 trans[10];
    int32_t disp_count;
    int32_t count;
    bool field_C3C;

    rob_chara_age_age_object();

    void calc_vertex(rob_chara_age_age_object_vertex*& vtx_data,
        obj_mesh* mesh, const mat4& mat, float_t alpha);
    void disp(render_context* rctx, size_t chara_index,
        bool npr, bool reflect, const vec3& a5, bool chara_color);
    bool get_obj_set_handler_object_index(object_info obj_info);
    ::obj* get_obj_set_obj();
    std::vector<texture*>* get_obj_set_texture();
    void load(object_info obj_info, int32_t count);
    void reset();
    void update(rob_chara_age_age_data* data, int32_t count, float_t alpha);

    static obj_bounding_sphere combine_bounding_spheres(
        obj_bounding_sphere* src0, obj_bounding_sphere* src1);
};

struct rob_chara_age_age {
    rob_chara_age_age_data data[10];
    float_t frame;
    float_t field_9B4;
    mat4 mat;
    float_t step;
    bool visible;
    bool skip;
    float_t move_cancel;
    float_t alpha;
    rob_chara_age_age_object object;
    bool npr;
    float_t rot_speed;
    bool step_full;

    rob_chara_age_age();
    ~rob_chara_age_age();

    void ctrl(mat4& mat);
    void ctrl_data(rob_chara_age_age_data* data, mat4& mat);
    void ctrl_skip();
    void disp(render_context* rctx, size_t chara_id,
        bool npr, bool reflect, const vec3& a5, bool chara_color);
    void load(object_info obj_info, rob_chara_age_age_data* data, int32_t count);
    void reset();
    void set_alpha(float_t value);
    void set_disp(bool value);
    void set_skip();
    void set_move_cancel(float_t value);
    void set_npr(bool value);
    void set_rot_speed(float_t value);
    void set_step(float_t value);
    void set_step_full();
};

struct rob_sleeve {
    rob_sleeve_type type;
    rob_sleeve_data* l;
    rob_sleeve_data* r;

    rob_sleeve();
    ~rob_sleeve();
};

struct rob_sleeve_handler {
    bool ready;
    std::list<p_file_handler*> file_handlers;
    std::map<std::pair<chara_index, int32_t>, rob_sleeve> sleeves;

    rob_sleeve_handler();
    ~rob_sleeve_handler();

    void clear();
    void get_sleeve_data(::chara_index chara_index,
        int32_t cos, rob_sleeve_data& l, rob_sleeve_data& r);
    bool load();
    void parse(p_file_handler* pfhndl);
    void parse_sleeve_data(key_val& kv, rob_sleeve_data*& data);
    void read();
};

struct osage_set_motion {
    uint32_t motion_id;
    prj::vector_pair<float_t, int32_t> frames;

    osage_set_motion();
    ~osage_set_motion();

    void init_frame(uint32_t motion_id, float_t frame, int32_t stage_index);
};

struct skeleton_rotation_offset {
    bool x;
    bool y;
    bool z;
    vec3 rotation;

    skeleton_rotation_offset();
    skeleton_rotation_offset(bool x, bool y, bool z, vec3 rotation);
};

class PvOsageManager : app::Task {
public:
    int32_t state;
    int32_t chara_id;
    bool reset;
    int32_t field_74;
    uint32_t motion_index;
    std::vector<pv_data_set_motion> pv_set_motion;
    std::vector<osage_set_motion> osage_set_motion;
    const pv_db_pv* pv;
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

    void AddMotionFrameResetData(int32_t stage_index, uint32_t motion_id, float_t frame, int32_t iterations);
    bool CheckResetFrameNotFound(uint32_t motion_id, float_t frame);
    bool GetDisp();
    bool GetNotReset();
    void Reset();
    void SetDisp(bool value);
    void SetNotReset(bool value);
    void SetPvId(int32_t pv_id, int32_t chara_id, bool reset);
    void SetPvSetMotion(const std::vector<pv_data_set_motion>& set_motion);

    void sub_1404F77E0();
    bool sub_1404F7AF0();
    void sub_1404F7BD0(bool not_reset);
    void sub_1404F82F0();
    void sub_1404F83A0(::osage_set_motion* a2);
    void sub_1404F88A0(uint32_t stage_index, uint32_t motion_id, float_t frame);
    void sub_1404F8AA0();

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

class TaskRobLoad : public app::Task {
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
    bool AppendFreeReqDataObj(chara_index chara_index, const item_cos_data* cos);
    bool AppendLoadReqData(chara_index chara_index);
    bool AppendLoadReqDataObj(chara_index chara_index, const item_cos_data* cos);
    void AppendLoadedReqData(ReqData* req_data);
    void AppendLoadedReqDataObj(ReqDataObj* req_data_obj);
    int32_t CtrlFunc1();
    int32_t CtrlFunc2();
    int32_t CtrlFunc3();
    int32_t CtrlFunc4();
    void FreeLoadedReqData(ReqData* req_data);
    void FreeLoadedReqDataObj(ReqDataObj* req_data_obj);
    void LoadCharaItem(chara_index chara_index,
        int32_t item_no, void* data, const object_database* obj_db);
    bool LoadCharaItemCheckNotRead(chara_index chara_index, int32_t item_no);
    void LoadCharaItems(chara_index chara_index,
        item_cos_data* cos, void* data, const object_database* obj_db);
    bool LoadCharaItemsCheckNotRead(chara_index chara_index, item_cos_data* cos);
    bool LoadCharaItemsCheckNotReadParent(chara_index chara_index, item_cos_data* cos);
    void LoadCharaItemsParent(chara_index chara_index,
        item_cos_data* cos, void* data, const object_database* obj_db);
    void LoadCharaObjSetMotionSet(chara_index chara_index,
        void* data, const object_database* obj_db, const motion_database* mot_db);
    bool LoadCharaObjSetMotionSetCheck(chara_index chara_index);
    void ResetReqData();
    void ResetReqDataObj();
    void UnloadCharaItem(chara_index chara_index, int32_t item_no);
    void UnloadCharaItems(chara_index chara_index, item_cos_data* cos);
    void UnloadCharaItemsParent(chara_index chara_index, item_cos_data* cos);
    void UnloadCharaObjSetMotionSet(chara_index chara_index);
    void UnloadLoadedChara();
};

class RobImplTask : public app::Task {
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

class TaskRobManager : public app::Task {
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
    bool GetWait(rob_chara* rob_chr);
};

struct rob_manager_rob_impl {
    RobImplTask* task;
    const char* name;
};

static float_t bone_data_limit_angle(float_t angle);
static void bone_data_mult_0(bone_data* a1, int32_t skeleton_select);
static void bone_data_mult_1(bone_data* a1, mat4* parent_mat, bone_data* a3, bool solve_ik);
static bool bone_data_mult_1_ik(bone_data* a1, bone_data* a2);
static void bone_data_mult_1_ik_hands(bone_data* a1, vec3* a2);
static void bone_data_mult_1_ik_hands_2(bone_data* a1, vec3* a2, float_t a3);
static void bone_data_mult_1_ik_legs(bone_data* a1, vec3* a2);
static bool bone_data_mult_1_exp_data(bone_data* a1, bone_node_expression_data* a2, bone_data* a3);
static void bone_data_mult_ik(bone_data* a1, int32_t skeleton_select);

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data);
static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    const std::vector<bone_database_bone>* bones, const vec3* common_translation, const vec3* translation);
static void bone_data_parent_load_rob_chara(bone_data_parent* bone);

static void mot_blend_interpolate(mot_blend* a1, std::vector<bone_data>* bones,
    std::vector<uint16_t>* bone_indices, bone_database_skeleton_type skeleton_type);
static void mot_blend_load_motion(mot_blend* a1, uint32_t motion_id, const motion_database* mot_db);
static void mot_blend_set_blend_duration(mot_blend* a1, float_t duration, float_t step, float_t offset);
static void mot_blend_set_frame(mot_blend* a1, float_t frame);

static void mot_key_frame_interpolate(mot* a1, float_t frame, float_t* value,
    mot_key_set* a4, uint32_t key_set_count, struc_369* a6);
static uint32_t mot_load_last_key_calc(uint16_t keys_count);
static bool mot_load_file(mot* a1, const mot_data* a2);

static void mot_key_data_get_key_set_count_by_bone_database_bones(mot_key_data* a1,
    const std::vector<bone_database_bone>* a2);
static void mot_key_data_get_key_set_count(mot_key_data* a1, size_t motion_bone_count, size_t ik_bone_count);
static void mot_key_data_init_key_sets(mot_key_data* a1, bone_database_skeleton_type type,
    size_t motion_bone_count, size_t ik_bone_count);
static void mot_key_data_interpolate(mot_key_data* a1, float_t frame,
    uint32_t key_set_offset, uint32_t key_set_count);
static const mot_data* mot_key_data_load_file(mot_key_data* a1, uint32_t motion_id, const motion_database* mot_db);
static void mot_key_data_reserve_key_sets(mot_key_data* a1);

static void mothead_func_0(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_1(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_2(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_3(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_4(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_5(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_6(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_7(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_8(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_9(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_10(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_11(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_12(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_13(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_14(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_15(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_16(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_17(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_18(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_19(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_20(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_21(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_22(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_23(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_24(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_25(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_26(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_27(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_28(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_29(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_30(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_31(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_32(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_33(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_34(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_35(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_36(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_37(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_38(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_39(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_40(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_41(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_42(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_43(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_44(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_45(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_46(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_47(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_48(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_49(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_50_set_face_motion_id(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_51(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_52(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_53_set_face_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_54_set_hand_r_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_55_set_hand_l_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_56_set_mouth_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_57_set_eyes_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_58_set_eyelid_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_59_set_rob_chara_head_object(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_60(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_61_set_eyelid_motion_from_face(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_62_rob_parts_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_63(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_64_osage_reset(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_65_motion_skin_param(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_66_osage_step(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_67_sleeve(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_68(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_69_motion_max_frame(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_70(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_71_osage_move_cancel(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_72(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_73_rob_hand_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_74(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_75_rob_adjust_global(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_76_rob_arm_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_77_disable_eye_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_78(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_79_rob_chara_coli_ring(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_80_adjust_get_global_trans(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);

static void mothead_mot_msgpack_read(const char* path, const char* set_name, mothead* mhd); // X

static void motion_blend_mot_interpolate(motion_blend_mot* a1);
static void motion_blend_mot_load_bone_data(motion_blend_mot* a1,
    rob_chara_bone_data* a2, PFNMOTIONBONECHECKFUNC bone_check_func, const bone_database* bone_data);
static bool motion_blend_mot_interpolate_get_reverse(int32_t* a1);
static void motion_blend_mot_load_file(motion_blend_mot* a1, uint32_t motion_id,
    MotionBlendType blend_type, float_t blend, const bone_database* bone_data, const motion_database* mot_db);
static void motion_blend_mot_mult_mat(motion_blend_mot* a1, mat4* mat);
static void motion_blend_mot_set_blend(motion_blend_mot* a1,
    MotionBlendType blend_type, float_t blend);
static void motion_blend_mot_set_duration(motion_blend_mot* mot,
    float_t duration, float_t step, float_t offset);

static bool pv_osage_manager_array_get_disp(int32_t* chara_id);
static PvOsageManager* pv_osage_manager_array_get(int32_t chara_id);

static int pv_data_set_motion_quicksort_compare_func(void const* src1, void const* src2);

static void rob_base_rob_chara_init(rob_chara* rob_chr);
static void rob_base_rob_chara_ctrl(rob_chara* rob_chr);
static void rob_base_rob_chara_ctrl_thread_main(rob_chara* rob_chr);
static void rob_base_rob_chara_free(rob_chara* rob_chr);

static void rob_disp_rob_chara_init(rob_chara* rob_chr,
    const bone_database* bone_data, void* data, const object_database* obj_db);
static void rob_disp_rob_chara_ctrl(rob_chara* rob_chr);
static void rob_disp_rob_chara_ctrl_thread_main(rob_chara* rob_chr);
static void rob_disp_rob_chara_disp(rob_chara* rob_chr);
static void rob_disp_rob_chara_free(rob_chara* rob_chr);

static void rob_chara_age_age_ctrl(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, mat4& mat);
static void rob_chara_age_age_disp(rob_chara_age_age* arr,
    render_context* rctx, int32_t chara_id, bool reflect, bool chara_color);
static void rob_chara_age_age_load(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id);
static void rob_chara_age_age_reset(rob_chara_age_age* arr, int32_t chara_id);
static void rob_chara_age_age_set_alpha(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value);
static void rob_chara_age_age_set_disp(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, bool value);
static void rob_chara_age_age_set_move_cancel(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value);
static void rob_chara_age_age_set_npr(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, bool value);
static void rob_chara_age_age_set_rot_speed(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value);
static void rob_chara_age_age_set_skip(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id);
static void rob_chara_age_age_set_step(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value);
static void rob_chara_age_age_set_step_full(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id);

static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    const std::vector<bone_database_bone>* bones);
static void rob_chara_bone_data_eyes_xrot_adjust(rob_chara_bone_data* rob_bone_data,
    const struc_241* a2, eyes_adjust* a3);
static void rob_chara_bone_data_get_ik_scale(
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data);
static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index);
static bone_node* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index);
static void rob_chara_bone_data_ik_scale_calculate(
    rob_chara_bone_data_ik_scale* ik_scale, std::vector<bone_data>* bones,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, const bone_database* bone_data);
static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, const bone_database* bone_data);
static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, const bone_database* bone_data);
static void rob_chara_bone_data_motion_blend_mot_free(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_list_free(rob_chara_bone_data* rob_bone_data,
    size_t last_index);
static void rob_chara_bone_data_motion_blend_mot_list_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data,
    uint32_t motion_id, MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db);
static void rob_chara_bone_data_reserve(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_set_eyelid_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_eyelid_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_eyelid_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_eyes_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_eyes_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_eyes_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_face_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_face_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_face_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_hand_l_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_hand_l_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_hand_l_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_hand_r_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_hand_r_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_hand_r_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
    const std::vector<bone_database_bone>* bones, const std::string* motion_bones);
static void rob_chara_bone_data_set_motion_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_mouth_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset);
static void rob_chara_bone_data_set_mouth_frame(rob_chara_bone_data* rob_bone_data, float_t frame);
static void rob_chara_bone_data_set_mouth_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step);
static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    const uint16_t* parent_indices);
static void rob_chara_bone_data_set_rot_y(rob_chara_bone_data* rob_bone_data, float_t rot_y);
static void rob_chara_bone_data_set_step(rob_chara_bone_data* rob_bone_data, float_t step);

static void rob_chara_age_age_ctrl(rob_chara* rob_chr, int32_t part_id, const char* name);
static object_info rob_chara_get_head_object(rob_chara* rob_chr, int32_t head_object_id);
static object_info rob_chara_get_object_info(rob_chara* rob_chr, item_id id);
static void rob_chara_load_default_motion(rob_chara* rob_chr,
    const bone_database* bone_data, const motion_database* mot_db);
static void rob_chara_load_default_motion_sub(rob_chara* rob_chr, int32_t skeleton_select,
    uint32_t motion_id, const bone_database* bone_data, const motion_database* mot_db);

static void rob_chara_set_adjust(rob_chara* rob_chr, rob_chara_data_adjust* adjust_new,
    rob_chara_data_adjust* adjust, rob_chara_data_adjust* adjust_prev);
static void rob_chara_set_pv_data(rob_chara* rob_chr, int8_t chara_id,
    chara_index chara_index, int32_t cos_id, const rob_chara_pv_data& pv_data);

static void rob_cmn_mottbl_read(void* a1, const void* data, size_t size);

static rob_manager_rob_impl* rob_manager_rob_impls1_get(TaskRobManager* rob_mgr);
static rob_manager_rob_impl* rob_manager_rob_impls2_get(TaskRobManager* rob_mgr);

rob_chara* rob_chara_array;
rob_chara_pv_data* rob_chara_pv_data_array;

std::map<uint32_t, MhdFile> mothead_storage_data;

rob_manager_rob_impl rob_manager_rob_impls1[2];
bool rob_manager_rob_impls1_init = false;
rob_manager_rob_impl rob_manager_rob_impls2[7];
bool rob_manager_rob_impls2_init = false;

OpdMakeManager* opd_make_manager;
OpdMaker* opd_maker_array;
OsagePlayDataManager* osage_play_data_manager;
PvOsageManager* pv_osage_manager_array;
rob_cmn_mottbl_header* rob_cmn_mottbl_data;
rob_chara_age_age* rob_chara_age_age_array;
rob_sleeve_handler* rob_sleeve_handler_data;
RobThreadHandler* rob_thread_handler;
TaskRobLoad* task_rob_load;
TaskRobManager* task_rob_manager;

static int32_t opd_maker_counter = 0;
static int32_t pv_osage_manager_counter = 0;
static int32_t rob_thread_parent_counter = 0;

static const mothead_mot mothead_mot_null;

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
    { mothead_func_67_sleeve, 0 },
    { mothead_func_68, 0 },
    { mothead_func_69_motion_max_frame, 0 },
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
        0x0000, BONE_DATABASE_SKELETON_MIKU, object_info(0x0000, 0x0000),
        {
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
        0x0107, BONE_DATABASE_SKELETON_RIN, object_info(0x0000, 0x0000),
        {
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
        0x0105, BONE_DATABASE_SKELETON_LEN, object_info(0x0000, 0x0000),
        {
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
        0x0106, BONE_DATABASE_SKELETON_LUKA, object_info(0x0000, 0x0000),
        {
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
        0x0108, BONE_DATABASE_SKELETON_NERU, object_info(0x0000, 0x0000),
        {
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
        0x0109, BONE_DATABASE_SKELETON_HAKU, object_info(0x0000, 0x0000),
        {
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
        0x010D, BONE_DATABASE_SKELETON_KAITO, object_info(0x0000, 0x0000),
        {
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
        0x010E, BONE_DATABASE_SKELETON_MEIKO, object_info(0x0000, 0x0000),
        {
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
        0x010F, BONE_DATABASE_SKELETON_SAKINE, object_info(0x0000, 0x0000),
        {
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
        0x063D, BONE_DATABASE_SKELETON_TETO, object_info(0x0000, 0x0000),
        {
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
        { 0.0148999998345971f, -0.0212999992072582f, 0.0f, },
        1.0f, 1.0f, -3.8f, 6.0f, -3.8f, 6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0750491619110107f, 0.104719758033752f, -0.0733038261532783f, 0.125663697719574f,
        { 0.033500000834465f, -0.0111999996006489f, 0.0f, },
        1.0f, 1.0f, -5.5f, 10.0f, -5.5f, 6.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0837758034467697f, 0.0855211317539215f, -0.104719758033752f, 0.120427720248699f,
        { 0.0166999995708466f, -0.00540000014007092f, 0.0f, },
        1.0f, 1.0f, -6.5f, 6.0f, -6.0f, 2.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0349065847694874f, 0.0698131695389748f, -0.0872664600610733f, 0.113446399569511f,
        { 0.011400000192225f, 0.00810000021010637f, 0.0f, },
        1.0f, 1.0f, -3.5f, 6.0f, -3.5f, 6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0349065847694874f, 0.0733038261532783f, -0.0907571166753769f, 0.13962633907795f,
        { 0.0151000004261732f, -0.0439999997615814f, 0.0f, },
        1.0f, 1.0f, -2.6f, 6.0f, -2.6f, 6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0523598790168762f, 0.122173048555851f, -0.0872664600610733f, 0.130899697542191f,
        { 0.0221999995410442f, -0.000199999994947575f, 0.0f, }, 1.0f,
        1.0f, -4.0f, 7.5f, -3.5f,
        6.0f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0733038261532783f, 0.101229101419449f, -0.0785398185253143f, 0.113446399569511f,
        { 0.025000000372529f, -0.0230999998748302f, 0.0f, },
        1.0f, 1.0f, -4.0f, 6.0f, -3.8f, 3.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0663225129246712f, 0.0593411959707737f, -0.0785398185253143f, 0.113446399569511f,
        { 0.0118000004440546f, -0.00510000018402934f, 0.0f, },
        1.0f, 1.0f, -5.6f, 6.0f, -5.6f, 5.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0645771846175194f, 0.0663225129246712f, -0.0698131695389748f, 0.113446399569511f,
        { 0.0229000002145767f, -0.0148999998345971f, 0.0f, },
        1.0f, 1.0f, -5.0f, 4.0f, -5.0f, 2.5f,
    },
    {
        -0.0610865242779255f, 0.104719758033752f, -0.0802851468324661f, 0.113446399569511f,
        -0.0698131695389748f, 0.0698131695389748f, -0.113446399569511f, 0.122173048555851f,
        { 0.0137000000104308f, -0.00800000037997961f, 0.0f, },
        1.0f, 1.0f, -5.5f, 6.0f, -5.0f, 4.0f,
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

const rob_chara_age_age_init_data rob_chara_age_age_init_data_left[] = {
    { { 0.277f, 0.04f,  0.028f }, 4.65f, 0.115f, 0.0027f, 1.30f,  0.01f, 0.0f,   0.0f, 90.0f },
    { { 0.265f, 0.04f, -0.080f }, 4.25f, 0.070f, 0.0025f, 1.40f, -0.01f, 0.0f,  20.0f, 90.0f },
    { { 0.220f, 0.04f,  0.065f }, 4.60f, 0.119f, 0.0028f, 1.10f,  0.01f, 0.0f,  40.0f, 90.0f },
    { { 0.175f, 0.04f, -0.105f }, 4.20f, 0.049f, 0.0025f, 0.80f,  0.02f, 0.0f,  60.0f, 90.0f },
    { { 0.275f, 0.04f, -0.013f }, 4.30f, 0.112f, 0.0025f, 0.75f,  0.01f, 0.0f,  80.0f, 90.0f },
    { { 0.277f, 0.04f,  0.028f }, 4.65f, 0.115f, 0.0027f, 1.30f,  0.01f, 0.0f, 100.0f, 90.0f },
    { { 0.265f, 0.04f, -0.080f }, 4.25f, 0.070f, 0.0025f, 1.40f, -0.01f, 0.0f, 120.0f, 90.0f },
    { { 0.220f, 0.04f,  0.065f }, 4.60f, 0.119f, 0.0028f, 1.10f, -0.02f, 0.0f, 140.0f, 90.0f },
    { { 0.175f, 0.04f, -0.105f }, 4.20f, 0.049f, 0.0025f, 0.80f,  0.02f, 0.0f, 160.0f, 90.0f },
    { { 0.275f, 0.04f, -0.013f }, 4.30f, 0.112f, 0.0025f, 0.75f, -0.01f, 0.0f, 180.0f, 90.0f },
};

const rob_chara_age_age_init_data rob_chara_age_age_init_data_right[] = {
    { { 0.220f, 0.00f,  0.070f }, 1.70f, 0.160f, 0.0028f, 1.10f, -0.02f, 0.0f,   0.0f, 90.0f },
    { { 0.175f, 0.00f, -0.067f }, 2.23f, 0.095f, 0.0025f, 0.80f,  0.02f, 0.0f,  20.0f, 90.0f },
    { { 0.277f, 0.00f, -0.035f }, 1.80f, 0.147f, 0.0025f, 0.75f,  0.01f, 0.0f,  40.0f, 90.0f },
    { { 0.278f, 0.01f,  0.055f }, 1.78f, 0.170f, 0.0027f, 1.30f, -0.01f, 0.0f,  60.0f, 90.0f },
    { { 0.264f, 0.00f, -0.040f }, 2.20f, 0.124f, 0.0025f, 1.40f,  0.01f, 0.0f,  80.0f, 90.0f },
    { { 0.220f, 0.00f,  0.070f }, 1.70f, 0.160f, 0.0028f, 1.10f,  0.01f, 0.0f, 100.0f, 90.0f },
    { { 0.175f, 0.00f, -0.067f }, 2.23f, 0.095f, 0.0025f, 0.80f, -0.02f, 0.0f, 120.0f, 90.0f },
    { { 0.277f, 0.00f, -0.035f }, 1.80f, 0.147f, 0.0025f, 0.75f, -0.01f, 0.0f, 140.0f, 90.0f },
    { { 0.278f, 0.01f,  0.055f }, 1.78f, 0.170f, 0.0027f, 1.30f, -0.01f, 0.0f, 160.0f, 90.0f },
    { { 0.264f, 0.00f, -0.040f }, 2.20f, 0.124f, 0.0025f, 1.40f,  0.01f, 0.0f, 180.0f, 90.0f },
};

extern render_context* rctx_ptr;

extern uint32_t cmn_set_id;

const chara_init_data* chara_init_data_get(chara_index chara_index) {
    if (chara_index >= 0 && chara_index < CHARA_MAX)
        return &chara_init_data_array[chara_index];
    return 0;
}

int32_t chara_init_data_get_chara_size_index(chara_index chara_index) {
    const chara_init_data* chara_init_data = chara_init_data_get(chara_index);
    if (chara_init_data)
        return chara_init_data->field_848.chara_size_index;
    return 1;
}

int32_t chara_init_data_get_swim_costume(chara_index chara_index) {
    const chara_init_data* chara_init_data = chara_init_data_get(chara_index);
    if (chara_init_data)
        return chara_init_data->field_848.swim_costume;
    return 0;
}

int32_t chara_init_data_get_swim_s_costume(chara_index chara_index) {
    const chara_init_data* chara_init_data = chara_init_data_get(chara_index);
    if (chara_init_data)
        return chara_init_data->field_848.swim_s_costume;
    return 0;
}

float_t chara_size_table_get_value(uint32_t index) {
    static const float_t chara_size_table[] = {
        1.07f, 1.0f, 0.96f, 0.987f, 1.025f, 1.045f
    };

    if (index >= 0 && index < 5)
        return chara_size_table[index];
    return 1.0f;
}

float_t chara_pos_adjust_y_table_get_value(uint32_t index) {
    static const float_t chara_pos_adjust_y_table[] = {
        0.071732f, 0.0f, -0.03859f, 0.0f, 0.0f
    };

    if (index >= 0 && index < 5)
        return chara_pos_adjust_y_table[index];
    return 0.0f;
}

bool check_cos_id_is_501(int32_t cos_id) {
    if (cos_id == -1 || cos_id >= 502)
        return false;
    return cos_id == 501;
}

const uint32_t* get_opd_motion_set_ids() {
    static const uint32_t opd_motion_set_ids[] = {
        51, 194, 797, (uint32_t)-1,
    };

    return opd_motion_set_ids;
}

const float_t get_osage_gravity_const() {
    return 0.00299444468691945f;
}

void motion_set_load_mothead(uint32_t set, std::string&& mdata_dir, const motion_database* mot_db) {
    const motion_set_info* set_info = mot_db->get_motion_set_by_id(set);
    if (!set_info)
        return;

    std::string file;
    file.assign("mothead_");
    file.append(set_info->name);
    file.append(".bin");

    std::string path;
    path.assign("rom/rob/");
    if (data_list[DATA_AFT].check_file_exists(mdata_dir.c_str(), file.c_str()))
        path.assign(mdata_dir);

    auto elem = mothead_storage_data.find(set);
    if (elem == mothead_storage_data.end())
        elem = mothead_storage_data.insert({ set, {} }).first;

    elem->second.LoadFile(path.c_str(), file.c_str(), set);
}

void motion_set_unload_mothead(uint32_t set) {
    auto elem = mothead_storage_data.find(set);
    if (elem != mothead_storage_data.end() && elem->second.Unload())
        mothead_storage_data.erase(elem);
}

bool opd_make_manager_del_task() {
    return opd_make_manager->DelTask();
}

bool osage_play_data_manager_add_task() {
    return osage_play_data_manager->AddTask();
}

void osage_play_data_manager_append_chara_motion_id(rob_chara* rob_chr, uint32_t motion_id) {
    std::vector<uint32_t> motion_ids;
    motion_ids.push_back(motion_id);
    osage_play_data_manager->AppendCharaMotionId(rob_chr, motion_ids);
}

void osage_play_data_manager_append_chara_motion_ids(
    rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids) {
    osage_play_data_manager->AppendCharaMotionId(rob_chr, motion_ids);
}

bool osage_play_data_manager_check_task_ready() {
    return osage_play_data_manager->CheckTaskReady();
}

void osage_play_data_manager_get_opd_file_data(object_info obj_info,
    uint32_t motion_id, const float_t*& data, uint32_t& count) {
    osage_play_data_manager->GetOpdFileData(obj_info, motion_id, data, count);
}

void osage_play_data_manager_reset() {
    osage_play_data_manager->Reset();
}

void pv_osage_manager_array_reset(int32_t chara_id) {
    pv_osage_manager_array_get(chara_id)->Reset();
}

void rob_init() {
    if (!opd_make_manager)
        opd_make_manager = new OpdMakeManager;

    if (!opd_maker_array)
        opd_maker_array = new OpdMaker[4];

    if (!osage_play_data_manager)
        osage_play_data_manager = new OsagePlayDataManager;

    if (!pv_osage_manager_array)
        pv_osage_manager_array = new PvOsageManager[ROB_CHARA_COUNT];

    if (!rob_chara_age_age_array)
        rob_chara_age_age_array = new rob_chara_age_age[18];

    if (!rob_chara_array)
        rob_chara_array = new rob_chara[ROB_CHARA_COUNT];

    if (!rob_chara_pv_data_array)
        rob_chara_pv_data_array = new rob_chara_pv_data[ROB_CHARA_COUNT];

    {
        p_file_handler rob_mot_tbl_file_handler;
        rob_mot_tbl_file_handler.read_file(&data_list[DATA_AFT], "rom/rob/", "rob_mot_tbl.bin");
        rob_mot_tbl_file_handler.set_callback_data(0, (PFNFILEHANDLERCALLBACK*)rob_cmn_mottbl_read, 0);
        rob_mot_tbl_file_handler.read_now();
        rob_mot_tbl_file_handler.reset();
    }

    if (!rob_sleeve_handler_data)
        rob_sleeve_handler_data = new rob_sleeve_handler;

    if (!rob_thread_handler)
        rob_thread_handler = new RobThreadHandler;

    if (!task_rob_load)
        task_rob_load = new TaskRobLoad;

    if (!task_rob_manager)
        task_rob_manager = new TaskRobManager;
}

void rob_free() {
    if (task_rob_manager) {
        delete task_rob_manager;
        task_rob_manager = 0;
    }

    if (task_rob_load) {
        delete task_rob_load;
        task_rob_load = 0;
    }

    if (rob_thread_handler) {
        delete rob_thread_handler;
        rob_thread_handler = 0;
    }

    if (rob_sleeve_handler_data) {
        delete rob_sleeve_handler_data;
        rob_sleeve_handler_data = 0;
    }

    free_def(rob_cmn_mottbl_data);

    if (rob_chara_pv_data_array) {
        delete[] rob_chara_pv_data_array;
        rob_chara_pv_data_array = 0;
    }

    if (rob_chara_array) {
        delete[] rob_chara_array;
        rob_chara_array = 0;
    }

    if (rob_chara_age_age_array) {
        delete[] rob_chara_age_age_array;
        rob_chara_age_age_array = 0;
    }

    if (pv_osage_manager_array) {
        delete[] pv_osage_manager_array;
        pv_osage_manager_array = 0;
    }

    if (osage_play_data_manager) {
        delete osage_play_data_manager;
        osage_play_data_manager = 0;
    }

    if (opd_maker_array) {
        delete[] opd_maker_array;
        opd_maker_array = 0;
    }

    if (opd_make_manager) {
        delete opd_make_manager;
        opd_make_manager = 0;
    }
}

static void rob_chara_item_equip_object_ctrl_iterate_nodes(
    rob_chara_item_equip_object* itm_eq_obj, int32_t osage_iterations) {
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
        const float_t* opd_data = 0;
        uint32_t opd_count = 0;
        osage_play_data_manager_get_opd_file_data(itm_eq_obj->obj_info, i.motion_id, opd_data, opd_count);
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
    for (int32_t i = ITEM_BODY; i < ITEM_MAX; i++)
        rob_chara_item_equip_object_ctrl_iterate_nodes(&rob_itm_equip->item_equip_object[i], iterations);
}

static void rob_chara_item_equip_object_ctrl(rob_chara_item_equip_object* itm_eq_obj) {
    if (itm_eq_obj->osage_iterations > 0) {
        rob_chara_item_equip_object_ctrl_iterate_nodes(itm_eq_obj, itm_eq_obj->osage_iterations);
        itm_eq_obj->osage_iterations = 0;
    }

    if (!itm_eq_obj->node_blocks.size()) // Added
        return;

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

mat4* rob_chara_bone_data_get_mats_mat(rob_chara_bone_data* rob_bone_data, size_t index) {
    if (index < rob_bone_data->mats.size())
        return &rob_bone_data->mats[index];
    return 0;
}

static void rob_chara_data_adjust_ctrl(rob_chara* rob_chr,
    rob_chara_data_adjust* adjust, rob_chara_data_adjust* adjust_prev) {
    float_t cycle = adjust->cycle;
    adjust->frame += rob_chr->data.motion.step_data.frame;
    if (rob_chr->data.field_1588.field_0.field_58 == 1)
        cycle *= (60.0f / rob_chr->data.field_1588.field_0.frame);
    cycle *= adjust->frame * 2.0f * (float_t)M_PI * (float_t)(1.0 / 60.0f);

    float_t cycle_val = 0.0f;
    switch (adjust->cycle_type) {
    case 1:
        cycle_val = cosf(adjust->phase * (float_t)M_PI + cycle);
        break;
    case 2:
        cycle_val = sinf(adjust->phase * (float_t)M_PI + cycle);
        break;
    }

    adjust->curr_external_force = (adjust->external_force_cycle + cycle_val)
        * (adjust->external_force * adjust->external_force_cycle_strength) + adjust->external_force;

    if (adjust->type == 1) {
        mat4 mat = mat4_identity;
        mat4_rotate_y_mult(&mat, (float_t)((double_t)rob_chr->data.miku_rot.rot_y_int16
            * M_PI * (1.0 / 32768.0)), &mat);
        mat4_mult_vec3(&mat, &adjust->curr_external_force, &adjust->curr_external_force);
    }
    else if (adjust->type == 2) {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, MOT_BONE_KL_MUNE_B_WJ)->ex_data_mat;
        if (mat)
            mat4_mult_vec3(mat, &adjust->curr_external_force, &adjust->curr_external_force);
    }
    else if (adjust->type == 3) {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, MOT_BONE_KL_KUBI)->ex_data_mat;
        if (mat)
            mat4_mult_vec3(mat, &adjust->curr_external_force, &adjust->curr_external_force);
    }
    else if (adjust->type == 4) {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, MOT_BONE_FACE_ROOT)->ex_data_mat;
        if (mat)
            mat4_mult_vec3(mat, &adjust->curr_external_force, &adjust->curr_external_force);
    }
    else if (adjust->type == 5) {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, MOT_BONE_KL_KOSI_ETC_WJ)->ex_data_mat;
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
    if (adjust->transition_duration > adjust->transition_frame
        && fabsf(adjust->transition_duration - adjust->transition_frame) > 0.000001f) {
        transition_frame_step = true;
        float_t blend = (adjust->transition_frame + 1.0f) / (adjust->transition_duration + 1.0f);
        adjust->curr_external_force = vec3::lerp(adjust_prev->curr_external_force,
            adjust->curr_external_force, blend);
        adjust->curr_force = lerp_def(adjust_prev->curr_force, adjust->curr_force, blend);
    }

    if (adjust->strength_transition > adjust->transition_frame
        && fabsf(adjust->strength_transition - adjust->transition_frame) > 0.000001f) {
        float_t blend = (adjust->transition_frame + 1.0f) / (adjust->strength_transition + 1.0f);
        adjust->curr_strength = lerp_def(adjust_prev->curr_strength, adjust->curr_strength, blend);
    }
    else if (!transition_frame_step)
        return;

    adjust->transition_frame += rob_chr->data.motion.step_data.frame;
}

static void rob_chara_item_equip_object_set_parts_external_force(
    rob_chara_item_equip_object* itm_eq_obj, rob_osage_parts_bit parts_bits,
    vec3* weight, float_t force, float_t strength) {
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks) {
        RobOsage* rob_osg = &i->rob;
        if (rob_osg->CheckPartsBits(parts_bits)) {
            rob_osg->SetNodesExternalForce(weight, strength);
            rob_osg->SetNodesForce(force);
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

    rob_osage_parts_bit parts_bits = (rob_osage_parts_bit)(1 << parts);
    if (!rob_itm_eq->parts_short) {
        if (parts == ROB_OSAGE_PARTS_LEFT)
            enum_or(parts_bits, ROB_OSAGE_PARTS_SHORT_L_BIT);
        else if (parts == ROB_OSAGE_PARTS_RIGHT)
            enum_or(parts_bits, ROB_OSAGE_PARTS_SHORT_R_BIT);
    }

    if (!rob_itm_eq->parts_append) {
        if (parts == ROB_OSAGE_PARTS_LEFT)
            enum_or(parts_bits, ROB_OSAGE_PARTS_APPEND_L_BIT);
        else if (parts == ROB_OSAGE_PARTS_RIGHT)
            enum_or(parts_bits, ROB_OSAGE_PARTS_APPEND_R_BIT);
    }

    if (!rob_itm_eq->parts_white_one_l && parts == ROB_OSAGE_PARTS_LONG_C)
        enum_or(parts_bits, ROB_OSAGE_PARTS_WHITE_ONE_L_BIT);

    rob_chara_item_equip_object_set_parts_external_force(
        &rob_itm_eq->item_equip_object[id], parts_bits, external_force, force, strength);
}

static void rob_chara_item_equip_object_set_external_force(
    rob_chara_item_equip_object* itm_eq_obj, vec3* external_force) {
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks) {
        i->rob.set_external_force = true;
        i->rob.external_force = *external_force;
    }

    for (ExClothBlock*& i : itm_eq_obj->cloth_blocks) {
        i->rob.set_external_force = true;
        i->rob.external_force = *external_force;
    }
}

static void rob_chara_item_equip_set_external_force(
    rob_chara_item_equip* rob_itm_eq, vec3* external_force) {
    for (int32_t i = rob_itm_eq->first_item_equip_object; i < rob_itm_eq->max_item_equip_object; i++)
        rob_chara_item_equip_object_set_external_force(&rob_itm_eq->item_equip_object[i], external_force);
}

void rob_chara::adjust_ctrl() {
    rob_chara_item_equip* rob_itm_equip = item_equip;

    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++) {
        rob_chara_data_adjust* parts_adjust = &data.motion.parts_adjust[i];
        if (parts_adjust->enable) {
            rob_chara_data_adjust_ctrl(this, parts_adjust, &data.motion.parts_adjust_prev[i]);
            rob_chara_item_equip_set_parts_external_force(rob_itm_equip, i,
                &parts_adjust->curr_external_force, parts_adjust->curr_force, parts_adjust->curr_strength);
        }
    }

    rob_chara_data_adjust* adjust_global = &data.motion.adjust_global;
    if (adjust_global->enable) {
        rob_chara_data_adjust_ctrl(this, adjust_global, &data.motion.adjust_global_prev);
        rob_chara_item_equip_set_external_force(rob_itm_equip, &adjust_global->curr_external_force);
    }

    rob_itm_equip->parts_short = false;
    rob_itm_equip->parts_append = false;
    rob_itm_equip->parts_white_one_l = false;
}

static motion_bone_index rob_motion_c_kata_bone_get(int32_t index) {
    switch (index) {
    case 0:
        return MOTION_BONE_C_KATA_L;
    case 1:
        return MOTION_BONE_C_KATA_R;
    default:
        return MOTION_BONE_NONE;
    }
}

static void rob_chara_bone_data_set_motion_arm_length(rob_chara_bone_data* rob_bone_data,
    motion_bone_index motion_bone_index, float_t value) {
    motion_blend_mot* v3 = rob_bone_data->motion_loaded.front();
    v3->bone_data.bones.data()[motion_bone_index].arm_length = value;
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

void rob_chara::arm_adjust_ctrl() {
    for (int32_t i = 0; i < 2; i++)
        rob_chara_data_arm_adjust_ctrl(this, i, &data.motion.arm_adjust[i]);
}

void rob_chara::autoblink_disable() {
    data.motion.field_2A |= 0x02;
}

void rob_chara::autoblink_enable() {
    data.motion.field_2A &= ~0x02;
}

mat4* rob_chara::get_bone_data_mat(size_t index) {
    return rob_chara_bone_data_get_mat(bone_data, index);
}

object_info rob_chara::get_chara_init_data_face_object(int32_t index) {
    if (index >= 0 && index < 15)
        return chara_init_data->face_objects[index];
    return {};
}

float_t rob_chara::get_frame() {
    return bone_data->get_frame();
}

float_t rob_chara::get_frame_count() {
    return bone_data->get_frame_count();
}

float_t rob_chara::get_max_face_depth() {
    return item_cos_data.get_max_face_depth();
}

uint32_t rob_chara::get_rob_cmn_mottbl_motion_id(int32_t id) {
    if (id >= 0xD6 && id <= 0xDF)
        return pv_data.motion_face_ids[id - 0xD6];
    else if (id == 0xE0)
        return data.motion.motion_id;
    else if (id >= 0xE2 && id <= 0xEB)
        return pv_data.motion_face_ids[id - 0xE2];

    int32_t v2 = data.field_8.field_1A4;
    if (rob_cmn_mottbl_data
        && chara_index >= 0 && chara_index < rob_cmn_mottbl_data->chara_count
        && id >= 0 && id < rob_cmn_mottbl_data->mottbl_indices_count) {
        rob_cmn_mottbl_sub_header* v4 = (rob_cmn_mottbl_sub_header*)((uint8_t*)rob_cmn_mottbl_data
            + rob_cmn_mottbl_data->subheaders_offset);
        if (v2 >= 0 && v2 < v4[chara_index].field_4)
            return ((uint32_t*)((uint8_t*)rob_cmn_mottbl_data
                + *(uint32_t*)((uint8_t*)rob_cmn_mottbl_data
                    + v2 * sizeof(uint32_t) + v4[chara_index].data_offset)))[id];
    }
    return -1;
}

float_t rob_chara::get_trans_scale(int32_t bone, vec3& trans) {
    if (bone < 0 || bone > 26)
        return 0.0f;
    trans = data.field_1E68.field_DF8[bone].trans;
    return data.field_1E68.field_DF8[bone].scale;
}

bool rob_chara::is_visible() {
    return !!(data.field_0 & 0x01);
}

static int16_t sub_14054FE90(rob_chara* rob_chr, bool a3) {
    int16_t v3 = rob_chr->data.miku_rot.rot_y_int16;
    if (!a3 || !(rob_chr->data.field_0 & 0x10)) {
        int16_t v4 = rob_chr->data.field_8.field_B8.field_8;
        if (v4)
            return v3 + v4;
    }

    if (rob_chr->data.field_1588.field_0.field_20.field_0 & 0x100)
        return v3 + rob_chr->data.field_1588.field_0.field_274;
    return v3;
}

static void sub_1405F9520(rob_chara_item_equip_object* itm_eq_obj, rob_osage_parts_bit a2, bool a3) {
    for (ExOsageBlock*& i : itm_eq_obj->osage_blocks)
        if (i->rob.CheckPartsBits(a2))
            i->rob.field_1F0F = a3;
}

static void sub_140513EE0(rob_chara_item_equip* rob_itm_eq, rob_osage_parts a2, bool a3) {
    sub_1405F9520(&rob_itm_eq->item_equip_object[a2 == ROB_OSAGE_PARTS_MUFFLER ? ITEM_OUTER : ITEM_KAMI],
        (rob_osage_parts_bit)(1 << a2), a3);
}

static void sub_14041B9D0(rob_chara_bone_data* rob_bone_data) {
    rob_chara_bone_data_sleeve_adjust& sleeve_adjust = rob_bone_data->sleeve_adjust;
    sleeve_adjust.enable1 = false;
    sleeve_adjust.enable2 = false;
    sleeve_adjust.field_5C = 0.0f;
    sleeve_adjust.field_68 = 0.0f;
    sleeve_adjust.field_74 = 0.0f;
    sleeve_adjust.field_80 = 0.0f;
    sleeve_adjust.radius = 1.0f;
    sleeve_adjust.bones = 0;
    sleeve_adjust.step = 1.0f;
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
        if (!(rob_chr->data.field_0 & 0x80) || rob_chr->data.field_0 & 0x40)
            return 1.0f;
    }
    else {
        if (!(rob_chr->data.field_1588.field_0.field_20.field_0 & 0x8000)
            || !(rob_chr->data.field_1588.field_0.field_20.field_0 & 0x01))
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

static void sub_140414F00(struc_308* a1, float_t rot_y) {
    a1->prev_rot_y = a1->rot_y;
    a1->rot_y = rot_y;
}

static void sub_14041D270(rob_chara_bone_data* rob_bone_data, float_t rot_y) {
    sub_140414F00(&rob_bone_data->motion_loaded.front()->field_4F8, rot_y);
}

static void sub_14041D2A0(rob_chara_bone_data* rob_bone_data, float_t rot_y) {
    rob_bone_data->motion_loaded.front()->field_4F8.prev_rot_y = rot_y;
}

static void sub_140555B00(rob_chara* rob_chr, bool a2) {
    bool v3;
    if (!a2)
        v3 = 0;
    else if (rob_chr->data.motion.prev_motion_id == -1)
        v3 = 0;
    else {
        v3 = !(rob_chr->data.field_1588.field_0.field_20.field_0 & 0x400)
            || !(rob_chr->data.field_1588.field_0.field_10.field_0 & 0x40)
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

    float_t v12 = rob_chr->data.field_1588.field_0.frame;
    float_t v13 = v12 - v9;
    if (v3 && v6 && (v13 < v11 || v13 <= v8)) {
        float_t v14 = rob_chr->data.field_1588.field_0.frame;
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
                v17 = rob_chr->data.field_1588.field_0.frame - v9;
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
            if (!a3->type) {
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
        v19->field_0.loop_count = ((int16_t*)v35)[5];
        v19->field_0.loop_begin = (float_t)((int16_t*)v35)[12];
        v19->field_0.loop_end = (float_t)((int16_t*)v35)[13];
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
        if (a1->rob_chara_data->motion.field_28 & 0x08)
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
            if (a1->rob_chara_data->motion.field_28 & 0x08)
                v19->field_0.field_274 = -((int16_t*)v35)[0];
            else
                v19->field_0.field_274 = ((int16_t*)v35)[0];
        }
    } break;
    case 51: {
        if (a1->rob_chara_data->motion.field_28 & 0x08)
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
        if (a1->rob_chara_data->motion.field_28 & 0x08)
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
    uint32_t motion_id, float_t frame_count, const motion_database* mot_db) {
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

    a1->field_330.field_0.current = v7->data.data();
    a1->field_330.field_0.data = v7->data.data();
    a1->field_330.field_0.is_x = v7->is_x;
    a1->field_7A0 = v7->field_28.data();
    a1->motion_set_id = mot_db->get_motion_set_id_by_motion_id(motion_id);
    a1->field_0.field_10 = v7->field_0;
    a1->field_0.field_20 = v7->field_0;
    a1->field_0.field_20.field_0 &= ~0x4000;
    if (a1->field_0.field_10.field_0 & 0x100)
        a1->field_0.field_274 = (int16_t)0x8000;
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
        a1->field_0.frame = frame_count - 1.0f;
    else
        a1->field_0.frame = a1->field_0.frame_count;

    if (a1->field_0.field_20.field_0 & 0x200000)
        a1->field_0.field_20.field_0 |= 0x100;
    if (a1->field_0.field_20.field_C & 0x20000 && !(rob_chr->data.field_3D9C & 0x400))
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

void rob_chara::load_body_parts_object_info(item_id item_id, object_info obj_info,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    item_equip->load_body_parts_object_info(item_id, obj_info, bone_data, data, obj_db);
}

void rob_chara::load_motion(uint32_t motion_id, bool a3, float_t frame,
    MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db) {
    data.miku_rot.rot_y_int16 = sub_14054FE90(this, false);
    if (!(data.field_1588.field_0.field_20.field_0 & 0x10)) {
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
    data.motion.loop_index = 0;
    data.motion.field_30 = 0;

    rob_chara_data_adjust* parts_adjust = data.motion.parts_adjust;
    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++, parts_adjust++)
        parts_adjust->reset();

    data.motion.adjust_global.reset();

    data.adjust_data.offset = 0.0f;
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
    this->bone_data->set_disable_eye_motion(false);

    item_equip->set_osage_move_cancel(0, 0.0f);
    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        sub_140513EE0(item_equip, (rob_osage_parts)i, false);
    data.field_1588.field_0.field_30 = data.field_1588.field_0.field_10;
    data.field_1588.field_0.field_40 = data.field_1588.field_0.field_20;
    data.field_1588.field_0.field_276 = data.field_1588.field_0.field_274;
    data.field_1588.reset();
    sub_14053A9C0(&data.field_1588, this, motion_id,
        motion_storage_get_mot_data_frame_count(motion_id, mot_db), mot_db);
    if (data.field_1588.field_0.field_40.field_C & 0x20)
        data.field_1588.field_0.field_20.field_0 &= ~0x08;
    if (data.field_1588.field_0.field_40.field_C & 0x40)
        data.field_1588.field_0.field_20.field_8 |= 0x10000000;
    if (!(data.field_1588.field_0.field_20.field_0 & 0x10) && frame > 0.0f)
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
    item_equip->set_osage_step(step);

    rob_chara_bone_data_motion_load(this->bone_data, motion_id, blend_type, bone_data, mot_db);
    this->bone_data->set_motion_frame(data.motion.step_data.frame,
        data.motion.step_data.frame, data.field_1588.field_0.frame_count);
    this->bone_data->set_motion_playback_state(MOT_PLAY_FRAME_DATA_PLAYBACK_FORWARD);
    if (data.field_1588.field_0.field_58 == 1) {
        this->bone_data->set_motion_loop_state(MOT_PLAY_FRAME_DATA_LOOP_CONTINUOUS);
        this->bone_data->set_motion_loop(data.field_1588.field_0.loop_begin,
            data.field_1588.field_0.loop_count, -1.0f);
    }
    rob_chara_bone_data_set_rot_y(this->bone_data,
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

void rob_chara::load_outfit_object_info(item_id id, object_info obj_info,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    item_equip->load_outfit_object_info(id, obj_info, osage_reset, bone_data, data, obj_db);
}

static void sub_14053BEE0(rob_chara* rob_chr);
static bool rob_chara_hands_adjust(rob_chara* rob_chr);
static bool sub_14053B580(rob_chara* rob_chr, int32_t a2);
static void sub_14053B260(rob_chara* rob_chr);

static bool sub_14053B530(rob_chara* rob_chr) {
    return sub_14053B580(rob_chr, 2) | sub_14053B580(rob_chr, 3);
}

static void rob_chara_bone_data_set_left_hand_scale(rob_chara_bone_data* rob_bone_data, float_t scale) {
    mat4* kl_te_wj_mat = rob_chara_bone_data_get_mats_mat(rob_bone_data, ROB_BONE_KL_TE_L_WJ);
    if (!kl_te_wj_mat)
        return;

    vec3 trans;
    mat4_get_translation(kl_te_wj_mat, &trans);
    trans -= trans * scale;

    mat4 mat;
    mat4_scale(scale, scale, scale, &mat);
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

static void rob_chara_bone_data_set_right_hand_scale(rob_chara_bone_data* rob_bone_data, float_t scale) {
    mat4* kl_te_wj_mat = rob_chara_bone_data_get_mats_mat(rob_bone_data, ROB_BONE_KL_TE_R_WJ);
    if (!kl_te_wj_mat)
        return;

    vec3 trans;
    mat4_get_translation(kl_te_wj_mat, &trans);
    trans -= trans * scale;

    mat4 mat;
    mat4_scale(scale, scale, scale, &mat);
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

void rob_chara::rob_motion_modifier_ctrl() {
    sub_14053BEE0(this);
    rob_chara_hands_adjust(this);
    if (!sub_14053B530(this))
        sub_14053B260(this);

    float_t left_hand_scale_default = data.adjust_data.left_hand_scale_default;
    float_t right_hand_scale_default = data.adjust_data.right_hand_scale_default;
    float_t left_hand_scale = data.adjust_data.left_hand_scale;
    float_t right_hand_scale = data.adjust_data.right_hand_scale;

    if (left_hand_scale_default > 0.0f) {
        if (left_hand_scale <= 0.0f)
            left_hand_scale = data.adjust_data.left_hand_scale_default;
        else
            left_hand_scale *= left_hand_scale_default;
    }

    if (right_hand_scale_default > 0.0f) {
        if (right_hand_scale <= 0.0f)
            right_hand_scale = data.adjust_data.right_hand_scale_default;
        else
            right_hand_scale *= right_hand_scale_default;
    }

    if (left_hand_scale > 0.0f)
        rob_chara_bone_data_set_left_hand_scale(this->bone_data, left_hand_scale);

    if (right_hand_scale > 0.0f)
        rob_chara_bone_data_set_right_hand_scale(this->bone_data, right_hand_scale);
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
    const bone_database* bone_data, const motion_database* mot_db) {
    this->bone_data->reset();
    rob_chara_bone_data_init_data(this->bone_data,
        BONE_DATABASE_SKELETON_COMMON, chara_init_data->skeleton_type, bone_data);
    data.reset();
    data_prev.reset();
    item_equip->get_parent_bone_nodes(rob_chara_bone_data_get_node(
        this->bone_data, MOT_BONE_N_HARA_CP), bone_data);
    type = pv_data->type;
    rob_chara_data* rob_chr_data = &data;
    rob_chr_data->miku_rot.rot_y_int16 = pv_data->rot_y_int16;
    rob_chr_data->miku_rot.field_6 = pv_data->rot_y_int16;
    float_t scale = chara_size_table_get_value(this->pv_data.chara_size_index);
    rob_chr_data->adjust_data.scale = scale;
    rob_chr_data->adjust_data.item_scale = scale; // X
    rob_chr_data->adjust_data.height_adjust = this->pv_data.height_adjust;
    rob_chr_data->adjust_data.pos_adjust_y = chara_pos_adjust_y_table_get_value(this->pv_data.chara_size_index);
    rob_chara_bone_data_eyes_xrot_adjust(this->bone_data, chara_some_data_array, &pv_data->eyes_adjust);
    this->bone_data->sleeve_adjust.sleeve_l = pv_data->sleeve_l;
    this->bone_data->sleeve_adjust.sleeve_r = pv_data->sleeve_r;
    this->bone_data->sleeve_adjust.enable1 = true;
    this->bone_data->sleeve_adjust.enable2 = false;
    rob_chara_bone_data_get_ik_scale(this->bone_data, bone_data);
    rob_chara_load_default_motion(this, bone_data, mot_db);
    rob_chr_data->field_8.field_4.field_0 = 1;
    rob_chr_data->field_8.field_4.motion_id = get_rob_cmn_mottbl_motion_id(0);
    rob_chr_data->field_8.field_0 = 0;
    rob_chr_data->field_8.field_4.field_10 = pv_data->field_5;
    load_motion(rob_chr_data->field_8.field_4.motion_id,
        pv_data->field_5, 0.0f, MOTION_BLEND, bone_data, mot_db);
    field_C = true;
    item_equip->field_DC = 0;
    sub_140513C60(item_equip, ITEM_ATAMA, false);
    sub_140513C60(item_equip, ITEM_TE_R, false);
    sub_140513C60(item_equip, ITEM_TE_L, false);
    object_info head_obj = rob_chara_get_head_object(this, 0);
    rob_chr_data->motion.field_150.head_object = head_obj;
    rob_chr_data->motion.field_3B0.head_object = head_obj;
    set_parts_disp(ITEM_MAX, true);
}

void rob_chara::reset_osage() {
    for (rob_chara_data_adjust& i : data.motion.parts_adjust)
        i.reset();

    for (rob_chara_data_adjust& i : data.motion.parts_adjust_prev)
        i.reset();

    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        item_equip->reset_nodes_external_force((rob_osage_parts)i);

    data.motion.adjust_global.reset();
    data.motion.adjust_global_prev.reset();

    item_equip->reset_external_force();
    item_equip->parts_short = false;
    item_equip->parts_append = false;
    item_equip->parts_white_one_l = false;

    set_osage_step(-1.0f);
}

void rob_chara::set_bone_data_frame(float_t frame) {
    bone_data->set_frame(frame);
    bone_data->interpolate();
    bone_data->update(0);
}

void rob_chara::set_chara_color(bool value) {
    item_equip->chara_color = value;
}

void rob_chara::set_chara_height_adjust(bool value) {
    data.adjust_data.height_adjust = value;
}

void rob_chara::set_chara_pos_adjust(vec3& value) {
    data.adjust_data.pos_adjust = value;
}

void rob_chara::set_chara_pos_adjust_y(float_t value) {
    data.adjust_data.pos_adjust_y = value;
}

void rob_chara::set_chara_size(float_t value) {
    data.adjust_data.scale = value;
    data.adjust_data.item_scale = value; // X
}

static bool sub_14053F290(RobPartialMotion* a1);
static bool sub_14053F280(RobPartialMotion* a1);
static bool sub_14053F2A0(RobPartialMotion* a1);
static bool sub_14053F270(RobPartialMotion* a1);
static bool sub_14053F2B0(RobPartialMotion* a1);

static void rob_chara_set_eyelid_motion(rob_chara* rob_chr,
    RobEyelidMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 1 || type == 2) {
        rob_chr->data.motion.field_3B0.eyelid.data = motion->data;
        if (!(rob_chr->data.motion.field_29 & 0x80))
            return;
    }
    else {
        rob_chr->data.motion.field_150.eyelid.data = motion->data;
        if (rob_chr->data.motion.field_29 & 0x80)
            return;
    }

    rob_chr->bone_data->load_eyelid_motion(motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_eyelid_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_eyelid_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_eyelid_anim_blend_duration(rob_chr->bone_data,
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_eyes_motion(rob_chara* rob_chr,
    RobEyesMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 1 || type == 2) {
        rob_chr->data.motion.field_3B0.eyes.data = motion->data;
        if (!(rob_chr->data.motion.field_29 & 0x40))
            return;
    }
    else {
        rob_chr->data.motion.field_150.eyes.data = motion->data;
        if (rob_chr->data.motion.field_29 & 0x40)
            return;
    }

    rob_chr->bone_data->load_eyes_motion(motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_eyes_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_eyes_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_eyes_anim_blend_duration(rob_chr->bone_data,
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_face_motion(rob_chara* rob_chr,
    RobFaceMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 2 || type == 1) {
        rob_chr->data.motion.field_3B0.face.data = motion->data;
        if (!(rob_chr->data.motion.field_29 & 0x04))
            return;
    }
    else {
        rob_chr->data.motion.field_150.face.data = motion->data;
        if (rob_chr->data.motion.field_29 & 0x04)
            return;
    }

    rob_chr->bone_data->load_face_motion(motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_face_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_face_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_face_anim_blend_duration(rob_chr->bone_data,
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_hand_l_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 2) {
        rob_chr->data.motion.field_3B0.hand_l.data = motion->data;
        if (!(rob_chr->data.motion.field_29 & 0x08) || (rob_chr->data.motion.field_2A & 0x04))
            return;
    }
    else if (type == 1) {
        rob_chr->data.motion.hand_r.data = motion->data;
        if ((rob_chr->data.motion.field_29 & 0x08) || !(rob_chr->data.motion.field_2A & 0x04))
            return;
    }
    else {
        rob_chr->data.motion.field_150.hand_l.data = motion->data;
        if ((rob_chr->data.motion.field_29 & 0x08) || (rob_chr->data.motion.field_2A & 0x04))
            return;
    }

    rob_chr->bone_data->load_hand_l_motion(motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_hand_l_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_hand_l_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_hand_l_anim_blend_duration(rob_chr->bone_data,
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_hand_r_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 2) {
        rob_chr->data.motion.field_3B0.hand_r.data = motion->data;
        if (!(rob_chr->data.motion.field_29 & 0x10) || (rob_chr->data.motion.field_2A & 0x08))
            return;
    }
    else if (type == 1) {
        rob_chr->data.motion.hand_r.data = motion->data;
        if ((rob_chr->data.motion.field_29 & 0x10) || !(rob_chr->data.motion.field_2A & 0x08))
            return;
    }
    else {
        rob_chr->data.motion.field_150.hand_r.data = motion->data;
        if ((rob_chr->data.motion.field_29 & 0x10) || (rob_chr->data.motion.field_2A & 0x08))
            return;
    }

    rob_chr->bone_data->load_hand_r_motion(motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_hand_r_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_hand_r_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_hand_r_anim_blend_duration(rob_chr->bone_data,
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_mouth_motion(rob_chara* rob_chr,
    RobMouthMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 1 || type == 2) {
        rob_chr->data.motion.field_3B0.mouth.data = motion->data;
        if (!(rob_chr->data.motion.field_29 & 0x20))
            return;
    }
    else {
        rob_chr->data.motion.field_150.mouth.data = motion->data;
        if (rob_chr->data.motion.field_29 & 0x20)
            return;
    }

    rob_chr->bone_data->load_mouth_motion(motion->data.motion_id, mot_db);
    rob_chara_bone_data_set_mouth_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_mouth_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_mouth_anim_blend_duration(rob_chr->bone_data,
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void sub_1405500F0(rob_chara* rob_chr) {
    rob_chr->data.motion.field_29 &= ~0x01;
    int32_t v1 = rob_chr->data.motion.field_150.field_1C0;
    if (v1 == 2 || v1 == 3)
        return;

    switch (rob_chr->data.motion.field_150.face.data.mottbl_index) {
    case 0x07: // FACE_RESET
    case 0x09: // FACE_RESET_OLD
    case 0x0B: // FACE_SAD
    case 0x0D: // FACE_SAD_OLD
    case 0x13: // FACE_SURPRISE
    case 0x15: // FACE_SURPRISE_OLD
    case 0x19: // FACE_ADMIRATION
    case 0x1B: // FACE_ADMIRATION_OLD
    case 0x1D: // FACE_SMILE
    case 0x1F: // FACE_SMILE_OLD
    case 0x21: // FACE_SETTLED
    case 0x23: // FACE_SETTLED_OLD
    case 0x25: // FACE_DAZZLING
    case 0x27: // FACE_DAZZLING_OLD
    case 0x29: // FACE_LASCIVIOUS
    case 0x2B: // FACE_LASCIVIOUS_OLD
    case 0x2D: // FACE_STRONG
    case 0x2F: // FACE_STRONG_OLD
    case 0x31: // FACE_CLARIFYING
    case 0x33: // FACE_CLARIFYING_OLD
    case 0x35: // FACE_GENTLE
    case 0x37: // FACE_GENTLE_OLD
    case 0x41: // FACE_NAGASI
    case 0x43: // FACE_NAGASI_OLD
    case 0x45: // FACE_KIRI
    case 0x47: // FACE_KIRI_OLD
    case 0x49: // FACE_UTURO
    case 0x4B: // FACE_UTURO_OLD
    case 0x4D: // FACE_OMOU
    case 0x4F: // FACE_OMOU_OLD
    case 0x51: // FACE_SETUNA
    case 0x53: // FACE_SETUNA_OLD
    case 0x55: // FACE_GENKI
    case 0x57: // FACE_GENKI_OLD
    case 0x59: // FACE_YARU
    case 0x5B: // FACE_YARU_OLD
    case 0x5D: // FACE_COOL
    case 0x5F: // FACE_KOMARIWARAI
    case 0x61: // FACE_KUMON
    case 0x63: // FACE_KUTSUU
    case 0x65: // FACE_NAKI
    case 0x67: // FACE_NAYAMI
    case 0x69: // FACE_SUPSERIOUS
    case 0x6B: // FACE_TSUYOKIWARAI
        rob_chr->data.motion.field_29 |= 0x01;
        break;
    }
}

static vec3* rob_chara_bone_data_get_global_trans(rob_chara_bone_data* rob_bone_data) {
    return &rob_bone_data->motion_loaded.front()->bone_data.global_trans;
}

static void rob_chara_data_adjuct_set_trans(rob_chara_adjust_data* rob_chr_adj,
    vec3& trans, bool pos_adjust, vec3* global_trans) {
    float_t scale = rob_chr_adj->scale;
    float_t item_scale = rob_chr_adj->item_scale; // X
    vec3 _offset = rob_chr_adj->offset;
    if (global_trans)
        _offset.y += global_trans->y;

    vec3 _trans = trans;
    vec3 _item_trans = trans;
    if (rob_chr_adj->height_adjust) {
        _trans.y += rob_chr_adj->pos_adjust_y;
        _item_trans.y += rob_chr_adj->pos_adjust_y; // X
    }
    else {
        vec3 temp = (_trans - _offset) * scale + _offset;
        vec3 arm_temp = (_item_trans - _offset) * item_scale + _offset;

        if (!rob_chr_adj->offset_x) {
            _trans.x = temp.x;
            _item_trans.x = arm_temp.x; // X
        }

        if (!rob_chr_adj->offset_y) {
            _trans.y = temp.y;
            _item_trans.y = arm_temp.y; // X
        }

        if (!rob_chr_adj->offset_z) {
            _trans.z = temp.z;
            _item_trans.z = arm_temp.z; // X
        }
    }

    if (pos_adjust) {
        _trans = rob_chr_adj->pos_adjust + _trans;
        _item_trans = rob_chr_adj->pos_adjust + _item_trans; //X
    }

    rob_chr_adj->trans = _trans - trans * scale;
    rob_chr_adj->item_trans = _item_trans - trans * item_scale; // X
}

void rob_chara::set_data_adjust_mat(rob_chara_adjust_data* rob_chr_adj, bool pos_adjust) {
    mat4* mat = rob_chara_bone_data_get_mats_mat(bone_data, ROB_BONE_N_HARA_CP);

    vec3 trans;
    mat4_get_translation(mat, &trans);

    vec3* global_trans = 0;
    if (rob_chr_adj->get_global_trans)
        global_trans = rob_chara_bone_data_get_global_trans(bone_data);
    rob_chara_data_adjuct_set_trans(rob_chr_adj, trans, pos_adjust, global_trans);

    float_t scale = rob_chr_adj->scale;
    mat4_scale(scale, scale, scale, &rob_chr_adj->mat);
    mat4_set_translation(&rob_chr_adj->mat, &rob_chr_adj->trans);

    float_t item_scale = rob_chr_adj->item_scale; // X
    mat4_scale(item_scale, item_scale, item_scale, &rob_chr_adj->item_mat);
    mat4_set_translation(&rob_chr_adj->item_mat, &rob_chr_adj->item_trans);
}

void rob_chara::set_data_miku_rot_position(vec3& value) {
    data.miku_rot.position = value;
}

void rob_chara::set_data_miku_rot_rot_y_int16(int16_t value) {
    data.miku_rot.rot_y_int16 = value;
}

void rob_chara::set_eyelid_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t blend_duration,
    float_t a7, float_t step, int32_t a9, float_t blend_offset, const motion_database* mot_db) {
    RobEyelidMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.state = state;
    motion.data.play_frame_step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.field_24 = a7;
    if (sub_14053F290(&motion)) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.field_38 = a9;
    if (motion.data.motion_id != -1) {
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion.data.motion_id, mot_db);
        motion.data.frame = (motion.data.frame_count - 1.0f) * value;
    }

    if (!type) {
        data.motion.field_29 &= ~2;
        data.motion.field_150.time = 0.0f;
    }
    rob_chara_set_eyelid_motion(this, &motion, type, mot_db);
}

void rob_chara::set_eyelid_mottbl_motion_from_face(int32_t a2,
    float_t blend_duration, float_t value, float_t blend_offset, const motion_database* mot_db) {
    if (!a2)
        a2 = data.motion.field_150.field_1C0;

    RobEyelidMotion motion;
    switch (a2) {
    case 0:
    case 2:
        motion.data = data.motion.field_150.face.data;
        break;
    case 1:
        value = 1.0f;
    case 3: {
        int32_t mottbl_index = data.motion.field_150.face.data.mottbl_index;
        switch (mottbl_index) {
        case 0x07: // FACE_RESET
        case 0x09: // FACE_RESET_OLD
        case 0x0B: // FACE_SAD
        case 0x0D: // FACE_SAD_OLD
        case 0x0F: // FACE_LAUGH
        case 0x11: // FACE_LAUGH_OLD
        case 0x13: // FACE_SURPRISE
        case 0x15: // FACE_SURPRISE_OLD
        case 0x17: // FACE_WINK_OLD
        case 0x19: // FACE_ADMIRATION
        case 0x1B: // FACE_ADMIRATION_OLD
        case 0x1D: // FACE_SMILE
        case 0x1F: // FACE_SMILE_OLD
        case 0x21: // FACE_SETTLED
        case 0x23: // FACE_SETTLED_OLD
        case 0x25: // FACE_DAZZLING
        case 0x27: // FACE_DAZZLING_OLD
        case 0x29: // FACE_LASCIVIOUS
        case 0x2B: // FACE_LASCIVIOUS_OLD
        case 0x2D: // FACE_STRONG
        case 0x2F: // FACE_STRONG_OLD
        case 0x31: // FACE_CLARIFYING
        case 0x33: // FACE_CLARIFYING_OLD
        case 0x35: // FACE_GENTLE
        case 0x37: // FACE_GENTLE_OLD
        case 0x3D: // FACE_CLOSE
        case 0x3F: // FACE_CLOSE_OLD
        case 0x41: // FACE_NAGASI
        case 0x43: // FACE_NAGASI_OLD
        case 0x45: // FACE_KIRI
        case 0x47: // FACE_KIRI_OLD
        case 0x49: // FACE_UTURO
        case 0x4B: // FACE_UTURO_OLD
        case 0x4D: // FACE_OMOU
        case 0x4F: // FACE_OMOU_OLD
        case 0x51: // FACE_SETUNA
        case 0x53: // FACE_SETUNA_OLD
        case 0x55: // FACE_GENKI
        case 0x57: // FACE_GENKI_OLD
        case 0x59: // FACE_YARU
        case 0x5B: // FACE_YARU_OLD
        case 0x5D: // FACE_COOL
        case 0x5F: // FACE_KOMARIWARAI
        case 0x61: // FACE_KUMON
        case 0x63: // FACE_KUTSUU
        case 0x65: // FACE_NAKI
        case 0x67: // FACE_NAYAMI
        case 0x69: // FACE_SUPSERIOUS
        case 0x6B: // FACE_TSUYOKIWARAI
        case 0x6D: // FACE_WINK_L
        case 0x6F: // FACE_WINK_R
        case 0x71: // FACE_WINKG_L
        case 0x73: // FACE_WINKG_R
        case 0x7F: // FACE_WINK_OLD
        case 0x81: // FACE_NEW_IKARI_OLD
        case 0xEC: // FACE_EYEBROW_UP_RIGHT
        case 0xEE: // FACE_EYEBROW_UP_LEFT
        case 0xF0: // FACE_KOMARIEGAO
        case 0xF2: // FACE_KONWAKU
            motion.data.mottbl_index = ++mottbl_index;
            break;
        }

        uint32_t motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
        motion.data.motion_id = motion_id;
        if (motion_id == -1) {
            blend_duration = 0.0f;
            a2 = 0;
            motion.data = data.motion.field_150.face.data;
        }
        else if (value <= 0.0f) {
            a2 = 0;
            motion.data = data.motion.field_150.face.data;
        }
        else {
            motion.data.state = 0;
            motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
            motion.data.frame = (motion.data.frame_count - 1.0f) * value;
        }
    } break;
    }
    motion.data.blend_offset = blend_offset;
    motion.data.blend_duration = blend_duration;

    data.motion.field_150.field_1C0 = a2;
    sub_1405500F0(this);
    data.motion.field_29 &= ~0x02;
    data.motion.field_150.time = 0.0f;
    rob_chara_set_eyelid_motion(this, &motion, 0, mot_db);
}

void rob_chara::set_eyes_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t blend_duration,
    float_t a7, float_t step, int32_t a9, float_t blend_offset, const motion_database* mot_db) {
    RobEyesMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.state = state;
    motion.data.play_frame_step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.field_24 = a7;
    if (sub_14053F290(&motion)) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.field_38 = a9;
    if (motion.data.motion_id != -1) {
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion.data.motion_id, mot_db);
        motion.data.frame = (motion.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_eyes_motion(this, &motion, type, mot_db);
}

static void sub_140553970(rob_chara* rob_chr, object_info a2, int32_t type) {
    if (type != 1 && type == 2)
        rob_chr->data.motion.field_3B0.head_object = a2;
    else
        rob_chr->data.motion.field_150.head_object = a2;
}

void rob_chara::set_face_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t blend_duration,
    float_t a7, float_t step, int32_t a9, float_t blend_offset, bool a11, const motion_database* mot_db) {
    RobFaceMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.state = state;
    motion.data.play_frame_step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.field_24 = a7;
    if (sub_14053F290(&motion)) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.field_38 = a9;
    if (motion.data.motion_id != -1) {
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion.data.motion_id, mot_db);
        motion.data.frame = (motion.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_face_motion(this, &motion, type, mot_db);

    int32_t head_object_id = 0;
    switch (mottbl_index) {
    case 0x39: // FACE_CRY / Crying (><)
        head_object_id = 1;
        break;
    case 0x3B: // FACE_CRY_OLD
        head_object_id = 7;
        break;
    case 0x75: // FACE_RESET / Crying (><) + Shock
        head_object_id = 2;
        break;
    case 0x77: // FACE_RESET / White-Eyed
        head_object_id = 3;
        break;
    case 0x79: // FACE_RESET / White-Eyed + Shock
        head_object_id = 4;
        break;
    case 0x7B: // FACE_RESET / -.-
        head_object_id = 5;
        break;
    case 0x7D: // FACE_RESET / -o-
        head_object_id = 6;
        break;
    }

    object_info v17 = rob_chara_get_head_object(this, head_object_id);
    if (head_object_id) {
        sub_140553970(this, v17, type);
        blend_duration = 0.0f;
    }
    else if (type == 2) {
        if (rob_chara_get_object_info(this, ITEM_ATAMA) != v17) {
            sub_140553970(this, v17, 2);
            blend_duration = 0.0f;
        }

    }
    else if (data.motion.field_150.head_object != v17) {
        sub_140553970(this, v17, type);
        data.motion.field_150.eyes.data.blend_duration = 0.0f;
        rob_chara_set_eyes_motion(this, &data.motion.field_150.eyes, 0, mot_db);
        blend_duration = 0.0f;
    }

    if (type == 1 || type != 2) {
        if (a11)
            set_eyelid_mottbl_motion_from_face(2, blend_duration, -1.0f, blend_offset, mot_db);
        else if (data.motion.field_29 & 0x02) {
            set_eyelid_mottbl_motion_from_face(
                data.motion.field_150.field_1C0, 3.0f, -1.0f, 0.0f, mot_db);
            data.motion.field_29 |= 0x02;
        }
        else {
            float_t value = -1.0f;
            if (data.motion.field_150.field_1C0 == 3) {
                float_t frame_count = data.motion.field_150.eyelid.data.frame_count - 1.0f;
                if (frame_count > 0.0f)
                    value = data.motion.field_150.eyelid.data.frame / frame_count;

                PartialMotionBlendFreeze& blend = bone_data->eyelid.blend;
                if (blend.duration > blend.frame) {
                    blend_duration = blend.duration - blend.frame;
                    blend_offset = 1.0f;
                }
            }
            set_eyelid_mottbl_motion_from_face(0, blend_duration, value, blend_offset, mot_db);
        }
    }
    else {
        RobEyelidMotion _motion;
        _motion.data = motion.data;
        _motion.data.blend_duration = blend_duration;
        _motion.data.blend_offset = blend_offset;
        data.motion.field_29 |= 0x80;
        rob_chara_set_eyelid_motion(this, &_motion, 2, mot_db);
    }
}

void rob_chara::set_face_object(object_info obj_info, int32_t a3) {
    if (a3 == 2)
        data.motion.field_3B0.face_object = obj_info;
    else
        data.motion.field_150.face_object = obj_info;
}

void rob_chara::set_face_object_index(int32_t index) {
    set_face_object(get_chara_init_data_face_object(index), 0);
}

void rob_chara::set_frame(float_t frame) {
    bone_data->set_frame(frame);
    data.motion.frame_data.frame = frame;
    //bone_data->interpolate();
    //bone_data->update(0);
    //sub_140509D30();
}

void rob_chara::set_hand_l_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t blend_duration,
    float_t a7, float_t step, int32_t a9, float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.state = state;
    motion.data.play_frame_step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.field_24 = a7;
    if (sub_14053F290(&motion)) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.field_38 = a9;
    if (motion.data.motion_id != -1) {
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion.data.motion_id, mot_db);
        motion.data.frame = (motion.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_hand_l_motion(this, &motion, type, mot_db);
}

void rob_chara::set_hand_r_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t blend_duration,
    float_t a7, float_t step, int32_t a9, float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.state = state;
    motion.data.play_frame_step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.field_24 = a7;
    if (sub_14053F290(&motion)) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.field_38 = a9;
    if (motion.data.motion_id != -1) {
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion.data.motion_id, mot_db);
        motion.data.frame = (motion.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_hand_r_motion(this, &motion, type, mot_db);
}

void rob_chara::set_left_hand_scale(float_t value) {
    data.adjust_data.left_hand_scale = value;
}

bool rob_chara::set_motion_id(uint32_t motion_id,
    float_t frame, float_t blend_duration, bool blend, bool set_motion_reset_data,
    MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db) {
    if (!blend && data.motion.motion_id == motion_id)
        return false;

    if (blend_duration <= 0.0f)
        blend_type = MOTION_BLEND;
    load_motion(motion_id, false, frame, blend_type, bone_data, mot_db);
    rob_chara_bone_data_set_motion_duration(this->bone_data, blend_duration, 1.0f, 0.0f);
    data.motion.field_28 |= 0x80;
    data.motion.frame_data.last_set_frame = frame;
    set_motion_skin_param(motion_id, (float_t)(int32_t)frame);
    if (blend_duration == 0.0f) {
        if (set_motion_reset_data)
            rob_chara::set_motion_reset_data(motion_id, frame);

        item_equip->item_equip_object[ITEM_TE_L].osage_iterations = 60;
        item_equip->item_equip_object[ITEM_TE_R].osage_iterations = 60;

        if (check_for_ageageagain_module()) {
            rob_chara_age_age_array_set_skip(chara_id, 1);
            rob_chara_age_age_array_set_skip(chara_id, 2);
        }
    }
    return true;
}

void rob_chara::set_motion_loop(float_t loop_begin, float_t loop_end, int32_t loop_count) {
    bone_data->set_motion_loop_state(MOT_PLAY_FRAME_DATA_LOOP_CONTINUOUS);
    bone_data->set_motion_loop(loop_begin, loop_count, loop_end);
    if (data.field_1588.field_0.field_58 != 1) {
        data.field_1588.field_0.field_58 = 1;
        data.field_1588.field_0.frame = data.field_1588.field_0.frame_count - 1.0f;
    }
}

void rob_chara::set_motion_reset_data(uint32_t motion_id, float_t frame) {
    item_equip->set_motion_reset_data(motion_id, frame);
}

void rob_chara::set_motion_skin_param(uint32_t motion_id, float_t frame) {
    item_equip->set_motion_skin_param(chara_id, motion_id, (int32_t)roundf(frame));
}

void rob_chara::set_motion_step(float_t value) {
    data.motion.step_data.step = value;
}

void rob_chara::set_mouth_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, int32_t state, float_t blend_duration,
    float_t a7, float_t step, int32_t a9, float_t blend_offset, const motion_database* mot_db) {
    RobMouthMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.state = state;
    motion.data.play_frame_step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.field_24 = a7;
    if (sub_14053F290(&motion)) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.field_38 = a9;
    if (motion.data.motion_id != -1) {
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion.data.motion_id, mot_db);
        motion.data.frame = (motion.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_mouth_motion(this, &motion, type, mot_db);
}

void rob_chara::set_osage_move_cancel(uint8_t id, float_t value) {
    item_equip->set_osage_move_cancel(id, value);
    if (id < 2) {
        rob_chara_age_age_array_set_move_cancel(chara_id, 1, value);
        rob_chara_age_age_array_set_move_cancel(chara_id, 2, value);
    }
}

void rob_chara::set_osage_reset() {
    item_equip->set_osage_reset();
}

void rob_chara::set_osage_step(float_t value) {
    data.motion.step = value;
    if (value < 0.0f)
        value = data.motion.step_data.frame;
    item_equip->set_osage_step(value);
}

void rob_chara::set_parts_disp(item_id id, bool disp) {
    if (id < ITEM_BODY)
        return;
    else if (id < ITEM_ITEM16)
        item_equip->set_disp(id, disp);
    else if (id == ITEM_MAX)
        for (int32_t i = ITEM_ATAMA; i <= ITEM_ITEM16; i++) {
            item_equip->set_disp((item_id)i, disp);
            if (i == ITEM_ATAMA && check_for_ageageagain_module()) {
                rob_chara_age_age_array_set_disp(chara_id, 1, disp);
                rob_chara_age_age_array_set_disp(chara_id, 2, disp);
                rob_chara_age_age_array_set_skip(chara_id, 1);
                rob_chara_age_age_array_set_skip(chara_id, 2);
            }
        }
}

void rob_chara::set_right_hand_scale(float_t value) {
    data.adjust_data.right_hand_scale = value;
}

void rob_chara::set_shadow_cast(bool value) {
    if (value)
        item_equip->field_A0 |= 0x04;
    else
        item_equip->field_A0 &= ~0x04;
}

void rob_chara::set_step_motion_step(float_t value) {
    data.motion.step_data.step = value;
    if (value < 0.0f)
        data.motion.step_data.frame = 1.0f;
    else
        data.motion.step_data.frame = value;

    rob_chara_bone_data_set_step(bone_data, data.motion.step_data.frame);

    float_t step = data.motion.step;
    if (step < 0.0f)
        step = data.motion.step_data.frame;
    item_equip->set_osage_step(step);
}

void rob_chara::set_use_opd(bool value) {
    item_equip->use_opd = value;
}

void rob_chara::set_visibility(bool value) {
    if (value) {
        data.field_0 |= 0x01;
        data.field_3 |= 0x01;
    }
    else {
        data.field_0 &= ~0x01;
        data.field_3 &= ~0x01;
    }

    if (check_for_ageageagain_module()) {
        rob_chara_age_age_array_set_disp(chara_id, 1, value);
        rob_chara_age_age_array_set_disp(chara_id, 2, value);
    }
}

void rob_chara::set_wind_strength(float_t value) {
    item_equip->wind_strength = value;
}

static void sub_140554830(rob_chara* rob_chr, uint8_t a2, uint8_t a3,
    float_t a4, float_t a5, float_t a6, float_t a7, float_t a8, bool a9);

static bool sub_140413710(mot_play_data* a1) {
    if (a1->loop_frames_enabled)
        return a1->loop_frames <= 0.0f;
    else if (a1->frame_data.loop_state >= MOT_PLAY_FRAME_DATA_LOOP_ONCE
        && a1->frame_data.loop_state <= MOT_PLAY_FRAME_DATA_LOOP_REVERSE)
        return false;
    else if (a1->frame_data.playback_state == MOT_PLAY_FRAME_DATA_PLAYBACK_FORWARD)
        return a1->frame_data.last_frame < a1->frame_data.frame;
    else if (a1->frame_data.playback_state == MOT_PLAY_FRAME_DATA_PLAYBACK_BACKWARD)
        return a1->frame_data.frame < 0.0f;
    else
        return false;
}

static bool sub_140419E90(rob_chara_bone_data* rob_bone_data) {
    return sub_140413710(&rob_bone_data->motion_loaded.front()->mot_play_data);
}

static void sub_140505310(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
    switch (rob_chr->data.field_1588.field_0.field_58) {
    case -1: {
        rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x81;
        rob_chr->data.motion.frame_data.frame = rob_chr->data.field_1588.field_0.frame;
    } break;
    case 0: {
        uint32_t motion_id = rob_chr->data.field_1588.field_0.field_5C;
        if (motion_id == -1)
            rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x81;
        else
            rob_chr->load_motion(motion_id, !!(rob_chr->data.field_1588.field_0.field_60 & 0x01),
                0.0f, MOTION_BLEND, bone_data, mot_db);
    } break;
    case 1: {
        struc_223* v1 = &rob_chr->data.field_1588;
        int32_t loop_count = rob_chr->data.field_1588.field_0.loop_count;
        if (loop_count > 0) {
            uint32_t motion_id = rob_chr->data.field_1588.field_0.field_64;
            if (motion_id != -1) {
                if (rob_chr->data.motion.loop_index >= loop_count) {
                    int32_t v6 = mothead_storage_get_mot_by_motion_id(motion_id, mot_db)->field_0.field_0;
                    if (!(v6 & 0x80000) && !(v6 & 0x100000))
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

        rob_chr->data.motion.frame_data.frame = rob_chr->bone_data->get_frame();
        v1->field_330.field_0.current = v1->field_330.field_0.data;
        rob_chr->data.motion.loop_index++;
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
        && !(rob_chr->data.field_1588.field_0.field_20.field_4 & 0x10000000)
        && !(rob_chr->data.field_1588.field_0.field_20.field_4 & 0x20000000))
        rob_chr->data.field_1588.field_0.field_20.field_0 = v3 & ~0x01;

    v4 = rob_chr->data.field_1588.field_0.field_10.field_0;
    if (v4 & 0x10000 && v4 & 0x20000)
        rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x20000;
    rob_chr->data.motion.field_24 = 2;

LABEL_23:
    float_t v5 = rob_chr->data.field_1588.field_0.field_80;
    if (v5 >= 0.0f && v5 <= rob_chr->data.motion.frame_data.frame) {
        if (v5 < (rob_chr->data.field_1588.field_0.frame - 1.0f)
            || rob_chr->data.field_1588.field_0.field_58 == -1
            || rob_chr->data.field_1588.field_0.field_58 == 2) {
            rob_chr->data.field_1588.field_0.field_20.field_0 &= ~0x2020081;
            rob_chr->data.field_2 |= 0x20;
        }
        rob_chr->data.motion.field_24 = 3;
    }
}

static void mothead_apply(struc_223* a1, rob_chara* rob_chr, float_t frame, const motion_database* mot_db) {
    a1->field_330.field_338 = 0;

    struc_377& v4 = a1->field_330.field_0;
    if (!v4.current)
        return;

    mothead_func_data v7;
    v7.rob_chr = rob_chr;
    v7.rob_chr_data = &rob_chr->data;
    if (rob_chr->field_20)
        v7.field_10 = (rob_chara_data*)((size_t)rob_chr->field_20 + 0x440);
    else
        v7.field_10 = 0;
    v7.field_18 = rob_chr;
    v7.field_28 = &rob_chr->data.field_1588;
    v7.field_20 = &rob_chr->data;
    v7.is_x = v4.is_x; // X

    int32_t frame_int = (int32_t)frame;
    while (v4.current->frame <= frame_int && v4.current->type >= MOTHEAD_DATA_TYPE_0) {
        if (v4.current->type >= MOTHEAD_DATA_MAX) {
            v4.current++;
            continue;
        }

        mothead_data_type type = v4.current->type;
        mothead_func func = mothead_func_array[type].func;
        if (func) {
            if (!(mothead_func_array[type].flags & 0x01) || frame_int <= 0)
                func(&v7, v4.current->data, v4.current, frame_int, mot_db);
            else {
                frame_int -= v4.current->frame;
                if (frame_int <= 1)
                    func(&v7, v4.current->data, v4.current, frame_int, mot_db);
            }
        }
        v4.current++;
    }
}

static void sub_1405044B0(rob_chara* rob_chr) {
    if (rob_chr->data.field_1588.field_330.arm_adjust_duration > 0) { // X
        float_t blend = 1.0f;
        if (fabsf(rob_chr->data.field_1588.field_330.arm_adjust_duration) > 0.000001f)
            blend = (rob_chr->data.motion.frame_data.frame
                - (float_t)rob_chr->data.field_1588.field_330.arm_adjust_start_frame)
            / rob_chr->data.field_1588.field_330.arm_adjust_duration;

        blend = blend > 0.0f ? min_def(blend, 1.0f) : 0.0f;
        if (fabsf(blend - 1.0f) <= 0.000001f)
            rob_chr->data.field_1588.field_330.arm_adjust_duration = -1.0f;
        rob_chr->data.arm_adjust_scale = lerp_def(rob_chr->data.field_1588.field_330.arm_adjust_prev_value,
            rob_chr->data.field_1588.field_330.arm_adjust_next_value, blend);

        float_t default_scale = chara_size_table_get_value(1);
        rob_chr->data.adjust_data.item_scale = default_scale
            + (rob_chr->data.adjust_data.scale - default_scale) * rob_chr->data.arm_adjust_scale;
    }

    float_t v2 = rob_chr->bone_data->ik_scale.ratio0;
    float_t v4 = v2;
    if (!rob_chr->data.field_1588.field_330.field_31C) {
        rob_chr->data.motion.field_138 = v2;
        rob_chr->data.motion.field_13C = v2;
        rob_chr->data.motion.field_140 = 0.0f;
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

void rob_chara::sub_1405070E0(const bone_database* bone_data, const motion_database* mot_db) {
    if (sub_140419E90(this->bone_data) || this->bone_data->get_motion_has_looped())
        sub_140505310(this, bone_data, mot_db);
    sub_140505980(this);
    mothead_apply(&data.field_1588, this, data.motion.frame_data.frame, mot_db);
    sub_1405044B0(this);
}

void rob_chara::sub_140509D30() {
    struc_195* v39 = data.field_1E68.field_DF8;
    struc_195* v40 = data.field_1E68.field_1230;
    struc_195* v41 = data.field_1E68.field_1668;
    mat4* v42 = data.field_1E68.field_78;
    mat4* v43 = data.field_1E68.field_738;

    const struc_218* v3 = chara_init_data->field_828;
    const struc_218* v6 = chara_init_data->field_830;
    mat4* m;
    mat4 mat;
    for (int32_t i = 0; i < 27; i++) {
        m = rob_chara_bone_data_get_mats_mat(bone_data, v3->bone_index);
        mat4_translate_mult(m, &v3->bone_offset, &mat);
        *v42 = mat;

        m = rob_chara_bone_data_get_mats_mat(bone_data, v6->bone_index);
        mat4_translate_mult(m, &v3->bone_offset, &mat);
        *v43 = mat;

        float_t chara_scale = data.adjust_data.scale;

        vec3 v23 = *(vec3*)&v42->row3 * chara_scale + data.adjust_data.trans;
        *(vec3*)&v42->row3 = v23;

        vec3 v29 = *(vec3*)&v43->row3 * chara_scale + data.adjust_data.trans;
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

void rob_chara::rob_info_ctrl() {
    data.field_3DA0 = data.field_3D9C;
    if (data.field_1E68.field_64 > 0.01f)
        data.field_2 |= 0x40;

    //sub_140517CC0(this);
    //sub_140517B80(this);
    //sub_140517F20(this);

    /*int32_t v4 = sub_14013C8C0()->sub_1400E7910();
    if (v4 >= 4 && v4 != 8)
        sub_14054FF20(this);*/
}

void rob_chara::sub_140551000() {
    struc_223* v1 = &data.field_1588;
    v1->field_330.field_0.current = v1->field_330.field_0.data;
}

void rob_chara::sub_140554790(bool a2, float_t a3, float_t a5, float_t a6, float_t a7, float_t a8) {
    sub_140554830(this, true, a2, a3, a5, a6, a7, a8, false);
}

void rob_chara::sub_1405547E0(bool a2, float_t a3, float_t a5, float_t a6, float_t a7, float_t a8) {
    sub_140554830(this, true, a2, a3, a5, a6, a7, a8, false);
}

pv_data_set_motion::pv_data_set_motion() : frame_stage_index() {
    motion_id = -1;
}

pv_data_set_motion::pv_data_set_motion(uint32_t motion_id) : frame_stage_index() {
    this->motion_id = motion_id;
}

pv_data_set_motion::pv_data_set_motion(uint32_t motion_id, std::pair<float_t, int32_t> frame_stage_index) {
    this->motion_id = motion_id;
    this->frame_stage_index = frame_stage_index;
}

osage_init_data::osage_init_data() : rob_chr() {
    pv_id = -1;
    motion_id = -1;
    frame = -1;
}

osage_init_data::osage_init_data(rob_chara* rob_chr, uint32_t motion_id) {
    this->rob_chr = rob_chr;
    pv_id = -1;
    this->motion_id = motion_id;
    frame = -1;
}

osage_init_data::osage_init_data(rob_chara* rob_chr, int32_t pv_id,
    uint32_t motion_id, std::string&& path, int32_t frame) {
    this->rob_chr = rob_chr;
    this->pv_id = pv_id;
    this->motion_id = motion_id;
    this->path.assign(path);
    this->frame = frame;
}

osage_init_data::~osage_init_data() {

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
    if (a1->disable_mot_anim)
        return;

    mat4 mat;
    if (a1->has_parent)
        mat = *a1->parent_mat;
    else
        mat = mat4_identity;

    if (a1->type == BONE_DATABASE_BONE_POSITION) {
        mat4_translate_mult(&mat, &a1->trans, &mat);
        a1->rot_mat[0] = mat4_identity;
    }
    else if (a1->type == BONE_DATABASE_BONE_TYPE_1) {
        mat4_mult_vec3_inv_trans(&mat, &a1->trans, &a1->trans);
        mat4_translate_mult(&mat, &a1->trans, &mat);
        a1->rot_mat[0] = mat4_identity;
    }
    else {
        if (a1->motion_bone_index == MOTION_BONE_KL_EYE_L
            || a1->motion_bone_index == MOTION_BONE_KL_EYE_R) {
            if (a1->rotation.x > 0.0f)
                a1->rotation.x *= a1->eyes_xrot_adjust_pos;
            else if (a1->rotation.x < 0.0f)
                a1->rotation.x *= a1->eyes_xrot_adjust_neg;
        }

        mat4 rot_mat;
        if (a1->type == BONE_DATABASE_BONE_POSITION_ROTATION)
            mat4_rotate_zyx_mult(&a1->rot_mat[0], &a1->rotation, &rot_mat);
        else {
            a1->trans = a1->base_translation[skeleton_select];
            mat4_rotate_zyx(&a1->rotation, &rot_mat);
        }

        mat4_translate_mult(&mat, &a1->trans, &mat);
        mat4_mult(&rot_mat, &mat, &mat);
        a1->rot_mat[0] = rot_mat;
    }

    *a1->node[0].mat = mat;

    bone_data_mult_ik(a1, skeleton_select);
}

static void bone_data_mult_1(bone_data* a1, mat4* parent_mat, bone_data* a3, bool solve_ik) {
    mat4 mat;
    if (a1->has_parent)
        mat = *a1->parent_mat;
    else
        mat = *parent_mat;

    if (a1->type != BONE_DATABASE_BONE_TYPE_1 && a1->type != BONE_DATABASE_BONE_POSITION) {
        if (a1->type != BONE_DATABASE_BONE_POSITION_ROTATION)
            a1->trans = a1->base_translation[1];

        mat4_translate_mult(&mat, &a1->trans, &mat);
        if (solve_ik) {
            a1->node[0].exp_data.rotation = 0.0f;
            if (!a1->disable_mot_anim)
                mat4_get_rotation(&a1->rot_mat[0], &a1->node[0].exp_data.rotation);
            else if (bone_data_mult_1_exp_data(a1, &a1->node[0].exp_data, a3))
                mat4_rotate_zyx(&a1->node[0].exp_data.rotation, &a1->rot_mat[0]);
            else {
                *a1->node[0].mat = mat;

                if (bone_data_mult_1_ik(a1, a3)) {
                    mat4 rot_mat;
                    mat4_invrot_normalized(&mat, &rot_mat);
                    mat4_mult(a1->node[0].mat, &rot_mat, &rot_mat);
                    mat4_clear_trans(&rot_mat, &rot_mat);
                    mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
                    a1->rot_mat[0] = rot_mat;
                }
                else
                    a1->rot_mat[0] = mat4_identity;
            }
        }

        mat4_mult(&a1->rot_mat[0], &mat, &mat);
    }
    else {
        mat4_translate_mult(&mat, &a1->trans, &mat);
        if (solve_ik)
            a1->node[0].exp_data.rotation = 0.0f;
    }

    *a1->node[0].mat = mat;
    if (solve_ik) {
        a1->node[0].exp_data.position = a1->trans;
        a1->node[0].exp_data.reset_scale();
    }

    if (a1->type < BONE_DATABASE_BONE_HEAD_IK_ROTATION)
        return;

    mat4_mult(&a1->rot_mat[1], &mat, &mat);
    *a1->node[1].mat = mat;

    if (a1->type == BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        mat4_translate_mult(&mat, a1->ik_segment_length[1], 0.0f, 0.0f, &mat);
        *a1->node[2].mat = mat;
        if (!solve_ik)
            return;

        a1->node[1].exp_data.position = 0.0f;
        mat4_get_rotation(&a1->rot_mat[1], &a1->node[1].exp_data.rotation);
        a1->node[1].exp_data.reset_scale();
        a1->node[2].exp_data.set_position_rotation(
            a1->ik_segment_length[1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    else {
        mat4_translate_mult(&mat, a1->ik_segment_length[1], 0.0f, 0.0f, &mat);
        mat4_mult(&a1->rot_mat[2], &mat, &mat);
        *a1->node[2].mat = mat;

        mat4_translate_mult(&mat, a1->ik_2nd_segment_length[1], 0.0f, 0.0f, &mat);
        *a1->node[3].mat = mat;
        if (!solve_ik)
            return;

        a1->node[1].exp_data.position = 0.0f;
        mat4_get_rotation(&a1->rot_mat[1], &a1->node[1].exp_data.rotation);
        a1->node[1].exp_data.reset_scale();
        a1->node[2].exp_data.position = { a1->ik_segment_length[1], 0.0f, 0.0f };
        mat4_get_rotation(&a1->rot_mat[2], &a1->node[2].exp_data.rotation);
        a1->node[2].exp_data.reset_scale();
        a1->node[3].exp_data.set_position_rotation(
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
        v30 = -v30;
        bone_data_mult_1_ik_legs(a1, &v30);
        break;
    case MOTION_BONE_N_MOMO_A_R_WJ_CD_EX:
        v5 = a2[MOTION_BONE_CL_MOMO_R].node;
        mat4_get_translation(v5[2].mat, &v30);
        mat4_mult_vec3_inv_trans(a1->node[0].mat, &v30, &v30);
        v30 = -v30;
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
    len = vec3::length_squared(v15);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        v15 *= 1.0f / len;

    vec3 v17;
    v17.x = -v15.z * v15.x - v15.z;
    v17.y = -v15.y * v15.z;
    v17.z = v15.x * v15.x + v15.y * v15.y + v15.x;
    len = vec3::length_squared(v17);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        v17 *= 1.0f / len;

    vec3 v16 = vec3::cross(v17, v15);

    mat4 rot_mat = mat4_identity;
    *(vec3*)&rot_mat.row0 = v15;
    *(vec3*)&rot_mat.row1 = v16;
    *(vec3*)&rot_mat.row2 = v17;

    mat4_mult(&rot_mat, a1->node[0].mat, a1->node[0].mat);
}

static void bone_data_mult_1_ik_hands_2(bone_data* a1, vec3* a2, float_t a3) {
    vec3 v8;
    mat4_mult_vec3_inv_trans(a1->node[0].mat, a2, &v8);
    v8.x = 0.0f;

    float_t len = vec3::length(v8);
    if (len <= 0.000001f)
        return;

    v8 *= 1.0f / len;
    float_t angle = atan2f(v8.z, v8.y);
    mat4_rotate_x_mult(a1->node[0].mat, angle * a3, a1->node[0].mat);
}

static void bone_data_mult_1_ik_legs(bone_data* a1, vec3* a2) {
    vec3 v9 = *a2;

    float_t len;
    len = vec3::length_squared(v9);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        v9 *= 1.0f / len;

    vec3 v8;
    v8.x = -v9.z * v9.x;
    v8.y = -v9.z * v9.y - v9.z;
    v8.z = v9.y * v9.y - v9.x * -v9.x + v9.y;

    len = vec3::length_squared(v8);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        v8 *= 1.0f / len;

    vec3 v10 = vec3::cross(v9, v8);

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
    float_t v6 = vec2::length_squared(*(vec2*)&v30);
    float_t v8 = vec3::length_squared(v30);
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
        float_t pole_target_length = vec2::length(*(vec2*)&pole_target.y);
        if (pole_target_length > 0.000001f) {
            pole_target *= 1.0f / pole_target_length;
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

        rot_cos = clamp_def(rot_cos, -1.0f, 1.0f);
        rot_2nd_cos = clamp_def(rot_2nd_cos, -1.0f, 1.0f);

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
        else if (!data->disable_mot_anim) {
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
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data) {
    const char* base_name = bone_database_skeleton_type_to_string(rob_bone_data->base_skeleton_type);
    const char* name = bone_database_skeleton_type_to_string(rob_bone_data->skeleton_type);
    const std::vector<bone_database_bone>* common_bones = bone_data->get_skeleton_bones(base_name);
    const std::vector<vec3>* common_translation = bone_data->get_skeleton_positions(base_name);
    const std::vector<vec3>* translation = bone_data->get_skeleton_positions(name);
    if (!common_bones || !common_translation || !translation)
        return;

    bone->rob_bone_data = rob_bone_data;
    bone_data_parent_load_rob_chara(bone);
    bone_data_parent_load_bone_database(bone, common_bones, common_translation->data(), translation->data());
}

static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    const std::vector<bone_database_bone>* bones, const vec3* common_translation, const vec3* translation) {
    rob_chara_bone_data* rob_bone_data = bone->rob_bone_data;
    size_t chain_pos = 0;
    size_t total_bone_count = 0;
    size_t ik_bone_count = 0;

    bone_data* bone_node = bone->bones.data();
    for (const bone_database_bone& i : *bones ) {
        bone_node->motion_bone_index = (motion_bone_index)(&i - bones->data());
        bone_node->type = i.type;
        bone_node->mirror = i.mirror;
        bone_node->parent = i.parent;
        bone_node->disable_mot_anim = i.disable_mot_anim;
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
    const mot_data* a2, const bone_database* bone_data, const motion_database* mot_db) {
    if (!a2)
        return;

    std::vector<uint16_t>& bone_indices = a1->bone_indices;
    bone_indices.clear();

    uint16_t key_set_count = a2->key_set_count - 1;
    if (!key_set_count)
        return;

    bone_database_skeleton_type skeleton_type = a1->rob_bone_data->base_skeleton_type;
    const char* skeleton_type_string = bone_database_skeleton_type_to_string(skeleton_type);
    const std::string* bone_names = mot_db->bone_name.data();

    const mot_bone_info* bone_info = a2->bone_info_array;
    for (size_t key_set_offset = 0, i = 0; key_set_offset < key_set_count; i++) {
        motion_bone_index bone_index = (motion_bone_index)bone_data->get_skeleton_bone_index(
            skeleton_type_string, bone_names[bone_info[i].index].c_str());
        if (bone_index == -1) {
            i++;
            bone_index = (motion_bone_index)bone_data->get_skeleton_bone_index(
                skeleton_type_string, bone_names[bone_info[i].index].c_str());
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

static void mot_blend_load_motion(mot_blend* a1, uint32_t motion_id, const motion_database* mot_db) {
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

static void mot_blend_set_blend_duration(mot_blend* a1, float_t duration, float_t step, float_t offset) {
    a1->blend.SetDuration(duration, step, offset);
}

static void mot_blend_set_frame(mot_blend* a1, float_t frame) {
    a1->mot_play_data.frame_data.set_frame(frame);
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
                if (key[temp = length / 2] > frame_int)
                    length /= 2;
                else {
                    key += temp + 1;
                    length -= temp + 1;
                }
            key_index = key - frames;
        }
        else {
            const uint16_t* key = &frames[current_key];
            for (const uint16_t* key_end = &frames[keys_count]; key != key_end; key++)
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
                float_t df = frame - curr_frame;
                float_t t = df / (next_frame - curr_frame);
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
                    float_t t_1 = t - 1.0f;
                    *value = (t_1 * t1 + t * t2) * t_1 * df
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

    if (keys_count & ~0x01)
        keys_count = 2;
    return v3 - keys_count;
}

static bool mot_load_file(mot* a1, const mot_data* a2) {
    a1->frame_count = 0;
    if (!a2)
        return 0;

    a1->frame_count = a2->frame_count;

    int32_t key_set_count = a2->key_set_count;
    bool skeleton_select = a2->skeleton_select != 0;
    a1->key_set_count = key_set_count;
    a1->field_4 = a2->info & 0x8000;

    const mot_key_set_data* key_set_file = a2->key_set_array;
    mot_key_set* key_set = a1->key_sets;

    for (int32_t i = 0; i < key_set_count; i++, key_set++, key_set_file++) {
        key_set->type = key_set_file->type;
        if (key_set->type == MOT_KEY_SET_STATIC)
            key_set->values = key_set_file->values;
        else if (key_set->type != MOT_KEY_SET_NONE) {
            uint16_t keys_count = key_set_file->keys_count;
            key_set->frames = key_set_file->frames;
            key_set->values = key_set_file->values;

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
    const std::vector<bone_database_bone>* a2) {
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

static const mot_data* mot_key_data_load_file(mot_key_data* a1, uint32_t motion_id, const motion_database* mot_db) {
    if (a1->motion_id != motion_id) {
        const mot_data* mot_data = motion_storage_get_mot_data(motion_id, mot_db);
        a1->mot_data = mot_data;
        if (mot_data) {
            a1->skeleton_select = mot_load_file(&a1->mot, mot_data);
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

static void mothead_func_0(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_1(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_2(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr_data->field_1588.field_0.field_20.field_0 |= 0x100;
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    if (rob_chr_data->motion.field_28 & 0x08)
        rob_chr_data->field_1588.field_0.field_274 -= ((int16_t*)data)[0];
    else
        rob_chr_data->field_1588.field_0.field_274 += ((int16_t*)data)[0];
}

static void mothead_func_3(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    int32_t v4 = ((int32_t*)data)[0];
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    (&rob_chr_data->field_1588.field_0.field_20.field_0)[v4 >> 5] |= 1 << (v4 & 0x1F);
}

static void mothead_func_4(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    int32_t v4 = ((int32_t*)data)[0];
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    (&rob_chr_data->field_1588.field_0.field_20.field_0)[v4 >> 5] &= ~(1 << (v4 & 0x1F));
}

static void mothead_func_5(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    rob_chr_data->motion.field_28 ^= (rob_chr_data->motion.field_28 ^ ~rob_chr_data->motion.field_28) & 0x04;
}

static void mothead_func_6(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr_data->field_8.field_1B4 = 0;
    func_data->rob_chr_data->field_1588.field_0.field_20.field_0 |= 0x400;
    func_data->rob_chr_data->field_1588.field_0.field_20.field_0 |= 0x800;
}

static void sub_140551AF0(rob_chara* rob_chr) {
    if (!(rob_chr->data.field_0 & 0x02)) {
        rob_chr->data.miku_rot.position.y = rob_chr->data.miku_rot.field_24.y;
        rob_chr->data.field_0 |= 0x02;
        if (!(rob_chr->data.field_1588.field_0.field_1E8 & 0x01)) {
            rob_chr->data.miku_rot.position.x = rob_chr->data.miku_rot.field_24.x;
            rob_chr->data.miku_rot.position.z = rob_chr->data.miku_rot.field_24.z;
        }
    }
    rob_chr->data.field_1588.field_0.field_20.field_0 |= 0x40;
}

static void mothead_func_7(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    //sub_14036D130(0, &((int16_t*)data)[2]);
}

static void mothead_func_9(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_10(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_11(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_12(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_13(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    v10 = -1;
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
        HIBYTE(v12) = !v9;
    v14 = ((int32_t*)data)[2];
    v13 = (v6 & 4) != 0;
    sub_1401FCE40((int64_t)&v10);*/
}

static void mothead_func_14(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_15(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_16(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_17(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    if (!(v4 & 0x01))
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
    *(int64_t*)&v39.x = 0;
    v39.z = 0.0f;
    v5->field_1588.field_330.field_20.x = v15;
    v19 = ((int16_t*)data)[0];
    *(int64_t*)&a3.x = 0;
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
        vec3::normalize(&v5->field_1588.field_330.field_20);
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
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_19(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_20(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_21(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_22(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_23(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_24(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_25(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_26(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_27(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_28(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_29(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr_data->miku_rot.field_30.x = 0.0f;
    func_data->rob_chr_data->miku_rot.field_30.z = 0.0f;
}

static void mothead_func_31(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static int32_t sub_140533440(int32_t a1) {
    static const uint8_t byte_140A2D538[] = {
       0x00, 0x01, 0x02, 0x03, 0x04, 0x0A, 0x0B, 0x0C, 0x0D,
       0x0E, 0x05, 0x06, 0x07, 0x08, 0x09, 0x15, 0x16, 0x17,
       0x18, 0x19, 0x1A, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
    };

    int32_t v3 = a1 & 0xF800001F;
    for (int32_t i = 0; i < 27; i++)
        if (a1 & (1 << i))
            v3 |= 1 << byte_140A2D538[i];
    return v3;
}

static void mothead_func_32(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    struc_651& v2 = rob_chr_data->field_1588.field_330;
    if (func_data->is_x) { // X
        float_t value = ((float_t*)data)[0];
        float_t duration = ((float_t*)data)[1];
        v2.arm_adjust_next_value = value;
        v2.arm_adjust_prev_value = rob_chr_data->arm_adjust_scale;
        v2.arm_adjust_start_frame = mhd_data->frame;
        v2.arm_adjust_duration = max_def(duration, 0.0f);
        return;
    }

    float_t v8 = (float_t)((int16_t*)data)[0];
    int32_t v5 = ((int32_t*)data)[1];
    float_t v9 = ((float_t*)data)[2];
    int32_t v10 = ((int32_t*)data)[3];

    if (rob_chr_data->motion.field_28 & 0x08)
        v5 = sub_140533440(v5);

    float_t v7 = rob_chr_data->field_1588.field_0.frame - 1.0f
        - rob_chr_data->motion.frame_data.frame;
    if (v7 >= v8)
        v7 = v8;

    for (int32_t i = 0; i < 3; i++) {
        if (!(v10 & (1 << i)))
            continue;

        for (int32_t j = 0; j < 27; j++) {
            if (!(v5 & (1 << j)))
                continue;

            v2.field_2C[i].field_0[j] = 1;
            v2.field_80[i].field_0[j] = v9;
            v2.field_1C4[i].field_0[j] = v7;
        }
    }
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
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_35(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_36(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_37(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_38(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_39(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_40(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_41(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_42(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_43(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_44(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_45(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_46(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_47(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_48(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_49(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void rob_chara_set_face_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db);

static void mothead_func_50_set_face_motion_id(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_set_face_motion_id(func_data->rob_chr, ((int32_t*)data)[0],
        ((float_t*)data)[1], 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_51(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_52(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_53_set_face_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
     func_data->rob_chr->set_face_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
         ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, false, mot_db);
}

static void mothead_func_54_set_hand_r_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_hand_r_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 12.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_55_set_hand_l_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_hand_l_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 12.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_56_set_mouth_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_mouth_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_57_set_eyes_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_eyes_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_58_set_eyelid_mottbl_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_eyelid_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_eyelid_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db);
static void rob_chara_set_eyes_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db);

static void rob_chara_set_head_object(rob_chara* rob_chr, int32_t head_object_id, const motion_database* mot_db) {
    rob_chr->data.motion.field_150.head_object = rob_chara_get_head_object(rob_chr, head_object_id);
    rob_chara_set_face_motion_id(rob_chr, -1, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
    rob_chara_set_eyelid_motion_id(rob_chr, -1, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
    rob_chara_set_eyes_motion_id(rob_chr, -1, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
    rob_chr->bone_data->field_788.field_91 = true;
}

static void mothead_func_59_set_rob_chara_head_object(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_set_head_object(func_data->rob_chr, ((int32_t*)data)[0], mot_db);
}

static void sub_14041C950(rob_chara_bone_data* rob_bone_data, bool a2, bool a3,
    float_t a4, float_t a5, float_t a6, float_t a7, float_t a8, bool a9) {
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
    v11->field_98 = a5 < 0.0f ? 1.0f : a5;
    v11->field_9C = a7 < 0.0f ? 1.0f : a7;
    v11->field_A0 = a6 < 0.0f ? 1.0f : a6;
    v11->field_A4 = 0.0f;
    v11->field_A8 = 1.0f;
    v11->field_AC = 0.0f;
    v11->field_B0 = a8;
    v11->field_1C4 = a9;
}

static void sub_140554830(rob_chara* rob_chr, uint8_t a2, uint8_t a3,
    float_t a4, float_t a5, float_t a6, float_t a7, float_t a8, bool a9) {
    sub_14041C950(rob_chr->bone_data, a2, a3, a4, a5, a6, a7, a8, a9);
    if (fabsf(a6) <= 0.000001f)
        rob_chr->data.field_2 |= 0x80;
}

static void mothead_func_60(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    sub_140554830(func_data->rob_chr, 1, ((int32_t*)data)[0] != 0,
        ((float_t*)data)[1], ((float_t*)data)[2], ((float_t*)data)[3] * 6.0f, 0.25f, 0.0f, false);
}

static void mothead_func_61_set_eyelid_motion_from_face(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;

    rob_chara_data_adjust v16;
    v16.reset();

    int8_t type = ((int8_t*)data)[5];
    if (type >= 0 && type < 6) {
        float_t set_frame = (float_t)mhd_data->frame;
        v16.enable = true;
        v16.frame = rob_chr->data.motion.frame_data.frame - set_frame;
        v16.transition_frame = rob_chr->data.motion.frame_data.frame - set_frame;
        v16.motion_id = rob_chr->data.motion.motion_id;
        v16.set_frame = set_frame;
        v16.transition_duration = (float_t)((int32_t*)data)[0];
        v16.type = type;
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
        v16.strength_transition = (float_t)((int32_t*)data)[15];
    }
    rob_chara_set_parts_adjust_by_index(rob_chr, (rob_osage_parts)((uint8_t*)data)[4], &v16);
}

static void mothead_func_63(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_64_osage_reset(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_osage_reset();
}

static void mothead_func_65_motion_skin_param(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;
    uint32_t motion_id = rob_chr->data.motion.motion_id;
    float_t _frame = (float_t)mhd_data->frame;
    rob_chr->set_motion_skin_param(motion_id, _frame);
    rob_chr->set_motion_reset_data(motion_id, _frame);
}

static void mothead_func_66_osage_step(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_osage_step(((float_t*)data)[0]);
}

static void mothead_func_67_sleeve(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    rob_bone_data->sleeve_adjust.enable1 = v6;
    rob_bone_data->sleeve_adjust.enable2 = v5;
    rob_bone_data->sleeve_adjust.radius = ((float_t*)data)[1];
}

static void mothead_func_68(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    motion_blend_mot* v2 = func_data->rob_chr->bone_data->motion_loaded.front();
    v2->mot_key_data.frame = -1.0f;
    v2->mot_key_data.field_68.field_0 = ((int32_t*)data)[0];
    v2->mot_key_data.field_68.field_4 = ((float_t*)data)[0];
}

static void mothead_func_69_motion_max_frame(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    float_t max_frame = (float_t)*(int32_t*)data;
    func_data->rob_chr->bone_data->set_motion_max_frame(max_frame);
    /*pv_game* v6 = pv_game_get();
    if (v6)
        pv_game::set_data_itmpv_max_frame(v6, func_data->rob_chr->chara_id, max_frame);*/
}

static void mothead_func_70(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_osage_move_cancel(((uint8_t*)data)[0], ((float_t*)data)[1]);
}

static void mothead_func_72(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_adjust_data* v6 = &func_data->rob_chr->data.adjust_data;
    v6->offset.x = ((float_t*)data)[0];
    v6->offset.y = ((float_t*)data)[1];
    v6->offset.z = ((float_t*)data)[2];
    v6->offset_x = !!((uint8_t*)data)[24];
    v6->offset_y = !!((uint8_t*)data)[25];
    v6->offset_z = !!((uint8_t*)data)[26];
}

static void mothead_func_73_rob_hand_adjust(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_motion* v4 = &func_data->rob_chr->data.motion;
    int32_t hand = ((int16_t*)data)[0];

    rob_chara_data_hand_adjust* hand_adjust = &v4->hand_adjust[hand];
    rob_chara_data_hand_adjust* hand_adjust_prev = &v4->hand_adjust_prev[hand];
    *hand_adjust_prev = *hand_adjust;

    hand_adjust->reset();
    hand_adjust->scale_select = ((int16_t*)data)[1];
    hand_adjust->duration = ((float_t*)data)[1];
    hand_adjust->type = (rob_chara_data_hand_adjust_type)((int16_t*)data)[4];
    hand_adjust->scale = ((float_t*)data)[3];
    hand_adjust->current_time = v4->frame_data.frame - (float_t)mhd_data->frame;

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
        hand_adjust->offset.x = ((float_t*)data)[5];
        hand_adjust->offset.y = ((float_t*)data)[6];
        hand_adjust->offset.z = ((float_t*)data)[7];
        hand_adjust->enable_scale = !!((uint8_t*)data)[32];
        hand_adjust->disable_x = !!((uint8_t*)data)[33];
        hand_adjust->disable_y = !!((uint8_t*)data)[34];
        hand_adjust->disable_z = !!((uint8_t*)data)[35];
        hand_adjust->scale_blend = ((float_t*)data)[9];
        hand_adjust->arm_length = ((float_t*)data)[10];
        hand_adjust->field_40 = ((int32_t*)data)[11];
        break;
    }
}

static void sub_140555120(rob_chara* rob_chr, rob_osage_parts a2, bool a3) {
    sub_140513EE0(rob_chr->item_equip, a2, a3);
}

static void mothead_func_74(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    sub_140555120(func_data->rob_chr, (rob_osage_parts)((uint8_t*)data)[0], !!((uint8_t*)data)[1]);
}

void rob_chara_set_adjust_global(rob_chara* rob_chr, rob_chara_data_adjust* a2) {
    rob_chara_set_adjust(rob_chr, a2,
        &rob_chr->data.motion.adjust_global, &rob_chr->data.motion.adjust_global_prev);
}

static void mothead_func_75_rob_adjust_global(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;

    rob_chara_data_adjust v14;
    v14.reset();

    int8_t type = ((int8_t*)data)[4];
    if (type >= 0 && type < 6) {
        float_t set_frame = (float_t)mhd_data->frame;
        v14.enable = true;
        v14.frame = rob_chr->data.motion.frame_data.frame - set_frame;
        v14.transition_frame = rob_chr->data.motion.frame_data.frame - set_frame;
        v14.motion_id = rob_chr->data.motion.motion_id;
        v14.set_frame = set_frame;
        v14.transition_duration = (float_t)((int32_t*)data)[0];
        v14.type = type;
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
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;

    rob_chara_data_arm_adjust* arm_adjust = &rob_chr->data.motion.arm_adjust[((int16_t*)data)[0]];
    arm_adjust->enable = true;
    arm_adjust->prev_value = arm_adjust->value;
    arm_adjust->value = 0.0f;
    arm_adjust->next_value = ((float_t*)data)[2];
    arm_adjust->duration = ((float_t*)data)[1];
}

static void mothead_func_77_disable_eye_motion(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->bone_data->set_disable_eye_motion(((uint8_t*)data)[0] != 0);
}

static void mothead_func_78(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    static const int16_t word_140A2D430[] = {
        1, 0, 3, 2, 4, 0, 0, 0
    };

    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    uint16_t v5 = ((uint8_t*)data)[2];
    if (rob_chr_data->motion.field_28 & 0x08)
        v5 = word_140A2D430[v5];

    struc_306* v7 = &rob_chr_data->field_1588.field_330.field_33C[v5];
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
    v7->field_10 = 0.0f;
    v7->field_1C = 0.0f;
    v7->field_28 = 0.0f;
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
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    //rob_chara_set_coli_ring(func_data->rob_chr, ((int8_t*)data)[0]);
}

static void mothead_func_80_adjust_get_global_trans(mothead_func_data* func_data,
    void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr_data->adjust_data.get_global_trans = ((uint8_t*)data)[0];
}

static size_t mothead_data_radix_index_func_frame(mothead_data* data, size_t index) {
    return (uint32_t)data[index].frame;
}

#pragma warning(push)
#pragma warning(disable: 6386)
static void mothead_mot_msgpack_read(const char* path, const char* set_name, mothead* mhd) { // X
    char buf[0x200];
    sprintf_s(buf, sizeof(buf), "%s\\%s.json", path, set_name);
    if (!path_check_file_exists(buf))
        return;

    msgpack msg;

    file_stream s;
    s.open(buf, "rb");
    io_json_read(s, &msg);
    s.close();

    if (msg.type != MSGPACK_ARRAY)
        return;

    size_t data_x_size = 0;
    msgpack_array* ptr = msg.data.arr;
    for (msgpack& i : *ptr) {
        msgpack* mot_id = i.read("MotionID");
        msgpack* arr = i.read_array("Array");
        if (!mot_id || !arr)
            continue;

        uint32_t motion_id = mot_id->read_int32_t();
        if (motion_id < mhd->first_mot_id || motion_id > mhd->last_mot_id)
            continue;

        mothead_mot* mhdm = mhd->mots[(ssize_t)motion_id - mhd->first_mot_id];

        msgpack_array* ptr = arr->data.arr;
        for (msgpack& j : *ptr) {
            msgpack* type = j.read("Type", MSGPACK_STRING);
            msgpack* frm = j.read("Frame", MSGPACK_INT32);
            msgpack* data = j.read("Data", MSGPACK_MAP);
            if (!type || !frm)
                continue;

            if (!type->data.str->compare("32") && data) {
                if (data->read("Value", MSGPACK_FLOAT32) && data->read("Duration", MSGPACK_FLOAT32))
                    data_x_size += 0x08;
            }
            else if (!type->data.str->compare("RobHandAdjust") && data) {
                msgpack* hnd = data->read("Hand", MSGPACK_INT16);
                if (hnd) {
                    int32_t frame = frm->read_int32_t();
                    int16_t hand = hnd->read_int16_t();
                    for (mothead_data& j : mhdm->data)
                        if (j.frame == frame && j.type == MOTHEAD_DATA_ROB_HAND_ADJUST
                            && ((int16_t*)j.data)[0] == hand) {
                            data_x_size += 0x30;
                            break;
                        }
                }
            }
            else if (!type->data.str->compare("RobArmAdjust") && data) {
                msgpack* idx = data->read("Index", MSGPACK_INT16);
                if (idx) {
                    int32_t frame = frm->read_int32_t();
                    int16_t index = idx->read_int16_t();
                    for (mothead_data& j : mhdm->data)
                        if (j.frame == frame && j.type == MOTHEAD_DATA_ROB_ARM_ADJUST
                            && ((int16_t*)j.data)[0] == index) {
                            data_x_size += 0x0C;
                            break;
                        }
                }
            }
        }
    }

    size_t data_x = (size_t)malloc(data_x_size);
    if (!data_x)
        return;

    mhd->data_x = (void*)data_x;

    for (msgpack& i : *ptr) {
        msgpack* mot_id = i.read("MotionID");
        msgpack* arr = i.read_array("Array");
        if (!mot_id || !arr)
            continue;

        uint32_t motion_id = mot_id->read_uint32_t();
        if ((int32_t)motion_id < mhd->first_mot_id || (int32_t)motion_id > mhd->last_mot_id)
            continue;
        else if (!arr->data.arr->size())
            continue;

        mothead_mot* mhdm = mhd->mots[(ssize_t)(int32_t)motion_id - mhd->first_mot_id];

        msgpack_array* ptr = arr->data.arr;
        for (msgpack& j : *ptr) {
            msgpack* type = j.read("Type", MSGPACK_STRING);
            msgpack* frm = j.read("Frame", MSGPACK_INT32);
            msgpack* data = j.read("Data", MSGPACK_MAP);
            if (!type || !frm)
                continue;

            int32_t frame = frm->read_int32_t();

            if (!type->data.str->compare("32") && data) {
                mhdm->data.push_back({ MOTHEAD_DATA_TYPE_32, frame, (void*)data_x });
                ((float_t*)data_x)[0] = data->read_float_t("Value");
                ((float_t*)data_x)[1] = data->read_float_t("Duration");
                data_x += 0x08;
            }
            else if (!type->data.str->compare("RobHandAdjust") && data) {
                msgpack* hnd = data->read("Hand", MSGPACK_INT16);
                if (hnd) {
                    int32_t frame = frm->read_int32_t();
                    int16_t hand = hnd->read_int16_t();
                    for (mothead_data& j : mhdm->data) {
                        if (j.frame != frame || j.type != MOTHEAD_DATA_ROB_HAND_ADJUST
                            || ((int16_t*)j.data)[0] != hand)
                            continue;

                        j.data = (void*)data_x;
                        ((int16_t*)data_x)[0] = hand;
                        ((int16_t*)data_x)[1] = data->read_int16_t("ScaleSelect");
                        ((float_t*)data_x)[1] = data->read_float_t("Duration");
                        ((int16_t*)data_x)[4] = data->read_int16_t("Type");
                        ((float_t*)data_x)[3] = data->read_float_t("Scale");
                        ((float_t*)data_x)[4] = data->read_float_t("RotBlend");
                        ((float_t*)data_x)[5] = data->read_float_t("OffsetX");
                        ((float_t*)data_x)[6] = data->read_float_t("OffsetY");
                        ((float_t*)data_x)[7] = data->read_float_t("OffsetZ");
                        ((uint8_t*)data_x)[32] = data->read_bool("EnableScale") ? 0x01 : 0x00;
                        ((uint8_t*)data_x)[33] = data->read_bool("DisableX") ? 0x01 : 0x00;
                        ((uint8_t*)data_x)[34] = data->read_bool("DisableY") ? 0x01 : 0x00;
                        ((uint8_t*)data_x)[35] = data->read_bool("DisableZ") ? 0x01 : 0x00;
                        ((float_t*)data_x)[9] = data->read_float_t("OffsetBlend");
                        ((float_t*)data_x)[10] = data->read_float_t("ArmLength");
                        ((int32_t*)data_x)[11] = data->read_int32_t("i2C");
                        data_x += 0x30;
                        break;
                    }
                }
            }
            else if (!type->data.str->compare("RobArmAdjust") && data) {
                msgpack* idx = data->read("Index", MSGPACK_INT16);
                if (idx) {
                    int32_t frame = frm->read_int32_t();
                    int16_t index = idx->read_int16_t();
                    for (mothead_data& j : mhdm->data) {
                        if (j.frame != frame || j.type != MOTHEAD_DATA_ROB_ARM_ADJUST
                            || ((int16_t*)j.data)[0] != index)
                            continue;

                        j.data = (void*)data_x;
                        ((int16_t*)data_x)[0] = index;
                        ((float_t*)data_x)[1] = data->read_float_t("Duration");
                        ((float_t*)data_x)[2] = data->read_float_t("Value");
                        data_x += 0x0C;
                        break;
                    }
                }
            }
        }

        radix_sort_custom(mhdm->data.data(), mhdm->data.size(), sizeof(mothead_data),
            sizeof(uint32_t), (radix_index_func)mothead_data_radix_index_func_frame);
        mhdm->is_x = true;
    }
}
#pragma warning(pop)

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
    vec3 global_trans = reverse ? -keyframe_data[0] : keyframe_data[0];
    vec3 global_rotation = keyframe_data[1];
    a1->bone_data.global_trans = global_trans;
    a1->bone_data.global_rotation = global_rotation;

    float_t rot_y = a1->bone_data.rot_y;
    mat4 mat;
    mat4_rotate_y(rot_y, &mat);
    mat4_translate_mult(&mat, &global_trans, &mat);
    mat4_rotate_zyx_mult(&mat, &global_rotation, &mat);
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
    return *a1 & 0x01 && !(*a1 & 0x08) || !(*a1 & 0x01) && *a1 & 0x08;
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

static void sub_140413350(struc_240* a1, PFNMOTIONBONECHECKFUNC bone_check_func, size_t motion_bone_count) {
    a1->bone_check_func = bone_check_func;
    a1->motion_bone_count = motion_bone_count;
    sub_1404119A0(&a1->field_8);
    sub_140414550(&a1->field_8, motion_bone_count, 0);
    sub_1404146F0(a1);
}

static void motion_blend_mot_load_bone_data(motion_blend_mot* a1,
    rob_chara_bone_data* a2, PFNMOTIONBONECHECKFUNC bone_check_func, const bone_database* bone_data) {
    bone_data_parent_data_init(&a1->bone_data, a2, bone_data);
    mot_key_data_init_key_sets(
        &a1->mot_key_data,
        a1->bone_data.rob_bone_data->base_skeleton_type,
        a1->bone_data.motion_bone_count,
        a1->bone_data.ik_bone_count);
    sub_140413350(&a1->field_0, bone_check_func, a1->bone_data.motion_bone_count);
}

static void sub_1401EB1D0(bone_data* a1, int32_t skeleton_select) {
    if (!a1->disable_mot_anim) {
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
    for (bone_data& i : a1->bone_data.bones)
        if ((1 << (i.motion_bone_index & 0x1F))
            & a1->field_0.field_8.bitfield.data()[i.motion_bone_index >> 5]) {
            sub_1401EB1D0(&i, skeleton_select);
            if (i.type == BONE_DATABASE_BONE_POSITION_ROTATION && (a1->field_4F8.field_0 & 0x02))
                i.trans_prev[skeleton_select] += a1->field_4F8.field_90;
        }
}

static void sub_140412F20(mot_blend* a1, std::vector<bone_data>* a2) {
    for (bone_data& i : *a2)
        if (((1 << (i.motion_bone_index & 0x1F))
            & a1->field_0.field_8.bitfield.data()[i.motion_bone_index >> 5]) != 0)
            sub_1401EB1D0(&i, 0);
}

static void motion_blend_mot_load_file(motion_blend_mot* a1, uint32_t motion_id,
    MotionBlendType blend_type, float_t blend, const bone_database* bone_data, const motion_database* mot_db) {
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
    a1->field_4F8.mat = mat4_identity;
}

static void sub_140414900(struc_308* a1, mat4* a2) {
    if (a2) {
        a1->field_8C = true;
        a1->mat = *a2;
    }
    else {
        a1->field_8C = false;
        a1->mat = mat4_identity;
    }
}

static void motion_blend_mot_mult_mat(motion_blend_mot* a1, mat4* mat) {
    sub_140414900(&a1->field_4F8, mat);

    mat4& m = a1->field_4F8.mat;
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
        a1->freeze.Reset();
        a1->blend = &a1->freeze;
        sub_140412E10(a1, 0);
        break;
    case MOTION_BLEND_CROSS:
        a1->cross.Reset();
        a1->blend = &a1->cross;
        break;
    case MOTION_BLEND_COMBINE:
        a1->combine.Reset();
        a1->blend = &a1->combine;

        a1->blend->blend = clamp_def(blend, 0.0f, 1.0f);
        a1->blend->enable = true;
        a1->blend->rot_y = true;
        break;
    }
}

static void motion_blend_mot_set_duration(motion_blend_mot* mot,
    float_t duration, float_t step, float_t offset) {
    if (mot->blend)
        mot->blend->SetDuration(duration, step, offset);
}

static int16_t opd_data_encode_shift(float_t value, uint8_t shift) {
    int32_t val = (int)(float)((float)(1 << shift) * value);
    if (val & 0x01)
        if (val & 0x8000)
            val++;
        else
            val--;
    return val;
}

static float_t opd_data_decode_shift(int16_t value, uint8_t shift) {
    return (float_t)value / (float_t)(1 << shift);
}

static void opd_data_decode(const int16_t* src_data, size_t count, uint8_t shift, float_t* dst_data) {
    size_t index = 0;
    for (size_t i = 0; i < count; i++, index++) {
        if (src_data[i] & 0x01) {
            float_t first_val = dst_data[index - 1];
            float_t last_val = opd_data_decode_shift(src_data[i + 1], shift);
            float_t diff_val = last_val - first_val;

            int32_t lerp_count = (int32_t)((src_data[i] >> 1) - (index - 1));
            float_t scale =  1.0f / (float_t)lerp_count;
            for (int32_t j = 1; j < lerp_count; j++, index++)
                dst_data[index] = (float_t)j * diff_val * scale + first_val;
            dst_data[index] = last_val;
            i++;
        }
        else
            dst_data[index] = opd_data_decode_shift(src_data[i], shift);
    }
}

static bool opd_decode(const osage_play_data_header* file_head, float_t** opd_decod_buf, osage_play_data_header* head) {
    const osage_play_data_node_header* node = (osage_play_data_node_header*)&file_head[1];
    float_t* buf = force_malloc_s(float_t, 3ULL * file_head->frame_count * file_head->nodes_count);
    *opd_decod_buf = buf;
    if (!buf)
        return false;

    for (size_t i = 0; i < file_head->nodes_count; i++)
        for (uint32_t i = 3; i; i--) {
            opd_data_decode(node->data, node->count, (uint8_t)node->shift, buf);
            buf += file_head->frame_count;
            node = (const osage_play_data_node_header*)((size_t)node
                + sizeof(const osage_play_data_node_header) + node->count * sizeof(uint16_t));
        }

    *head = *file_head;
    return true;
}

static bool opd_decode_data(const void* data, float_t** opd_decod_buf, osage_play_data_header* head) {
    if (!*opd_decod_buf)
        return opd_decode((const osage_play_data_header*)data, opd_decod_buf, head);
    return false;
}

static bool pv_osage_manager_array_get_disp(int32_t* chara_id) {
    return pv_osage_manager_array_get(*chara_id)->GetDisp();
}

static PvOsageManager* pv_osage_manager_array_get(int32_t chara_id) {
    if (chara_id >= ROB_CHARA_COUNT)
        chara_id = 0;
    return &pv_osage_manager_array[chara_id];
}

static int pv_data_set_motion_quicksort_compare_func(void const* src1, void const* src2) {
    pv_data_set_motion* pv1 = (pv_data_set_motion*)src1;
    pv_data_set_motion* pv2 = (pv_data_set_motion*)src2;
    if (pv1->motion_id > pv2->motion_id)
        return 1;
    else if (pv1->motion_id < pv2->motion_id)
        return -1;
    else if (pv1->frame_stage_index.first > pv2->frame_stage_index.first)
        return 1;
    else if (pv1->frame_stage_index.first < pv2->frame_stage_index.first)
        return -1;
    else if (pv1->frame_stage_index.second > pv2->frame_stage_index.second)
        return 1;
    else if (pv1->frame_stage_index.second < pv2->frame_stage_index.second)
        return -1;
    else
        return 0;
}

static void rob_base_rob_chara_init(rob_chara* rob_chr) {
    //sub_14054F4A0(&rob_chr->rob_touch);
    //sub_14054F830(&rob_chr->rob_touch, rob_chr->pv_data.field_70 != 0);
}

static mat4* sub_140504E80(rob_chara* rob_chr) {
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
    sub_14041DA50(rob_chr->bone_data, sub_140504E80(rob_chr));
    rob_chr->data.miku_rot.field_48 = rob_chr->data.miku_rot.field_24;

    mat4* v3 = rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, ROB_BONE_N_HARA);
    mat4_get_translation(v3, &rob_chr->data.miku_rot.field_24);
    rob_chr->data.miku_rot.field_54 = rob_chr->data.miku_rot.field_24
        - rob_chr->data.miku_rot.field_48;

    mat4* v8 = rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, ROB_BONE_N_HARA_CP);
    vec3 v10 = { 0.0f, 0.0f, 1.0f };
    mat4_mult_vec3(v8, &v10, &v10);
    rob_chr->data.miku_rot.field_6 = (int32_t)((float_t)(atan2f(v10.x, v10.z) * 32768.0f) * (1.0 / M_PI));
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

static void rob_base_rob_chara_ctrl(rob_chara* rob_chr) {
    sub_140507F60(rob_chr);
    rob_chr->adjust_ctrl();
    sub_140505B20(rob_chr);
    //sub_14036D130(7, &rob_chr);
    rob_chr->set_data_adjust_mat(&rob_chr->data.adjust_data);
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

static void sub_1405145F0(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
    if (rob_chr->data.field_8.field_0)
        return;

    float_t frame;
    if (!(rob_chr->data.motion.field_28 & 0x80))
        frame = 0.0f;
    else
        frame = rob_chr->data.motion.frame_data.last_set_frame;
    rob_chr->load_motion(rob_chr->data.field_8.field_4.motion_id,
        rob_chr->data.field_8.field_4.field_10, frame, MOTION_BLEND, bone_data, mot_db);
    rob_chara_bone_data_set_motion_duration(rob_chr->bone_data, 0.0f, 1.0f, 1.0f);
    rob_chr->set_motion_reset_data(rob_chr->data.field_8.field_4.motion_id, 0.0f);
    rob_chr->data.field_8.field_0 = 1;
}

static void sub_1405144C0(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
    if (rob_chr->data.field_8.field_4.field_0 == 1) {
        pv_expression_array_ctrl(rob_chr);
        sub_1405145F0(rob_chr, bone_data, mot_db);
    }
}

static void sub_140514520(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
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
        a1->data.field_0->Field_10(a1->data.param);

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

static void sub_1405077D0(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
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

static void rob_chara_set_face_motion(rob_chara* rob_chr,
    RobFaceMotion* motion, int32_t type, const motion_database* mot_db);

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

static void rob_chara_set_face_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db) {
    RobFaceMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.state = state;
        motion.data.frame = frame;
        motion.data.play_frame_step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.field_24 = a6;
        if (sub_14053F290(&motion)) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.field_38 = a8;
    }
    rob_chara_set_face_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_1405509D0(rob_chara* rob_chr, const motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.face))
        rob_chara_set_face_motion_id(rob_chr, -1, 0.0f, -1, 6.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_hand_l_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db);

static void sub_140553E30(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.state = state;
        motion.data.frame = frame;
        motion.data.play_frame_step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.field_24 = a6;
        if (sub_14053F290(&motion)) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.field_38 = a8;
    }
    rob_chara_set_hand_l_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550A40(rob_chara* rob_chr, const motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.hand_l))
        sub_140553E30(rob_chr, -1, 0.0f, -1, 12.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_hand_r_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db);

static void sub_140554370(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.state = state;
        motion.data.frame = frame;
        motion.data.play_frame_step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.field_24 = a6;
        if (sub_14053F290(&motion)) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.field_38 = a8;
    }
    rob_chara_set_hand_r_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550AB0(rob_chara* rob_chr, const motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.hand_r))
        sub_140553E30(rob_chr, -1, 0.0f, -1, 12.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_mouth_motion(rob_chara* rob_chr,
    RobMouthMotion* motion, int32_t type, const motion_database* mot_db);

static void sub_140554C40(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db) {
    RobMouthMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.state = state;
        motion.data.frame = frame;
        motion.data.play_frame_step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.field_24 = a6;
        if (sub_14053F290(&motion)) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.field_38 = a8;
    }
    rob_chara_set_mouth_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550B20(rob_chara* rob_chr, const motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.mouth))
        sub_140554C40(rob_chr, -1, 0.0f, -1, 6.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_eyes_motion(rob_chara* rob_chr,
    RobEyesMotion* motion, int32_t type, const motion_database* mot_db);

static void rob_chara_set_eyes_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db) {
    RobEyesMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.state = state;
        motion.data.frame = frame;
        motion.data.play_frame_step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.field_24 = a6;
        if (sub_14053F290(&motion)) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.field_38 = a8;
    }
    rob_chara_set_eyes_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550960(rob_chara* rob_chr, const motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.eyes))
        rob_chara_set_eyes_motion_id(rob_chr, -1, 0.0f, -1, 6.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void rob_chara_set_eyelid_motion(rob_chara* rob_chr,
    RobEyelidMotion* motion, int32_t type, const motion_database* mot_db);

static void rob_chara_set_eyelid_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db) {
    RobEyelidMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.state = state;
        motion.data.frame = frame;
        motion.data.play_frame_step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.field_24 = a6;
        if (sub_14053F290(&motion)) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.field_38 = a8;
    }
    rob_chara_set_eyelid_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_1405508F0(rob_chara* rob_chr, const motion_database* mot_db) {
    if (sub_14053F1B0(&rob_chr->data.motion.field_150.eyelid))
        rob_chara_set_eyelid_motion_id(rob_chr, -1, 0.0f, -1, 6.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static bool sub_1404190E0(rob_chara_bone_data* rob_bone_data) {
    return rob_bone_data->eyelid.blend.enable || rob_bone_data->eyelid.blend.rot_y;
}

static void sub_140555F70(rob_chara* rob_chr, const motion_database* mot_db) {
    if (sub_1404190E0(rob_chr->bone_data))
        return;

    if (rob_chr->data.motion.field_29 & 0x80)
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
    if (!(rob_chr->data.motion.field_2A & 0x02) && rob_chr->data.motion.field_29 & 0x01) {
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
    if (a1->frame_data.playback_state != MOT_PLAY_FRAME_DATA_PLAYBACK_EXTERNAL)
        return;

    if (a1->ext_frame)
        a1->frame_data.frame = *a1->ext_frame;
    if (a1->ext_step)
        a1->frame_data.step = *a1->ext_step;
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
        && !(rob_chr->data.field_1588.field_0.field_20.field_8 & 0x10000000))
        return;

    vec3 v9 = 0.0f;
    sub_140419800(rob_chr->bone_data, &v9);
    float_t v3 = sub_1405503B0(rob_chr) * rob_chr->data.motion.step_data.frame;
    float_t v4 = sub_1405501F0(rob_chr) * rob_chr->data.motion.step_data.frame;
    v9.x -= v3 * rob_chr->data.field_1588.field_0.field_318.x;
    v9.y -= v4 * rob_chr->data.field_1588.field_0.field_318.y;
    v9.z -= v3 * rob_chr->data.field_1588.field_0.field_318.z;

    mat4 mat;
    mat4_rotate_y((float_t)((double_t)rob_chr->data.miku_rot.rot_y_int16 * M_PI * (1.0 / 32768.0)), &mat);
    mat4_mult_vec3(&mat, &v9, &v9);

    if (!(rob_chr->data.field_1588.field_0.field_20.field_0 & 0x08)) {
        rob_chr->data.field_1588.field_0.field_20.field_0 |= 0x08;
        rob_chr->data.miku_rot.position.x = rob_chr->data.miku_rot.field_24.x - v9.x;
        rob_chr->data.miku_rot.position.z = rob_chr->data.miku_rot.field_24.z - v9.z;
    }

    if (rob_chr->data.field_1588.field_0.field_20.field_8 & 0x10000000) {
        rob_chr->data.field_1588.field_0.field_20.field_8 &= ~0x10000000;
        rob_chr->data.miku_rot.position.y = rob_chr->data.miku_rot.field_24.y - v9.y;
    }
}

static void sub_140504710(rob_chara* rob_chr, const motion_database* mot_db,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
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

    RobFaceMotion* v2 = &rob_chr->data.motion.field_150.face;
    if ((rob_chr->data.motion.field_29 & 0x04))
        v2 = &rob_chr->data.motion.field_3B0.face;
    sub_14053F2C0(v2);
    rob_chara_bone_data_set_face_frame(rob_chr->bone_data, v2->data.frame);
    rob_chara_bone_data_set_face_play_frame_step(rob_chr->bone_data, v2->data.play_frame_step);

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

    RobEyesMotion* v6 = &rob_chr->data.motion.field_150.eyes;
    if (rob_chr->data.motion.field_29 & 0x40)
        v6 = &rob_chr->data.motion.field_3B0.eyes;
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
        rob_chr->load_outfit_object_info(ITEM_ATAMA,
            v8, false, bone_data, data, obj_db);
    object_info v9 = rob_chara_get_head_object(rob_chr, 1);
    object_info v10 = rob_chara_get_head_object(rob_chr, 7);

    bool v11 = v8 == v9 || v8 == v10;
    rob_chr->bone_data->eyes.field_30 = v11;
    rob_chr->bone_data->field_788.field_91 = v11;

    object_info v12 = sub_140550350(rob_chr);
    if (rob_chara_get_object_info(rob_chr, ITEM_TE_L) != v12)
        rob_chr->load_outfit_object_info(ITEM_TE_L,
            v12, true, bone_data, data, obj_db);

    object_info v13 = sub_140550380(rob_chr);
    if (rob_chara_get_object_info(rob_chr, ITEM_TE_R) != v13)
        rob_chr->load_outfit_object_info(ITEM_TE_R,
            v13, true, bone_data, data, obj_db);

    object_info v14 = sub_140550310(rob_chr);
    if (rob_chara_get_object_info(rob_chr, ITEM_HARA) != v14)
        rob_chr->load_outfit_object_info(ITEM_HARA,
            v14, false, bone_data, data, obj_db);

    rob_chr->bone_data->interpolate();
    rob_chr->bone_data->update(0);
    sub_140504F00(rob_chr);

    rob_chr->data.motion.field_28 &= ~0x80;
}

static void sub_140504AC0(rob_chara* rob_chr) {
    vec3 v4 = rob_chr->data.miku_rot.field_30;
    vec3 v20 = 0.0f;
    if (rob_chr->data.field_0 & 0x02) {
        v20 = v4 + rob_chr->data.miku_rot.field_3C + rob_chr->data.field_8.field_B8.field_10;
        v4.y = rob_chr->data.field_8.field_B8.field_10.y
            - get_osage_gravity_const() * rob_chr->data.field_1588.field_0.field_200;
    }
    else if (!(rob_chr->data.field_1 & 0x80)) {
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
            v20 = 0.0f;
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
    rob_chr->data.miku_rot.field_3C = 0.0f;
    rob_chr->data.miku_rot.position = v20 + rob_chr->data.miku_rot.position;
}

static void rob_base_rob_chara_ctrl_thread_main(rob_chara* rob_chr) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    sub_140514520(rob_chr, aft_bone_data, aft_mot_db);
    sub_14054CC80(rob_chr);
    sub_1405077D0(rob_chr, aft_bone_data, aft_mot_db);
    rob_chr->sub_1405070E0(aft_bone_data, aft_mot_db);
    rob_chr->arm_adjust_ctrl();
    sub_140504710(rob_chr, aft_mot_db, aft_bone_data, aft_data, aft_obj_db);
    sub_140504AC0(rob_chr);
}

static void rob_base_rob_chara_free(rob_chara* rob_chr) {
    //sub_14054F370(&rob_chr->rob_touch);
    rob_chara_bone_data_motion_blend_mot_free(rob_chr->bone_data);
}

static void rob_disp_rob_chara_init(rob_chara* rob_chr,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    const chara_init_data* chr_init_data = rob_chr->chara_init_data;
    bone_node* v3 = rob_chara_bone_data_get_node(rob_chr->bone_data, MOT_BONE_N_HARA_CP);
    rob_itm_equip->reset_init_data(v3);
    rob_itm_equip->set_item_equip_range(rob_chr->cos_id == 501);
    rob_itm_equip->load_object_info({}, ITEM_BODY, false, bone_data, data, obj_db);
    for (int32_t i = ITEM_BODY; i < ITEM_ASI; i++)
        rob_itm_equip->load_outfit_object_info((item_id)i,
            chr_init_data->field_7E4[i], false, aft_bone_data, aft_data, aft_obj_db);
    rob_itm_equip->set_shadow_type(rob_chr->chara_id);
    rob_chr->item_equip->field_A0 = 0x05;
    rob_chr->item_cos_data.reload_items(rob_chr->chara_id, bone_data, data, obj_db);
    if (rob_chr->item_cos_data.check_for_npr_flag())
        rob_chr->item_equip->npr_flag = true;

    if (rob_chr->check_for_ageageagain_module()) {
        rob_chara_age_age_array_load(rob_chr->chara_id, 1);
        rob_chara_age_age_array_load(rob_chr->chara_id, 2);
    }
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

static bool sub_140413810(motion_blend_mot* a1) {
    if (a1->blend && a1->blend->Field_30())
        return a1->blend->rot_y;
    return false;
}

static void rob_disp_rob_chara_ctrl_thread_main(rob_chara* rob_chr) {
    if (rob_chr->bone_data->get_frame() < 0.0f)
        rob_chr->bone_data->set_frame(0.0f);

    float_t frame_count = rob_chr->bone_data->get_frame_count();
    if (rob_chr->bone_data->get_frame() > frame_count)
        rob_chr->bone_data->set_frame(frame_count);

    rob_chr->item_equip->set_opd_blend_data(&rob_chr->bone_data->motion_loaded);

    vec3 trans = 0.0f;
    rob_chr->get_trans_scale(0, trans);
    rob_chr->item_equip->position = trans;
    rob_chara_item_equip_ctrl(rob_chr->item_equip);
    if (rob_chr->check_for_ageageagain_module()) {
        rob_chara_age_age_array_set_step(rob_chr->chara_id, 1, rob_chr->item_equip->osage_step);
        rob_chara_age_age_ctrl(rob_chr, 1, "j_tail_l_006_wj");
        rob_chara_age_age_array_set_step(rob_chr->chara_id, 2, rob_chr->item_equip->osage_step);
        rob_chara_age_age_ctrl(rob_chr, 2, "j_tail_r_006_wj");
    }
}

static void rob_disp_rob_chara_disp(rob_chara* rob_chr) {
    rob_chr->item_equip->texture_color_coefficients = 1.0f;
    rob_chr->item_equip->mat = rob_chr->data.adjust_data.mat;
    rob_chr->item_equip->disp(rob_chr->chara_id, rctx_ptr);
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

static void sub_140409B70(struc_258* a1, vec3* a2, mat4* a3, bool a4) {
    if (a2) {
        a1->field_190 = true;
        a1->field_191 = a4;
        a1->field_192 = a4;
        a1->field_193 = a4;
        a1->field_194 = a4;
        a1->field_195 = a4;

        if (a4 && a1->field_A0 > 0.0f && a1->field_A0 > a1->field_AC
            && fabsf(a1->field_A0 - a1->field_AC) > 0.000001f) {
            a1->field_AC += a1->field_A8;
            a1->field_198 = (a1->field_AC + 1.0f) / (a1->field_A0 + 1.0f);
        }
        else {
            a1->field_191 = false;
            a1->field_198 = 1.0f;
        }

        mat4 mat;
        mat4_inverse(a3, &mat);

        vec3 v24;
        mat4_mult_vec3_trans(&mat, a2, &v24);

        float_t v12 = v24.x - 0.05f;
        if (a1->field_184.x >= v12) {
            v12 = v24.x + 0.05f;
            if (a1->field_184.x <= v12)
                v12 = a1->field_184.x;
        }
        a1->field_184.x = v12;

        float_t v15 = v24.y - 0.05f;
        if (a1->field_184.y >= v15) {
            v15 = v24.y + 0.05f;
            if (a1->field_184.y <= v15)
                v15 = a1->field_184.y;
        }
        a1->field_184.y = v15;

        float_t v18 = v24.z - 0.05f;
        if (a1->field_184.z >= v18) {
            v18 = v24.z + 0.05f;
            if (a1->field_184.z <= v18)
                v18 = a1->field_184.z;
        }
        a1->field_184.z = v18;
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

static void sub_14041C620(rob_chara_bone_data* rob_bone_data, vec3* a2, mat4* a3, bool a4) {
    sub_140409B70(&rob_bone_data->field_788, a2, a3, a4);
}

static float_t sub_14040ADE0(float_t a1, float_t a2) {
    float_t v1 = a1 - a2;
    if (v1 > (float_t)M_PI)
        return v1 - (float_t)(M_PI * 2.0);
    else if (v1 < (float_t)-M_PI)
        return v1 + (float_t)(M_PI * 2.0);
    return v1;
}

static void sub_140407280(struc_258* a1, std::vector<bone_data>* a2, mat4* mat, float_t step) {
    bone_data* v7 = a2->data();

    vec3 v69;
    mat4_get_translation(v7[MOTION_BONE_CL_KAO].node[2].mat, &v69);
    if (!a1->field_190 && !a1->field_191)
        return;

    float_t v14 = step * 0.25f;
    float_t v15 = a1->field_15C.field_8;
    float_t v18 = -v15;
    float_t v16 = a1->field_15C.field_C;
    mat4* v19 = v7[MOTION_BONE_C_KATA_R].node->mat;
    mat4* v20 = v7[MOTION_BONE_C_KATA_L].node->mat;
    float_t v21 = v19->row3.x - v20->row3.x;
    float_t v22 = v19->row3.z - v20->row3.z;
    mat4* v73 = v7[MOTION_BONE_N_KAO].node->mat;
    float_t v23 = atan2f(v73->row0.x, v73->row0.z);
    float_t v24 = atan2f(v21, v22);
    float_t v25 = sub_14040ADE0(v24, v23);
    if (v25 < 0.0f)
        v15 = v25 + v15;
    else if (v25 > 0.0f)
        v18 = v25 - v15;

    vec3 v67;
    mat4_mult_vec3_inv_trans(v73, &a1->field_184, &v67);

    if (vec3::length(v67) < 0.000001f)
        return;

    vec3 v68;
    mat4_mult_vec3_inv_trans(v73, &v69, &v68);

    float_t v28 = vec3::length(v68);
    if (v28 < 0.000001f)
        return;

    v67 = vec3::normalize(v67);
    v68 = v67 * v28;
    float_t v39 = vec2::length(*(vec2*)&v67);

    float_t v38;
    if (a1->field_1C8 == 3)
        v38 = (float_t)(75.0 * DEG_TO_RAD) - (float_t)(30.0 * DEG_TO_RAD);
    else
        v38 = (float_t)(75.0 * DEG_TO_RAD);

    float_t v31 = vec3::length(v67);

    bool v36;
    float_t v40;
    if (v39 >= v31 * cosf(v38)) {
        v40 = atan2f(-v67.x, v67.y);
        v36 = false;
    }
    else {
        v40 = a1->field_19C;
        v36 = true;
    }

    float_t v42 = v40;
    if (a1->field_190) {
        if (a1->field_1C8 == 3)
            v16 -= (float_t)(100.0 * DEG_TO_RAD);

        if (v36 || v40 > v16 || v40 < -v16) {
            v40 = 0.0f;
            v14 *= 0.5f;
            a1->field_1C8 = 3;
        }
        else if (v40 > v15) {
            v40 = v15;
            a1->field_1C8 = 2;
            v14 *= 0.5f;
        }
        else if (v40 < v18) {
            v40 = v18;
            v14 *= 0.5f;
            a1->field_1C8 = 1;
        }
        else
            return;
    }
    else
        a1->field_1C8 = 0;

    int32_t v43 = a1->field_1C8;
    float_t v44 = a1->field_94;
    if (v43 != 3 && a1->field_14C) {
        if (!v43)
            v14 *= 0.5f;

        float_t v45 = a1->field_154;
        if (fabsf(a1->field_154) <= 0.000001f) {
            if (v40 >= 0.0f) {
                v40 = v40 + -(float_t)(50.0 * DEG_TO_RAD);
                if (v40 < v18)
                    v40 = v18;
            }
            else {
                v40 = v40 + (float_t)(50.0 * DEG_TO_RAD);
                if (v40 > v15)
                    v40 = v15;
            }
        }
        else if (v45 >= 0.0f) {
            float_t v46 = v40 + (float_t)(50.0 * DEG_TO_RAD);
            if (v46 > v15)
                v46 = v15;

            float_t v47 = v40 - (float_t)(50.0 * DEG_TO_RAD);
            if (v47 < v18)
                v47 = v18;

            if (v40 + (float_t)(50.0 * DEG_TO_RAD) < v45) {
                if (v42 < (float_t)(-700.0 * DEG_TO_RAD)) {
                    a1->field_1C8 = 3;
                    v46 = 0.0f;
                }
                else
                    v46 = a1->field_154;

                v40 = v46;
            }
            else if (v46 - (float_t)(30.0 * DEG_TO_RAD) >= v40)
                v40 = v46;
            else
                v40 = v47;
        }
        else {
            float_t v46 = v40 - (float_t)(50.0 * DEG_TO_RAD);
            if (v40 - (float_t)(50.0 * DEG_TO_RAD) < v18)
                v46 = v18;

            float_t v47 = v40 + (float_t)(50.0 * DEG_TO_RAD);
            if (v40 + (float_t)(50.0 * DEG_TO_RAD) > v15)
                v47 = v15;

            if (v40 - (float_t)(50.0 * DEG_TO_RAD) > v45) {
                if (v42 > (float_t)(70.0 * DEG_TO_RAD)) {
                    a1->field_1C8 = 3;
                    v46 = 0.0f;
                }
                else
                    v46 = a1->field_154;
                v40 = v46;
            }
            else if (v46 + (float_t)(30.0 * DEG_TO_RAD) <= v40)
                v40 = v46;
            else
                v40 = v47;
        }

        v44 = a1->field_150;
        a1->field_154 = v40;
    }
    else
        a1->field_154 = 0.0f;

    float_t v48 = v40 * v44;
    float_t v49 = v48;
    if (a1->field_191) {
        a1->field_19C = v48;
        if (!a1->field_190)
            v48 = a1->field_1AC;
        else
            a1->field_1AC = v48;
    }
    else {
        if (a1->field_192)
            v48 = sub_14040ADE0(v48, a1->field_19C) * v14 + a1->field_19C;
        a1->field_19C = v48;
        a1->field_1AC = v48;
    }

    float_t v51 = (v48 - atan2f(-v68.x, v68.y)) * a1->field_198;
    vec3 v71 = *(vec3*)&v73->row2;
    v71.y += 1.0f;
    v71 *= 0.5f;

    mat4 v72 = *v7[MOTION_BONE_CL_KAO].node[1].mat;
    mat4_mult_axis_angle(&v72, &v71, v51, &v72);
    mat4_mult_vec3_inv_trans(&v72, &a1->field_184, &v67);

    float_t v52;
    if (v36)
        v52 = a1->field_1A0;
    else
        v52 = vec2::length(*(vec2*)&v67);

    if (a1->field_190) {
        float_t v55 = fabsf(v49);
        float_t v56;
        if (v52 >= 0.0f)
            v56 = v55 * a1->field_15C.field_24;
        else
            v56 = v55 * a1->field_15C.field_20;
        v52 = v52 * (float_t)((float_t)(a1->field_15C.field_8 - v56) / a1->field_15C.field_8);
        float_t v57 = a1->field_15C.field_0;
        if (v52 < v57 || (v57 = a1->field_15C.field_4, v52 > v57))
            v52 = v57;
    }

    if (a1->field_1C8 == 3 || a1->field_14C)
        v52 = 0.0f;

    float_t v58 = v52 * a1->field_94;
    a1->field_158 = 0.0f;

    if (!a1->field_191) {
        if (a1->field_193)
            v58 = sub_14040ADE0(v58, a1->field_1A0) * v14 + a1->field_1A0;
        a1->field_1A0 = v58;
        a1->field_1B0 = v58;
    }
    else {
        a1->field_1A0 = v58;
        if (a1->field_190)
            a1->field_1B0 = v58;
        else
            v58 = a1->field_1B0;

    }

    mat4_rotate_y_mult(&v72, v58 * a1->field_198, &v72);

    mat4 v60 = v7[MOTION_BONE_CL_KAO].rot_mat[1];

    mat4 v74;
    mat4_inverse(v7[MOTION_BONE_CL_KAO].node->mat, &v74);
    mat4_clear_trans(&v74, &v74);
    mat4_mult(&v7[MOTION_BONE_CL_KAO].rot_mat[1], &v74, &v72);

    for (int32_t i = MOTION_BONE_CL_KAO; i <= MOTION_BONE_N_KUBI_WJ_EX; i++)
        bone_data_mult_1(&v7[i], mat, v7, true);

    v7[MOTION_BONE_CL_KAO].rot_mat[1] = v60;
}

static void sub_1404189A0(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    sub_140407280(&rob_bone_data->field_788, &v2->bone_data.bones,
        &v2->field_4F8.mat, v2->mot_play_data.frame_data.step);
}

static void sub_140406FC0(struc_258* a1, bone_data* a2, mat4* a3,
    vec3* a4, vec3* a5, vec3* a6, bool a7, float_t a8, float_t a9) {
    float_t v14;
    float_t v17;

    a4->z = fabsf(a4->z);

    float_t v15 = sqrt(a4->x * a4->x + a4->z * a4->z);
    if (fabsf(v15) <= 0.000001f) {
        v14 = 0.0f;
        if (a4->y <= 0.0f)
            v17 = a6->x;
        else
            v17 = a5->x;
    }
    else {
        v14 = atan2f(a4->x, a4->z);
        v17 = atanf(-a4->y / v15);

        if (v17 > 0.0f)
            v17 *= a1->field_1BC.y;
        else if (v17 < 0.0f)
            v17 *= a1->field_1BC.x;

        if (v17 < a5->x)
            v17 = a5->x;
        else if (v17 > a6->x)
            v17 = a6->x;

        if (v14 < a5->y)
            v14 = a5->y;
        else if (v14 > a6->y)
            v14 = a6->y;
    }

    if (!a1->field_14C || fabsf(a1->field_150) <= 0.000001f) {
        if (fabsf(a1->field_158) > 0.000001f) {
            if (a1->field_158 < 0.0f)
                v17 = a5->x;
            else
                v17 = a6->x;
        }
        else {
            if (v17 < 0.0f)
                v17 = a6->x;
            else
                v17 = a5->x;
        }

        if (fabsf(a1->field_154) > 0.000001f) {
            if (a1->field_154 < 0.0f)
                v14 = a5->y;
            else
                v14 = a6->y;
        }
        else {
            if (v14 < 0.0f)
                v14 = a6->y;
            else
                v14 = a5->y;
        }
    }

    mat4 mat = mat4_identity;
    mat4_rotate_y_mult(&mat, v14, &mat);
    mat4_rotate_x_mult(&mat, v17, &mat);

    mat4 v25;
    mat4_lerp_rotation(&v25, &mat, &mat, a1->field_98);

    if (a7)
        mat4_lerp_rotation(&a2->rot_mat[0], &mat, &mat, a8);
    else
        mat4_lerp_rotation(a3, &mat, &mat, a9);

    *a3 = mat;
    a2->rot_mat[0] = mat;
}

static void sub_140409170(struc_258* a1, mat4* a2, std::vector<bone_data>* a3, mat4* a4, float_t step) {
    if (a1->field_91)
        return;

    a1->field_0 = a3;
    a1->field_8 = *a4;

    bool a7 = false;
    float_t v11 = 0.0f;
    if (a1->field_A0 > 0.0f && a1->field_A0 > a1->field_A4 && fabsf(a1->field_A0 - a1->field_A4) > 0.000001f) {
        a7 = true;
        v11 = (a1->field_A4 + 1.0f) / (a1->field_A0 + 1.0f);
        if (!a1->field_90)
            v11 = 1.0f - v11;
        a1->field_A4 += step;
    }

    float_t v15 = a1->field_9C * step;
    if (fabsf(a1->field_9C - 1.0f) <= 0.000001f || v15 > 1.0f || a1->field_8F && !a7
        || rctx_ptr->camera->fast_change_hist0 && rctx_ptr->camera->fast_change_hist1)
        v15 = 1.0f;

    a1->field_8D = false;
    a1->field_8F = false;
    if (!(a1->field_90 || a1->field_14C && fabsf(a1->field_150) > 0.000001f || a7)
        || !(a1->field_94 <= 0.0f || a1->field_1C8 != 3))
        return;

    a1->field_C0 = a1->field_B4;

    mat4 mat;
    mat4_inverse(a2, &mat);
    mat4_mult_vec3_trans(&mat, &a1->field_C0, &a1->field_C0);

    float_t v29 = a1->field_48.field_8;
    float_t v31 = a1->field_48.field_C;
    float_t v32 = -a1->field_48.field_8;
    if (a1->field_1C4 != 0) {
        v29 = a1->field_48.field_18;
        v31 = a1->field_48.field_1C;
        v32 = -a1->field_48.field_18;
    }

    std::vector<bone_data>* v19 = a1->field_0;
    if (a1->field_B0 > 0.0f) {
        mat4* mat = v19->data()[MOTION_BONE_KL_EYE_L].parent_mat;

        vec3 v52;
        vec3 v54;
        mat4_get_translation(v19->data()[MOTION_BONE_KL_EYE_L].parent_mat, &v52);
        mat4_get_translation(v19->data()[MOTION_BONE_KL_EYE_R].parent_mat, &v54);

        v54 = (v52 + v54) * 0.5f;
        mat4_set_translation(mat, &v54);

        vec3 v53;
        mat4_mult_vec3_inv_trans(mat, &a1->field_C0, &v53);

        float_t v37 = vec3::length(v53);
        if (fabsf(v37) <= 0.000001f || v53.x * v53.x + v53.z * v53.z <= 0.01f) {
            v29 = 0.0f;
            v32 = 0.0f;
        }
        else if (v37 < a1->field_B0) {
            v53.z = fabsf(v53.z);

            v53 *= a1->field_B0 / v37;
            mat4_mult_vec3_trans(mat, &v53, &v53);

            vec3 v56;
            mat4_mult_vec3_inv_trans(v19->data()[MOTION_BONE_KL_EYE_L].parent_mat, &v53, &v56);
            float_t v40 = atan2f(v56.x - a1->field_48.field_20.x, v56.z);
            if (v29 < v40)
                if (v31 > v40)
                    v29 = v40;
                else
                    v29 = v31;

            vec3 v57;
            mat4_mult_vec3_inv_trans(v19->data()[MOTION_BONE_KL_EYE_R].parent_mat, &v53, &v57);
            float_t v41 = atan2f(v57.x + a1->field_48.field_20.x, v57.z);
            if (v32 > v41)
                if (-v31 < v41)
                    v32 = v41;
                else
                    v32 = -v31;
        }
    }

    float_t v42;
    float_t v43;
    if (a1->field_1C4) {
        v42 = a1->field_48.field_10 * -3.8f / a1->field_48.field_34 * a1->field_1BC.x;
        v43 = a1->field_48.field_14 * 6.0f / a1->field_48.field_38 * a1->field_1BC.y;
    }
    else {
        v42 = a1->field_1BC.x * a1->field_48.field_0;
        v43 = a1->field_1BC.y * a1->field_48.field_4;
    }

    vec3 v58;
    mat4_mult_vec3_inv_trans(v19->data()[MOTION_BONE_KL_EYE_L].parent_mat, &a1->field_C0, &v58);
    v58 -= a1->field_48.field_20;

    vec3 v56;
    v56.x = v43;
    v56.y = v31;
    v56.z = 0.0f;

    vec3 v55;
    v55.x = v42;
    v55.y = v29;
    v55.z = 0.0f;

    sub_140406FC0(a1, &v19->data()[MOTION_BONE_KL_EYE_L], &a1->field_CC, &v58, &v55, &v56, a7, v11, v15);

    v55.y = -v31;
    v56.y = v32;

    vec3 v59;
    mat4_mult_vec3_inv_trans(v19->data()[MOTION_BONE_KL_EYE_R].parent_mat, &a1->field_C0, &v59);
    vec3 v60 = a1->field_48.field_20;
    v60.x = -v60.x;
    v59 -= v60;

    sub_140406FC0(a1, &v19->data()[MOTION_BONE_KL_EYE_R], &a1->field_10C, &v59, &v55, &v56, a7, v11, v15);

    bone_data* v47 = a3->data();
    for (int32_t i = MOTION_BONE_KL_EYE_L; i <= MOTION_BONE_KL_HIGHLIGHT_L_WJ; i++)
        bone_data_mult_1(&v47[i], a4, v47, true);

    for (int32_t i = MOTION_BONE_KL_EYE_R; i <= MOTION_BONE_KL_HIGHLIGHT_R_WJ; i++)
        bone_data_mult_1(&v47[i], a4, v47, true);
}

static void sub_14041A160(rob_chara_bone_data* rob_bone_data, mat4* a2) {
    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    sub_140409170(&rob_bone_data->field_788, a2, &v2->bone_data.bones,
        &v2->field_4F8.mat, v2->mot_play_data.frame_data.step);
}

static void sub_14053BEE0(rob_chara* rob_chr) {
    if (rob_chr->type != ROB_CHARA_TYPE_2)
        return;

    bool v4 = !(rob_chr->data.field_2 & 0x80);
    rob_chr->data.field_2 &= ~0x80;

    rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;
    vec3* v3 = 0;
    if (sub_14041A010(rob_bone_data) || rob_bone_data->field_788.field_14C) {
        if (rob_bone_data->field_788.field_8C)
            rctx_ptr->camera->get_view_point(rob_bone_data->field_788.field_B4);

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
        adjust->scale = chara_size_table_get_value(1);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_SHORT:
        adjust->scale = chara_size_table_get_value(2);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_TALL:
        adjust->scale = chara_size_table_get_value(0);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_MIN:
        adjust->scale = min_def(opposite_chara_scale, chara_scale);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_MAX:
        adjust->scale = max_def(opposite_chara_scale, chara_scale);
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
    case ROB_CHARA_DATA_HAND_ADJUST_ITEM: // X
        adjust->scale = rob_chr->data.adjust_data.item_scale;
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
        adjust->current_scale = lerp_def(prev_scale, scale, adjust->scale_blend);
        if (fabsf(adjust->current_scale - chara_scale) <= 0.000001f
            && adjust->type != ROB_CHARA_DATA_HAND_ADJUST_ITEM) {
            adjust->current_scale = chara_scale;
            adjust->enable = false;
        }
    }
    else {
        float_t t = (adjust->current_time + 1.0f) / (adjust->duration + 1.0f);
        adjust->current_scale = lerp_def(prev_scale, scale, t * adjust->scale_blend);
        adjust->current_time += rob_chr->data.motion.step_data.frame;
    }
}

static void rob_chara_set_hands_adjust(rob_chara* rob_chr) {
    for (int32_t i = 0; i < 2; i++)
        rob_chara_set_hand_adjust(rob_chr, &rob_chr->data.motion.hand_adjust[i],
            &rob_chr->data.motion.hand_adjust_prev[i]);
}

static void sub_14040AE10(mat4* mat, const vec3 a2) {
    vec3 v8;
    mat4_mult_vec3_inv_trans(mat, &a2, &v8);
    float_t v5 = vec2::length(*(vec2*)&v8);
    float_t v6 = vec3::length(v8);
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
        sub_14040AE10(&v38, v14->ik_target);
        mat4_mult_vec3_inv_trans(&v38, &v36, &v36);

        v38 = *v14->node->mat;
        sub_14040AE10(&v38, *a4);

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
        if (rotation_blend < 1.0f)
            mat4_lerp_rotation(&v25->rot_mat[0], &v38, &v38, rotation_blend);
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

    rob_chara_adjust_data adjust_data = rob_chr->data.adjust_data;
    float_t scale = adjust_scale / chara_scale;
    adjust_data.scale = scale;
    adjust_data.item_scale = scale; // X
    rob_chr->set_data_adjust_mat(&adjust_data, false);

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

    mat4* v40 = rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, rob_kl_te_bones[hand]);
    mat4 v42;
    mat4_mult(v40, &adjust_data.mat, &v42);

    vec3 v37 = rob_chr->data.motion.hand_adjust[hand].offset * (1.0f - chara_scale / adjust_scale);

    vec3 v38;
    mat4_mult_vec3_trans(&v42, &v37, &v38);

    vec3 v27 = 0.0f;
    if (rob_chr->data.motion.hand_adjust[hand].disable_x)
        v27.x = v40->row3.x - v38.x;
    if (rob_chr->data.motion.hand_adjust[hand].disable_y)
        v27.y = v40->row3.y - v38.y;
    if (rob_chr->data.motion.hand_adjust[hand].disable_z)
        v27.z = v40->row3.z - v38.z;

    vec3 v39 = v38 + v27;

    vec3 v18 = 0.0f;
    if (v15 > 0) {
        mat4_mult_vec3_trans(&v42, &rob_chr->data.motion.hand_adjust[hand].offset, &v38);
        v18 = v38 + v27;
    }

    sub_140418A00(rob_chr->bone_data, &v39, off_140C9E020[hand], rotation_blend,
        rob_chr->data.motion.hand_adjust[hand].arm_length, solve_ik);

    while (v15 > 0) {
        mat4* v40 = rob_chara_bone_data_get_mats_mat(rob_chr->bone_data, rob_kl_te_bones[hand]);
        mat4_set_translation(v40, &v18);

        vec3 v37 = -rob_chr->data.motion.hand_adjust[hand].offset;
        mat4_mult_vec3_trans(v40, &v37, &v39);
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
    switch (v8) {
    case 0:
        if (v9 < v0.frame)
            return false;
        break;
    case 1:
        if (v9 < v0.frame || v9 >= v0.field_8)
            return false;
        break;
    default:
        return false;
    }

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
        v45 = v0.field_10 + v13;
        break;
    case 10: {
        float_t ratio0 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio0;
        v45 = v13 * ratio0;
    } break;
    case 11: {
        float_t ratio1 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio1;
        v45 = v13 * ratio1;
    } break;
    case 12: {
        float_t ratio3 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))->ik_scale.ratio3;
        v45 = v13 * ratio3;
    } break;
    case 13: {
        float_t ratio0 = rob_chr->bone_data->ik_scale.ratio0;
        v45 = v13 * (1.0f / ratio0);
    } break;
    case 14: {
        float_t ratio1 = rob_chr->bone_data->ik_scale.ratio1;
        v45 = v13 * (1.0f / ratio1);
    } break;
    case 15: {
        float_t ratio3 = rob_chr->bone_data->ik_scale.ratio3;
        v45 = v13 * (1.0f / ratio3);
    } break;
    default:
        if (v0.field_C < 16) {
            mat4* v26 = rob_chara_bone_data_get_mats_mat(
                (rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28), dword_140A2DDD0[v0.field_C]);
            mat4_get_translation(v26, &v45);
            v45 += v0.field_10;
        }
        else {
            mat4* v23 = rob_chara_bone_data_get_mats_mat(
                (rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28), dword_140A2DDD0[v0.field_C]);

            mat4 v49;
            mat4_translate_mult(v23, &v0.field_10, &v49);
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
        v31 = clamp_def(v31, 0.0f, 1.0f);
    }

    int32_t v33 = v0.field_0;
    vec3 v48 = 0.0f;
    switch (v0.field_0) {
    case 0:
        v48 = vec3::lerp(v45, v13, v31);
        break;
    case 1:
        v48 = vec3::lerp(v13, v45, v31);
        break;
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

    vec3 v36 = v48 - v13;
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
    vec3 v4 = -*a2;
    float_t v7 = vec3::dot(*a1, v4);
    float_t v8 = vec3::dot(v4, v4);
    v8 -= a3 * a3;
    if (v8 > 0.0f && v7 > 0.0f)
        return false;

    float_t v10 = v7 * v7 - v8;
    if (v10 < 0.0f)
        return false;

    *a4 = *a1 * (sqrtf(v10) - v7);
    return true;
}

static void sub_1403FA040(vec3* a1, float_t a2, mat4* a3, float_t ymin,
    float_t ymax, float_t zmin, float_t zmax, vec3* a8, float_t step) {
    vec3 v50;
    vec3 v51;
    vec3 v52;
    vec3 v53;
    mat4_mult_vec3_inv(a3, a1, &v50);

    v50.x = 0.0f;

    float_t v23 = 0.0f;
    float_t v40 = 0.0f;
    if (vec3::length_squared(v50) <= 0.000001f)
        goto LABEL_66;

    v50 = vec3::normalize(v50);
    if (zmax - zmin >= ymax - ymin) {
        v23 = (ymax - ymin) * 0.5f;
        v52.x = 0.0f;
        v52.y = ymax - v23;
        v52.z = zmax - v23;
        v53.x = 0.0f;
        v53.y = ymax - v23;
        v53.z = zmin + v23;
        if (fabsf(v50.y) <= 0.000001f) {
            if (fabsf(ymin) <= 0.000001f || fabsf(ymax) <= 0.000001f) {
                v50 *= a2;
                if (v50.z < zmin + v23)
                    v50.z = zmin + v23;
                else if (v50.z > zmax - v23)
                    v50.z = zmax - v23;
                else if (vec3::dot(*a8, v50) != 0.0f) {
                    float_t v43 = vec3::length(*a8);
                    float_t v44 = v43 + (v43 * 0.97f - v43) * step;
                    if (vec3::length(v50) < v44)
                        v50 = vec3::normalize(v50) * v44;
                }
                goto LABEL_66;
            }
        }
        else if ((fabsf(ymin) > 0.000001f || v50.y >= 0.0f)
            && (fabsf(ymax) > 0.000001f || v50.y <= 0.0f)) {
            float_t v21 = ymax;
            if (v50.y * ymax <= 0.0f)
                v21 = ymin;
            float_t v31 = (v21 * v50.z) / v50.y;
            if (v31 >= zmin + v23 && v31 <= zmax - v23) {
                v40 = sqrtf(v31 * v31 + v21 * v21);
                goto LABEL_55;
            }
        }
        else {
            v50.y = 0.0f;
            if (fabsf(v50.z) > 0.000001f) {
                v50 = vec3::normalize(v50) * a2;
                if (v50.z < zmin)
                    v50.z = zmin;
                else if (v50.z > zmax)
                    v50.z = zmax;
            }
            goto LABEL_66;
        }
    }
    else {
        v23 = (zmax - zmin) * 0.5f;
        v52.x = 0.0f;
        v52.y = 0.0f;
        v52.z = zmax - v23;
        v53.x = 0.0f;
        v53.y = ymax - v23;
        v53.z = zmax - v23;
        if (fabsf(v50.z) <= 0.000001f) {
            if (fabsf(zmin) <= 0.000001f || fabsf(zmax) <= 0.000001f) {
                v50 *= a2;
                if (v50.y < ymin + v23)
                    v50.y = ymin + v23;
                else if (v50.y > ymax - v23)
                    v50.y = ymax - v23;
                else if (vec3::dot(*a8, v50) != 0.0f) {
                    float_t v43 = vec3::length(*a8);
                    float_t v44 = v43 + (v43 * 0.97f - v43) * step;
                    if (vec3::length(v50) < v44)
                        v50 = vec3::normalize(v50) * v44;
                }
                goto LABEL_66;
            }
        }
        else if ((fabsf(zmin) > 0.000001f || v50.z >= 0.0f)
            && (fabsf(zmax) > 0.000001f || v50.z <= 0.0f)) {
            float_t v20 = zmax;
            if (v50.z * zmax <= 0.0f)
                v20 = zmin;
            float_t v38 = (v20 * v50.y) / v50.z;
            if (v38 >= ymin + v23 && v38 <= ymax - v23) {
                v40 = sqrtf(v38 * v38 + v20 * v20);
                goto LABEL_55;
            }
        }
        else {
            v50.z = 0.0f;
            if (fabsf(v50.y) > 0.000001f) {
                v50 = vec3::normalize(v50) * a2;
                if (v50.y < ymin)
                    v50.y = ymin;
                else if (v50.y > ymax)
                    v50.y = ymax;
            }
            goto LABEL_66;
        }
    }

    v51 = 0.0f;
    if (sub_14040AC40(&v50, &v53, v23, &v51)
        || sub_14040AC40(&v50, &v52, v23, &v51)) {
        v40 = vec3::length(v51);
        goto LABEL_55;
    }
    else {
        v50 = 0.0f;
        goto LABEL_66;
    }

LABEL_55:
    if (v40 > a2) {
        v50 *= a2;
        if (vec3::dot(*a8, v50) != 0.0f) {
            float_t v43 = vec3::length(*a8);
            float_t v44 = v43 + (v43 * 0.97f - v43) * step;
            if (vec3::length(v50) < v44)
                v50 = vec3::normalize(v50) * v44;
        }
    }
    else
        v50 *= v40;
    goto LABEL_66;

LABEL_66:
    mat4_mult_vec3(a3, &v50, a1);
    *a8 = v50;
}

static void sub_1403FA770(rob_chara_bone_data_sleeve_adjust* a1) {
    mat4* v4 = a1->bones->data()[MOTION_BONE_C_KATA_L].node[3].mat;
    mat4* v5 = a1->bones->data()[MOTION_BONE_C_KATA_R].node[3].mat;

    vec3 v42;
    mat4_get_translation(v4, &v42);
    vec3 v43;
    mat4_get_translation(v5, &v43);

    vec3 v41;
    sub_1403F9A40(&v41, &v42, v4, a1->sleeve_l.cyofs, a1->sleeve_l.czofs);
    vec3 v40;
    sub_1403F9A40(&v40, &v43, v5, a1->sleeve_r.cyofs, a1->sleeve_r.czofs);

    vec3 v45 = v41 - v40;
    vec3 v44 = v40 - v41;

    float_t v20 = vec3::distance(v41, v40);
    float_t v21 = (a1->sleeve_l.radius * a1->radius + a1->sleeve_r.radius - v20) * 0.5f;
    float_t v22 = (a1->sleeve_r.radius * a1->radius + a1->sleeve_l.radius - v20) * 0.5f;

    bool v16 = true;
    bool v17 = false;
    if (v21 <= 0.0f) {
        v16 = false;
        v21 = vec3::length(a1->field_5C);
        float_t v25 = (0.97f * v21 - v21) * a1->step + v21;
        if (v21 <= 0.001f || v25 <= 0.0f) {
            a1->field_5C = 0.0f;
            v45 = 0.0f;
        }
        else {
            a1->field_5C *= v25 / v21;
            mat4_mult_vec3(v4, &a1->field_5C, &v45);
            v17 = true;
        }
    }

    bool v18 = false;
    bool v19 = true;
    if (v22 <= 0.0f) {
        v19 = false;
        v22 = vec3::length(a1->field_68);
        float_t v31 = (0.97f * v22 - v22) * a1->step + v22;
        if (v22 <= 0.001f || v31 <= 0.0f) {
            a1->field_68 = 0.0f;
            v44 = 0.0f;
        }
        else {
            a1->field_68 *= v31 / v22;
            mat4_mult_vec3(v5, &a1->field_68, &v44);
            v18 = true;
        }
    }

    if (v16)
        sub_1403FA040(&v45, v21, v4, a1->sleeve_l.ymin, a1->sleeve_l.ymax,
            a1->sleeve_l.zmin, a1->sleeve_l.zmax, &a1->field_5C, a1->step);

    if (v19)
        sub_1403FA040(&v44, v22, v5, a1->sleeve_r.ymin, a1->sleeve_r.ymax,
            a1->sleeve_r.zmin, a1->sleeve_r.zmax, &a1->field_68, a1->step);

    if (v16 || v17) {
        mat4 mat = *a1->bones->data()[MOTION_BONE_C_KATA_L].node[2].mat;
        sub_14040AE10(&mat, v42 + v45);
        *a1->bones->data()[MOTION_BONE_C_KATA_L].node[2].mat = mat;
    }

    if (v19 || v18) {
        mat4 mat = *a1->bones->data()[MOTION_BONE_C_KATA_R].node[2].mat;
        sub_14040AE10(&mat, v43 + v44);
        *a1->bones->data()[MOTION_BONE_C_KATA_R].node[2].mat = mat;
    }
}

static void sub_1403F9B20(rob_chara_bone_data_sleeve_adjust* a1, motion_bone_index motion_bone_index) {
    float_t sleeve_radius;
    float_t sleeve_cyofs;
    float_t sleeve_czofs;
    vec3* v6;
    float_t sleeve_ymin;
    float_t sleeve_ymax;
    float_t sleeve_zmin;
    float_t sleeve_zmax;
    vec3 sleeve_mune_ofs;
    float_t sleeve_mune_rad;
    if (motion_bone_index == MOTION_BONE_C_KATA_L) {
        sleeve_radius = a1->sleeve_l.radius;
        sleeve_cyofs = a1->sleeve_l.cyofs;
        sleeve_czofs = a1->sleeve_l.czofs;
        sleeve_ymin = a1->sleeve_l.ymin;
        sleeve_ymax = a1->sleeve_l.ymax;
        sleeve_zmin = a1->sleeve_l.zmin;
        sleeve_zmax = a1->sleeve_l.zmax;
        sleeve_mune_ofs.x = a1->sleeve_l.mune_xofs;
        sleeve_mune_ofs.x = a1->sleeve_l.mune_yofs;
        sleeve_mune_ofs.x = a1->sleeve_l.mune_zofs;
        sleeve_mune_rad = a1->sleeve_l.mune_rad;
        v6 = &a1->field_74;
    }
    else if (motion_bone_index == MOTION_BONE_C_KATA_R) {
        sleeve_radius = a1->sleeve_r.radius;
        sleeve_cyofs = a1->sleeve_r.cyofs;
        sleeve_czofs = a1->sleeve_r.czofs;
        sleeve_ymin = a1->sleeve_r.ymin;
        sleeve_ymax = a1->sleeve_r.ymax;
        sleeve_zmin = a1->sleeve_r.zmin;
        sleeve_zmax = a1->sleeve_r.zmax;
        sleeve_mune_ofs.x = a1->sleeve_r.mune_xofs;
        sleeve_mune_ofs.z = a1->sleeve_r.mune_yofs;
        sleeve_mune_ofs.z = a1->sleeve_r.mune_zofs;
        sleeve_mune_rad = a1->sleeve_r.mune_rad;
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
    sub_1403F9A40(&v37, &v39, v19, sleeve_cyofs, sleeve_czofs);

    vec3 v40;
    mat4* v23 = v15[MOTION_BONE_KL_MUNE_B_WJ].node->mat;
    mat4_mult_vec3_trans(v23, &sleeve_mune_ofs, &v40);

    vec3 v41 = v37 - v40;

    float_t v26 = sleeve_radius * a1->radius + sleeve_mune_rad - vec3::length(v41);
    if (v26 > 0.0f)
        sub_1403FA040(&v41, v26, v19, sleeve_ymin, sleeve_ymax, sleeve_zmin, sleeve_zmax, v6, a1->step);
    else {
        float_t v31 = vec3::length(*v6);
        float_t v32 = (0.97f * v31 - v31) * a1->step + v31;
        if (v31 <= 0.001f || v32 <= 0.0f) {
            *v6 = 0.0f;
            return;
        }
        *v6 *= v32 / v31;
        mat4_mult_vec3(v19, v6, &v41);
    }

    mat4* v42 = v15[motion_bone_index].node[2].mat;
    sub_14040AE10(v42, v39 + v41);
}

static void sub_1403FAF30(rob_chara_bone_data_sleeve_adjust* a1, std::vector<bone_data>* bones, float_t step) {
    a1->step = step;
    a1->bones = bones;
    if (a1->enable1)
        sub_1403FA770(a1);

    if (a1->enable2) {
        sub_1403F9B20(a1, MOTION_BONE_C_KATA_L);
        sub_1403F9B20(a1, MOTION_BONE_C_KATA_R);
    }
}

static void sub_14041AB50(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    sub_1403FAF30(&rob_bone_data->sleeve_adjust, &v2->bone_data.bones, v2->mot_play_data.frame_data.step);
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

static float_t sub_140406E90(bone_data* a1, bone_data* a2, float_t a3, vec3* a4) {
    vec3 v18 = 0.0f;
    mat4_mult_vec3_trans(a1->node[3].mat, &v18, &v18);
    *a4 = v18;

    vec3 v17;
    v17.x = 0.01f;
    v17.y = -0.05f;
    v17.z = 0.0f;
    mat4_mult_vec3_trans(a2->node[0].mat, &v18, &v17);

    if (v18.y - a3 <= v17.y)
        return v18.y - a3;
    return  v17.y;
}

static float_t sub_14064AD10(vec3* a1, float_t a2) {
    task_stage_info stg_info = task_stage_get_current_stage_info();
    if (stg_info.check()) {
        stage* stg = task_stage_get_stage(stg_info);
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
    a2->ik_target.y = v18.y + lerp_def(v12, v11, a5->z);
    a2->ik_target.z = v18.z;
}

static void sub_1404065B0(vec3* a1, std::vector<bone_data>* a2, mat4* a3, float_t a4,
    bone_database_skeleton_type skeleton_type, const motion_bone_index* a6, const motion_bone_index* a7) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;

    const char* name = bone_database_skeleton_type_to_string(skeleton_type);
    const float_t* heel_height = aft_bone_data->get_skeleton_heel_height(name);
    if (!heel_height)
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

static void rob_chara_age_age_ctrl(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, mat4& mat) {
    arr[chara_id * 3 + part_id].ctrl(mat);
}

static void rob_chara_age_age_disp(rob_chara_age_age* arr,
    render_context* rctx, int32_t chara_id, bool reflect, bool chara_color) {
    bool npr = !!rctx->render_manager.npr_param;
    mat4& view = rctx->camera->view;
    vec3 v11 = { view.row0.z, view.row1.z, view.row2.z };
    arr[chara_id * 3 + 0].disp(rctx, chara_id, npr, reflect, v11, chara_color);
    arr[chara_id * 3 + 1].disp(rctx, chara_id, npr, reflect, v11, chara_color);
    arr[chara_id * 3 + 2].disp(rctx, chara_id, npr, reflect, v11, chara_color);
}

static void rob_chara_age_age_load(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id) {
    if (part_id != 1 && part_id != 2)
        return;

    rob_chara_age_age_data data[10];
    for (rob_chara_age_age_data& i : data) {
        i.index = (int32_t)(&i - data);
        i.remaining = -1.0f;
        i.part_id = part_id;
    }

    arr[chara_id * 3 + part_id].load({ 0, 3291 }, data, 10);
}

static void rob_chara_age_age_reset(rob_chara_age_age* arr, int32_t chara_id) {
    arr[chara_id * 3 + 0].reset();
    arr[chara_id * 3 + 1].reset();
    arr[chara_id * 3 + 2].reset();
}

static void rob_chara_age_age_set_alpha(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value) {
    arr[chara_id * 3 + part_id].set_alpha(value);
}

static void rob_chara_age_age_set_disp(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, bool value) {
    arr[chara_id * 3 + part_id].set_disp(value);
}

static void rob_chara_age_age_set_move_cancel(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value) {
    arr[chara_id * 3 + part_id].set_move_cancel(value);
}

static void rob_chara_age_age_set_npr(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, bool value) {
    arr[chara_id * 3 + part_id].set_npr(value);
}

static void rob_chara_age_age_set_rot_speed(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value) {
    arr[chara_id * 3 + part_id].set_rot_speed(value);
}

static void rob_chara_age_age_set_skip(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id) {
    arr[chara_id * 3 + part_id].set_skip();
}

static void rob_chara_age_age_set_step(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value) {
    arr[chara_id * 3 + part_id].set_step(value);
}

static void rob_chara_age_age_set_step_full(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id) {
    arr[chara_id * 3 + part_id].set_step_full();
}

static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    const std::vector<bone_database_bone>* bones) {
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

static void rob_chara_bone_data_get_ik_scale(
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data) {
    if (rob_bone_data->motion_loaded.size() < 0)
        return;

    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    rob_chara_bone_data_ik_scale_calculate(&rob_bone_data->ik_scale, &v2->bone_data.bones,
        rob_bone_data->base_skeleton_type, rob_bone_data->skeleton_type, bone_data);
    float_t ratio0 = rob_bone_data->ik_scale.ratio0;
    v2->field_4F8.field_C0 = ratio0;
    v2->field_4F8.field_C4 = ratio0;
    v2->field_4F8.field_C8 = 0.0f;
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
    bone_database_skeleton_type skeleton_type, const bone_database* bone_data) {
    const char* base_name = bone_database_skeleton_type_to_string(base_skeleton_type);
    const char* name = bone_database_skeleton_type_to_string(skeleton_type);
    const float_t* base_heel_height = bone_data->get_skeleton_heel_height(base_name);
    const float_t* heel_height = bone_data->get_skeleton_heel_height(name);
    if (!base_heel_height || !heel_height)
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
    bone_database_skeleton_type type, const bone_database* bone_data) {
    const char* name = bone_database_skeleton_type_to_string(type);
    const std::vector<bone_database_bone>* bones = bone_data->get_skeleton_bones(name);
    if (!bones)
        return;

    mot_key_data_get_key_set_count_by_bone_database_bones(a1, bones);
    mot_key_data_reserve_key_sets(a1);
    a1->skeleton_type = type;
}

static void mot_blend_reserve_key_sets(mot_blend* blend, bone_database_skeleton_type type,
    PFNMOTIONBONECHECKFUNC bone_check_func, size_t motion_bone_count, const bone_database* bone_data) {
    mot_key_data_reserve_key_sets_by_skeleton_type(&blend->mot_key_data, type, bone_data);
    sub_140413350(&blend->field_0, bone_check_func, motion_bone_count);
}

static bool motion_blend_mot_check(motion_bone_index bone_index) {
    return true;
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
    bone_database_skeleton_type type, const bone_database* bone_data) {
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
    bone_database_skeleton_type skeleton_type, const bone_database* bone_data) {
    if (rob_bone_data->base_skeleton_type == base_skeleton_type
        && rob_bone_data->skeleton_type == skeleton_type)
        return;

    const char* name = bone_database_skeleton_type_to_string(base_skeleton_type);
    const std::vector<bone_database_bone>* bones = bone_data->get_skeleton_bones(name);
    const std::vector<uint16_t>* parent_indices = bone_data->get_skeleton_parent_indices(name);
    const std::vector<std::string>* motion_bones = bone_data->get_skeleton_motion_bones(name);
    if (!bones || !parent_indices || !motion_bones)
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
    sub_14041B440(a1);
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
    v7.rot_y = a1->field_4F8.rot_y;
    v7.prev_rot_y = a1->field_4F8.prev_rot_y;
    a1->blend->Step(&v7);
}

static bool sub_140410A20(motion_blend_mot* a1) {
    return !a1->blend || !a1->blend->rot_y;
}

static void sub_140415B30(mot_blend* a1) {
    struc_400 v3;
    v3.field_0 = false;
    v3.field_1 = false;
    v3.field_2 = false;
    v3.field_3 = false;
    v3.field_4 = false;
    v3.frame = 0.0f;
    v3.rot_y = 0.0f;
    v3.prev_rot_y = 0.0f;
    a1->blend.Step(&v3);
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


static void rob_chara_bone_data_motion_blend_mot_free(rob_chara_bone_data* rob_bone_data) {
    rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 0);
    for (motion_blend_mot*& i : rob_bone_data->motions)
        delete i;
    rob_bone_data->motions.clear();
    rob_bone_data->motion_indices.clear();
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
    rob_bone_data->mats.clear();
    rob_bone_data->mats.resize(rob_bone_data->object_bone_count);

    for (mat4& i : rob_bone_data->mats)
        i = mat4_identity;

    rob_bone_data->mats2.clear();
    rob_bone_data->mats2.resize(rob_bone_data->total_bone_count - rob_bone_data->object_bone_count);

    for (mat4& i : rob_bone_data->mats2)
        i = mat4_identity;

    rob_bone_data->nodes.clear();
    rob_bone_data->nodes.resize(rob_bone_data->total_bone_count);
}

static void sub_1401EAD00(bone_data* a1, bone_data* a2) {
    if (a1->disable_mot_anim)
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

static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data, uint32_t motion_id,
    MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db) {
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
    a1->field_8 = mat4_identity;
    a1->field_48.field_0 = 0.0f;
    a1->field_48.field_4 = 0.0f;
    a1->field_48.field_8 = 0.0f;
    a1->field_48.field_C = 0.0f;
    a1->field_48.field_10 = 0.0f;
    a1->field_48.field_14 = 0.0f;
    a1->field_48.field_18 = 0.0f;
    a1->field_48.field_1C = 0.0f;
    a1->field_48.field_20 = 0.0f;
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
    a1->field_B4 = 0.0f;
    a1->field_C0 = 0.0f;
    a1->field_CC = mat4_identity;
    a1->field_10C = mat4_identity;
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
    a1->field_1BC = 1.0f;
    a1->field_1C4 = 0;
    a1->field_1C8 = 0;
}

static void rob_chara_bone_data_set_eyelid_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_blend_duration(&rob_bone_data->eyelid, duration, step, offset);
}

static void rob_chara_bone_data_set_eyelid_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->eyelid, frame);
}

static void rob_chara_bone_data_set_eyelid_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->eyelid.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_eyes_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_blend_duration(&rob_bone_data->eyes, duration, step, offset);
}

static void rob_chara_bone_data_set_eyes_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->eyes, frame);
}

static void rob_chara_bone_data_set_eyes_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->eyes.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_face_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_blend_duration(&rob_bone_data->face, duration, step, offset);
}

static void rob_chara_bone_data_set_face_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->face, frame);
}

static void rob_chara_bone_data_set_face_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->face.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_hand_l_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_blend_duration(&rob_bone_data->hand_l, duration, step, offset);
}

static void rob_chara_bone_data_set_hand_l_frame(rob_chara_bone_data* rob_bone_data, float_t frame) {
    mot_blend_set_frame(&rob_bone_data->hand_l, frame);
}

static void rob_chara_bone_data_set_hand_l_play_frame_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    mot_play_frame_data* frame_data = &rob_bone_data->hand_l.mot_play_data.frame_data;
    frame_data->step_prev = frame_data->step;
    frame_data->step = step;
}

static void rob_chara_bone_data_set_hand_r_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_blend_duration(&rob_bone_data->hand_r, duration, step, offset);
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
    const std::vector<bone_database_bone>* bones, const std::string* motion_bones) {
    size_t chain_pos = 0;
    size_t ik_bone_count = 0;
    size_t total_bone_count = 0;

    mat4* mats = rob_bone_data->mats.data();
    mat4* mats2 = rob_bone_data->mats2.data();
    bone_node* nodes = rob_bone_data->nodes.data();
    for (const bone_database_bone& i : *bones)
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

static void rob_chara_bone_data_set_mouth_anim_blend_duration(rob_chara_bone_data* rob_bone_data,
    float_t duration, float_t step, float_t offset) {
    mot_blend_set_blend_duration(&rob_bone_data->mouth, duration, step, offset);
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
    const uint16_t* parent_indices) {
    if (rob_bone_data->nodes.size() < 1)
        return;

    parent_indices++;
    bone_node* i_begin = rob_bone_data->nodes.data();
    bone_node* i_begin_1 = rob_bone_data->nodes.data() + 1;
    bone_node* i_end = rob_bone_data->nodes.data() + rob_bone_data->nodes.size();
    for (bone_node* i = i_begin_1; i != i_end; i++)
        i->parent = &i_begin[*parent_indices++];
}

static void rob_chara_bone_data_set_rot_y(rob_chara_bone_data* rob_bone_data, float_t rot_y) {
    rob_bone_data->motion_loaded.front()->bone_data.rot_y = rot_y;
}

static void rob_chara_bone_data_set_step(rob_chara_bone_data* rob_bone_data, float_t step) {
    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        i->set_step(step);
}

static void sub_140413EB0(struc_308* a1) {
    a1->field_8C = false;
    a1->field_4C = a1->mat;
    a1->mat = mat4_identity;
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
    a1->field_4F8.field_90 = 0.0f;

    mat4 rot_mat = b_n_hara_cp->rot_mat[0];
    mat4 mat = b_kg_hara_y->rot_mat[0];
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat = b_kl_hara_xz->rot_mat[0];
    mat4_mult(&mat, &rot_mat, &rot_mat);
    mat = b_kl_hara_etc->rot_mat[0];
    mat4_mult(&mat, &rot_mat, &rot_mat);
    b_n_hara_cp->rot_mat[0] = rot_mat;
    b_kg_hara_y->rot_mat[0] = mat4_identity;
    b_kl_hara_xz->rot_mat[0] = mat4_identity;
    b_kl_hara_etc->rot_mat[0] = mat4_identity;

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
    if (!v4 || !v4->rot_y)
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
    if (!v3 || !v3->rot_y)
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
    if (!a1->blend.rot_y)
        return;

    for (bone_data& i : *a2)
        if (sub_140410250(&a1->field_0.field_8, i.motion_bone_index))
            a1->blend.Blend(&a2->data()[i.motion_bone_index], 0);
}

static void sub_1404182B0(rob_chara_bone_data* rob_bone_data) {
    if (!rob_bone_data->motion_loaded.size())
        return;

    motion_blend_mot* v3 = rob_bone_data->motion_loaded.front();

    auto i_begin = rob_bone_data->motion_loaded.rbegin();;
    auto i_end = rob_bone_data->motion_loaded.rend();
    for (auto i = i_begin; i != i_end; i++) {
        if (!sub_1404136B0(*i))
            continue;

        auto v4 = i;
        v4++;
        sub_140410A40(*i, &(*v4)->bone_data.bones, &(*i)->bone_data.bones);
    }

    auto j_begin = rob_bone_data->motion_loaded.rbegin();;
    auto j_end = rob_bone_data->motion_loaded.rend();
    for (auto j = j_begin; j != j_end; j++) {
        if (sub_1404136B0(*j))
            continue;

        if (j != rob_bone_data->motion_loaded.rbegin()) {
            auto v5 = j;
            v5--;
            sub_140410B70(*j, &(*v5)->bone_data.bones);
        }
        else if ((*j)->get_type() == MOTION_BLEND_FREEZE)
            sub_140410B70(*j, 0);
    }

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
        v7->trans -= v7->trans;
    }
}

void sub_14041B9F0(rob_chara_bone_data* rob_bone_data) {
    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        sub_1404146F0(&i->field_0);

    sub_1404146F0(&rob_bone_data->face.field_0);
    sub_1404146F0(&rob_bone_data->eyelid.field_0);
}

void sub_140506C20(struc_523* a1) {
    a1->field_0 = 0;
    a1->field_1 = 0;
    a1->field_4 = 0;
    a1->field_8 = 0;
    a1->field_C = 0;
    a1->field_10 = 0.0f;
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
    a1->field_4 = { 0, {}, (uint32_t)-1, 0 };
    a1->field_18 = { 0, {}, (uint32_t)-1, 0 };
    a1->field_2C = { 0, {}, (uint32_t)-1, 0 };
    a1->field_40 = { 0, {}, (uint32_t)-1, 0 };
    a1->field_54 = { 0, {}, (uint32_t)-1, 0 };
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
    a1->field_B4 = 0;
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

    for (mat4& i : a1->field_78)
        i = mat4_identity;

    for (mat4& i : a1->field_738)
        i = mat4_identity;

    for (struc_195& i : a1->field_DF8)
        i = {};

    for (struc_195& i : a1->field_1230)
        i = {};

    for (struc_195& i : a1->field_1668)
        i = {};

    for (pos_scale& i : a1->field_1AA0)
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

static void rob_chara_add_motion_reset_data(rob_chara* rob_chr,
    uint32_t motion_id, float_t frame, int32_t iterations) {
    if (motion_id != -1)
        rob_chr->item_equip->add_motion_reset_data(motion_id, frame, iterations);
}

static void rob_chara_age_age_ctrl(rob_chara* rob_chr, int32_t part_id, const char* name) {
    mat4 mat = *rob_chr->item_equip->get_ex_data_bone_node_mat(ITEM_KAMI, name);
    if (vec3::length(rob_chr->data.adjust_data.pos_adjust) > 0.000001f) {
        mat4 temp;
        mat4_translate(&rob_chr->data.adjust_data.pos_adjust, &temp);
        mat4_mult(&mat, &temp, &mat);
    }
    rob_chara_age_age_array_ctrl(rob_chr->chara_id, part_id, mat);
}

static object_info rob_chara_get_head_object(rob_chara* rob_chr, int32_t head_object_id) {
    if (head_object_id < 0 || head_object_id > 8)
        return {};
    object_info obj_info = rob_chr->item_cos_data.get_head_object_replace(head_object_id);
    if (obj_info.not_null())
        return obj_info;
    return rob_chr->chara_init_data->head_objects[head_object_id];
}

static object_info rob_chara_get_object_info(rob_chara* rob_chr, item_id id) {
    return rob_chr->item_equip->get_object_info(id);
}

static void rob_chara_load_default_motion(rob_chara* rob_chr,
    const bone_database* bone_data, const motion_database* mot_db) {
    uint32_t motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
    rob_chara_load_default_motion_sub(rob_chr, 1, motion_id, bone_data, mot_db);
}

static void sub_140419820(rob_chara_bone_data* rob_bone_data, int32_t skeleton_select) {
    sub_140412E10(rob_bone_data->motion_loaded.front(), skeleton_select);
}

static void rob_chara_load_default_motion_sub(rob_chara* rob_chr, int32_t skeleton_select,
    uint32_t motion_id, const bone_database* bone_data, const motion_database* mot_db) {
    rob_chr->bone_data->load_face_motion(-1, mot_db);
    rob_chr->bone_data->load_hand_l_motion(-1, mot_db);
    rob_chr->bone_data->load_hand_r_motion(-1, mot_db);
    rob_chr->bone_data->load_mouth_motion(-1, mot_db);
    rob_chr->bone_data->load_eyes_motion(-1, mot_db);
    rob_chr->bone_data->load_eyelid_motion(-1, mot_db);
    rob_chara_bone_data_motion_load(rob_chr->bone_data, motion_id, MOTION_BLEND_FREEZE, bone_data, mot_db);
    rob_chr->bone_data->set_frame(0.0f);
    sub_14041C680(rob_chr->bone_data, false);
    sub_14041C9D0(rob_chr->bone_data, false);
    sub_14041D2D0(rob_chr->bone_data, false);
    sub_14041BC40(rob_chr->bone_data, false);
    sub_14041D270(rob_chr->bone_data, 0.0f);
    sub_14041D2A0(rob_chr->bone_data, 0.0f);
    rob_chara_bone_data_set_rot_y(rob_chr->bone_data, 0.0f);
    rob_chara_bone_data_set_motion_duration(rob_chr->bone_data, 0.0f, 1.0f, 1.0f);
    sub_14041D310(rob_chr->bone_data, 0.0f, 0.0f, 2);
    rob_chr->bone_data->interpolate();
    rob_chr->bone_data->update(0);
    rob_chr->sub_140509D30();
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
    chara_index chara_index, int32_t cos_id, const rob_chara_pv_data& pv_data) {
    rob_chr->chara_id = chara_id;
    rob_chr->chara_index = chara_index;
    rob_chr->cos_id = cos_id;
    rob_chr->pv_data = pv_data;
    rob_chr->chara_init_data = chara_init_data_get(chara_index);
    if (!check_cos_id_is_501(cos_id)) {
        const item_cos_data* cos = item_table_handler_array_get_item_cos_data(chara_index, cos_id);
        if (cos)
            rob_chr->item_cos_data.set_chara_index_item_nos(chara_index, cos->arr);
    }
    rob_chr->item_cos_data.set_item_array(pv_data.item);
}

static void rob_cmn_mottbl_read(void* a1, const void* data, size_t size) {
    free_def(rob_cmn_mottbl_data);

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

void rob_chara_age_age_array_ctrl(int32_t chara_id, int32_t part_id, mat4& mat) {
    rob_chara_age_age_ctrl(rob_chara_age_age_array, chara_id, part_id, mat);
}

void rob_chara_age_age_array_disp(render_context* rctx,
    int32_t chara_id, bool reflect, bool chara_color) {
    mdl::ObjFlags obj_flags = rctx->disp_manager.get_obj_flags();
    rob_chara_age_age_disp(rob_chara_age_age_array, rctx, chara_id, reflect, chara_color);
    rctx->disp_manager.set_obj_flags(obj_flags);
}

void rob_chara_age_age_array_load(int32_t chara_id, int32_t part_id) {
    rob_chara_age_age_load(rob_chara_age_age_array, chara_id, part_id);
}

void rob_chara_age_age_array_reset(int32_t chara_id) {
    rob_chara_age_age_reset(rob_chara_age_age_array, chara_id);
}

void rob_chara_age_age_array_set_alpha(int32_t chara_id, int32_t part_id, float_t alpha) {
    rob_chara_age_age_set_alpha(rob_chara_age_age_array, chara_id, part_id, alpha);
}

void rob_chara_age_age_array_set_disp(int32_t chara_id, int32_t part_id, bool value) {
    rob_chara_age_age_set_disp(rob_chara_age_age_array, chara_id, part_id, value);
}

void rob_chara_age_age_array_set_move_cancel(int32_t chara_id, int32_t part_id, float_t value) {
    rob_chara_age_age_set_move_cancel(rob_chara_age_age_array, chara_id, part_id, value);
}

void rob_chara_age_age_array_set_npr(int32_t chara_id, int32_t part_id, bool value) {
    rob_chara_age_age_set_npr(rob_chara_age_age_array, chara_id, part_id, value);
}

void rob_chara_age_age_array_set_params(int32_t chara_id, int32_t part_id,
    int32_t npr, int32_t rot_speed, int32_t skip, int32_t disp) {
    if (!npr || npr == 1)
        rob_chara_age_age_array_set_npr(chara_id, part_id, npr);
    if (rot_speed != -1)
        rob_chara_age_age_array_set_rot_speed(chara_id, part_id, (float_t)rot_speed * 0.001f);
    if (skip != -1)
        rob_chara_age_age_array_set_skip(chara_id, part_id);
    if (!disp || disp == 1)
        rob_chara_age_age_array_set_disp(chara_id, part_id, !!disp);
}

void rob_chara_age_age_array_set_rot_speed(int32_t chara_id, int32_t part_id, float_t value) {
    rob_chara_age_age_set_rot_speed(rob_chara_age_age_array, chara_id, part_id, value);
}

void rob_chara_age_age_array_set_skip(int32_t chara_id, int32_t part_id) {
    rob_chara_age_age_set_skip(rob_chara_age_age_array, chara_id, part_id);
}

void rob_chara_age_age_array_set_step(int32_t chara_id, int32_t part_id, float_t step) {
    rob_chara_age_age_set_step(rob_chara_age_age_array, chara_id, part_id, step);
}

void rob_chara_age_age_array_set_step_full(int32_t chara_id, int32_t part_id) {
    rob_chara_age_age_set_step_full(rob_chara_age_age_array, chara_id, part_id);
}

bool rob_chara_array_check_visibility(int32_t chara_id) {
    if (rob_chara_pv_data_array_check_chara_id(chara_id))
        return rob_chara_array[chara_id].is_visible();
    return false;
}

rob_chara* rob_chara_array_get(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return 0;

    if (rob_chara_pv_data_array_check_chara_id(chara_id))
        return &rob_chara_array[chara_id];
    return 0;
}

rob_chara_bone_data* rob_chara_array_get_bone_data(int32_t chara_id) {
    return rob_chara_array[chara_id].bone_data;
}

float_t rob_chara_array_get_data_adjust_scale(int32_t chara_id) {
    if (rob_chara_pv_data_array_check_chara_id(chara_id))
        return rob_chara_array[chara_id].data.adjust_data.scale;
    return 1.0f;
}

rob_chara_item_cos_data* rob_chara_array_get_item_cos_data(int32_t chara_id) {
    return &rob_chara_array[chara_id].item_cos_data;
}

rob_chara_item_equip* rob_chara_array_get_item_equip(int32_t chara_id) {
    return rob_chara_array[chara_id].item_equip;
}

int32_t rob_chara_array_init_chara_index(chara_index chara_index,
    const rob_chara_pv_data& pv_data, int32_t cos_id, bool can_set_default) {
    if (!app::TaskWork::CheckTaskReady(task_rob_manager)
        || pv_data.type < ROB_CHARA_TYPE_0|| pv_data.type > ROB_CHARA_TYPE_3)
        return -1;

    int32_t chara_id = 0;
    while (rob_chara_pv_data_array[chara_id].type != ROB_CHARA_TYPE_NONE) {
        chara_id++;
        if (chara_id >= ROB_CHARA_COUNT)
            return -1;
    }

    if (can_set_default && (cos_id < 0 || cos_id > 498))
        cos_id = 0;
    rob_chara_pv_data_array[chara_id] = pv_data;
    rob_chara_set_pv_data(&rob_chara_array[chara_id], chara_id, chara_index, cos_id, pv_data);
    task_rob_manager->AppendInitCharaList(&rob_chara_array[chara_id]);
    return chara_id;
}

void rob_chara_array_free_chara_id(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    task_rob_manager->AppendFreeCharaList(&rob_chara_array[chara_id]);
    rob_chara_pv_data_array[chara_id].type = ROB_CHARA_TYPE_NONE;
}

void rob_chara_array_reset_bone_data_item_equip(int32_t chara_id) {
    rob_chara_item_equip* rob_itm_equip = rob_chara_array[chara_id].item_equip;
    rob_itm_equip->reset_init_data(rob_chara_bone_data_get_node(
        rob_chara_array[chara_id].bone_data, MOT_BONE_N_HARA_CP));
    rob_itm_equip->set_shadow_type(chara_id);
    rob_itm_equip->field_A0 = 0x05;
}

void rob_chara_array_set_alpha_obj_flags(int32_t chara_id, float_t alpha, mdl::ObjFlags flags) {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chr->item_equip->set_alpha_obj_flags(alpha, flags);

    if (rob_chr->check_for_ageageagain_module()) {
        rob_chara_age_age_array_set_alpha(chara_id, 1, alpha);
        rob_chara_age_age_array_set_alpha(chara_id, 2, alpha);
    }
}

void rob_chara_array_set_visibility(int32_t chara_id, bool value) {
    rob_chara_array[chara_id].set_visibility(value);
}

bool pv_osage_manager_array_get_disp() {
    bool disp = false;
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        disp |= pv_osage_manager_array_get(i)->GetDisp();
    return disp;
}

bool pv_osage_manager_array_get_disp(int32_t chara_id) {
    return pv_osage_manager_array_get(chara_id)->GetDisp();
}

void pv_osage_manager_array_set_motion_ids(
    int32_t chara_id, const std::vector<uint32_t>& motion_ids) {
    std::vector<pv_data_set_motion> set_motion;
    for (const uint32_t& i : motion_ids)
        set_motion.push_back({ i });
    pv_osage_manager_array_get(chara_id)->SetPvSetMotion(set_motion);
}

void pv_osage_manager_array_set_not_reset_true() {
    for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
        pv_osage_manager_array_get(i)->SetNotReset(true);
}

void pv_osage_manager_array_set_pv_id(int32_t chara_id, int32_t pv_id, bool reset) {
    pv_osage_manager_array_get(chara_id)->SetPvId(pv_id, chara_id, reset);
}

void pv_osage_manager_array_set_pv_set_motion(
    int32_t chara_id, const std::vector<pv_data_set_motion>& set_motion) {
    pv_osage_manager_array_get(chara_id)->SetPvSetMotion(set_motion);
}

int32_t rob_chara_array_reset_pv_data(int32_t chara_id) {
    rob_chara_pv_data_array[chara_id].type = ROB_CHARA_TYPE_NONE;
    return chara_id;
}

bool rob_chara_check_for_ageageagain_module(chara_index chara_index, int32_t cos_id) {
    return chara_index == CHARA_MIKU && cos_id == 148;
}

bool rob_chara_pv_data_array_check_chara_id(int32_t chara_id) {
    return rob_chara_pv_data_array[chara_id].type != ROB_CHARA_TYPE_NONE;
}

void rob_sleeve_handler_data_get_sleeve_data(
    ::chara_index chara_index, int32_t cos, rob_sleeve_data& l, rob_sleeve_data& r) {
    rob_sleeve_handler_data->get_sleeve_data(chara_index, cos, l, r);
}

bool rob_sleeve_handler_data_load() {
    return rob_sleeve_handler_data->load();
}

void rob_sleeve_handler_data_read() {
    rob_sleeve_handler_data->read();
}

bool task_rob_load_add_task() {
    return app::TaskWork::AddTask(task_rob_load, 0, "ROB LOAD", 0);
}

bool task_rob_load_append_free_req_data(chara_index chara_index) {
    return task_rob_load->AppendFreeReqData(chara_index);
}

bool task_rob_load_append_free_req_data_obj(chara_index chara_index, const item_cos_data* cos) {
    return task_rob_load->AppendFreeReqDataObj(chara_index, cos);
}

bool task_rob_load_append_load_req_data(chara_index chara_index) {
    return task_rob_load->AppendLoadReqData(chara_index);
}

bool task_rob_load_append_load_req_data_obj(chara_index chara_index, const item_cos_data* cos) {
    return task_rob_load->AppendLoadReqDataObj(chara_index, cos);
}

bool task_rob_load_check_load_req_data() {
    if (task_rob_load->field_F0 == 2 && !task_rob_load->load_req_data_obj.size())
        return task_rob_load->load_req_data.size() != 0;
    return true;
}

bool task_rob_load_del_task() {
    task_rob_load->DelTask();
    return true;
}

bool task_rob_manager_add_task() {
    return app::TaskWork::AddTask(task_rob_manager, "ROB_MANAGER TASK");
}

bool task_rob_manager_check_chara_loaded(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return false;
    return task_rob_manager->CheckCharaLoaded(&rob_chara_array[chara_id]);
}

bool task_rob_manager_check_task_ready() {
    return app::TaskWork::CheckTaskReady(task_rob_manager);
}

bool task_rob_manager_hide_task() {
    return task_rob_manager->HideTask();
}

bool task_rob_manager_get_wait(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return false;

    return task_rob_manager->GetWait(&rob_chara_array[chara_id]);
}

bool task_rob_manager_run_task() {
    return task_rob_manager->RunTask();
}

bool task_rob_manager_del_task() {
    if (!app::TaskWork::CheckTaskReady(task_rob_manager))
        return true;

    task_rob_manager->DelTask();
    return false;
}

bone_node_expression_data::bone_node_expression_data() {
    position = 0.0f;
    rotation = 0.0f;
    scale = 1.0f;
    parent_scale = 1.0f;
}

void bone_node_expression_data::mat_set(vec3& parent_scale, mat4& ex_data_mat, mat4& mat) {
    vec3 position = this->position * parent_scale;
    mat4_translate_mult(&ex_data_mat, &position, &ex_data_mat);
    mat4_rotate_zyx_mult(&ex_data_mat, &rotation, &ex_data_mat);
    this->parent_scale = scale * parent_scale;
    mat = ex_data_mat;
    mat4_scale_rot(&mat, &this->parent_scale, &mat);
}

void bone_node_expression_data::reset_scale() {
    scale = 1.0f;
    parent_scale = 1.0f;
}

void bone_node_expression_data::set_position_rotation(
    float_t position_x, float_t position_y, float_t position_z,
    float_t rotation_x, float_t rotation_y, float_t rotation_z) {
    position.x = position_x;
    position.y = position_y;
    position.z = position_z;
    rotation.x = rotation_x;
    rotation.z = rotation_z;
    rotation.y = rotation_y;
    scale = 1.0f;
    parent_scale = 1.0f;
}

void bone_node_expression_data::set_position_rotation(vec3& position, vec3& rotation) {
    this->position = position;
    this->rotation = rotation;
    scale = 1.0f;
    parent_scale = 1.0f;
}

bone_node::bone_node() : name(), mat(), parent(), ex_data_mat() {

}

float_t* bone_node::get_exp_data_component(size_t index, ex_expression_block_stack_type& type) {
    switch (index) {
    case 0:
        type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &exp_data.position.x;
    case 1:
        type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &exp_data.position.y;
    case 2:
        type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &exp_data.position.z;
    case 3:
        type = EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN;
        return &exp_data.rotation.x;
    case 4:
        type = EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN;
        return &exp_data.rotation.y;
    case 5:
        type = EX_EXPRESSION_BLOCK_STACK_VARIABLE_RADIAN;
        return &exp_data.rotation.z;
    case 6:
        type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &exp_data.scale.x;
    case 7:
        type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &exp_data.scale.y;
    case 8:
        type = EX_EXPRESSION_BLOCK_STACK_VARIABLE;
        return &exp_data.scale.z;
    default:
        return 0;
    }
}

void bone_node::set_name_mat_ex_data_mat(const char* name, mat4* mat, mat4* ex_data_mat) {
    this->name = name;

    this->mat = mat;
    if (mat)
        *mat = mat4_identity;

    this->parent = 0;
    exp_data = {};

    this->ex_data_mat = ex_data_mat;
    if (ex_data_mat)
        *ex_data_mat = mat4_identity;
}

mot_key_data::mot_key_data() : key_sets_ready(), key_set_count(), key_set(),
mot(), key_set_data(), mot_data(), skeleton_type(), skeleton_select(), field_68() {
    motion_id = -1;
    frame = -1.0f;
}

mot_key_data::~mot_key_data() {

}

eyes_adjust::eyes_adjust() : xrot_adjust(), base_adjust() {
    neg = -1.0f;
    pos = -1.0f;
}

bone_data::bone_data() : type(), has_parent(), motion_bone_index(), mirror(), parent(),
disable_mot_anim(), key_set_offset(), key_set_count(), frame(), base_translation(),
rotation(), ik_target(), trans(), rot_mat(), trans_prev(), rot_mat_prev(), pole_target_mat(),
parent_mat(), node(), ik_segment_length(), ik_2nd_segment_length(), arm_length() {
    eyes_xrot_adjust_neg = 1.0f;
    eyes_xrot_adjust_pos = 1.0f;
}

bone_data_parent::bone_data_parent() : rob_bone_data(),
motion_bone_count(), ik_bone_count(), chain_pos(), global_trans(),
global_rotation(), bone_key_set_count(), global_key_set_count(), rot_y() {

}

bone_data_parent::~bone_data_parent() {

}

void mot_play_frame_data::reset() {
    frame = -1.0f;
    step = 1.0f;
    step_prev = 1.0f;
    frame_count = 0.0f;
    last_frame = -1.0f;
    max_frame = -1.0f;
    playback_state = MOT_PLAY_FRAME_DATA_PLAYBACK_NONE;
    loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
    loop_begin = 0.0f;
    loop_end = -1.0f;
    looped = false;
    loop_count = -1;
}

void mot_play_frame_data::set_frame(float_t frame) {
    this->frame = frame;
    looped = false;
    if (max_frame >= 0.0f && (playback_state == MOT_PLAY_FRAME_DATA_PLAYBACK_BACKWARD
        && frame <= max_frame) || frame >= max_frame)
        max_frame = -1.0f;
}

void mot_play_data::reset() {
    frame_data.reset();
    loop_index = 0;
    loop_frames_enabled = false;
    loop_frames = 0.0f;
    ext_frame = 0;
    ext_step = 0;
}

struc_308::struc_308() : field_0(), field_4(), field_8(), field_8C(),
rot_y(), prev_rot_y(), field_BC(), field_BD(), field_C0(), field_C4() {

}

MotionBlend::MotionBlend() : enable(), rot_y(), duration(), frame(), offset(), blend() {
    step = 1.0f;
}

MotionBlend::~MotionBlend() {

}

void MotionBlend::Reset() {
    enable = false;
    rot_y = false;
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
        enable = false;
    }
    else {
        this->duration = duration;
        enable = true;
    }
    frame = 0.0f;
}

MotionBlendCross::MotionBlendCross() : field_20(), field_21() {
    rot_y_mat = mat4_identity;
    field_64 = mat4_identity;
    field_A4 = mat4_identity;
    field_E4 = mat4_identity;
}

MotionBlendCross::~MotionBlendCross() {

}

void MotionBlendCross::Reset() {
    MotionBlend::Reset();
    field_20 = false;
    field_21 = false;
    rot_y_mat = mat4_identity;
    field_64 = mat4_identity;
    field_A4 = mat4_identity;
    field_E4 = mat4_identity;
}

void MotionBlendCross::Field_10(float_t a2, float_t a3, int32_t a4) {

}

void MotionBlendCross::Step(struc_400* a2) {
    field_20 = a2->field_0;
    field_21 = true;
    if (duration <= 0.0f || duration <= frame || fabsf(duration - frame) <= 0.000001f) {
        enable = false;
        rot_y = false;
    }
    else {
        rot_y = true;
        blend = (frame + offset) / (duration + offset);
        frame += step;
    }

    if (rot_y)
        mat4_rotate_y(-a2->prev_rot_y, &rot_y_mat);
    else
        rot_y_mat = mat4_identity;
}

void MotionBlendCross::Field_20(std::vector<bone_data>* a2, std::vector<bone_data>* a3) {
    if (!a2 || !a3)
        return;

    bone_data* v4 = a2->data();
    bone_data* v7 = a3->data();

    mat4_mult(&v4[4].rot_mat[0], &v4[0].rot_mat[0], &field_64);
    mat4_mult(&v7[0].rot_mat[0], &rot_y_mat, &field_A4);
    mat4_blend_rotation(&v4[0].rot_mat[0], &field_A4, &v4[0].rot_mat[0], blend);
    mat4_mult(&v7[4].rot_mat[0], &field_A4, &field_A4);
    mat4_blend_rotation(&v4[4].rot_mat[0], &v7[4].rot_mat[0], &v4[4].rot_mat[0], blend);
    mat4_mult(&v4[4].rot_mat[0], &v4[0].rot_mat[0], &field_E4);
    mat4_transpose(&field_E4, &field_E4);
}

void MotionBlendCross::Blend(bone_data* a2, bone_data* a3) {
    if (!a2 || !a3 || a2->disable_mot_anim)
        return;

    mat4 v15;
    mat4 v16;

    switch (a2->type) {
    case BONE_DATABASE_BONE_ROTATION:
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
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
        a2->trans = vec3::lerp(a3->trans, a2->trans, blend);
        break;
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        if (field_21) {
            a2->trans.x = lerp_def(a3->trans.x, a2->trans.x, blend);
            a2->trans.z = lerp_def(a3->trans.z, a2->trans.z, blend);
        }
        if (field_20)
            a2->trans.y = lerp_def(a3->trans.y, a2->trans.y, blend);
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
    }
}

bool MotionBlendCross::Field_30() {
    return false;
}

MotionBlendCombine::MotionBlendCombine() {

}

MotionBlendCombine::~MotionBlendCombine() {

}

void MotionBlendCombine::Step(struc_400* a2) {
    field_20 = a2->field_0;
    field_21 = true;
    if (rot_y) {
        mat4 mat;
        mat4_rotate_y(-a2->prev_rot_y, &mat);
        rot_y_mat = mat;
    }
    else
        rot_y_mat = mat4_identity;
}

bool MotionBlendCombine::Field_30() {
    return true;
}

MotionBlendFreeze::MotionBlendFreeze() : field_20(),field_21(), field_24(), field_28(), field_2C(), field_30(),
rot_y_mat(mat4_identity), field_74(mat4_identity),
field_B4(mat4_identity), field_F4(mat4_identity) {

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
    rot_y_mat = mat4_identity;
    field_74 = mat4_identity;
    field_B4 = mat4_identity;
    field_F4 = mat4_identity;
}

void MotionBlendFreeze::Field_10(float_t a2, float_t a3, int32_t a4) {
    field_2C = a3;
    field_30 = a4;
    if (a2 < 0.0f)
        field_28 = 0.0f;
    else
        field_28 = a2;
}

void MotionBlendFreeze::Step(struc_400* a2) {
    field_24 = 2;
    if (duration <= 0.0f || duration <= frame || fabsf(duration - frame) <= 0.000001f) {
        float_t v7 = a2->frame;
        float_t v8 = field_2C;
        float_t v9 = v7 - v8;
        if (v7 < v8 || field_28 <= 0.0f || field_28 <= v9 || fabsf(field_28 - v9) <= 0.000001f) {
            enable = false;
            rot_y = false;
        }
        else {
            rot_y = true;
            field_24 = field_30;
            blend = 1.0f - (v9 + offset) / (field_28 + offset);
            if (a2->field_4)
                field_24 = field_30 + 1;
            field_20 = a2->field_2;
            field_21 = a2->field_3;
        }
    }
    else {
        rot_y = true;
        field_24 = 0;
        blend = (frame + offset) / (duration + offset);
        frame += step;
        field_20 = a2->field_0;
        field_21 = a2->field_1;
    }

    if (rot_y) {
        mat4 mat;
        if (field_24)
            mat4_rotate_y(a2->rot_y, &mat);
        else
            mat4_rotate_y(-a2->prev_rot_y, &mat);
        rot_y_mat = mat;
    }
    else
        rot_y_mat = mat4_identity;
}

void MotionBlendFreeze::Field_20(std::vector<bone_data>* a2, std::vector<bone_data>* a3) {
    if (!a2)
        return;

    bone_data* v2 = a2->data();
    mat4_mult(&v2[4].rot_mat[0], &v2->rot_mat[0], &field_74);
    mat4_mult(&v2->rot_mat_prev[0][field_24], &rot_y_mat, &field_B4);
    mat4_blend_rotation(v2->rot_mat, &field_B4, v2->rot_mat, blend);
    mat4_mult(&v2[4].rot_mat_prev[0][field_24], &field_B4, &field_B4);
    mat4_blend_rotation(&v2[4].rot_mat[0], &v2[4].rot_mat_prev[0][field_24], &v2[4].rot_mat[0], blend);
    mat4_mult(&v2[4].rot_mat[0], &v2->rot_mat[0], &field_F4);
    mat4_transpose(&field_F4, &field_F4);
}

void MotionBlendFreeze::Blend(bone_data* a2, bone_data* a3) {
    if (!a2 || a2->disable_mot_anim)
        return;

    mat4 v15;
    mat4 v16;
    switch (a2->type) {
    case BONE_DATABASE_BONE_ROTATION:
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
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
        a2->trans = vec3::lerp(a2->trans_prev[field_24], a2->trans, blend);
        break;
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        if (field_21) {
            a2->trans.x = lerp_def(a2->trans_prev[field_24].x, a2->trans.x, blend);
            a2->trans.z = lerp_def(a2->trans_prev[field_24].z, a2->trans.z, blend);
        }
        if (field_20)
            a2->trans.y = lerp_def(a2->trans_prev[field_24].y, a2->trans.y, blend);
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

void PartialMotionBlendFreeze::Step(struc_400* a1) {
    if (duration <= 0.0f || duration <= frame || fabsf(duration - frame) <= 0.000001f) {
        enable = false;
        rot_y = false;
    }
    else {
        rot_y = true;
        frame += step;
        blend = (offset + frame) / (offset + duration);
    }
}

void PartialMotionBlendFreeze::Field_20(std::vector<bone_data>* a1, std::vector<bone_data>* a2) {

}

void PartialMotionBlendFreeze::Blend(bone_data* a2, bone_data* a3) {
    if (!a2 || a2->disable_mot_anim)
        return;

    float_t inv_blend = 1.0f - blend;
    switch (a2->type) {
    case BONE_DATABASE_BONE_ROTATION:
    default:
        mat4_lerp_rotation(&a2->rot_mat_prev[0][0], &a2->rot_mat[0], &a2->rot_mat[0], blend);
        break;
    case BONE_DATABASE_BONE_POSITION:
    case BONE_DATABASE_BONE_TYPE_1:
        a2->trans = vec3::lerp(a2->trans_prev[0], a2->trans, blend);
        break;
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        mat4_lerp_rotation(&a2->rot_mat_prev[0][0], &a2->rot_mat[0], &a2->rot_mat[0], blend);
        a2->trans = vec3::lerp(a2->trans_prev[0], a2->trans, blend);
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        mat4_lerp_rotation(&a2->rot_mat_prev[2][0], &a2->rot_mat[2], &a2->rot_mat[2], blend);
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION: {
        mat4 v15;
        mat4 v16;
        mat4 v17;
        mat4_mult(&a2->rot_mat_prev[1][0], &a2->rot_mat_prev[0][0], &v16);
        mat4_mult(&a2->rot_mat[1], &a2->rot_mat[0], &v15);
        mat4_lerp_rotation(&v16, &v15, &v16, blend);
        mat4_transpose(&a2->rot_mat[0], &v17);
        mat4_mult(&v17, &v16, &a2->rot_mat[1]);
    } break;
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

MotionBlendType motion_blend_mot::get_type() {
    if (blend == &cross)
        return MOTION_BLEND_CROSS;
    if (blend == &freeze)
        return MOTION_BLEND_FREEZE;
    if (blend == &combine)
        return MOTION_BLEND_COMBINE;
    return MOTION_BLEND;
}

void motion_blend_mot::reset() {
    mot_play_data.reset();
    field_4F8.field_0 = 0;
    field_4F8.field_8 = 0;
    field_4F8.mat = mat4_identity;
    field_4F8.field_4C = mat4_identity;
    field_4F8.field_8C = false;
    field_4F8.rot_y = 0.0f;
    field_4F8.prev_rot_y = 0.0f;
    field_4F8.field_90 = 1.0f;
    field_4F8.field_9C = 1.0f;
    field_4F8.field_A8 = 1.0f;
    field_4F8.field_BC = 0;
    field_4F8.field_BD = 0;
    field_4F8.field_C0 = 1.0f;
    field_4F8.field_C4 = 1.0f;
    field_4F8.field_C8 = 0.0f;
}

void motion_blend_mot::set_step(float_t step) {
    mot_play_data.frame_data.step_prev = mot_play_data.frame_data.step;
    mot_play_data.frame_data.step = step;
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

struc_241::struc_241() : field_0(), field_4(), field_8(), field_C(), field_10(), field_14(),
field_18(), field_1C(), field_2C(), field_30(), field_34(), field_38(), field_3C(), field_40() {

}

struc_241::struc_241(float_t field_0, float_t field_4, float_t field_8, float_t field_C, float_t field_10,
    float_t field_14, float_t field_18, float_t field_1C, vec3 field_20, float_t field_2C,
    float_t field_30, float_t field_34, float_t field_38, float_t field_3C, float_t field_40) : field_0(field_0),
    field_4(field_4), field_8(field_8), field_C(field_C), field_10(field_10), field_14(field_14),
    field_18(field_18), field_1C(field_1C), field_2C(field_2C), field_30(field_30),
    field_34(field_34), field_38(field_38), field_3C(field_3C), field_40(field_40) {

}

struc_258::struc_258() : field_0(), field_8C(), field_8D(), field_8E(), field_8F(), field_90(),
field_91(), field_94(), field_98(), field_9C(), field_A0(), field_A4(), field_A8(), field_AC(),
field_B0(), field_14C(), field_150(), field_154(), field_158(), field_15C(), field_190(), field_191(),
field_192(), field_193(), field_194(), field_195(), field_198(), field_19C(), field_1A0(), field_1A4(),
field_1A8(), field_1AC(), field_1B0(), field_1B4(), field_1B8(), field_1C4(), field_1C8(), field_1CC() {

}

rob_chara_bone_data_sleeve_adjust::rob_chara_bone_data_sleeve_adjust() : sleeve_l(), sleeve_r(),
enable1(), enable2(), field_5C(), field_68(), field_74(), field_80(), radius(), bones(), step() {

}

void rob_chara_bone_data_sleeve_adjust::reset() {
    sleeve_l = {};
    sleeve_r = {};
    enable1 = false;
    enable2 = false;
    radius = 1.0f;
    bones = 0;
    step = 1.0f;
    field_5C = 0.0f;
    field_68 = 0.0f;
    field_74 = 0.0f;
    field_80 = 0.0f;
}

rob_chara_bone_data::rob_chara_bone_data() : field_0(), field_1(), object_bone_count(),
total_bone_count(), motion_bone_count(), ik_bone_count(), chain_pos(),
disable_eye_motion(), field_76C(), field_784(), field_788() {
    base_skeleton_type = BONE_DATABASE_SKELETON_NONE;
    skeleton_type = BONE_DATABASE_SKELETON_NONE;
}

rob_chara_bone_data::~rob_chara_bone_data() {
    for (motion_blend_mot*& i : motions) {
        delete i;
        i = 0;
    }
}

float_t rob_chara_bone_data::get_frame() {
    return motion_loaded.front()->mot_play_data.frame_data.frame;
}

float_t rob_chara_bone_data::get_frame_count() {
    return (float_t)motion_loaded.front()->mot_key_data.mot.frame_count;
}

bool rob_chara_bone_data::get_motion_has_looped() {
    return motion_loaded.front()->mot_play_data.frame_data.looped;
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
    field_76C[0] = 0.0f;
    field_76C[0] = 0.0f;
    sub_1404198D0(&field_788);
    sleeve_adjust.reset();
}

void rob_chara_bone_data::interpolate() {
    if (motion_loaded.size() < 0)
        return;

    sub_14041ABA0(this);
    sub_14041DAC0(this);

    for (motion_blend_mot*& i : motion_loaded)
        motion_blend_mot_interpolate(i);

    bone_database_skeleton_type skeleton_type = base_skeleton_type;
    motion_blend_mot* v5 = motion_loaded.front();
    std::vector<bone_data>* bones = &v5->bone_data.bones;
    std::vector<uint16_t>* bone_indices = &v5->bone_data.bone_indices;
    mot_blend_interpolate(&face, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&hand_l, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&hand_r, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&mouth, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&eyes, bones, bone_indices, skeleton_type);
    mot_blend_interpolate(&eyelid, bones, bone_indices, skeleton_type);
}

void rob_chara_bone_data::load_eyelid_motion(uint32_t motion_id, const motion_database* mot_db) {
    sub_14041AD50(this);
    sub_140412F20(&eyelid, &motion_loaded.front()->bone_data.bones);
    sub_1404146F0(&eyelid.field_0);
    mot_blend_load_motion(&eyelid, motion_id, mot_db);
}

void rob_chara_bone_data::load_eyes_motion(uint32_t motion_id, const motion_database* mot_db) {
    sub_140412F20(&eyes, &motion_loaded.front()->bone_data.bones);
    mot_blend_load_motion(&eyes, motion_id, mot_db);
}

void rob_chara_bone_data::load_face_motion(uint32_t motion_id, const motion_database* mot_db) {
    sub_14041AD90(this);
    sub_140412F20(&face, &motion_loaded.front()->bone_data.bones);
    sub_1404146F0(&face.field_0);
    mot_blend_load_motion(&face, motion_id, mot_db);
}

void rob_chara_bone_data::load_hand_l_motion(uint32_t motion_id, const motion_database* mot_db) {
    sub_140412F20(&hand_l, &motion_loaded.front()->bone_data.bones);
    mot_blend_load_motion(&hand_l, motion_id, mot_db);
}

void rob_chara_bone_data::load_hand_r_motion(uint32_t motion_id, const motion_database* mot_db) {
    sub_140412F20(&hand_r, &motion_loaded.front()->bone_data.bones);
    mot_blend_load_motion(&hand_r, motion_id, mot_db);
}

void rob_chara_bone_data::load_mouth_motion(uint32_t motion_id, const motion_database* mot_db) {
    sub_140412F20(&mouth, &motion_loaded.front()->bone_data.bones);
    mot_blend_load_motion(&mouth, motion_id, mot_db);
}

void rob_chara_bone_data::set_disable_eye_motion(bool value) {
    disable_eye_motion = value;
}

void rob_chara_bone_data::set_frame(float_t frame) {
    motion_loaded.front()->mot_play_data.frame_data.set_frame(frame);
}

void rob_chara_bone_data::set_motion_frame(float_t frame, float_t step, float_t frame_count) {
    mot_play_data* v5 = &motion_loaded.front()->mot_play_data;
    v5->frame_data.frame = frame;
    v5->frame_data.step_prev = v5->frame_data.step;
    v5->frame_data.step = step;
    v5->frame_data.frame_count = frame_count;
    v5->frame_data.last_frame = frame_count - 1.0f;
    v5->frame_data.max_frame = -1.0f;
    v5->frame_data.playback_state = MOT_PLAY_FRAME_DATA_PLAYBACK_NONE;
    v5->frame_data.loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
    v5->frame_data.loop_begin = 0.0f;
    v5->frame_data.loop_end = -1.0f;
    v5->frame_data.looped = 0;
    v5->frame_data.loop_count = -1;
    v5->loop_index = 0;
    v5->loop_frames_enabled = false;
    v5->loop_frames = 0.0f;
    v5->ext_frame = 0;
    v5->ext_step = 0;
}

void rob_chara_bone_data::set_motion_loop(float_t loop_begin, int32_t loop_count, float_t loop_end) {
    motion_blend_mot* v4 = motion_loaded.front();
    v4->mot_play_data.frame_data.loop_begin = loop_begin;
    v4->mot_play_data.frame_data.loop_end = loop_end;
    v4->mot_play_data.frame_data.loop_count = loop_count;
}

void rob_chara_bone_data::set_motion_loop_state(mot_play_frame_data_loop_state value) {
    motion_loaded.front()->mot_play_data.frame_data.loop_state = value;
}

void rob_chara_bone_data::set_motion_max_frame(float_t value) {
    motion_loaded.front()->mot_play_data.frame_data.max_frame = value;
}

void rob_chara_bone_data::set_motion_playback_state(mot_play_frame_data_playback_state value) {
    motion_loaded.front()->mot_play_data.frame_data.playback_state = value;
}

void rob_chara_bone_data::update(mat4* mat) {
    if (!motion_loaded.size())
        return;

    for (motion_blend_mot*& i : motion_loaded) {
        sub_1404117F0(i);
        sub_14040FBF0(i, ik_scale.ratio0);
    }

    sub_1404182B0(this);
    sub_14041B9F0(this);
    motion_blend_mot_mult_mat(motion_loaded.front(), mat);
}

static void sub_1404156B0(mot_play_data* a1) {
    mot_play_frame_data* frame_data = &a1->frame_data;
    frame_data->looped = false;
    if (a1->loop_frames_enabled && a1->loop_frames >= 0.0f)
        a1->loop_frames -= frame_data->step;

    switch (frame_data->playback_state) {
    case MOT_PLAY_FRAME_DATA_PLAYBACK_FORWARD: {
        frame_data->frame += frame_data->step;
        if (frame_data->max_frame >= 0.0f && frame_data->frame > frame_data->max_frame)
            frame_data->frame = frame_data->max_frame;

        float_t last_frame = frame_data->last_frame;
        switch (frame_data->loop_state) {
        case MOT_PLAY_FRAME_DATA_LOOP_ONCE:
        default:
            frame_data->frame = min_def(frame_data->frame, frame_data->frame_count);
            break;
        case MOT_PLAY_FRAME_DATA_LOOP_CONTINUOUS: {
            if (frame_data->loop_end >= 0.0f && frame_data->loop_end <= last_frame)
                last_frame = frame_data->loop_end;

            if (frame_data->frame < last_frame)
                break;

            if (frame_data->loop_count > 0 && a1->loop_index >= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            float_t frame = frame_data->frame - last_frame;
            float_t loop_range = last_frame - frame_data->loop_begin;
            if (frame >= loop_range && loop_range > 0.0f)
                frame = fmodf(frame, loop_range);
            frame_data->frame = frame + frame_data->loop_begin;
            frame_data->looped = true;
            a1->loop_index++;
        } break;
        case MOT_PLAY_FRAME_DATA_LOOP_RESET: {
            if (frame_data->loop_end >= 0.0f && frame_data->loop_end <= last_frame)
                last_frame = frame_data->loop_end;

            if (last_frame >= frame_data->frame)
                break;

            if (frame_data->loop_count > 0 && a1->loop_index >= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            frame_data->frame = frame_data->loop_begin;
            frame_data->looped = true;
            a1->loop_index++;
        } break;
        case MOT_PLAY_FRAME_DATA_LOOP_REVERSE: {
            if (frame_data->frame <= last_frame)
                return;

            if (frame_data->loop_count > 0 && a1->loop_index <= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            float_t frame = frame_data->frame - last_frame;
            if (frame >= last_frame && last_frame > 0.0f)
                frame = fmodf(frame, last_frame);
            frame_data->frame = last_frame - frame;
            frame_data->playback_state = MOT_PLAY_FRAME_DATA_PLAYBACK_BACKWARD;
            frame_data->looped = true;
            a1->loop_index++;
        } break;
        }
    } break;
    case MOT_PLAY_FRAME_DATA_PLAYBACK_BACKWARD: {
        float_t last_frame = frame_data->last_frame;
        frame_data->frame -= frame_data->step;
        switch (frame_data->loop_state) {
        case MOT_PLAY_FRAME_DATA_LOOP_ONCE:
        default:
            frame_data->frame = max_def(frame_data->frame, 0.0f);
            break;
        case MOT_PLAY_FRAME_DATA_LOOP_CONTINUOUS: {
            float_t loop_begin = 0.0f;
            if (frame_data->loop_begin >= 0.0f)
                loop_begin = frame_data->loop_begin;

            if (frame_data->frame > loop_begin)
                break;

            if (frame_data->loop_count > 0 && a1->loop_index >= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            if (frame_data->loop_end >= 0.0f && frame_data->loop_end <= last_frame)
                last_frame = frame_data->loop_end;

            float_t loop_range = last_frame - loop_begin;
            float_t frame = loop_begin - frame_data->frame;
            if (frame >= loop_range && loop_range > 0.0f)
                frame = fmodf(frame, loop_range);
            frame_data->frame = last_frame - frame;
            frame_data->looped = true;
            a1->loop_index++;
        } break;
        case MOT_PLAY_FRAME_DATA_LOOP_RESET: {
            float_t loop_begin = 0.0f;
            if (frame_data->loop_begin >= 0.0f)
                loop_begin = frame_data->loop_begin;

            if (frame_data->frame >= loop_begin)
                break;

            if (frame_data->loop_count > 0 && a1->loop_index >= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            if (frame_data->loop_end >= 0.0f && frame_data->loop_end <= last_frame)
                last_frame = frame_data->loop_end;
            frame_data->frame = last_frame;
            frame_data->looped = true;
            a1->loop_index++;
        } break;
        case MOT_PLAY_FRAME_DATA_LOOP_REVERSE: {
            if (frame_data->frame >= 0.0f)
                return;

            if (frame_data->loop_count > 0 && a1->loop_index >= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            float_t frame = -frame_data->frame;
            if (last_frame > 0.0f && frame >= last_frame)
                frame = fmodf(frame, last_frame);
            frame_data->frame = frame;
            frame_data->playback_state = MOT_PLAY_FRAME_DATA_PLAYBACK_FORWARD;
            frame_data->looped = true;
            a1->loop_index++;
        } break;
        }
    } break;
    case MOT_PLAY_FRAME_DATA_PLAYBACK_EXTERNAL: {
        if (a1->ext_frame)
            frame_data->frame = *a1->ext_frame;
        if (a1->ext_step)
            frame_data->step = *a1->ext_step;
    } break;
    }
}

void rob_chara_bone_data::sub_14041DBA0() {
    for (motion_blend_mot*& i : motion_loaded)
        sub_1404156B0(&i->mot_play_data);
}

rob_chara_pv_data::rob_chara_pv_data() : type(), field_4(), field_5(),
field_6(), rot_y_int16(), field_16(), sleeve_l(), sleeve_r(), field_70(),
motion_face_ids(), chara_size_index(), height_adjust(), item() {
    reset();
}

void rob_chara_pv_data::reset() {
    type = ROB_CHARA_TYPE_2;
    field_4 = true;
    field_5 = false;
    field_6 = false;
    field_8 = 0.0f;
    rot_y_int16 = 0;
    field_16 = 0xC9;
    sleeve_l = {};
    sleeve_r = {};
    field_70 = 0;
    for (uint32_t& i : motion_face_ids)
        i = -1;
    chara_size_index = 1;
    height_adjust = false;
    item = {};
    eyes_adjust = {};
}

rob_chara_item_equip_object::rob_chara_item_equip_object() : index(), mats(),
obj_info(), field_14(), texture_data(), null_blocks_data_set(), alpha(),
obj_flags(), can_disp(), field_A4(), mat(), osage_iterations(), bone_nodes(),
field_138(), field_1B8(), osage_nodes_count(), use_opd(), skin_ex_data(), skin(), item_equip() {
    init_members(0x12345678);
}

rob_chara_item_equip_object::~rob_chara_item_equip_object() {
    init_members(0xDDDDDDDD);
}

void rob_chara_item_equip_object::add_motion_reset_data(
    uint32_t motion_id, float_t frame, int32_t osage_iterations) {
    if (osage_iterations > 0)
        rob_chara_item_equip_object_ctrl_iterate_nodes(this, osage_iterations);

    for (ExOsageBlock*& i : osage_blocks)
        i->AddMotionResetData(motion_id, frame);

    for (ExClothBlock*& i : cloth_blocks)
        i->AddMotionResetData(motion_id, frame);
}

void rob_chara_item_equip_object::check_no_opd(std::vector<opd_blend_data>& opd_blend_data) {
    use_opd = true;
    if (!osage_blocks.size() && !cloth_blocks.size() || !opd_blend_data.size())
        return;

    for (::opd_blend_data& i : opd_blend_data) {
        const float_t* opd_data = 0;
        uint32_t opd_count = 0;
        osage_play_data_manager_get_opd_file_data(obj_info, i.motion_id, opd_data, opd_count);
        if (!opd_data) {
            use_opd = false;
            break;
        }
    }
}

void rob_chara_item_equip_object::clear_ex_data() {
    for (ExNullBlock*& i : null_blocks) {
        i->Reset();
        delete i;
    }
    null_blocks.clear();

    for (ExOsageBlock*& i : osage_blocks) {
        i->Reset();
        delete i;
    }
    osage_blocks.clear();

    for (ExConstraintBlock*& i : constraint_blocks) {
        i->Reset();
        delete i;
    }
    constraint_blocks.clear();

    for (ExExpressionBlock*& i : expression_blocks) {
        i->Reset();
        delete i;
    }
    expression_blocks.clear();

    for (ExClothBlock*& i : cloth_blocks) {
        i->Reset();
        delete i;
    }
    cloth_blocks.clear();

    node_blocks.clear();
}

void rob_chara_item_equip_object::disp(const mat4* mat, render_context* rctx) {
    if (obj_info.is_null())
        return;

    mdl::ObjFlags v2 = rctx->disp_manager.get_obj_flags();
    mdl::ObjFlags v4 = v2;
    if (fabsf(alpha - 1.0f) > 0.000001f)
        enum_or(v4, obj_flags);
    else
        enum_and(v4, ~(mdl::OBJ_ALPHA_ORDER_3 | mdl::OBJ_ALPHA_ORDER_2 | mdl::OBJ_ALPHA_ORDER_1));
    rctx->disp_manager.set_obj_flags(v4);
    if (can_disp) {
        rctx->disp_manager.entry_obj_by_object_info_object_skin(obj_info,
            &texture_pattern, &texture_data, alpha, mats, ex_data_bone_mats.data(), 0, mat);

        for (ExNodeBlock*& i : node_blocks)
            i->Disp(mat, rctx);
    }
    rctx->disp_manager.set_obj_flags(v2);
}

int32_t rob_chara_item_equip_object::get_bone_index(const char* name, const bone_database* bone_data) {
    int32_t bone_index = bone_data->get_skeleton_motion_bone_index(
        bone_database_skeleton_type_to_string(BONE_DATABASE_SKELETON_COMMON), name);
    if (bone_index == -1)
        for (auto& i : ex_bones)
            if (!str_utils_compare(name, i.first))
                return 0x8000 | i.second;
    return bone_index;
}

bone_node* rob_chara_item_equip_object::get_bone_node(
    int32_t bone_index) {
    if (!(bone_index & 0x8000))
        return &bone_nodes[bone_index & 0x7FFF];
    else if ((bone_index & 0x7FFF) < ex_data_bone_nodes.size())
        return &ex_data_bone_nodes[bone_index & 0x7FFF];
    return 0;
}

bone_node* rob_chara_item_equip_object::get_bone_node(const char* name, const bone_database* bone_data) {
    return get_bone_node(get_bone_index(name, bone_data));
}

const mat4* rob_chara_item_equip_object::get_ex_data_bone_node_mat(const char* name) {
    if (!name || !ex_data_bone_nodes.size())
        return &mat4_identity;

    for (bone_node& i : ex_data_bone_nodes)
        if (!str_utils_compare(i.name, name))
            return i.mat;

    return &mat4_identity;
}

RobOsageNode* rob_chara_item_equip_object::get_normal_ref_osage_node(const std::string& name, size_t* index) {
    if (!name.size())
        return 0;

    size_t v8 = name.find(',', 0);
    if (v8 == -1)
        return 0;

    std::string v22 = name.substr(0, v8);
    std::string v23 = name.substr(v8 + 1);
    size_t v11 = atoi(v23.c_str());
    if (index)
        *index = v11;

    RobOsageNode* node = 0;
    for (ExOsageBlock*& i : osage_blocks)
        if (!v22.compare(i->name)) {
            if (v11 + 1 < i->rob.nodes.size())
                node = i->rob.GetNode(v11 + 1);
            break;
        }
    return node;
}

void rob_chara_item_equip_object::get_parent_bone_nodes(bone_node* bone_nodes, const bone_database* bone_data) {
    this->bone_nodes = bone_nodes;
    mats = bone_nodes->mat;
    for (ExNodeBlock*& i : node_blocks)
        i->parent_bone_node = get_bone_node(i->parent_name.c_str(), bone_data);
}

void rob_chara_item_equip_object::init_ex_data_bone_nodes(obj_skin_ex_data* ex_data) {
    ex_data_bone_nodes.clear();
    ex_data_bone_mats.clear();
    ex_data_mats.clear();

    uint32_t num_bone_name = ex_data->num_bone_name;
    ex_data_bone_nodes.resize(num_bone_name);
    ex_data_bone_mats.resize(num_bone_name);
    ex_data_mats.resize(num_bone_name);

    bone_node* bone_nodes = ex_data_bone_nodes.data();
    mat4* bone_mats = ex_data_bone_mats.data();
    mat4* mats = ex_data_mats.data();
    for (uint32_t i = 0; i < num_bone_name; i++)
        bone_nodes[i].mat = &bone_mats[i];

    if (ex_data->bone_name_array) {
        ex_bones.clear();
        const char** bone_name_array = ex_data->bone_name_array;
        for (uint32_t i = 0; i < num_bone_name; i++) {
            bone_nodes[i].set_name_mat_ex_data_mat(bone_name_array[i], &bone_mats[i], &mats[i]);
            ex_bones.push_back(bone_name_array[i], i);
        }
    }
}

void rob_chara_item_equip_object::init_members(size_t index) {
    this->index = index;
    obj_info = {};
    field_14 = -1;
    mats = 0;
    texture_pattern.clear();
    texture_data.field_0 = -1;
    texture_data.texture_color_coefficients = 1.0f;
    texture_data.texture_color_offset = 0.0f;
    texture_data.texture_specular_coefficients = 1.0f;
    texture_data.texture_specular_offset = 0.0f;
    alpha = 1.0f;
    obj_flags = mdl::OBJ_ALPHA_ORDER_1;
    null_blocks_data_set = 0;
    can_disp = true;
    field_A4 = 0;
    mat = 0;
    bone_nodes = 0;
    clear_ex_data();
    ex_data_bone_nodes.clear();
    ex_data_bone_mats.clear();
    ex_data_mats.clear();
    ex_bones.clear();
    field_1B8 = false;
    use_opd = false;
    osage_nodes_count = 0;
}

void rob_chara_item_equip_object::load_ex_data(obj_skin_ex_data* ex_data,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!ex_data->block_array)
        return;

    prj::vector_pair<uint32_t, RobOsageNode*> v129;
    std::map<const char*, ExNodeBlock*> v138;
    clear_ex_data();

    size_t constraint_count = 0;
    size_t expression_count = 0;
    size_t osage_count = 0;
    size_t cloth_count = 0;
    size_t null_count = 0;
    const char** bone_name_array = ex_data->bone_name_array;
    obj_skin_block* block = ex_data->block_array;
    uint32_t num_block = ex_data->num_block;
    for (uint32_t i = 0; i < num_block; i++, block++) {
        ExNodeBlock* ex_node;
        switch (block->type) {
        case OBJ_SKIN_BLOCK_CLOTH: {
            if (cloth_count >= 0x08)
                continue;

            ExClothBlock* cls = new ExClothBlock;
            ex_node = cls;
            cloth_blocks.push_back(cls);
            cls->InitData(this, block->cloth, 0, bone_data);
            cls->index = cloth_count + osage_count;
            cls->name = block->cloth->mesh_name;
            cloth_count++;
        } break;
        case OBJ_SKIN_BLOCK_CONSTRAINT: {
            if (constraint_count >= 0x40)
                continue;

            ExConstraintBlock* cns = new ExConstraintBlock;
            ex_node = cns;
            constraint_blocks.push_back(cns);
            cns->InitData(this, block->constraint,
                bone_name_array[block->constraint->name_index & 0x7FFF], bone_data);
            constraint_count++;
        } break;
        case OBJ_SKIN_BLOCK_EXPRESSION: {
            if (expression_count >= 0x50)
                continue;

            ExExpressionBlock* exp = new ExExpressionBlock;
            ex_node = exp;
            expression_blocks.push_back(exp);
            exp->InitData(this, block->expression,
                bone_name_array[block->expression->name_index & 0x7FFF], obj_info, index, bone_data);
            expression_count++;
        } break;
        case OBJ_SKIN_BLOCK_OSAGE: {
            if (osage_count >= 0x100)
                continue;

            ExOsageBlock* osg = new ExOsageBlock;
            ex_node = osg;
            osage_blocks.push_back(osg);
            osg->InitData(this, block->osage,
                bone_name_array[block->osage->external_name_index & 0x7FFF],
                &skin_ex_data->osage_node_array[block->osage->start_index],
                bone_nodes, ex_data_bone_nodes.data(), skin);
            osg->sub_1405F3E10(block->osage,
                &skin_ex_data->osage_node_array[block->osage->start_index], &v129, &v138);
            osg->index = osage_count;
            osage_count++;
        } break;
        default: {
            if (null_count >= 0x4)
                continue;

            ExNullBlock* null = new ExNullBlock;
            ex_node = null;
            null_blocks.push_back(null);
            null->InitData(this, block->constraint,
                bone_name_array[block->constraint->name_index & 0x7FFF], bone_data);
            null_count++;
        } break;
        }

        obj_skin_block_node* node = block->node;
        bone_node_expression_data exp_data;
        exp_data.position = node->position;
        exp_data.rotation = node->rotation;
        exp_data.scale = node->scale;
        exp_data.parent_scale = 1.0f;
        ex_node->parent_name = node->parent_name;

        bone_node* parent_bone_node = get_bone_node(node->parent_name, bone_data);
        ex_node->parent_bone_node = parent_bone_node;
        if (ex_node->bone_node_ptr) {
            ex_node->bone_node_ptr->exp_data = exp_data;
            ex_node->bone_node_ptr->parent = parent_bone_node;
        }

        if (parent_bone_node) {
            auto elem = v138.find(parent_bone_node->name);
            if (elem != v138.end())
                ex_node->parent_node = elem->second;
        }

        v138.insert({ ex_node->name, ex_node });
        node_blocks.push_back(ex_node);
    }

    for (ExNodeBlock*& i : node_blocks) {
        ExNodeBlock* parent_node = i->parent_node;
        if (parent_node) {
            parent_node->has_children_node = true;
            if ((parent_node->type & ~0x03) || parent_node->type == EX_OSAGE
                || !parent_node->field_58)
                continue;
        }
        i->field_58 = true;
    }

    for (ExOsageBlock*& i : osage_blocks) {
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
            osg->rob.root_matrix_ptr = parent_bone_node->ex_data_mat;
            osg->rob.root_matrix = *parent_bone_node->ex_data_mat;
        }
    }

    if (ex_data->osage_sibling_info_array) {
        obj_skin_osage_sibling_info* sibling_info = ex_data->osage_sibling_info_array;
        for (uint32_t i = 0; i < ex_data->num_osage_sibling_info; i++, sibling_info++) {
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
                RobOsageNode* v102 = node->second;
                v102->sibling_node = sibling_node->second;
                v102->max_distance = sibling_info->max_distance;
            }
        }
    }

    if (osage_count || cloth_count)
        skp_load_file(data, bone_data, obj_db);

    size_t osage_nodes_count = 0;
    for (ExOsageBlock*& i : osage_blocks)
        osage_nodes_count += i->rob.nodes.size() - 1;
    for (ExClothBlock*& i : cloth_blocks)
        osage_nodes_count += i->rob.nodes.size() - i->rob.root_count;
    this->osage_nodes_count = osage_nodes_count;
}

void rob_chara_item_equip_object::load_object_info_ex_data(object_info obj_info, bone_node* bone_nodes,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    this->obj_info = obj_info;
    this->bone_nodes = bone_nodes;
    mats = bone_nodes->mat;
    ex_data_bone_nodes.clear();
    ex_bones.clear();
    ex_data_bone_mats.clear();
    clear_ex_data();
    obj_skin* skin = object_storage_get_obj_skin(this->obj_info);
    if (!skin)
        return;

    this->skin = skin;
    if (!skin->ex_data)
        return;

    this->skin_ex_data = skin->ex_data;
    init_ex_data_bone_nodes(skin->ex_data);
    load_ex_data(skin->ex_data, bone_data, data, obj_db);

    if (osage_reset && osage_blocks.size())
        osage_iterations = 60;
}

void rob_chara_item_equip::load_outfit_object_info(item_id id, object_info obj_info,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (id <= ITEM_BODY && id >= ITEM_KAMI)
        obj_info = {};
    load_object_info(obj_info, id, osage_reset, bone_data, data, obj_db);
}

void rob_chara_item_equip_object::reset_external_force() {
    for (ExOsageBlock*& i : osage_blocks)
        i->rob.ResetExtrenalForce();
    for (ExClothBlock*& i : cloth_blocks)
        i->rob.ResetExtrenalForce();
}

void rob_chara_item_equip_object::reset_nodes_external_force(rob_osage_parts_bit parts_bits) {
    for (ExOsageBlock*& i : osage_blocks)
        if (i->rob.CheckPartsBits(parts_bits)) {
            i->rob.SetNodesExternalForce(0, 1.0f);
            i->rob.SetNodesForce(1.0f);
        }
}

void rob_chara_item_equip_object::set_alpha_obj_flags(float_t alpha, int32_t flags) {
    obj_flags = (mdl::ObjFlags)flags;
    this->alpha = clamp_def(alpha, 0.0f, 1.0f);
}

bool rob_chara_item_equip_object::set_boc(
    const skin_param_osage_root& skp_root, ExOsageBlock* osg) {
    RobOsage* rob_osg = &osg->rob;
    RobOsageNode* i_begin = rob_osg->nodes.data() + 1;
    RobOsageNode* i_end = rob_osg->nodes.data() + rob_osg->nodes.size();
    for (RobOsageNode* i = i_begin; i != i_end; i++)
        i->data_ptr->boc.clear();

    bool has_boc_node = false;
    for (const skin_param_osage_root_boc& i : skp_root.boc) {
        for (ExOsageBlock*& j : osage_blocks) {
            if (i.ed_root.compare(j->name)
                || i.ed_node + 1ULL >= rob_osg->nodes.size()
                || i.st_node + 1ULL >= j->rob.nodes.size())
                continue;

            RobOsageNode* ed_node = rob_osg->GetNode(i.ed_node + 1ULL);
            RobOsageNode* st_node = j->rob.GetNode(i.st_node + 1ULL);
            ed_node->data_ptr->boc.push_back(st_node);
            has_boc_node = true;
            break;
        }
    }
    return has_boc_node;
}

void rob_chara_item_equip_object::set_collision_target_osage(
    const skin_param_osage_root& skp_root, skin_param* skp) {
    if (!skp_root.colli_tgt_osg.size())
        return;

    const char* colli_tgt_osg = skp_root.colli_tgt_osg.c_str();
    for (ExOsageBlock*& i : osage_blocks)
        if (!str_utils_compare(colli_tgt_osg, i->name)) {
            skp->colli_tgt_osg = &i->rob.nodes;
            break;
        }
}

void rob_chara_item_equip_object::set_motion_reset_data(uint32_t motion_id, float_t frame) {
    for (ExOsageBlock*& i : osage_blocks)
        i->SetMotionResetData(motion_id, frame);

    for (ExClothBlock*& i : cloth_blocks)
        i->SetMotionResetData(motion_id, frame);
}

void rob_chara_item_equip_object::set_motion_skin_param(int8_t chara_id, uint32_t motion_id, int32_t frame) {
    if (obj_info.is_null())
        return;

    std::vector<skin_param_file_data>* skp_file_data
        = skin_param_manager_get_skin_param_file_data(chara_id, obj_info, motion_id, frame);
    //if (!skp_file_data)
        return;

    field_1B8 = false;
    skin_param_file_data* j = skp_file_data->data();
    for (ExOsageBlock*& i : osage_blocks) {
        field_1B8 |= j->field_88;
        i->SetSkinParam(j++);
    }

    for (ExClothBlock*& i : cloth_blocks) {
        field_1B8 |= j->field_88;
        i->SetSkinParam(j++);
    }
}

void rob_chara_item_equip_object::set_null_blocks_expression_data(
    const vec3& position, const vec3& rotation, const vec3& scale) {
    const vec3 pos = position;
    const vec3 rot = rotation * DEG_TO_RAD_FLOAT;
    const vec3 sc = scale;

    null_blocks_data_set = true;
    for (ExNullBlock*& i : null_blocks) {
        if (!i || !i->bone_node_ptr)
            continue;

        bone_node_expression_data* node_exp_data = &i->bone_node_ptr->exp_data;
        node_exp_data->position = pos;
        node_exp_data->rotation = rot;
        node_exp_data->scale = sc;
    }
}

void rob_chara_item_equip_object::set_osage_play_data_init(const float_t* opdi_data) {
    for (ExOsageBlock*& i : osage_blocks)
        opdi_data = i->SetOsagePlayDataInit(opdi_data);

    for (ExClothBlock*& i : cloth_blocks)
        opdi_data = i->SetOsagePlayDataInit(opdi_data);
}

void rob_chara_item_equip_object::set_osage_reset() {
    for (ExOsageBlock*& i : osage_blocks)
        i->SetOsageReset();

    for (ExClothBlock*& i : cloth_blocks)
        i->SetOsageReset();
}

void rob_chara_item_equip_object::set_osage_move_cancel(float_t value) {
    for (ExOsageBlock*& i : osage_blocks)
        i->rob.move_cancel = value;
    for (ExClothBlock*& i : cloth_blocks)
        i->rob.move_cancel = value;
}

void rob_chara_item_equip_object::set_texture_pattern(texture_pattern_struct* tex_pat, size_t count) {
    texture_pattern.clear();
    if (count && tex_pat)
        for (size_t i = 0; i < count; i++)
            texture_pattern.push_back(tex_pat[i]);
}

void rob_chara_item_equip_object::skp_load(void* kv, const bone_database* bone_data) {
    key_val* _kv = (key_val*)kv;
    if (_kv->key.size() < 1)
        return;

    field_1B8 = false;
    for (ExOsageBlock*& i : osage_blocks) {
        ExOsageBlock* osg = i;
        skin_param_osage_root root;
        osg->rob.LoadSkinParam(_kv, osg->name, root, &obj_info, bone_data);
        set_collision_target_osage(root, osg->rob.skin_param_ptr);
        field_1B8 |= set_boc(root, osg);
        field_1B8 |= root.coli_type != 0;
        field_1B8 |= skp_load_normal_ref(root, 0);
    }

    for (ExClothBlock*& i : cloth_blocks) {
        ExClothBlock* cls = i;
        cls->rob.LoadSkinParam(_kv, cls->name, bone_data);
    }
}

void rob_chara_item_equip_object::skp_load(const skin_param_osage_root& skp_root,
    std::vector<skin_param_osage_node>& vec, skin_param_file_data* skp_file_data, const bone_database* bone_data) {
    set_collision_target_osage(skp_root, &skp_file_data->skin_param);
    skp_file_data->field_88 |= skp_file_data->skin_param.coli_type > 0;

    skin_param_osage_node* j = vec.data();
    size_t k = 0;
    for (RobOsageNodeData& i : skp_file_data->nodes_data)
        i.SetForce(skp_root, j++, k++);

    skp_file_data->field_88 |= skp_load_boc(skp_root, &skp_file_data->nodes_data);
    skp_file_data->field_88 |= skp_load_normal_ref(skp_root, &skp_file_data->nodes_data);
}

bool rob_chara_item_equip_object::skp_load_boc(
    const skin_param_osage_root& skp_root, std::vector<RobOsageNodeData>* node_data) {
    bool has_boc_node = false;
    for (const skin_param_osage_root_boc& i : skp_root.boc)
        for (ExOsageBlock*& j : osage_blocks)
            if (i.ed_root.compare(j->name)
                && i.ed_node >= 0 && i.ed_node < node_data->size()
                && i.st_node >= 0 && i.st_node < j->rob.nodes.size()) {
                node_data->data()[i.ed_node].boc.push_back(j->rob.GetNode(i.st_node + 1ULL));
                has_boc_node = true;
                break;
            }
    return has_boc_node;
}

void rob_chara_item_equip_object::skp_load_file(void* data,
    const bone_database* bone_data, const object_database* obj_db) {
    key_val* kv_ptr = skin_param_storage_get_key_val(obj_info);
    if (kv_ptr) {
        skp_load(kv_ptr, bone_data);
        return;
    }

    char buf[0x200];
    const char* name = obj_db->get_object_name(obj_info);
    sprintf_s(buf, sizeof(buf), "ext_skp_%s.txt", name);

    for (int32_t i = 0; buf[i]; i++) {
        char c = buf[i];
        if (c >= 'A' && c <= 'Z')
            c += 0x20;
        buf[i] = c;
    }

    key_val kv;
    ((data_struct*)data)->load_file(&kv, "rom/skin_param/", buf, key_val::load_file);
    skp_load(&kv, bone_data);
}

bool rob_chara_item_equip_object::skp_load_normal_ref(
    const skin_param_osage_root& skp_root, std::vector<RobOsageNodeData>* node_data) {
    if (!skp_root.normal_ref.size())
        return false;

    for (const skin_param_osage_root_normal_ref& i : skp_root.normal_ref) {
        size_t v12 = 0;
        RobOsageNode* v8 = get_normal_ref_osage_node(i.n, &v12);
        if (!v8)
            continue;

        RobOsageNodeData* v10;
        if (node_data)
            v10 = &(*node_data)[v12];
        else
            v10 = v8->data_ptr;

        v10->normal_ref.n = v8;
        v10->normal_ref.u = get_normal_ref_osage_node(i.u, 0);
        v10->normal_ref.d = get_normal_ref_osage_node(i.d, 0);
        v10->normal_ref.l = get_normal_ref_osage_node(i.l, 0);
        v10->normal_ref.r = get_normal_ref_osage_node(i.r, 0);
        v10->normal_ref.GetMat();
    }
    return true;
}

rob_chara_item_equip::rob_chara_item_equip() : bone_nodes(), matrices(), item_equip_object(), field_18(),
item_equip_range(), first_item_equip_object(), max_item_equip_object(), field_A0(), shadow_type(), position(),
field_B4(), field_D4(), disable_update(), field_DC(), texture_color_coefficients(), wet(), wind_strength(),
chara_color(), npr_flag(), mat(), field_13C(), field_8BC(), field_8C0(), field_8C4(), field_8C8(), field_8CC(),
field_8D0(), field_8D4(), field_8D8(), field_8DC(), field_8E0(), field_8E4(), field_8E8(), field_8EC(),
field_8F0(), field_8F4(), field_8F8(), field_8FC(), field_900(), field_908(), field_910(), field_918(),
field_920(), field_928(), field_930(), use_opd(), parts_short(), parts_append(), parts_white_one_l() {
    item_equip_object = new rob_chara_item_equip_object[ITEM_MAX];
    wind_strength = 1.0f;
    chara_color = true;
    osage_step = 1.0f;

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

void rob_chara_item_equip::add_motion_reset_data(uint32_t motion_id, float_t frame, int32_t iterations) {
    for (int32_t i = first_item_equip_object; i < max_item_equip_object; i++)
        item_equip_object[i].add_motion_reset_data(motion_id, frame, iterations);
}

static void sub_140512C20(rob_chara_item_equip* rob_itm_equip, render_context* rctx) {
    if (rob_itm_equip->field_D0.is_null() || rob_itm_equip->field_D4 == ITEM_NONE)
        return;

    mat4 mat = mat4_identity;
    const char* name;
    if (rob_itm_equip->field_DC == 1) {
        name = "kl_te_r_wj";
        mat4_translate(0.0f, 0.0f, 0.082f, &mat);
        mat4_rotate_zyx_mult(&mat, (float_t)(-90.9 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    if (rob_itm_equip->field_DC == 2) {
        name = "kl_te_l_wj";
        mat4_translate(0.0f, 0.0015f, -0.0812f, &mat);
        mat4_rotate_zyx_mult(&mat, (float_t)(-34.5 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    else
        name = "j_1_hyoutan_000wj";

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;

    bone_node* node = rob_itm_equip->item_equip_object[rob_itm_equip->field_D4].get_bone_node(name, aft_bone_data);
    if (!node || !node->mat)
        return;

    mat4_mult(&mat, node->mat, &mat);
    int32_t tex_pat_count = (int32_t)rob_itm_equip->texture_pattern.size();
    if (tex_pat_count)
        rctx->disp_manager.set_texture_pattern(tex_pat_count, rob_itm_equip->texture_pattern.data());
    rctx->disp_manager.entry_obj_by_object_info(0, rob_itm_equip->field_D0);
    if (tex_pat_count)
        rctx->disp_manager.set_texture_pattern();
}

void rob_chara_item_equip::disp(int32_t chara_id, render_context* rctx) {
    mdl::ObjFlags flags = (mdl::ObjFlags)0;
    if (rctx->chara_reflect)
        enum_or(flags, mdl::OBJ_CHARA_REFLECT);
    if (rctx->chara_refract)
        enum_or(flags, mdl::OBJ_REFRACT);

    mdl::DispManager& disp_manager = rctx->disp_manager;
    shadow* shad = rctx->render_manager.shadow_ptr;
    if (shadow_type != -1) {
        if (field_A0 & 0x04) {
            vec3 pos = position;
            pos.y -= 0.2f;
            shad->field_1D0[shadow_type].push_back(pos);

            float_t v9;
            if (sub_140512F60(this) <= -0.2f)
                v9 = -0.5f;
            else
                v9 = 0.05f;
            rctx->render_manager.shadow_ptr->field_1C0[shadow_type] = v9;
            disp_manager.set_shadow_type(shadow_type);
            enum_or(flags, mdl::OBJ_SHADOW);
        }

        if (field_A0 & 0x01)
            enum_or(flags, mdl::OBJ_4);
    }
    disp_manager.set_obj_flags(flags);
    disp_manager.set_chara_color(chara_color);

    vec4 temp_texture_color_coeff;
    disp_manager.get_texture_color_coeff(temp_texture_color_coeff);

    disp_manager.set_texture_color_coefficients(texture_color_coefficients);
    disp_manager.set_wet_param(wet);
    rctx->render_manager.field_31C |= npr_flag;
    sub_140512C20(this, rctx);
    rob_chara_age_age_array_disp(rctx, chara_id, rctx->chara_reflect, chara_color);

    if (item_equip_range)
        for (int32_t i = first_item_equip_object; i < max_item_equip_object; i++)
            item_equip_object[i].disp(&mat, rctx);
    else {
        for (int32_t i = ITEM_ATAMA; i < ITEM_MAX; i++) {
            mdl::ObjFlags v18 = (mdl::ObjFlags)0;
            if (!field_18[i]) {
                if (rctx->chara_reflect)
                    enum_or(v18, mdl::OBJ_CHARA_REFLECT);
                if (rctx->chara_refract)
                    enum_or(v18, mdl::OBJ_REFRACT);
            }

            mdl::ObjFlags v19 = (mdl::ObjFlags)(mdl::OBJ_4 | mdl::OBJ_SHADOW);
            if (i == ITEM_HARA)
                v19 = (mdl::ObjFlags)0;

            if (!(field_A0 & 0x04))
                enum_and(v19, ~mdl::OBJ_SHADOW);

            disp_manager.set_obj_flags((mdl::ObjFlags)(v18 | v19 | mdl::OBJ_SSS));
            item_equip_object[i].disp(&mat, rctx);
        }
    }
    disp_manager.set_texture_color_coefficients(temp_texture_color_coeff);
    disp_manager.set_wet_param();
    disp_manager.set_chara_color();
    disp_manager.set_obj_flags();
    disp_manager.set_shadow_type();
}

rob_chara_item_equip_object* rob_chara_item_equip::get_item_equip_object(item_id id) {
    if (id >= ITEM_BODY && id <= ITEM_ITEM16)
        return &item_equip_object[id];
    return 0;
}

object_info rob_chara_item_equip::get_object_info(item_id id) {
    if (id >= ITEM_BODY && id <= ITEM_ITEM16)
        return item_equip_object[id].obj_info;
    return {};
}

void rob_chara_item_equip::get_parent_bone_nodes(bone_node* bone_nodes, const bone_database* bone_data) {
    this->bone_nodes = bone_nodes;
    matrices = bone_nodes->mat;
    for (int32_t i = first_item_equip_object; i < max_item_equip_object; i++)
        item_equip_object[i].get_parent_bone_nodes(bone_nodes, bone_data);
}

const mat4* rob_chara_item_equip::get_ex_data_bone_node_mat(item_id id, const char* name) {
    return item_equip_object[id].get_ex_data_bone_node_mat(name);
}

void rob_chara_item_equip::load_body_parts_object_info(item_id id, object_info obj_info,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (id <= ITEM_BODY || id >= ITEM_MAX)
        obj_info = {};
    load_object_info(obj_info, id, false, bone_data, data, obj_db);
}

void rob_chara_item_equip::load_object_info(object_info obj_info, item_id id,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (id < ITEM_BODY || id > ITEM_ITEM16 || !bone_nodes)
        return;

    item_equip_object[id].load_object_info_ex_data(obj_info,
        bone_nodes, osage_reset, bone_data, data, obj_db);
    set_disp(id, true);
}

void rob_chara_item_equip::reset() {
    bone_nodes = 0;
    matrices = 0;
    for (int32_t i = ITEM_BODY; i < ITEM_MAX; i++)
        item_equip_object[i].init_members();
    item_equip_range = false;
    shadow_type = (shadow_type_enum)-1;
    field_A0 = 0x05;
    texture_pattern.clear();
    field_D0 = {};
    field_D4 = -1;
    disable_update = false;
    parts_short = false;
    parts_append = false;
    parts_white_one_l = false;
    osage_step = 1.0f;
    use_opd = false;
    first_item_equip_object = ITEM_ATAMA;
    max_item_equip_object = ITEM_ITEM16;
}

void rob_chara_item_equip::reset_external_force() {
    for (int32_t i = first_item_equip_object; i < max_item_equip_object; i++)
        item_equip_object[i].reset_external_force();
}

void rob_chara_item_equip::reset_init_data(bone_node* bone_nodes) {
    reset();
    this->bone_nodes = bone_nodes;
    matrices = bone_nodes->mat;

    mat4* v7 = field_13C;
    int32_t* v8 = field_18;
    for (int32_t i = ITEM_BODY; i < ITEM_MAX; i++) {
        item_equip_object[i].init_members(i);
        *v8++ = 0;
        *v7++ = mat4_identity;
    }

    osage_step = 1.0f;
    wet = 0.0f;
    wind_strength = 1.0f;
    chara_color = true;
    npr_flag = false;
    mat = mat4_identity;
}

void rob_chara_item_equip::reset_nodes_external_force(rob_osage_parts parts) {
    item_equip_object[parts == ROB_OSAGE_PARTS_MUFFLER ? ITEM_OUTER : ITEM_KAMI]
        .reset_nodes_external_force((rob_osage_parts_bit)(1 << parts));
}

void rob_chara_item_equip::set_alpha_obj_flags(float_t alpha, mdl::ObjFlags flags) {
    if (item_equip_range)
        for (int32_t i = first_item_equip_object; i < max_item_equip_object; i++)
            item_equip_object[i].set_alpha_obj_flags(alpha, flags);
    else
        for (int32_t i = ITEM_ATAMA; i < ITEM_MAX; i++)
            item_equip_object[i].set_alpha_obj_flags(alpha, flags);
}

void rob_chara_item_equip::set_disp(item_id id, bool value) {
    item_equip_object[id].can_disp = value;
}

void rob_chara_item_equip::set_item_equip_range(bool value) {
    item_equip_range = value;
    if (value) {
        first_item_equip_object = ITEM_BODY;
        max_item_equip_object = ITEM_ATAMA;
    }
    else {
        first_item_equip_object = ITEM_ATAMA;
        max_item_equip_object = ITEM_ITEM16;
    }
}

void rob_chara_item_equip::set_motion_reset_data(uint32_t motion_id, float_t frame) {
    for (int32_t i = first_item_equip_object; i < max_item_equip_object; i++)
        item_equip_object[i].set_motion_reset_data(motion_id, frame);

    task_wind->ptr->reset();
}

void rob_chara_item_equip::set_motion_skin_param(int8_t chara_id, uint32_t motion_id, int32_t frame) {
    for (int32_t i = ITEM_KAMI; i < ITEM_ITEM16; i++)
        item_equip_object[i].set_motion_skin_param(chara_id, motion_id, frame);
}

void rob_chara_item_equip::set_null_blocks_expression_data(item_id id,
    const vec3& position, const vec3& rotation, const vec3& scale) {
    item_equip_object[id].set_null_blocks_expression_data(position, rotation, scale);
}

void rob_chara_item_equip::set_object_texture_pattern(item_id id,
    texture_pattern_struct* tex_pat, size_t count) {
    item_equip_object[id].set_texture_pattern(tex_pat, count);
}

void rob_chara_item_equip::set_opd_blend_data(std::list<motion_blend_mot*>* a2) {
    opd_blend_data.clear();
    if (!a2)
        return;

    for (motion_blend_mot*& i : *a2) {
        ::opd_blend_data v11;
        v11.motion_id = i->mot_key_data.motion_id;
        v11.frame = i->mot_play_data.frame_data.frame;
        v11.frame_count = (float_t)i->mot_key_data.mot.frame_count - 1.0f;
        v11.field_C = false;
        v11.type = MOTION_BLEND_NONE;
        v11.blend = 0.0f;

        if (sub_140413810(i)) {
            v11.type = i->get_type();
            v11.field_C = true;
            v11.blend = i->blend->blend;
        }
        opd_blend_data.push_back(v11);
    }

    if (opd_blend_data.size()) {
        ::opd_blend_data* v9 = &opd_blend_data.back();
        if (v9->field_C && v9->type != MOTION_BLEND_FREEZE)
            v9->field_C = false;
    }

    if (use_opd)
        for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++)
            item_equip_object[i].check_no_opd(opd_blend_data);
}

void rob_chara_item_equip::set_osage_reset() {
    for (int32_t i = first_item_equip_object; i < max_item_equip_object; i++)
        item_equip_object[i].set_osage_reset();

    task_wind->ptr->reset();
}

void rob_chara_item_equip::set_osage_step(float_t value) {
    osage_step = value;
}

void rob_chara_item_equip::set_osage_move_cancel(uint8_t id, float_t value) {
    switch (id) {
    case 0:
        for (int32_t i = first_item_equip_object; i < max_item_equip_object; i++)
            item_equip_object[i].set_osage_move_cancel(value);
        break;
    case 1:
        item_equip_object[ITEM_KAMI].set_osage_move_cancel(value);
        break;
    case 2:
        item_equip_object[ITEM_OUTER].set_osage_move_cancel(value);
        break;
    }
}

void rob_chara_item_equip::set_osage_play_data_init(item_id id, const float_t* opdi_data) {
    if (id >= ITEM_KAMI && id <= ITEM_ITEM16)
        item_equip_object[id].set_osage_play_data_init(opdi_data);
}

void rob_chara_item_equip::set_shadow_type(int32_t chara_id) {
    if (!chara_id)
        shadow_type = SHADOW_CHARA;
    else
        shadow_type = SHADOW_STAGE;
}

void rob_chara_item_equip::set_texture_pattern(texture_pattern_struct* tex_pat, size_t count) {
    texture_pattern.clear();
    if (count && tex_pat)
        for (size_t i = 0; i < count; i++)
            texture_pattern.push_back(tex_pat[i]);
}

void rob_chara_item_equip::skp_load(item_id id, const skin_param_osage_root& skp_root,
    std::vector<skin_param_osage_node>& vec, skin_param_file_data* skp_file_data, const bone_database* bone_data) {
    if (id >= ITEM_BODY && id < ITEM_MAX)
        item_equip_object[id].skp_load(skp_root, vec, skp_file_data, bone_data);
}

item_cos_texture_change_tex::item_cos_texture_change_tex() : org(), chg(), changed() {

}

item_cos_texture_change_tex::~item_cos_texture_change_tex() {

}

rob_chara_item_cos_data::rob_chara_item_cos_data() : curr_chara_index(), chara_index(), curr_cos(), cos() {

}

rob_chara_item_cos_data::~rob_chara_item_cos_data() {

}

bool rob_chara_item_cos_data::check_for_npr_flag() {
    for (int32_t i = ITEM_SUB_ZUJO; i < ITEM_SUB_MAX; i++) {
        const item_table_item* item = item_table_handler_array_get_item(chara_index, cos.arr[i]);
        if (item && item->npr_flag)
            return true;
    }
    return false;
}

const item_cos_data* rob_chara_item_cos_data::get_cos() {
    return &cos;
}

object_info rob_chara_item_cos_data::get_head_object_replace(int32_t head_object_id) {
    auto elem = head_replace.find(head_object_id);
    if (elem != head_replace.end())
        return elem->second;
    return {};
}

float_t rob_chara_item_cos_data::get_max_face_depth() {
    float_t max_face_depth = 0.0f;
    for (int32_t i = ITEM_SUB_ZUJO; i < ITEM_SUB_MAX; i++) {
        const item_table_item* item = item_table_handler_array_get_item(chara_index, cos.arr[i]);
        if (item && max_face_depth < item->face_depth)
            max_face_depth = item->face_depth;
    }
    return max_face_depth;
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

static void sub_140526FD0(rob_chara_item_cos_data* item_cos_data,
    int32_t item_no, const item_table_item* item) {
    if (!(item->attr & 0x0C))
        return;

    if (!(item->attr & 0x08)) {
        std::vector<item_cos_texture_change_tex> tex_chg_vec;
        for (const item_table_item_data_tex& i : item->data.tex) {
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
        for (const item_table_item_data_tex& i : item->data.tex)
            chg_tex_ids.push_back(i.chg);
    else
        for (const item_table_item_data_col& i : item->data.col)
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

static void sub_140527280(rob_chara_item_cos_data* item_cos_data,
    int32_t item_no, const item_table_item* item) {
    if (!(item->attr & 0x20) || !(item->attr & 0xC))
        return;

    for (const item_table_item_data_obj& i : item->data.obj) {
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
    item_cos_data->texture_change_clear();
    item_cos_data->texture_pattern_clear();
    for (int32_t sub_id = ITEM_SUB_ZUJO; sub_id < ITEM_SUB_MAX; sub_id++) {
        int32_t item_no = item_cos_data->cos.arr[sub_id];
        if (!item_no || sub_id == ITEM_SUB_HADA)
            continue;

        const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_index, item_no);
        if (!item)
            continue;

        sub_140526FD0(item_cos_data, item_no, item);
        sub_140527280(item_cos_data, item_no, item);
    }
}

static void sub_14052B4C0(rob_chara_item_cos_data* item_cos_data,
    rob_chara_item_equip* rob_itm_equip, item_id id, bool object) {
    if (object)
        rob_itm_equip->set_texture_pattern(0, 0);
    else
        rob_itm_equip->set_object_texture_pattern(id, 0, 0);
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
        rob_itm_equip->set_texture_pattern(
            item_cos_data->texture_pattern[id].data(), item_cos_data->texture_pattern[id].size());
    else
        rob_itm_equip->set_object_texture_pattern(id,
            item_cos_data->texture_pattern[id].data(), item_cos_data->texture_pattern[id].size());
}

static void sub_140522A30(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    const chara_init_data* chr_init_data = chara_init_data_get(item_cos_data->chara_index);
    for (int32_t i = ITEM_ATAMA; i < ITEM_KAMI; i++) {
        rob_itm_equip->load_outfit_object_info((item_id)i,
            chr_init_data->field_7E4[i - 1], false, bone_data, data, obj_db);
        auto elem = item_cos_data->item_change.find(i);
        if (elem == item_cos_data->item_change.end())
            sub_14052B4C0(item_cos_data, rob_itm_equip, (item_id)i, false);
        else
            sub_14052CCC0(item_cos_data, rob_itm_equip, elem->second, (item_id)i, false);
    }

    for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++) {
        rob_itm_equip->load_body_parts_object_info((item_id)i, {}, bone_data, data, obj_db);
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
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (id <= ITEM_BODY || id >= ITEM_MAX) {
        a4 = {};
        a3 = {};
    }
    rob_itm_equip->field_D0 = a4;
    rob_itm_equip->field_D4 = id;
    rob_itm_equip->load_object_info(a3, id, false, bone_data, data, obj_db);
    rob_itm_equip->set_disp(id, false);
}

static void sub_14052C560(rob_chara_item_cos_data* item_sub_data,
    rob_chara_item_equip* rob_itm_equip, int32_t item_no, const item_table_item* item, item_sub_id sub_id,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    for (const item_table_item_data_obj& i : item->data.obj) {
        if (i.obj_info.is_null())
            break;

        if (item->type == 1) {
            if (i.rpk != ITEM_NONE) {
                rob_itm_equip->load_outfit_object_info(i.rpk,
                    i.obj_info, false, bone_data, data, obj_db);
                if (item->attr & 0xC)
                    item_sub_data->set_texture_pattern(rob_itm_equip, item_no, i.rpk, false);
            }
            continue;
        }

        else if (item->type)
            continue;

        item_id id = sub_140525B90(item_sub_data, sub_id);
        if (id != ITEM_NONE) {
            rob_itm_equip->load_body_parts_object_info(id, i.obj_info, bone_data, data, obj_db);
            item_sub_data->field_F0.insert({ i.obj_info, id });
            if (item->attr & 0xC)
                item_sub_data->set_texture_pattern(rob_itm_equip, item_no, id, false);
        }
        else {
            item_id id = sub_140512EF0(rob_itm_equip, ITEM_ITEM09);
            if (id != ITEM_NONE && (!(item->attr & 0x10) || &i - item->data.obj.data())) {
                bool v8 = false;
                if (item->attr & 0x10 && &i - item->data.obj.data() == 1) {
                    sub_140513B90(rob_itm_equip, id, item->data.obj[1].obj_info,
                        item->data.obj[0].obj_info, bone_data, data, obj_db);
                    v8 = true;
                }
                else
                    rob_itm_equip->load_body_parts_object_info(id, i.obj_info, bone_data, data, obj_db);
                item_sub_data->field_F0.insert({ i.obj_info, id });
                if (item->attr & 0xC)
                    item_sub_data->set_texture_pattern(rob_itm_equip, item_no, id, v8);
            }
        }
    }

    item_sub_data->head_replace.clear();
    if (!(item->attr & 0x800))
        return;

    const char* chara_name = chara_index_get_chara_name(item_sub_data->chara_index);
    char buf[0x200];
    for (int32_t i = 0; i < 9; i++) {
        sprintf_s(buf, sizeof(buf), "%sITM%03d_ATAM_HEAD_%02d_SP__DIVSKN", chara_name, item_no, i);
        object_info obj_info = obj_db->get_object_info(buf);
        if (obj_info.not_null())
            item_sub_data->head_replace.insert({ i, obj_info });
    }
}

static void sub_140522C60(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!item_table_handler_array_get_table(item_cos_data->chara_index))
        return;

    const item_sub_id dword_140A2BB70[] = {
        ITEM_SUB_ZUJO, ITEM_SUB_KAMI, ITEM_SUB_HITAI, ITEM_SUB_ME, ITEM_SUB_MEGANE,
        ITEM_SUB_MIMI, ITEM_SUB_KUCHI, ITEM_SUB_MAKI, ITEM_SUB_KUBI, ITEM_SUB_INNER,
        ITEM_SUB_KATA, ITEM_SUB_U_UDE, ITEM_SUB_L_UDE, ITEM_SUB_JOHA_MAE, ITEM_SUB_JOHA_USHIRO,
        ITEM_SUB_BELT, ITEM_SUB_KOSI, ITEM_SUB_SUNE, ITEM_SUB_KUTSU, ITEM_SUB_HEAD, ITEM_SUB_MAX,
    };

    const item_sub_id* v6 = dword_140A2BB70;
    while (*v6 != ITEM_SUB_MAX) {
        int32_t item_no = item_cos_data->cos.arr[*v6];
        if (item_no) {
            const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_index, item_no);
            if (item && item->type != 3 && item->attr & 0x01)
                sub_14052C560(item_cos_data, rob_itm_equip, item_no, item, *v6, bone_data, data, obj_db);
        }
        v6++;
    }
}

static void sub_14052C8C0(rob_chara_item_cos_data* item_cos_data,
    rob_chara_item_equip* rob_itm_equip, int32_t item_no, const item_table_item* item,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    for (const item_table_item_data_obj& i : item->data.obj) {
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
                rob_itm_equip->load_outfit_object_info(id,
                    v11, false, bone_data, data, obj_db);
            }
            else {
                item_id v13 = sub_140512EF0(rob_itm_equip, ITEM_KAMI);
                if (v13 == ITEM_NONE)
                    continue;

                rob_itm_equip->load_body_parts_object_info(v13, i.obj_info, bone_data, data, obj_db);
                item_cos_data->field_F0.insert({ i.obj_info, v13 });
                item_cos_data->field_100.insert({ id, v13 });
                id = v13;
            }
        }
        else if (!item->type) {
            id = sub_140512EF0(rob_itm_equip, ITEM_ITEM09);
            if (id != ITEM_NONE) {
                rob_itm_equip->load_body_parts_object_info(id, i.obj_info, bone_data, data, obj_db);
                item_cos_data->field_F0.insert({ i.obj_info, id });
                continue;
            }
        }
        else
            continue;

        if (item->attr & 0x0C)
            item_cos_data->set_texture_pattern(rob_itm_equip, item_no, id, false);
    }
}

static void sub_140522D00(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!item_table_handler_array_get_table(item_cos_data->chara_index))
        return;

    const item_sub_id dword_140A2BBC8[] = {
        ITEM_SUB_TE, ITEM_SUB_ASI, ITEM_SUB_MAX,
    };

    const item_sub_id* v6 = dword_140A2BBC8;
    while (*v6 != ITEM_SUB_MAX) {
        int32_t item_no = item_cos_data->cos.arr[*v6];
        if (item_no) {
            const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_index, item_no);
            if (item && item->type != 3 && item->attr & 0x01)
                sub_14052C8C0(item_cos_data, rob_itm_equip, item_no, item, bone_data, data, obj_db);
        }
        v6++;
    }
}

static void sub_140525D90(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    int32_t a3, const bone_database* bone_data, void* data, const object_database* obj_db) {
    auto elem = item_cos_data->field_100.find(a3);
    if (elem != item_cos_data->field_100.end())
        rob_itm_equip->load_body_parts_object_info(elem->second, {}, bone_data, data, obj_db);
}

static void sub_140522D90(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!item_table_handler_array_get_table(item_cos_data->chara_index))
        return;

    const item_sub_id dword_140A2BBD8[] = {
        ITEM_SUB_PANTS, ITEM_SUB_OUTER, ITEM_SUB_MAX,
    };

    const item_sub_id* v6 = dword_140A2BBD8;
    while (*v6 != ITEM_SUB_MAX) {
        int32_t item_no = item_cos_data->cos.arr[*v6];
        if (!item_no) {
            v6++;
            continue;
        }

        const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_index, item_no);
        if (!item || item->type != 1)
            continue;

        for (const item_table_item_data_obj& i : item->data.obj) {
            if (i.obj_info.is_null())
                continue;

            item_id id = sub_140525B90(item_cos_data, *v6);
            rob_itm_equip->load_body_parts_object_info(id, i.obj_info, bone_data, data, obj_db);
            item_cos_data->field_F0.insert({ i.obj_info, id });
            if (item->attr & 0xC)
                item_cos_data->set_texture_pattern(rob_itm_equip, item_no, id, false);

            if (i.rpk == ITEM_NONE)
                continue;

            rob_itm_equip->load_outfit_object_info(i.rpk,
                {}, false, bone_data, data, obj_db);
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
    if (!item_table_handler_array_get_table(item_cos_data->chara_index))
        return;

    int32_t item_no = item_cos_data->cos.arr[ITEM_SUB_HADA];
    if (!item_no)
        return;

    const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_index, item_no);
    if (!item || item->data.col.size())
        return;

    vec3 texture_color_coefficients = item->data.col[0].col_tone.blend;
    vec3 texture_color_offset = item->data.col[0].col_tone.offset;
    vec3 texture_specular_coefficients = 1.0f;
    vec3 texture_specular_offset = 0.0f;
    if (item->data.col.size() > 1) {
        texture_specular_coefficients = item->data.col[1].col_tone.blend;
        texture_specular_offset = item->data.col[1].col_tone.offset;
    }

    for (int32_t i = ITEM_ATAMA; i <= ITEM_ITEM16; i++) {
        if (rob_itm_equip->get_object_info((item_id)i).is_null())
            continue;

        rob_chara_item_equip_object* itm_eq_obj = &rob_itm_equip->item_equip_object[i];
        itm_eq_obj->texture_data.field_0 = false;
        itm_eq_obj->texture_data.texture_color_coefficients = texture_color_coefficients;
        itm_eq_obj->texture_data.texture_color_offset = texture_color_offset;
        itm_eq_obj->texture_data.texture_specular_coefficients = texture_specular_coefficients;
        itm_eq_obj->texture_data.texture_specular_offset = texture_specular_offset;
    }
}

static void sub_140523230(rob_chara_item_cos_data* item_cos_data,
    rob_chara_item_equip* rob_itm_equip, int32_t item_no) {
    if (!item_no)
        return;

    const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_index, item_no);
    if (!item || !((item->type + 1) & ~0x04))
        return;

    if (item->attr & 0x02)
        for (const item_table_item_data_ofs& i : item->data.ofs) {
            if (item_cos_data->cos.arr[i.sub_id] != i.no)
                continue;

            for (const item_table_item_data_obj& j : item->data.obj) {
                if (j.obj_info.is_null())
                    break;

                auto elem = item_cos_data->field_F0.find(j.obj_info);
                if (elem != item_cos_data->field_F0.end())
                    rob_itm_equip->set_null_blocks_expression_data(
                        elem->second, i.position, i.rotation, i.scale);
            }
            break;
        }

    for (const item_table_item_data_obj& i : item->data.obj) {
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
    if (!item_table_handler_array_get_table(item_cos_data->chara_index))
        return;

    for (int32_t i = 0; i < ITEM_SUB_MAX; i++)
        sub_140523230(item_cos_data, rob_itm_equip, item_cos_data->cos.arr[i]);
}

static void sub_1405234E0(rob_chara_item_cos_data* item_cos_data) {
    item_cos_data->item_change.clear();
    item_cos_data->field_F0.clear();
    item_cos_data->field_100.clear();
}

void rob_chara_item_cos_data::reload_items(int32_t chara_id,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    rob_chara_item_equip* rob_itm_equip = rob_chara_array_get_item_equip(chara_id);
    sub_140522990(this);
    sub_140522A30(this, rob_itm_equip, bone_data, data, obj_db);
    sub_140522C60(this, rob_itm_equip, bone_data, data, obj_db);
    sub_140522D00(this, rob_itm_equip, bone_data, data, obj_db);
    sub_140522D90(this, rob_itm_equip, bone_data, data, obj_db);
    sub_140522F90(this, rob_itm_equip);
    sub_1405231D0(this, rob_itm_equip);
    sub_1405234E0(this);
}

void rob_chara_item_cos_data::set_chara_index(::chara_index chara_index) {
    this->chara_index = chara_index;
}

void rob_chara_item_cos_data::set_chara_index_item(::chara_index chara_index, int32_t item_no) {
    set_chara_index(chara_index);
    set_item(item_no);
}

void rob_chara_item_cos_data::set_chara_index_item_nos(::chara_index chara_index, const int32_t* items) {
    set_chara_index(chara_index);
    set_item_nos(items);
}

void rob_chara_item_cos_data::set_chara_index_item_zero(::chara_index chara_index, int32_t item_no) {
    set_chara_index(chara_index);
    set_item_zero(item_no);
}

void rob_chara_item_cos_data::set_item(int32_t item_no) {
    if (!item_no)
        return;

    const item_table_item* item = item_table_handler_array_get_item(chara_index, item_no);
    if (item)
        cos.arr[item->sub_id] = item_no;
}

void rob_chara_item_cos_data::set_item_array(const rob_chara_pv_data_item& item) {
    for (int32_t i = 0; i < 4; i++)
        set_item(item.arr[i]);
}

void rob_chara_item_cos_data::set_item_no(item_sub_id sub_id, int32_t item_no) {
    cos.arr[sub_id] = item_no;
}

void rob_chara_item_cos_data::set_item_nos(const int32_t* item_nos) {
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++)
        set_item_no((item_sub_id)i, item_nos[i]);
}

void rob_chara_item_cos_data::set_item_zero(int32_t item_no) {
    const item_table_item* item = item_table_handler_array_get_item(chara_index, item_no);
    if (item)
        cos.arr[item->sub_id] = 0;
}

void rob_chara_item_cos_data::set_texture_pattern(rob_chara_item_equip* rob_itm_equip,
    uint32_t item_no, item_id id, bool tex_pat_for_all) {
    if (id < ITEM_BODY || id >= ITEM_MAX)
        return;

    auto elem = texture_change.find(item_no);
    if (elem == texture_change.end())
        return;

    for (item_cos_texture_change_tex& i : elem->second)
        if (i.org && i.chg)
            texture_pattern[id].push_back({ i.org->id, i.chg->id });

    if (tex_pat_for_all)
        rob_itm_equip->set_texture_pattern(texture_pattern[id].data(), texture_pattern[id].size());
    else
        rob_itm_equip->set_object_texture_pattern(id,
            texture_pattern[id].data(), texture_pattern[id].size());
}

void rob_chara_item_cos_data::texture_change_clear() {
    for (auto i : texture_change)
        for (item_cos_texture_change_tex& j : i.second)
            if (j.changed) {
                texture_free(j.chg);
                j.chg = 0;
            }
    texture_change.clear();
}

void rob_chara_item_cos_data::texture_pattern_clear() {
    for (std::vector<texture_pattern_struct>& i : texture_pattern)
        i.clear();
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
field_B0(), field_B4(), field_140(), field_144(), field_145(), field_146(), field_148(), field_150(),
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

RobSubAction::Data::Data() : field_0(), field_8(), param(), field_18() {

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
    data.param = 0;
    data.field_18 = 0;
    data.cry.Field_8();
    data.shake_hand.Field_8();
    data.embarrassed.Field_8();
    data.angry.Field_8();
    data.laugh.Field_8();
}

void RobSubAction::SetSubActParam(SubActParam* value) {
    if (!value)
        return;

    if (data.param) {
        delete data.param;
        data.param = 0;
    }
    data.param = value;
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
    data.Reset();
}

RobHandMotion::RobHandMotion() {
    Reset();
}

RobHandMotion::~RobHandMotion() {

}

void RobHandMotion::Reset() {
    data.Reset();
}

RobMouthMotion::RobMouthMotion() {
    Reset();
}

RobMouthMotion::~RobMouthMotion() {

}

void RobMouthMotion::Reset() {
    data.Reset();
}

RobEyesMotion::RobEyesMotion() {
    Reset();
}

RobEyesMotion::~RobEyesMotion() {

}

void RobEyesMotion::Reset() {
    data.Reset();
}

RobEyelidMotion::RobEyelidMotion() {
    Reset();
}

RobEyelidMotion::~RobEyelidMotion() {

}

void RobEyelidMotion::Reset() {
    data.Reset();
}

rob_chara_data_adjust::rob_chara_data_adjust() : enable(), frame(), transition_frame(),
curr_force(), curr_strength(), motion_id(), set_frame(), transition_duration(), type(),
cycle_type(), ignore_gravity(), cycle(), phase(), force(), strength(), strength_transition() {
    reset();
}

void rob_chara_data_adjust::reset() {
    enable = false;
    frame = 0.0f;
    transition_frame = 0.0f;
    curr_external_force = 0.0f;
    curr_force = 1.0f;
    curr_strength = 1.0f;
    motion_id = -1;
    set_frame = 0.0f;
    transition_duration = 0.0f;
    type = 6;
    cycle_type = 0;
    ignore_gravity = true;
    external_force = 0.0f;
    external_force_cycle_strength = 0.0f;
    external_force_cycle = 0.0f;
    cycle = 1.0f;
    phase = 0.0f;
    force = 1.0f;
    strength = 1.0f;
    strength_transition = 0.0f;
}

rob_chara_data_hand_adjust::rob_chara_data_hand_adjust() : enable(), scale_select(), type(),
current_scale(), scale(), duration(), current_time(), rotation_blend(), scale_blend(),
enable_scale(), disable_x(), disable_y(), disable_z(), arm_length(), field_40() {
    reset();
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
    offset = 0.0f;
    field_30 = 0.0f;
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

rob_chara_motion::rob_chara_motion() : frame_data(), step_data(), field_24(), field_28(), field_29(),
field_2A(), loop_index(), field_30(), field_34(), field_36(), field_38(), field_B8(),
field_138(), field_13C(), field_140(),  field_14C(), field_314(), parts_adjust(), parts_adjust_prev(),
adjust_global(), adjust_global_prev(), hand_adjust(), hand_adjust_prev(), arm_adjust() {
    motion_id = -1;
    prev_motion_id = -1;
    step = 1.0f;
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
    loop_index = 0;
    field_30 = 0;
    field_34 = 0;
    field_36 = 0;

    for (int32_t& i : field_38)
        i = 0;
    for (int32_t& i : field_B8)
        i = 0;

    field_138 = 1.0f;
    field_13C = 1.0f;
    field_140 = 0.0f;
    field_150.Reset();
    hand_l.Reset();
    hand_r.Reset();
    hand_l_object = {};
    hand_r_object = {};
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
    is_x = false; // X
}

mothead_mot::~mothead_mot() {

}

mothead::mothead() {
    mot_set_id = -1;
    first_mot_id = 0;
    last_mot_id = 0;
    data_x = 0; // X
}

mothead::~mothead() {
    for (mothead_mot*& i : mots)
        if (i) {
            delete i;
            i = 0;
        }

    if (data_x) { // X
        free(data_x);
        data_x = 0;
    }
}

struc_306::struc_306() : field_0(), frame(), field_8(), field_C(), field_E(),
field_34(), field_38(), field_3C(), field_40(), field_44(), field_48() {

}

struc_306::struc_306(int16_t field_0, float_t frame, float_t field_8) : field_0(field_0),
frame(frame), field_8(field_8), field_C(), field_E(),
field_34(), field_38(), field_3C(), field_40(), field_44(), field_48() {

}

struc_652::struc_652() : motion_id(), frame_count(), frame(), field_C(), field_10(), field_20(),
field_30(), field_40(), field_50(), field_52(), field_54(), field_58(), field_5C(), field_60(),
field_64(), field_68(), loop_count(), loop_begin(), loop_end(), field_78(), field_7C(), field_80(),
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

void struc_652::reset() {
    motion_id = -1;
    frame_count = 0.0f;
    frame = 0.0f;
    field_C = 0;
    sub_140536DD0();
    field_50 = 0;
    field_52 = 0;
    field_54 = 0;
    field_58 = -1;
    field_5C = -1;
    field_60 = 0;
    field_64 = -1;
    field_68 = 0;
    loop_count = -1;
    loop_begin = 0.0f;
    loop_end = -1.0f;
    field_78 = 0.0f;
    field_7C = 0.0f;
    field_80 = 0.0f;
    field_1E8 = 0;
    field_1EC = 0.0f;
    field_1F0 = 0.0f;
    field_1F4 = 0.0f;
    field_1F8 = -1.0f;
    field_1FC = 0.0f;
    field_200 = 1.0f;
    field_204 = -1;
    field_208 = 0;
    field_20C = -1;
    field_210 = 0;
    field_218 = -1.0f;
    field_21C = -1.0f;
    field_220 = 0;
    field_228.clear();
    field_238 = -1;
    field_23C = 0.0f;
    field_240 = 0;
    field_244 = 0;
    field_248 = 0;
    field_250 = 0;
    field_258 = -1.0f;
    field_25C = 0;
    field_268 = 0;
    field_274 = 0;
    field_278 = 0;
    field_27C = -1;
    field_280 = 0;
    field_284 = 0;
    field_288 = 0;
    field_290 = 0;
    field_294 = 0;
    field_298 = 0;
    field_29C = 0.0f;
    field_2A0 = 0;
    field_2A4 = 0.0f;
    field_2A8 = 0.0f;
    field_2AC = 0.0f;
    field_2B8 = 0;
    field_2BC = 0;
    field_2C0 = -1.0f;
    field_2C4 = -1.0f;
    field_2C8 = 0;
    field_2E8 = -1;
    field_2EC = 0;
    field_2F0 = 0;
    field_2F4 = 0;
    field_2FC = -1;
    field_300 = 100;
    field_304 = 1;
    field_308 = 0;
    field_30C = 1.0f;
    field_318 = 0.0f;
    field_324 = 0.05f;
    field_328 = -1.0f;
    iterations = -1;
}

void struc_652::sub_140536DD0() {
    field_84 = 0;
    field_88 = 0;
    field_8C = 0;
    field_90 = 0;
    field_94 = 0;
    field_96 = 0;
    field_98 = -1;
    field_9C = 0;
    field_A0 = 0;
    field_A4 = 0;
    field_A8 = 0;
    field_B0[0] = { 0, 1.2f, 1.0f };
    field_B0[1] = { 0, -1.0f, -1.0f };
    field_B0[2] = { 0, -1.0f, -1.0f };
    field_B0[3] = { 0, -1.0f, -1.0f };
    field_B0[4] = { 0, -1.0f, -1.0f };
    field_B0[5] = { 0, -1.0f, -1.0f };
    field_B0[6] = { 0, -1.0f, -1.0f };
    field_B0[7] = { 0, -1.0f, -1.0f };
    field_B0[8] = { 0, -1.0f, -1.0f };
    field_B0[9] = { 0, -1.0f, -1.0f };
    field_B0[10] = { 0, -1.0f, -1.0f };
    field_B0[11] = { 0, -1.0f, -1.0f };
    field_B0[12] = { 0, -1.0f, -1.0f };
    field_B0[13] = { 0, -1.0f, -1.0f };
    field_B0[14] = { 0, -1.0f, -1.0f };
    field_B0[15] = { 0, -1.0f, -1.0f };
    field_B0[16] = { 0, -1.0f, -1.0f };
    field_B0[17] = { 0, -1.0f, -1.0f };
    field_B0[18] = { 0, -1.0f, -1.0f };
    field_B0[19] = { 0, -1.0f, -1.0f };
    field_B0[20] = { 0, -1.0f, -1.0f };
    field_B0[21] = { 0, -1.0f, -1.0f };
    field_B0[22] = { 0, -1.0f, -1.0f };
    field_B0[23] = { 0, -1.0f, -1.0f };
    field_B0[24] = { 0, -1.0f, -1.0f };
    field_B0[25] = { 0, -1.0f, -1.0f };
    field_260 = 0;
    field_270 = 0;
    field_2B0 = 0;
    field_2D8 = field_2D0;
    field_310 = 3;
    field_314 = 0;
}

struc_651::struc_651() : field_0(), field_10(), field_14(), field_18(), field_1C(), field_2C(),
field_80(), field_1C4(), field_308(), arm_adjust_next_value(), arm_adjust_prev_value(),
arm_adjust_start_frame(), arm_adjust_duration(), field_310(), field_314(), field_318(), field_31C(),
field_320(), field_324(), field_328(), field_32C(), field_330(), field_334(), field_338() {

}

void struc_651::reset() {
    field_0.current = 0;
    field_0.data = 0;
    field_0.is_x = false; // X
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
    field_1C = 0.0f;
    field_20 = 0.0f;

    for (struc_226& i : field_2C)
        for (int8_t& j : i.field_0)
            j = 0;

    for (struc_225& i : field_80)
        for (float_t& j : i.field_0)
            j = 1.0f;

    for (struc_224& i : field_1C4)
        for (float_t& j : i.field_0)
            j = 0.0f;

    field_308 = 0;
    arm_adjust_next_value = 0.0f; // X
    arm_adjust_prev_value = 0.0f; // X
    arm_adjust_start_frame = -1;  // X
    arm_adjust_duration = -1.0f;  // X
    field_310 = -1.0f;
    field_314 = -1.0f;
    field_318 = 0;
    field_31C = 0;
    field_320 = 1.0f;
    field_324 = 1.0f;
    field_32C = 0;
    field_330 = -1.0f;
    field_334 = -1.0f;
    field_338 = 0;
    field_33C[0] = { -1, -1.0f, -1.0f };
    field_33C[1] = { -1, -1.0f, -1.0f };
    field_33C[2] = { -1, -1.0f, -1.0f };
    field_33C[3] = { -1, -1.0f, -1.0f };
}

struc_223::struc_223() : field_330(), field_7A0(), motion_set_id() {

}

struc_223::~struc_223() {

}

void struc_223::reset() {
    field_0.field_10 = {};
    field_0.field_20 = {};
    field_0.reset();
    field_330.reset();
    field_7A0 = 0;
    motion_set_id = -1;
}

rob_chara_data_miku_rot::rob_chara_data_miku_rot() : rot_y_int16(), field_2(), field_4(),
field_6(), field_8(), field_C(), field_10(), field_14(), field_60(), field_64(), field_68() {
    reset();
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
    position = 0.0f;
    field_24 = 0.0f;
    field_30 = 0.0f;
    field_3C = 0.0f;
    field_48 = 0.0f;
    field_54 = 0.0f;
    field_60 = 0;
    field_64 = 0;
    field_68 = 0;
    field_6C = mat4_identity;
}

rob_chara_adjust_data::rob_chara_adjust_data() : scale(), height_adjust(), pos_adjust_y(),
offset_x(), offset_y(), offset_z(), get_global_trans(), left_hand_scale(),
right_hand_scale(), left_hand_scale_default(), right_hand_scale_default() {
    reset();
}

void rob_chara_adjust_data::reset() {
    scale = 1.0f;
    height_adjust = false;
    pos_adjust_y = 0.0f;
    pos_adjust = 0.0f;
    offset = 0.0f;
    offset_x = true;
    offset_y = false;
    offset_z = true;
    get_global_trans = false;
    mat4_translate(&trans, &mat);
    left_hand_scale = -1.0f;
    right_hand_scale = -1.0f;
    left_hand_scale_default = -1.0f;
    right_hand_scale_default = -1.0f;
    mat4_translate(&item_trans, &item_mat);
    item_scale = 1.0f; // X
}

struc_195::struc_195() : scale(), field_1C(), field_20(), field_24() {

}

pos_scale::pos_scale() : scale() {

}

void pos_scale::get_screen_pos_scale(mat4& mat, vec3& trans, bool apply_offset) {
    vec4 v15;
    *(vec3*)&v15 = trans;
    v15.w = 1.0f;
    mat4_mult_vec(&mat, &v15, &v15);

    if (fabsf(v15.w) >= 1.0e-10f) {
        post_process* pp = &rctx_ptr->post_process;
        if (apply_offset) {
            float_t v11 = (1.0f + v15.x * (1.0f / v15.w)) * 0.5f * (float_t)pp->screen_width;
            float_t v12 = (1.0f - v15.y * (1.0f / v15.w)) * 0.5f * (float_t)pp->screen_height;
            v11 += (float_t)pp->screen_x_offset;
            v12 += (float_t)pp->screen_y_offset;
            pos = { v11, v12 };
        }
        else{
            float_t v11 = (1.0f + v15.x * (1.0f / v15.w)) * 0.5f * (float_t)pp->render_width;
            float_t v12 = (1.0f - v15.y * (1.0f / v15.w)) * 0.5f * (float_t)pp->render_height;
            pos = { v11, v12 };
        }
        scale = -v15.w;
    }
    else {
        pos = 0.0f;
        scale = 0.0f;
    }
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
adjust_data(), field_3D90(), field_3D94(), field_3D98(), field_3D9A(), field_3D9C(), field_3DA0(),
field_3DA4(), field_3DA8(), field_3DB0(), field_3DB8(), field_3DBC(), field_3DC0(), field_3DC4(), field_3DC8(),
field_3DCC(), field_3DD0(), field_3DD4(), field_3DD8(), field_3DDC(), field_3DE0(), arm_adjust_scale() {

}

rob_chara_data::~rob_chara_data() {

}

void rob_chara_data::reset() {
    sub_140505C90(&field_8);
    rob_sub_action.Reset();
    motion.reset();
    field_1588.reset();
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
    arm_adjust_scale = 1.0f;
}

rob_chara::rob_chara() : chara_id(), type(), field_C(), field_D(),
chara_index(), cos_id(), chara_init_data(), field_20() {
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

void mothead_storage_init() {
    mothead_storage_data = {};
}

bool mothead_storage_check_mhd_file_not_ready(uint32_t set_id) {
    auto elem = mothead_storage_data.find(set_id);
    if (elem != mothead_storage_data.end())
        return elem->second.CheckNotReady();
    return false;
}

const mothead_mot* mothead_storage_get_mot_by_motion_id(
    uint32_t motion_id, const motion_database* mot_db) {
    uint32_t set_id = mot_db->get_motion_set_id_by_motion_id(motion_id);
    if (set_id == -1)
        return &mothead_mot_null;

    auto elem = mothead_storage_data.find(set_id);
    if (elem == mothead_storage_data.end() || !elem->second.data)
        return &mothead_mot_null;

    mothead* mhd = elem->second.data;
    if ((int32_t)motion_id < mhd->first_mot_id || (int32_t)motion_id > mhd->last_mot_id)
        return &mothead_mot_null;

    mothead_mot* mot = mhd->mots[(ssize_t)(int32_t)motion_id - mhd->first_mot_id];
    if (!mot)
        return &mothead_mot_null;
    return mot;
}

void mothead_storage_free() {
    mothead_storage_data.clear();
}

int32_t expression_id_to_mottbl_index(int32_t expression_id) {
    static const int32_t expression_id_to_mottbl_index_table[] = {
         11,  15,  57,  19,  23,  25,  29,  33,
         37,  41,  45,  49,  53,  65,   7,  69,
         73,  77,  81,  85,  89,   7,  61,   6,
        214, 215, 216, 217, 218, 219, 220, 221,
        222, 223,  93,  95,  97,  99, 101, 103,
        105, 107, 109, 111, 113, 115, 117, 119,
        121, 123, 125, 127,  13,  21,  31,  39,
         43,  47,  51,  55,  67,  71,  79,  83,
        129,  59,  17,  91,  27,  87,  35,  75,
          9,  63, 236, 238, 240, 242,
    };

    if (expression_id >= 0 && expression_id
        < sizeof(expression_id_to_mottbl_index_table) / sizeof(int32_t))
        return expression_id_to_mottbl_index_table[expression_id];
    return 6;
}

/*
195 CMN_HAND_OPEN   0
196 CMN_HAND_CLOSE  1
194 CMN_HAND_NORMAL 2
197 CMN_HAND_PEACE  3
201 CMN_HAND_NEGI   4
198 CMN_HAND_MIC    5
199 CMN_HAND_ONE    6
202 CMN_HAND_SIZEN  7
203 CMN_HAND_PICK   8
192 (null)          9
200 CMN_HAND_THREE  10
204 CMN_HAND_MIC    11,12,13
193 CMN_HAND_RESET  14*/

int32_t hand_anim_id_to_mottbl_index(int32_t hand_anim_id) {
    static const int32_t hand_anim_id_to_mottbl_index_table[] = {
        195, 196, 194, 197, 201, 198, 199, 202,
        203, 192, 200, 204, 204, 204, 193,
    };

    if (hand_anim_id >= 0 && hand_anim_id
        < sizeof(hand_anim_id_to_mottbl_index_table) / sizeof(int32_t))
        return hand_anim_id_to_mottbl_index_table[hand_anim_id];
    return 192;
}

int32_t look_anim_id_to_mottbl_index(int32_t look_anim_id) {
    static const int32_t look_anim_id_to_mottbl_index_table[] = {
        168, 170, 174, 172, 178, 176, 182, 180,
        166, 165, 224, 169, 171, 175, 173, 179,
        177, 183, 181, 167,
    };

    if (look_anim_id >= 0 && look_anim_id
        < sizeof(look_anim_id_to_mottbl_index_table) / sizeof(int32_t))
        return look_anim_id_to_mottbl_index_table[look_anim_id];
    return 165;
}

int32_t mouth_anim_id_to_mottbl_index(int32_t mouth_anim_id) {
    static const int32_t mouth_anim_id_to_mottbl_index_table[] = {
        134, 140, 142, 146, 144, 148, 150, 152,
        132, 131, 136, 138, 154, 155, 156, 157,
        158, 159, 160, 161, 162, 163, 164, 151,
        135, 143, 147, 145, 133, 137, 139, 141,
        149, 153, 244, 245, 246, 247, 248, 249,
        250, 251, 252,
    };

    if (mouth_anim_id >= 0 && mouth_anim_id
        < sizeof(mouth_anim_id_to_mottbl_index_table) / sizeof(int32_t))
        return mouth_anim_id_to_mottbl_index_table[mouth_anim_id];
    return 131;
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
    file_handler.reset();
    load_count = 0;
}

void MhdFile::LoadFile(const char* path, const char* file, uint32_t set) {
    if (load_count > 0) {
        load_count++;
        return;
    }

    this->set = set;
    file_path.assign(path);
    file_path.append(file);
    if (file_handler.read_file(&data_list[DATA_AFT], path, file))
        file_handler.set_callback_data(0, (PFNFILEHANDLERCALLBACK*)ParseFileParent, this);
    load_count = 1;
}

void MhdFile::ParseFile(const void* data) {
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

    { // X
        const char* _file = strrchr(file_path.c_str(), '\\');
        if (!_file)
            _file = strrchr(file_path.c_str(), '/');

        if (_file) {
            _file++;

            size_t _file_len;
            const char* _file_end = strrchr(_file, '.');
            if (_file_end)
                _file_len = _file_end - _file;
            else
                _file_len = utf8_length(_file);

            std::string file(_file, _file_len);
            mothead_mot_msgpack_read("patch\\AFT\\rob", file.c_str(), mhd);
        }
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

void MhdFile::ParseFileParent(MhdFile* mhd, const void* file_data, size_t size) {
    mhd->ParseFile(file_data);
}

OpdMaker::Data::Data() : empty() {
    Reset();
}

OpdMaker::Data::~Data() {
    Reset();
}

bool OpdMaker::Data::CheckOpdiFilesNotReady() {
    for (auto i : opdi_files)
        if (i.second->check_not_ready())
            return true;
    return false;
}

const void* OpdMaker::Data::GetOpdiFileData(object_info obj_info, uint32_t motion_id) {
    auto elem = opdi_files.find({ obj_info, motion_id });
    if (elem != opdi_files.end())
        return elem->second->get_data();
    return 0;
}

bool OpdMaker::Data::IsValidOpdiFile(rob_chara* rob_chr, uint32_t motion_id) {
    if (!rob_chr)
        return false;

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++) {
        rob_chara_item_equip_object* itm_eq_obj = rob_itm_equip->get_item_equip_object((item_id)i);
        object_info obj_info = itm_eq_obj->obj_info;
        if (obj_info.not_null() && itm_eq_obj->osage_nodes_count) {
            size_t data = (size_t)GetOpdiFileData(obj_info, motion_id);
            if (!data
                || *((uint16_t*)(data + 0x04)) != (uint16_t)obj_info.id
                || *((uint16_t*)(data + 0x06)) != (uint16_t)obj_info.set_id
                || *((int32_t*)(data + 0x08)) != motion_id
                || *((uint16_t*)(data + 0x0C)) != itm_eq_obj->osage_nodes_count)
                return false;
        }
    }
    return true;
}

void OpdMaker::Data::Reset() {
    for (auto i : opdi_files)
        if (i.second) {
            i.second->reset();
            delete i.second;
            i.second = 0;
        }

    opdi_files.clear();
    empty = true;
}

OpdMaker::OpdMaker() {
    Reset();
}

OpdMaker::~OpdMaker() {
    if (thread) {
        thread->join();
        delete thread;
        thread = 0;
    }
    Reset();
}

static void sub_14053D450(struc_527* a1) {
    if (a1->type_62)
        delete a1->type_62;
    a1->type_62 = 0;
    a1->type_62_data = 0;

    if (a1->type_75)
        delete a1->type_75;
    a1->type_75 = 0;
    a1->type_75_data = 0;

    if (a1->type_67)
        delete a1->type_67;
    a1->type_67 = 0;
    a1->type_67_data = 0;

    if (a1->type_74)
        delete a1->type_74;
    a1->type_74 = 0;
    a1->type_74_data = 0;

    if (a1->type_79)
        delete a1->type_79;
    a1->type_79_data = 0;
    a1->type_79 = 0;
}

static void sub_14053D3E0(struc_527* a1) {
    a1->rob_chr = 0;
    a1->motion_id = -1;
    a1->frame = 0.0f;
    a1->last_frame = 0.0f;
    sub_14053D450(a1);
}

static void sub_14053D6C0(struc_527* a1, const motion_database* mot_db) {
    sub_14053D450(a1);
    struc_528* v3 = new struc_528;
    if (v3) {
        v3->data = 0;
        v3->type = MOTHEAD_DATA_ROB_PARTS_ADJUST;
        v3->data = mothead_storage_get_mot_by_motion_id(a1->motion_id, mot_db)->data.data();
        v3->field_0 = true;
    }
    else
        v3 = 0;
    a1->type_62 = v3;

    const mothead_data* v5 = v3->data;
    const mothead_data* v6 = 0;
    if (v5) {
        if (v3->field_0)
            v3->field_0 = false;
        else
            v3->data = v5 + 1;
        v6 = v3->data;
        if (!v6 || v6->type < 0) {
        LABEL_14:
            v3->data = 0;
            v6 = 0;
        }
        else {
            mothead_data_type v7 = v6->type;
            while (v7 != v3->type) {
                v6++;
                v7 = v6->type;
                if (v6->type < 0)
                    goto LABEL_14;
            }
            v3->data = v6;
        }
    }
    a1->type_62_data = v6;

    struc_528* v9 = new struc_528;
    if (v9) {
        v9->data = 0;
        v9->type = MOTHEAD_DATA_ROB_ADJUST_GLOBAL;
        v9->data = mothead_storage_get_mot_by_motion_id(a1->motion_id, mot_db)->data.data();
        v9->field_0 = true;
    }
    else
        v9 = 0;
    a1->type_75 = v9;

    const mothead_data* v11 = v9->data;
    const mothead_data* v12 = 0;
    if (v11) {
        if (v9->field_0)
            v9->field_0 = false;
        else
            v9->data = v11 + 1;

        v12 = v9->data;
        if (!v12 || v12->type < 0) {
            v9->data = 0;
            v12 = 0;
        }
        else {
            mothead_data_type v13 = v12->type;
            while (v13 != v9->type) {
                v12++;
                v13 = v12->type;
                if (v12->type < 0) {
                    v9->data = 0;
                    v12 = 0;
                    break;
                }
            }
            v9->data = v12;
        }
    }
    a1->type_75_data = v12;

    struc_528* v15 = new struc_528;
    if (v15) {
        v15->data = 0;
        v15->type = MOTHEAD_DATA_TYPE_67;
        v15->data = mothead_storage_get_mot_by_motion_id(a1->motion_id, mot_db)->data.data();
        v15->field_0 = true;
    }
    else
        v15 = 0;
    a1->type_67 = v15;

    const mothead_data* v17 = v15->data;
    const mothead_data* v18 = 0;
    if (v17) {
        if (v15->field_0)
            v15->field_0 = false;
        else
            v15->data = v17 + 1;

        v18 = v15->data;
        if (!v18 || v18->type < 0) {
            v15->data = 0;
            v18 = 0;
        }
        else {
            mothead_data_type v19 = v18->type;
            while (v19 != v15->type) {
                v18++;
                v19 = v18->type;
                if (v18->type < 0) {
                    v15->data = 0;
                    v18 = 0;
                    break;
                }
            }
            v15->data = v18;
        }
    }
    a1->type_67_data = v18;

    struc_528* v21 = new struc_528;
    if (v21) {
        v21->data = 0;
        v21->type = MOTHEAD_DATA_TYPE_74;
        v21->data = mothead_storage_get_mot_by_motion_id(a1->motion_id, mot_db)->data.data();
        v21->field_0 = true;
    }
    else
        v21 = 0;
    a1->type_74 = v21;

    const mothead_data* v23 = v21->data;
    const mothead_data* v24 = 0;
    if (v23) {
        if (v21->field_0)
            v21->field_0 = false;
        else
            v21->data = v23 + 1;

        v24 = v21->data;
        if (!v24 || v24->type < 0) {
            v21->data = 0;
            v24 = 0;
        }
        else {
            mothead_data_type v25 = v24->type;
            while (v25 != v21->type) {
                v24++;
                v25 = v24->type;
                if (v24->type < 0) {
                    v21->data = 0;
                    v24 = 0;
                    break;
                }
            }
            v21->data = v24;
        }
    }
    a1->type_74_data = v24;

    struc_528* v27 = new struc_528;
    if (v27) {
        v27->data = 0;
        v27->type = MOTHEAD_DATA_ROB_CHARA_COLI_RING;
        v27->data = mothead_storage_get_mot_by_motion_id(a1->motion_id, mot_db)->data.data();
        v27->field_0 = true;
    }
    else
        v27 = 0;
    a1->type_79 = v27;

    const mothead_data* v29 = v27->data;
    const mothead_data* v30 = 0;
    if (v29) {
        if (v27->field_0)
            v27->field_0 = false;
        else
            v27->data = v29 + 1;

        v30 = v27->data;
        if (!v30 || v30->type < 0) {
            v27->data = 0;
            v30 = 0;
        }
        else {
            mothead_data_type v31 = v30->type;
            while (v31 != v27->type) {
                v30++;
                v31 = v30->type;
                if (v30->type < 0) {
                    v27->data = 0;
                    v30 = 0;
                    break;
                }
            }
            v27->data = v30;
        }
    }
    a1->type_79_data = v30;
}

static void sub_14053E810(rob_chara* rob_chr, void* data) {
    rob_chara_data_adjust v16;
    v16.reset();

    int8_t type = ((int8_t*)data)[5];
    if (type >= 0 && type <= 5) {
        float_t set_frame = (float_t)v16.set_frame;
        v16.enable = true;
        v16.frame = rob_chr->data.motion.frame_data.frame - set_frame;
        v16.transition_frame = rob_chr->data.motion.frame_data.frame - set_frame;
        v16.motion_id = rob_chr->data.motion.motion_id;
        v16.set_frame = set_frame;
        v16.transition_duration = (float_t)((int32_t*)data)[0];
        v16.type = type;
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
        v16.strength_transition = (float_t)((int32_t*)data)[15];
    }
    else
        rob_chr->item_equip->reset_nodes_external_force((rob_osage_parts)((uint8_t*)data)[4]);

    rob_chara_set_parts_adjust_by_index(rob_chr, (rob_osage_parts)((uint8_t*)data)[4], &v16);
}

static void sub_14053EA00(rob_chara* rob_chr, void* data) {
    rob_chara_data_adjust v14;
    v14.reset();

    int8_t type = ((int8_t*)data)[4];
    if (type >= 0 && type <= 5) {
        float_t set_frame = (float_t)v14.set_frame;
        v14.enable = true;
        v14.frame = rob_chr->data.motion.frame_data.frame - set_frame;
        v14.transition_frame = rob_chr->data.motion.frame_data.frame - set_frame;
        v14.motion_id = rob_chr->data.motion.motion_id;
        v14.set_frame = set_frame;
        v14.transition_duration = (float_t)((int32_t*)data)[0];
        v14.type = type;
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

static void sub_14053EBE0(rob_chara* rob_chr, void* data) {
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

    rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;
    rob_bone_data->sleeve_adjust.enable1 = v6;
    rob_bone_data->sleeve_adjust.enable2 = v5;
    rob_bone_data->sleeve_adjust.radius = ((float_t*)data)[1];
}

static void sub_14053E7C0(rob_chara* rob_chr, void* data) {
    sub_140555120(rob_chr, (rob_osage_parts)((uint8_t*)data)[0], !!((uint8_t*)data)[1]);
}

static void sub_14053EC90(rob_chara* rob_chr, void* data) {
    //rob_chara_set_coli_ring(>rob_chr, ((int8_t*)data)[0]);
}

static void sub_14053E170(struc_527* a1) {
    while (a1->type_62_data) {
        if ((float_t)a1->type_62_data->frame > a1->frame)
            break;

        sub_14053E810(a1->rob_chr, a1->type_62_data->data);

        struc_528* v2 = a1->type_62;
        const mothead_data* v3 = v2->data;
        const mothead_data* v4 = 0;
        if (v3) {
            if (v2->field_0)
                v2->field_0 = 0;
            else
                v2->data = v3 + 1;

            v4 = v2->data;
            if (v4 && v4->type >= MOTHEAD_DATA_TYPE_0) {
                mothead_data_type v5 = v4->type;
                while (v5 != v2->type) {
                    v4++;
                    v5 = v4->type;
                    if (v5 < MOTHEAD_DATA_TYPE_0) {
                        v2->data = 0;
                        goto LABEL_1;
                    }
                }
                v2->data = v4;
            }
            else
                v2->data = 0;
        }
    LABEL_1:
        a1->type_62_data = v4;
    }

    while (a1->type_75_data) {
        if ((float_t)a1->type_75_data->frame > a1->frame)
            break;

        sub_14053EA00(a1->rob_chr, a1->type_75_data->data);

        struc_528* v6 = a1->type_75;
        const mothead_data* v7 = v6->data;
        const mothead_data* v8 = 0;
        if (v7) {
            if (v6->field_0)
                v6->field_0 = 0;
            else
                v6->data = v7 + 1;

            v8 = v6->data;
            if (v8 && v8->type >= MOTHEAD_DATA_TYPE_0) {
                mothead_data_type v9 = v8->type;
                while (v9 != v6->type) {
                    v8++;
                    v9 = v8->type;
                    if (v9 < MOTHEAD_DATA_TYPE_0) {
                        v6->data = 0;
                        goto LABEL_2;
                    }
                }
                v6->data = v8;
            }
            else
                v6->data = 0;
        }
    LABEL_2:
        a1->type_75_data = v8;
    }

    while (a1->type_67_data) {
        if ((float_t)a1->type_67_data->frame > a1->frame)
            break;

        sub_14053EBE0(a1->rob_chr, a1->type_67_data->data);

        struc_528* v10 = a1->type_67;
        const mothead_data* v11 = v10->data;
        const mothead_data* v12 = 0;
        if (v11) {
            if (v10->field_0)
                v10->field_0 = 0;
            else
                v10->data = v11 + 1;

            v12 = v10->data;
            if (v12 && v12->type >= MOTHEAD_DATA_TYPE_0) {
                mothead_data_type v13 = v12->type;
                while (v13 != v10->type) {
                    v12++;
                    v13 = v12->type;
                    if (v13 < MOTHEAD_DATA_TYPE_0) {
                        v10->data = 0;
                        goto LABEL_3;
                    }
                }
                v10->data = v12;
            }
            else
                v10->data = 0;
        }
    LABEL_3:
        a1->type_67_data = v12;
    }

    while (a1->type_74_data) {
        if ((float_t)a1->type_74_data->frame > a1->frame)
            break;

        sub_14053E7C0(a1->rob_chr, a1->type_74_data->data);

        struc_528* v14 = a1->type_74;
        const mothead_data* v15 = v14->data;
        const mothead_data* v16 = 0;
        if (v15) {
            if (v14->field_0)
                v14->field_0 = 0;
            else
                v14->data = v15 + 1;

            v16 = v14->data;
            if (v16 && v16->type >= MOTHEAD_DATA_TYPE_0) {
                mothead_data_type v17 = v16->type;
                while (v17 != v14->type) {
                    v16++;
                    v17 = v16->type;
                    if (v17 < MOTHEAD_DATA_TYPE_0) {
                        v14->data = 0;
                        goto LABEL_4;
                    }
                }
                v14->data = v16;
            }
            else
                v14->data = 0;
        }
    LABEL_4:
        a1->type_74_data = v16;
    }

    while (a1->type_79_data) {
        if ((float_t)a1->type_79_data->frame > a1->frame)
            return;

        sub_14053EC90(a1->rob_chr, a1->type_79_data->data);

        struc_528* v18 = a1->type_79;
        const mothead_data* v19 = v18->data;
        const mothead_data* v20 = 0;
        if (v19) {
            if (v18->field_0)
                v18->field_0 = 0;
            else
                v18->data = v19 + 1;

            v20 = v18->data;
            if (v20 && v20->type >= MOTHEAD_DATA_TYPE_0) {
                mothead_data_type v21 = v20->type;
                while (v21 != v18->type) {
                    v20++;
                    v21 = v20->type;
                    if (v21 < MOTHEAD_DATA_TYPE_0)
                        v18->data = 0;
                        goto LABEL_5;
                }
                v18->data = v20;
            }
            else
                v18->data = 0;
        }
    LABEL_5:
        a1->type_79_data = v20;
    }
}

static void sub_14053E7B0(struc_527* a1, float_t frame) {
    if (frame < a1->last_frame)
        a1->frame = frame;
}

static void sub_14053D360(struc_527* a1) {
    rob_chara* rob_chr = a1->rob_chr;
    if (!rob_chr)
        return;

    rob_chr->set_bone_data_frame(a1->frame);
    sub_14053E170(a1);
    a1->rob_chr->adjust_ctrl();
    a1->rob_chr->rob_motion_modifier_ctrl();
    rob_disp_rob_chara_ctrl(a1->rob_chr);
}

void OpdMaker::Ctrl() {
    /*unsigned __int64 v11; // rax
    float_t v12; // xmm0_4
    unsigned __int64 v13; // rdx
    unsigned __int64 v14; // rcx
    __int64 v15; // rbx
    unsigned __int64 v16; // rdi
    float_t v17; // xmm1_4
    float_t v18; // xmm0_4
    __int64 v19; // rbx
    float_t v20; // xmm0_4
    std::string* v21; // r8
    std::string* v22; // rbx
    std::string* i; // rbx
    size_t v24; // rax
    size_t v25; // rax
    unsigned __int64 v26; // rsi
    const char* v27; // rcx
    const char* v28; // rdi
    const char* v29; // rax
    char* v30; // rdi
    std::string* v31; // rdi
    std::string* v32; // r9
    std::string* v33; // r8
    std::string* v34; // r9
    std::string* v35; // r8
    std::string* v36; // rdi
    int32_t v37; // esi
    const char* v38; // rdi
    const char* v39; // rdi
    __int64 v40; // rcx
    unsigned __int8 v41; // al
    int32_t v42; // eax
    std::string* v43; // rax
    ssize_t v44; // rdi
    std::string* v45; // rdi
    std::string* v46; // r8
    size_t v47; // r15
    void* v48; // rsi
    std::string* v49; // rcx
    FILE* v50; // rax
    FILE* v51; // r14
    size_t v52; // rdi
    std::string* v53; // rcx
    FILE* v54; // rax
    FILE* v55; // rdi
    std::string* v56; // rbx
    std::string* v57; // rdi
    std::string v58; // [rsp+48h] [rbp-B8h] BYREF
    std::string v59; // [rsp+68h] [rbp-98h] BYREF
    std::string v60; // [rsp+88h] [rbp-78h] BYREF
    std::string v61; // [rsp+A8h] [rbp-58h] BYREF
    std::string v62; // [rsp+C8h] [rbp-38h] BYREF
    __int64 v63; // [rsp+E8h] [rbp-18h] BYREF
    std::string v65; // [rsp+108h] [rbp+8h] BYREF
    __int64 v66; // [rsp+128h] [rbp+28h] BYREF
    int32_t v67[4]; // [rsp+130h] [rbp+30h] BYREF

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    this->index = 0;
    int32_t chara_id = rob_chr->chara_id;
    sub_14048D9A0(chara_id);
    do {
        if (sub_140478330()) {
            SetWaiting(false);
            return;
        }

        uint32_t motion_id = (*motion_ids)[index];
        if (motion_id == -1)
            break;

        rob_chr->set_motion_id(motion_id, 0.0f, 0.0f, true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);

        float_t v6 = (float_t)(int32_t)rob_chr->bone_data->get_frame_count() - 1.0f;
        int32_t iterations = 60;
        if (rob_chr->data.field_1588.field_0.iterations > 0)
            iterations = rob_chr->data.field_1588.field_0.iterations;

        float_t v8 = 0.0f;
        bool v9 = true;
        if (data && data->IsValidOpdiFile(rob_chr, motion_id)) {
            v9 = false;
            v11 = 0;
            v12 = v6;
            if (v6 >= 9.223372e18) {
                v12 = v6 - 9.223372e18;
                if ((float_t)(v6 - 9.223372e18) < 9.223372e18)
                    v11 = 0x8000000000000000;
            }
            v13 = v11 + (uint32_t)(int32_t)v12;
            v14 = v13;
            v8 = v6 - 1.0;
            while (v14 < iterations) {
                v14 += v13;
                v8 = (float_t)((int32_t)v14 - 1);
                if ((__int64)(v14 - 1) < 0)
                    v8 = v8 + 1.8446744e19;
            }
        }

        struc_527 v68(rob_chr, 0, motion_id, v8, aft_bone_data, aft_mot_db);
        if (v9) {
            v15 = 0;
            v16 = 0;
            while (1) {
                v17 = (float_t)(int32_t)v15;
                if (v15 < 0)
                    v17 = v17 + 1.8446744e19;
                sub_14053E7B0(&v68, v17);
                sub_14053D360(&v68);
                v15++;
                v16++;
                v18 = (float_t)(int32_t)v15;
                if (v15 < 0)
                    v18 = v18 + 1.8446744e19;
                if (v18 >= v6) {
                    if (v16 > iterations)
                        goto LABEL_29;
                    v15 = 0;
                    sub_14053D6C0(&v68, aft_mot_db);
                }
                Sleep(0);
            }
        }
        sub_14053D360(&v68);
        SetOsagePlayInitData(motion_id);
    LABEL_29:
        sub_14053D6C0(&v68, aft_mot_db);
        //sub_14048E840(chara_id, motion_id);
        v19 = 0;
        v20 = (float_t)0;
        while (v20 < v6) {
            sub_14053E7B0(&v68, v20);
            sub_14053D360(&v68);
            sub_14048E050(chara_id);
            v20 = (float_t)(int32_t)++v19;
            if (v19 < 0)
                v20 = v20 + 1.8446744e19;
        }
        sub_14048D200(chara_id);
        sub_14053C720(&v68);
    } while (sub_140479610(this));

    sub_14048C920(chara_id);
    sub_140489250(&v62);
    v22 = sprintf_s_string(&v60, "%s/%d", v62.c_str(), chara_id);
    v62 = v60;

    std::vector<std::string> v64;
    if (!sub_1400AC0B0(&v62, &v64)) {
        for (std::string& i : v64) {
            v59.capacity = 15;
            v59.length = 0;
            v59.data.data[0] = 0;
            v24 = i.size();
            if (v24) {
                v25 = v24 - 1;
                v26 = i.capacity();
                v27 = i.data();
                for (v28 = v27 + v25; *v28 != '/'; --v28) {
                    if (v28 == sub_140007680(&i))
                        goto LABEL_136;
                }
                v29 = i.data();
                v30 = (char*)(j - v29);
                if (v30 != (char*)-1) {
                    v31 = sub_1400215A0(i, &v60, (ssize_t)(v30 + 1), -1);
                    v59 = v60;
                    sprintf_s_string(&v65, "%s/%s", v62.c_str(), v59.c_str());
                    sub_140489030(&v61);
                    sprintf_s_string(&v60, "%s/%s", v61.c_str(), v59.c_str());
                    v61 = v60;
                    if (stat_get_no_error(&v61))
                        sub_1400ACBE0(&v61);
                    v37 = stat_get_file_size(&v65);
                    if (v37 >= 0) {
                        v63 = 0;
                        if (v59.size() >= 5) {
                            v38 = v59.data();
                            for (v39 = v38 + v59.size() - 5; ; --v39) {
                                if (*v39 == '.') {
                                    v40 = 0;
                                    while (1) {
                                        v41 = v39[v40++];
                                        if (v41 != aFarc[v40 - 1])
                                            break;
                                        if (v40 == 5) {
                                            v42 = 0;
                                            goto LABEL_101;
                                        }
                                    }
                                    v42 = v41 < (uint32_t)aFarc[v40 - 1] ? -1 : 1;
                                LABEL_101:
                                    if (!v42)
                                        break;
                                }
                                if (v39 == sub_140007680(&v59))
                                    goto LABEL_130;
                            }

                            v44 = v39 - v59.data();
                            if (v44 != -1) {
                                sub_1400215A0(&v59, &v60, 0, v44);
                                v45 = string_init_to_upper(&v58, &v60);
                                if (&v60 != v45) {
                                    if (v60.capacity >= 0x10)
                                        operator delete(v60.data.ptr);
                                    v60.capacity = 15;
                                    v60.length = 0;
                                    v60.data.data[0] = 0;
                                    if (v45->capacity >= 0x10) {
                                        v60.data.ptr = v45->data.ptr;
                                        v45->data.ptr = 0;
                                    }
                                    else if (v45->length != -1) {
                                        memmove(&v60, v45, v45->length + 1);
                                    }
                                    v60.length = v45->length;
                                    v60.capacity = v45->capacity;
                                    v45->capacity = 15;
                                    v45->length = 0;
                                    v45->data.data[0] = 0;
                                }
                                if (v58.capacity >= 0x10)
                                    operator delete(v58.data.ptr);
                                sub_140472210((__int64*)&stru_1411A57C8, &v66, &v60);
                                LODWORD(v63) = v66;
                                if (v60.capacity >= 0x10)
                                    operator delete(v60.data.ptr);
                                v46 = &v65;
                                if (v65.capacity >= 0x10)
                                    v46 = (string*)v65.data.ptr;
                                v47 = v37;
                                v48 = HeapCMallocAllocate(HeapCMallocTemp, v37, (const char*)v46);
                                if (v48) {
                                    v49 = &v65;
                                    if (v65.capacity >= 0x10)
                                        v49 = (string*)v65.data.ptr;
                                    v50 = fopen((const char*)v49, "rb");
                                    v51 = v50;
                                    if (v50) {
                                        v52 = fread(v48, v47, 1u, v50);
                                        fclose(v51);
                                        if (v52) {
                                            sub_14008E1A0((__int64)v67);
                                            sub_14009E7A0((__int64)v67, (unsigned __int8*)v48, v47);
                                            HIDWORD(v63) = v67[0];
                                            HeapCMallocFree(HeapCMallocTemp, v48);
                                            v53 = &v65;
                                            if (v65.capacity >= 0x10)
                                                v53 = (string*)v65.data.ptr;
                                            v54 = fopen((const char*)v53, "ab");
                                            v55 = v54;
                                            if (v54) {
                                                fwrite(&v63, 8u, 1u, v54);
                                                fclose(v55);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                LABEL_130:
                }
            }
        LABEL_136:
            ;
        }
    }
    rob_chr = 0;
    index = 0;
    SetWaiting(false);*/
}

bool OpdMaker::InitThread(rob_chara* rob_chr, std::vector<int32_t>* motion_ids, OpdMaker::Data* data) {
    bool waiting = IsWaiting();
    if (waiting || rob_chr)
        return waiting;

    if (thread) {
        thread->join();
        delete thread;
        thread = 0;
    }
    Reset();

    this->rob_chr = rob_chr;
    this->motion_ids = motion_ids;
    this->data = data;
    SetWaiting(true);

    thread = new std::thread(OpdMaker::ThreadMain, this);
    if (thread) {
        wchar_t buf[0x80];
        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"OPD Maker #%d", opd_maker_counter++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf);
    }
    return true;
}

bool OpdMaker::IsWaiting() {
    std::unique_lock<std::mutex> u_lock(waiting_mtx);
    return waiting;
}

void OpdMaker::Reset() {
    rob_chr = 0;
    motion_ids = 0;
    waiting = false;
    index = 0;
    field_18 = false;
    data = 0;
    thread = 0;
}

bool OpdMaker::SetOsagePlayInitData(uint32_t motion_id) {
    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++) {
        rob_chara_item_equip_object* itm_eq_obj = rob_itm_equip->get_item_equip_object((item_id)i);
        if (itm_eq_obj->obj_info.is_null() || !itm_eq_obj->osage_nodes_count)
            continue;

        size_t data = (size_t)this->data->GetOpdiFileData(itm_eq_obj->obj_info, motion_id);
        if (data)
            rob_itm_equip->set_osage_play_data_init((item_id)i, (float_t*)(data + 0x10));
    }
    return true;
}

void OpdMaker::SetWaiting(bool value) {
    std::unique_lock<std::mutex> u_lock(waiting_mtx);
    waiting = value;
}

void OpdMaker::sub_140475AE0() {
    std::unique_lock<std::mutex> u_lock(field_40);
    field_18 = true;
}

bool OpdMaker::sub_140478330() {
    std::unique_lock<std::mutex> u_lock(field_40);
    return field_18;
}

void OpdMaker::ThreadMain(OpdMaker* opd_maker) {
    opd_maker->Ctrl();
}

OpdMakeWorker::OpdMakeWorker() : state(), chara_id(), field_70(), field_D4() {

}

OpdMakeWorker::~OpdMakeWorker() {

}

bool OpdMakeWorker::Init() {
    data.Reset();
    this->state = field_D4 ? 8 : 1;
    return true;
}

static void sub_140479950(struc_655* a1, chara_index chara_index, int32_t* a3) {
    std::vector<uint32_t>* v4 = a1->field_10[chara_index].field_0;
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++, v4++) {
        a3[i] = 0;
        if (!v4->size())
            continue;

        a3[i] = v4->back();
        v4->pop_back();
        a1->count--;
    }
}

static void sub_140479AA0(OpdMaker::Data* a1, rob_chara* rob_chr, std::vector<int32_t>& motions,
    void* data, const object_database* obj_db, const motion_database* mot_db) {
    if (!a1->empty || !rob_chr)
        return;

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++) {
        rob_chara_item_equip_object* itm_eq_obj = rob_itm_equip->get_item_equip_object((item_id)i);
        if (itm_eq_obj->obj_info.is_null() || !itm_eq_obj->osage_nodes_count)
            continue;

        object_info obj_info = itm_eq_obj->obj_info;
        for (int32_t& i : motions) {
            std::pair<object_info, int32_t> v24 = { obj_info, i };
            auto elem = a1->opdi_files.insert({ v24, 0 }).first;

            const char* object_name = obj_db->get_object_name(obj_info);
            const char* motion_name = mot_db->get_motion_name(i);
            if (object_name && motion_name) {
                char buf1[0x200];
                char buf2[0x200];
                sprintf_s(buf1, sizeof(buf1), "%s.opdi", object_name);
                sprintf_s(buf2, sizeof(buf2), "%s_%s.opdi", object_name, motion_name);

                for (char* j = buf1; *j; j++)
                    if (*j >= 'A' && *j <= 'Z')
                        *j += 0x20;

                for (char* j = buf2; *j; j++)
                    if (*j >= 'A' && *j <= 'Z')
                        *j += 0x20;

                p_file_handler* file_handler = new p_file_handler;
                file_handler->read_file(data, "rom/osage_play_data/opdi", buf1, buf2, true);
                elem->second = file_handler;
            }
        }
    }
    a1->empty = false;
}

static bool sub_140479090(struc_655& a1, chara_index chara_index) {
    std::vector<uint32_t>* v4 = a1.field_10[chara_index].field_0;
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++, v4++)
        if (v4->size())
            return false;
    return true;
}

bool OpdMakeWorker::Ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    chara_index chara_index = rob_chr->chara_index;
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    switch (state) {
    case 1: {
        sub_140479950(&opd_make_manager->field_88, chara_index, field_70);

        int32_t* v7 = field_70;
        for (int32_t i = 0; i < ITEM_SUB_MAX; i++, v7++) {
            rob_chr->item_cos_data.set_chara_index(chara_index);
            rob_chr->item_cos_data.set_item(*v7);
        }

        rob_chr->item_equip->reset_init_data(rob_chara_bone_data_get_node(
            rob_chara_array[chara_id].bone_data, MOT_BONE_N_HARA_CP));
        rob_chr->item_equip->set_shadow_type(chara_id);
        rob_chr->item_equip->field_A0 = 0x05;
        state = 2;
    }
    case 2: {
        if (!task_rob_load_check_load_req_data()) {
            task_rob_load_append_load_req_data_obj(chara_index, rob_chr->item_cos_data.get_cos());
            state = 3;
        }
    } return false;
    case 3: {
        if (task_rob_load_check_load_req_data())
            return false;

        rob_chr->item_cos_data.reload_items(chara_id, aft_bone_data, aft_data, aft_obj_db);

        for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++) {
            rob_chara_item_equip_object* itm_eq_obj = rob_chr->item_equip->get_item_equip_object((item_id)i);
            if (aft_obj_db->get_object_name(itm_eq_obj->obj_info) && itm_eq_obj->osage_nodes_count) {
                int32_t j = ITEM_ATAMA;
                const object_info* k = chr_init_data->field_7E4;
                object_info v14 = itm_eq_obj->obj_info;
                for (; *k != v14; j++, k++) {
                    if (j < ITEM_KAMI)
                        continue;

                    if (opd_make_manager->field_1889)
                        state = 4;
                    else
                        state = 6;
                    return false;
                }
            }
        }
        state = 10;
    } break;
    case 4: {
        sub_140479AA0(&data, rob_chr, opd_make_manager->motion_ids, aft_data, aft_obj_db, aft_mot_db);
        state = 5;
    } break;
    case 5: {
        if (data.CheckOpdiFilesNotReady())
            return false;
        state = 6;
    }
    case 6: {
        std::vector<osage_init_data> osage_init;
        for (int32_t& i : opd_make_manager->motion_ids)
            osage_init.push_back(osage_init_data(rob_chr, i));
        skin_param_manager_add_task(chara_id, osage_init);

        state = 7;
    } break;
    case 7: {
        if (skin_param_manager_check_task_ready(chara_id))
            return false;
        state = 8;
    }
    case 8: {
        OpdMaker::Data* v28 = 0;
        if (opd_make_manager->field_1889)
            v28 = &data;
        opd_maker_array[chara_id].InitThread(rob_chr, &opd_make_manager->motion_ids, v28);
        state = 9;
    } break;
    case 9: {
        if (opd_maker_array[chara_id].IsWaiting())
            return false;

        data.Reset();

        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "ram/osage_play_data_tmp/%d", chara_id);

        std::vector<std::string> files;
        for (std::string& i : files) {
            size_t v36 = i.find('/');
            if (v36 == -1)
                continue;

            const char* v57 = i.c_str() + v36 + 1;

            char buf1[0x200];
            char buf2[0x200];
            char buf3[0x200];
            sprintf_s(buf1, sizeof(buf1), "%s/%s", buf, v57);
            sprintf_s(buf2, sizeof(buf2), "ram/osage_play_data_tmp/%s.fs_copy_file.tmp", v57);
            sprintf_s(buf3, sizeof(buf3), "ram/osage_play_data_tmp/%s", v57);

            if (!path_check_file_exists(buf1))
                continue;

            if (path_check_directory_exists("ram/osage_play_data_tmp/") && path_check_file_exists(buf2)) {
                if (path_check_file_exists(buf3))
                    DeleteFileA(buf3);

                if (!rename(buf2, buf3) && path_check_file_exists(buf2))
                    DeleteFileA(buf2);
            }

            DeleteFileA(buf1);
        }

        if (field_D4)
            return true;
        state = 10;
    }
    case 10: {
        task_rob_load_append_free_req_data_obj(chara_index, rob_chr->item_cos_data.get_cos());

        int32_t* v45 = field_70;
        for (int32_t i = 0; i < ITEM_SUB_MAX; i++, v45++) {
            rob_chr->item_cos_data.set_chara_index(chara_index);
            rob_chr->item_cos_data.set_item_zero(*v45);
        }

        rob_chr->item_cos_data.reload_items(chara_id, aft_bone_data, aft_data, aft_obj_db);
        skin_param_manager_reset(chara_id);
        if (sub_140479090(opd_make_manager->field_88, chara_index))
            return true;
        state = 1;
    } break;
    }
    return false;
}

bool OpdMakeWorker::Dest() {
    if (opd_maker_array[chara_id].IsWaiting()) {
        opd_maker_array[chara_id].sub_140475AE0();
        return false;
    }
    else {
        opd_maker_array[chara_id].Reset();
        return true;
    }
}

void OpdMakeWorker::Disp() {

}

osage_play_data_header::osage_play_data_header() : frame_count(), nodes_count() {
    signature = 0x11033115;
    obj_info = { -1, -1 };
    motion_id = -1;
}

void opd_file_data::unload() {
    if (--load_count > 0)
        return;

    head = {};
    free_def(data);
}

OsagePlayDataManager::OsagePlayDataManager() : state() {
    Reset();
    state = 0;
}

OsagePlayDataManager::~OsagePlayDataManager() {

}

bool OsagePlayDataManager::Init() {
    LoadOpdFileList();
    state = 0;
    return true;
}

bool OsagePlayDataManager::Ctrl() {
    if (state)
        return false;

    for (auto i = file_handlers.begin(); i != file_handlers.end();) {
        if (!(*i)->check_not_ready()) {
            LoadOpdFile(*i);
            i = file_handlers.erase(i);
        }
        else
            i++;
    }

    return !file_handlers.size();
}

bool OsagePlayDataManager::Dest() {
    return true;
}

void OsagePlayDataManager::Disp() {

}

bool OpdMakeWorker::AddTask(bool a2) {
    if (!task_rob_manager_check_chara_loaded(chara_id))
        return false;

    field_D4 = a2;
    return app::TaskWork::AddTask(this, "OPD_MAKE_WORKER");
}

bool OsagePlayDataManager::AddTask() {
    return app::TaskWork::AddTask(this, "OSAGE_PLAY_DATA_MANAGER");
}

void OsagePlayDataManager::AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids) {
    if (CheckTaskReady() || !rob_chr)
        return;

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    for (int32_t i = ITEM_KAMI; i < ITEM_ITEM16; i++) {
        rob_chara_item_equip_object* itm_eq_obj = rob_itm_equip->get_item_equip_object((item_id)i);
        if (!itm_eq_obj || itm_eq_obj->obj_info.is_null()
            || (!itm_eq_obj->osage_blocks.size() && !itm_eq_obj->cloth_blocks.size()))
            continue;

        for (uint32_t j : motion_ids)
            opd_req_data.push_back(itm_eq_obj->obj_info, j);
    }
}

bool OsagePlayDataManager::CheckTaskReady() {
    return app::TaskWork::CheckTaskReady(this);
}

void OsagePlayDataManager::GetOpdFileData(object_info obj_info,
    uint32_t motion_id, const float_t*& data, uint32_t& count) {
    data = 0;
    count = 0;

    auto elem = opd_file_data.find({ obj_info, motion_id });
    if (elem != opd_file_data.end()) {
        data = elem->second.data;
        count = elem->second.head.frame_count;
    }
}

void OsagePlayDataManager::LoadOpdFile(p_file_handler* pfhndl) {
    ::opd_file_data data = {};
    if (opd_decode_data(pfhndl->get_data(), &data.data, &data.head))
        opd_file_data.insert({ { object_info(data.head.obj_info.first,
            data.head.obj_info.second), data.head.motion_id }, data });
}

void OsagePlayDataManager::LoadOpdFileList() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    prj::sort_unique(opd_req_data);
    for (const std::pair<object_info, int32_t>& i : opd_req_data) {
        auto elem = opd_file_data.find(i);
        if (elem != opd_file_data.end())
            continue;

        const char* object_name = aft_obj_db->get_object_name(i.first);
        if (!object_name)
            continue;

        const char* motion_name = aft_mot_db->get_motion_name(i.second);
        if (!motion_name)
            continue;

        char obj_name_buf[0x200];
        strcpy_s(obj_name_buf, sizeof(obj_name_buf), object_name);

        for (int32_t i = 0; obj_name_buf[i]; i++) {
            char c = obj_name_buf[i];
            if (c >= 'A' && c <= 'Z')
                c += 0x20;
            obj_name_buf[i] = c;
        }

        char mot_name_buf[0x200];
        strcpy_s(mot_name_buf, sizeof(mot_name_buf), motion_name);

        for (int32_t i = 0; mot_name_buf[i]; i++) {
            char c = mot_name_buf[i];
            if (c >= 'A' && c <= 'Z')
                c += 0x20;
            mot_name_buf[i] = c;
        }

        char file_buf[0x200];
        sprintf_s(file_buf, sizeof(file_buf), "%s_%s.%s", obj_name_buf, mot_name_buf, "opd");

        char farc_buf[0x200];
        sprintf_s(farc_buf, sizeof(farc_buf), "%s.farc", obj_name_buf);

        if (aft_data->check_file_exists("rom/osage_play_data/", farc_buf)) {
            file_handlers.push_back(new p_file_handler);
            file_handlers.back()->read_file(aft_data, "rom/osage_play_data/", farc_buf, file_buf, true);
        }
    }
    opd_req_data.clear();
}

void OsagePlayDataManager::Reset() {
    file_handlers.clear();
    opd_req_data.clear();
    opd_file_data.clear();
}

struc_527::struc_527(rob_chara* rob_chr, int32_t stage_index, uint32_t motion_id,
    float_t frame, const bone_database* bone_data, const motion_database* mot_db) : rob_chr(),
    motion_id(), frame(), last_frame(), type_62(), type_62_data(), type_75(), type_75_data(),
    type_67(), type_67_data(), type_74(), type_74_data(), type_79(), type_79_data(), field_68() {
    type_62 = 0;
    type_75 = 0;
    type_67 = 0;
    type_74 = 0;
    type_79 = 0;
    this->rob_chr = rob_chr;
    if (rob_chr) {
        rob_chr->set_motion_id(motion_id, frame, 0.0f,
            true, false, MOTION_BLEND_CROSS, bone_data, mot_db);
        this->frame = frame;
        this->motion_id = motion_id;
        last_frame = rob_chr->bone_data->get_frame_count() - 1.0f;
        rob_chr->reset_osage();
        rob_chr->set_bone_data_frame(frame);
        rob_chara_item_equip_ctrl_iterate_nodes(rob_chr->item_equip);
        sub_14053D6C0(this, mot_db);
        sub_14053E170(this);
    }
    else
        sub_14053D3E0(this);

}

struc_527::~struc_527() {
    sub_14053D3E0(this);
}

struc_656::struc_656() {

}

struc_656::~struc_656() {

}

struc_655::struc_655() : count(), index() {

}

struc_655::~struc_655() {

}

struc_657::struc_657() : mode(), progress() {
    chara = CHARA_MAX;
}

struc_657::~struc_657() {

}

struc_691::struc_691() : mode(), count(), index() {

}

struc_691::~struc_691() {

}

OpdMakeManager::OpdMakeManager() : mode(), workers(), field_1888(), field_1889() {
    chara = CHARA_MAX;
}

OpdMakeManager::~OpdMakeManager() {

}

bool OpdMakeManager::Init() {
    mode = 1;

    rctx_ptr->render_manager.set_pass_sw(rndr::RND_PASSID_ALL_3D, false);

    if (path_check_directory_exists("ram/osage_play_data"))
        RemoveDirectoryA("ram/osage_play_data");

    CreateDirectoryA("ram/osage_play_data", 0);

    if (field_1888 || !app::TaskWork::CheckTaskReady(task_rob_manager)) {
        task_rob_manager_add_task();
        return true;
    }
    else {
        task_rob_manager_del_task();
        return false;
    }
}

bool OpdMakeManager::Ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    bool ret = false;
    switch (mode) {
    case 1:
        mode = 3;
        break;
    case 3:
        motion_set_ids.clear();
        for (int32_t& i : motion_ids) {
            uint32_t set_id = aft_mot_db->get_motion_set_id_by_motion_id(i);
            if (set_id != -1)
                motion_set_ids.push_back(set_id);
        }

        prj::sort_unique(motion_set_ids);

        for (uint32_t& i : motion_set_ids) {
            motion_set_load_motion(i, "", aft_mot_db);
            motion_set_load_mothead(i, "", aft_mot_db);
        }
        mode = 4;
        break;
    case 4: {
        bool wait = false;
        for (uint32_t i : motion_set_ids)
            if (motion_storage_check_mot_file_not_ready(i) || mothead_storage_check_mhd_file_not_ready(i))
                wait = true;

        if (wait)
            break;

        chara = CHARA_MIKU;
        mode = 5;
        break;
    } break;
    case 5: {
        bool v8 = false;
        for (int32_t i = 0; i < CHARA_MAX; i++)
            if (!sub_140479090(field_88, (chara_index)i)) {
                chara = (chara_index)i;
                v8 = true;
                break;
            }

        if (v8) {
            for (int32_t i = 0; i < 4; i++) {
                rob_chara_pv_data pv_data;
                pv_data.type = ROB_CHARA_TYPE_3;
                rob_chara_array_init_chara_index(chara, pv_data, 499, false);
            }
            mode = 6;
        }
        else
            mode = 11;
    } break;
    case 6: {
        bool v11 = false;
        for (int32_t i = 0; i < 4; i++)
            if (!task_rob_manager_check_chara_loaded(i))
                v11 = true;

        if (!v11) {
            task_rob_manager->HideTask();
            mode = 7;
        }
    } break;
    case 7: {
        int32_t j = 0;
        for (OpdMakeWorker*& i : workers)
            if (rob_chara_array_get(j++))
                i->AddTask(field_1888);
        mode = 8;
    } break;
    case 8: {
        bool v18 = false;
        for (OpdMakeWorker*& i : workers)
            if (app::TaskWork::CheckTaskReady(i))
                v18 = true;

        if (!v18)
            mode = field_1888 ? 12 : 9;
    } break;
    case 9:
        for (int32_t i = 0; i < 4; i++)
            rob_chara_array_free_chara_id(i);
        task_rob_manager->RunTask();
        mode = 10;
        break;
    case 10:
        mode = 5;
        break;
    case 11:
        for (uint32_t& i : motion_set_ids) {
            motion_set_unload_motion(i);
            motion_set_unload_mothead(i);
        }
        motion_set_ids.clear();
        mode = 12;
        break;
    case 12:
        ret = true;
        break;
    default:
        break;
    }
    field_1860.mode = mode;
    field_1860.count = (uint32_t)field_88.count;
    field_1860.index = field_88.index;
    return ret;
}

bool OpdMakeManager::Dest() {
    for (OpdMakeWorker*& i : workers)
        if (app::TaskWork::CheckTaskReady(i))
            return false;

    if (!field_1888) {
        for (int32_t i = 0; i < 4; i++)
            rob_chara_array_free_chara_id(i);

        for (int32_t i = 0; i < ROB_CHARA_COUNT; i++)
            skin_param_manager_reset(i);

        for (uint32_t& i : motion_set_ids) {
            motion_set_unload_motion(i);
            motion_set_unload_mothead(i);
        }

        task_rob_manager_del_task();
    }

    rctx_ptr->render_manager.set_pass_sw(rndr::RND_PASSID_ALL_3D, true);

    if (path_check_directory_exists("ram/osage_play_data"))
        RemoveDirectoryA("ram/osage_play_data");

    return true;
}

void OpdMakeManager::Disp() {

}

bool OpdMakeManager::DelTask() {
    for (OpdMakeWorker*& i : workers)
        i->DelTask();
    return app::Task::DelTask();
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

rob_chara_age_age_data::rob_chara_age_age_data() : part_id(), field_14(),
rot_z(), field_1C(), rot_speed(), gravity(), alpha(), alive() {
    index = -1;
    scale = 1.0f;
    remaining = -1.0f;
}

void rob_chara_age_age_data::reset() {
    index = -1;
    part_id = 0;
    trans = 0.0f;
    field_14 = 0.0f;
    rot_z = 0.0f;
    field_1C = 0;
    rot_speed = 0.0f;
    gravity = 0.0f;
    scale = 1.0f;
    alpha = 0.0f;
    mat_scale = mat4_null;
    mat = mat4_null;
    prev_parent_mat;
    remaining = -1.0f;
    alive = false;
}

rob_chara_age_age_object::rob_chara_age_age_object() : obj_set_handler(),
obj_index(), field_C(), num_vertex(), num_index(), vertex_data(),
vertex_data_size(), vertex_array_size(), disp_count(), count(), field_C3C() {

}

void rob_chara_age_age_object::calc_vertex(rob_chara_age_age_object_vertex*& vtx_data,
    obj_mesh* mesh, const mat4& mat, float_t alpha) {
    const float_t alpha_1 = 1.0f - alpha;

    obj_vertex_data* vertex_array = mesh->vertex_array;
    uint32_t num_vertex = mesh->num_vertex;
    obj_vertex_data* obj_vtx = vertex_array;
    rob_chara_age_age_object_vertex* vtx = vtx_data;
    for (uint32_t i = num_vertex; i; i--, obj_vtx++, vtx++) {
        mat4_mult_vec3_trans(&mat, &obj_vtx->position, &vtx->position);
        mat4_mult_vec3(&mat, &obj_vtx->normal, &vtx->normal);
        mat4_mult_vec3(&mat, (vec3*)&obj_vtx->tangent, (vec3*)&vtx->tangent);
        vtx->tangent.w = obj_vtx->tangent.w;
        vtx->texcoord.x = obj_vtx->texcoord0.x + alpha_1;
        vtx->texcoord.y = obj_vtx->texcoord0.y;
    }
    vtx_data = vtx;
}

void rob_chara_age_age_object::disp(render_context* rctx, size_t chara_index,
    bool npr, bool reflect, const vec3& a5, bool chara_color) {
    int32_t disp_count = this->disp_count;
    if (!obj_set_handler || !disp_count)
        return;

    disp_count = min_def(disp_count, 10);

    std::pair<float_t, int32_t> v44[10];
    for (int32_t i = 0; i < disp_count; i++) {
        v44[i].first = vec3::dot(trans[i], a5);
        v44[i].second = i;
    }

    if (disp_count >= 2)
        std::sort(v44, v44 + disp_count,
            [](const std::pair<float_t, int32_t>& a, const std::pair<float_t, int32_t>& b) {
                return a.first < b.first;
            });

    obj_vert_buf.cycle_index();

    GLuint buffer = obj_vert_buf.get_buffer();
    size_t vtx_data;
    if (GLAD_GL_VERSION_4_5) {
        vtx_data = (size_t)glMapNamedBuffer(buffer, GL_WRITE_ONLY);
        if (!vtx_data) {
            glUnmapNamedBuffer(buffer);
            return;
        }
    }
    else {
        gl_state_bind_array_buffer(buffer);
        vtx_data = (size_t)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (!vtx_data) {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            gl_state_bind_array_buffer(0);
            return;
        }
    }

    size_t vertex_array_size = this->vertex_array_size;
    for (int32_t i = 0; i < disp_count; i++)
        memmove((void*)(vtx_data + vertex_array_size * i),
            (void*)((size_t)vertex_data + vertex_array_size * v44[i].second), vertex_array_size);

    if (GLAD_GL_VERSION_4_5)
        glUnmapNamedBuffer(buffer);
    else {
        glUnmapBuffer(GL_ARRAY_BUFFER);
        gl_state_bind_array_buffer(0);
    }

    mesh.num_vertex = disp_count * num_vertex;
    sub_mesh.num_index = disp_count * num_index;
    sub_mesh.material_index = npr ? 1 : 0;

    mdl::ObjFlags flags = (mdl::ObjFlags)(mdl::OBJ_SSS | mdl::OBJ_4 | mdl::OBJ_SHADOW);
    if (reflect)
        enum_or(flags, mdl::OBJ_CHARA_REFLECT);
    rctx->disp_manager.set_obj_flags(flags);
    rctx->disp_manager.set_chara_color(chara_color);
    rctx->disp_manager.set_shadow_type(chara_index ? SHADOW_STAGE : SHADOW_CHARA);
    rctx->disp_manager.entry_obj_by_obj(&mat4_identity, &obj,
        get_obj_set_texture(), &obj_vert_buf, &obj_index_buf, 0, 1.0f);
}

bool rob_chara_age_age_object::get_obj_set_handler_object_index(object_info obj_info) {
    obj_set_handler = object_storage_get_obj_set_handler(obj_info.set_id);
    if (!obj_set_handler)
        return false;

    auto elem = obj_set_handler->obj_id_data.find(obj_info.id);
    if (elem == obj_set_handler->obj_id_data.end())
return false;

obj_index = elem->second;
return true;
}

::obj* rob_chara_age_age_object::get_obj_set_obj() {
    return &obj_set_handler->obj_set->obj_data[obj_index];
}

std::vector<texture*>* rob_chara_age_age_object::get_obj_set_texture() {
    return &obj_set_handler->gentex;
}

void rob_chara_age_age_object::load(object_info obj_info, int32_t count) {
    reset();

    this->count = count;
    num_vertex = 0;
    num_index = 0;
    disp_count = 0;

    if (!get_obj_set_handler_object_index(obj_info)) {
        obj_index = -1;
        obj_set_handler = 0;
        return;
    }
    else {
        ::obj* o = get_obj_set_obj();
        if (o->num_mesh != 1 || o->mesh_array[0].num_submesh != 1
            || o->mesh_array[0].submesh_array[0].index_format != OBJ_INDEX_U16) {
            obj_index = -1;
            obj_set_handler = 0;
            return;
        }
    }

    if (!obj_set_handler)
        return;

    ::obj* o = get_obj_set_obj();
    obj_mesh* m = &o->mesh_array[0];
    obj_sub_mesh* sm = &m->submesh_array[0];

    uint32_t num_vertex = m->num_vertex;
    this->num_vertex = num_vertex;

    uint32_t num_index = sm->num_index;
    this->num_index = num_index;

    size_t vertex_array_size = sizeof(rob_chara_age_age_object_vertex) * num_vertex;
    this->vertex_array_size = (uint32_t)vertex_array_size;

    size_t vertex_data_size = vertex_array_size * count;
    this->vertex_data_size = (uint32_t)vertex_data_size;

    rob_chara_age_age_object_vertex* vtx_data = force_malloc_s(
        rob_chara_age_age_object_vertex, vertex_data_size);
    vertex_data = vtx_data;

    obj_vertex_data* vertex_array = m->vertex_array;
    for (int32_t i = count; i > 0; i--)
        for (uint32_t j = num_vertex, k = 0; j; j--, k++, vtx_data++) {
            vtx_data->position = vertex_array[k].position;
            vtx_data->normal = vertex_array[k].normal;
            vtx_data->tangent = vertex_array[k].tangent;
            vtx_data->texcoord = vertex_array[k].texcoord0;
        }

    obj_vert_buf.load_data(vertex_data_size, vertex_data, 2, true);

    this->num_index = num_index + 1;
    uint32_t num_idx_data = (uint32_t)(count * (num_index + 1));

    uint16_t* idx_data = force_malloc_s(uint16_t, num_idx_data);
    uint32_t* index_array = sm->index_array;
    uint32_t index_offset = 0;
    uint32_t l = 0;
    for (int32_t i = count; i > 0; i--) {
        for (uint32_t j = num_index, k = 0; j; j--, k++)
            if (index_array[k] == 0xFFFFFFFF)
                idx_data[l++] = 0xFFFF;
            else
                idx_data[l++] = (uint16_t)(index_offset + index_array[k]);
        idx_data[l++] = 0xFFFF;
        index_offset += num_vertex;
    }
    obj_index_buf.load_data(sizeof(uint16_t) * num_idx_data, idx_data);
    free_def(idx_data);

    material[0] = o->material_array[0];
    material[1] = o->material_array[0];
    material[1].material.attrib.m.alpha_texture = 0;
    material[1].material.attrib.m.alpha_material = 0;

    obj.bounding_sphere = o->bounding_sphere;
    obj.mesh_array = &mesh;
    obj.num_mesh = 1;
    obj.material_array = material;
    obj.num_material = 2;
    obj.flags = o->flags;
    memmove(obj.reserved, o->reserved, sizeof(uint32_t) * 10);
    obj.skin = 0;
    obj.name = o->name;
    obj.id = o->id;
    obj.hash = o->hash;

    mesh.flags = m->flags;
    mesh.bounding_sphere = m->bounding_sphere;
    mesh.num_submesh = 1;
    mesh.submesh_array = &sub_mesh;
    mesh.vertex_format = (obj_vertex_format)(OBJ_VERTEX_POSITION | OBJ_VERTEX_NORMAL
        | OBJ_VERTEX_TANGENT | OBJ_VERTEX_TEXCOORD0 | OBJ_VERTEX_TEXCOORD1);
    mesh.size_vertex = sizeof(rob_chara_age_age_object_vertex);
    mesh.num_vertex = m->num_vertex;
    mesh.vertex_array = 0;
    mesh.attrib = m->attrib;
    memmove(mesh.reserved, m->reserved, sizeof(uint32_t) * 6);
    memmove(mesh.name, m->name, 0x40);

    sub_mesh.flags = sm->flags;
    sub_mesh.material_index = 0;
    sub_mesh.bounding_sphere = sm->bounding_sphere;
    memmove(sub_mesh.uv_index, sm->uv_index, sizeof(uint8_t) * 8);
    sub_mesh.bone_index_array = 0;
    sub_mesh.num_bone_index = 0;
    sub_mesh.bones_per_vertex = 0;
    sub_mesh.primitive_type = sm->primitive_type;
    sub_mesh.index_format = OBJ_INDEX_U16;
    sub_mesh.index_array = sm->index_array;
    sub_mesh.num_index = num_idx_data;
    sub_mesh.attrib = sm->attrib;
    sub_mesh.axis_aligned_bounding_box = sm->axis_aligned_bounding_box;
    sub_mesh.first_index = 0;
    sub_mesh.last_index = (uint16_t)((size_t)num_vertex * count);
    sub_mesh.index_offset = 0;

    axis_aligned_bounding_box = sm->axis_aligned_bounding_box;
}

void rob_chara_age_age_object::reset() {
    obj_index_buf.unload();
    obj_vert_buf.unload();

    if (vertex_data) {
        free(vertex_data);
        vertex_data = 0;
    }

    vertex_data_size = 0;
    num_vertex = 0;
    num_index = 0;
    obj_set_handler = 0;
    obj_index = -1;
}

void rob_chara_age_age_object::update(rob_chara_age_age_data* data, int32_t count, float_t alpha) {
    if (!obj_set_handler)
        return;

    ::obj* o = get_obj_set_obj();
    obj_mesh* m = &o->mesh_array[0];
    obj_sub_mesh* sm = &m->submesh_array[0];

    rob_chara_age_age_object_vertex* vtx_data = vertex_data;

    int32_t disp_count = 0;
    float_t scale = 0.0f;
    for (int32_t i = count; i > 0; i--, data++)
        if (data->remaining >= 0.0f && data->alive) {
            calc_vertex(vtx_data, m, data->mat_scale, alpha * data->alpha);
            mat4_get_translation(&data->mat_scale, &trans[disp_count]);
            scale = max_def(scale, data->scale);
            disp_count++;
        }
    this->disp_count = disp_count;

    if (disp_count < 1)
        return;

    float_t radius = scale * sm->bounding_sphere.radius;

    obj_bounding_sphere v75;
    v75.center = trans[0];
    v75.radius = radius;

    for (int32_t i = 1; i < disp_count; i++) {
        obj_bounding_sphere v74;
        v74.center = trans[i];
        v74.radius = radius;
        v75 = combine_bounding_spheres(&v75, &v74);
    }

    sub_mesh.bounding_sphere = v75;
    mesh.bounding_sphere = v75;
    obj.bounding_sphere = v75;

    vec3 min = trans[0] - radius;
    vec3 max = trans[0] + radius;
    for (int32_t i = 1; i < disp_count; i++) {
        min = vec3::min(min, trans[i] - radius);
        max = vec3::max(max, trans[i] + radius);
    }
    sub_mesh.axis_aligned_bounding_box.center = (max + min) * 0.5f;
    sub_mesh.axis_aligned_bounding_box.size = (max - min) * 0.5f;
}

obj_bounding_sphere rob_chara_age_age_object::combine_bounding_spheres(
    obj_bounding_sphere* src0, obj_bounding_sphere* src1) {
    float_t radius_diff = src1->radius - src0->radius;
    float_t dist = vec3::distance_squared(src0->center, src1->center);
    if (dist >= radius_diff * radius_diff) {
        if (dist > 0.0f) {
            dist = sqrtf(dist);

            return {
                (src1->center - src0->center) * ((src1->radius - src0->radius
                    + dist) * 0.5f / dist) + src0->center,
                (src1->radius + src0->radius + dist) * 0.5f
            };
        }
        else
            return { src0->center,  max_def(src1->radius, src0->radius) };
    }
    else {
        if (src0->radius >= src1->radius)
            return *src0;
        else
            return *src1;
    }
}

rob_chara_age_age::rob_chara_age_age() {
    frame = 0.0f;
    field_9B4 = 0.1f;
    step = 1.0f;
    visible = true;
    skip = false;
    move_cancel = 0.0f;
    alpha = 1.0f;
    npr = false;
    rot_speed = 1.0f;
    step_full = false;
}

rob_chara_age_age::~rob_chara_age_age() {
    object.reset();
}

void rob_chara_age_age::ctrl(mat4& mat) {
    bool found = false;
    for (rob_chara_age_age_data& i : data)
        if (i.index != -1) {
            found = true;
            break;
        }

    if (!found)
        return;

    float_t step = this->step;
    float_t move_cancel = this->move_cancel;
    frame += step;

    vec3 trans[2];
    mat4_get_translation(&mat, &trans[0]);
    mat4_get_translation(&this->mat, &trans[1]);
    this->mat = mat;

    if (vec3::distance(trans[0], trans[1]) > 0.2f)
        frame += step * 2.0f;

    if (step_full) {
        this->step = 1.0f;
        this->move_cancel = 1.0f;
    }

    for (rob_chara_age_age_data& j : data)
        if (j.part_id == 1 || j.part_id == 2) {
            if (frame >= 200.0f)
                frame = 0.0f;

            ctrl_data(&j, mat);
        }

    if (step_full) {
        this->step = step;
        this->move_cancel = move_cancel;
        step_full = false;
    }

    if (skip) {
        ctrl_skip();
        skip = false;
    }

    object.update(data, 10, alpha);
}

void rob_chara_age_age::ctrl_data(rob_chara_age_age_data* data, mat4& mat) {
    const rob_chara_age_age_init_data* init_data;
    if (data->part_id == 1)
        init_data = &rob_chara_age_age_init_data_left[data->index];
    else if (data->part_id == 2)
        init_data = &rob_chara_age_age_init_data_right[data->index];
    else
        return;

    float_t step = skip ? 1.0f : this->step;
    if (data->remaining < 0.0f) {
        if (frame >= init_data->appear && frame < init_data->appear + 90.0f) {
            data->alpha = 0.0f;
            data->mat = mat;
            data->remaining = init_data->life_time;

            vec3 trans[2];
            mat4_get_translation(&mat, &trans[0]);
            mat4_get_translation(&data->prev_parent_mat, &trans[1]);

            vec3 v15 = (trans[0] - trans[1]) * (move_cancel - 1.0f);
            if (vec3::length(v15) <= 1.0f) {
                data->trans = v15;
                data->field_14 = 0.0f;
                data->rot_z = data->part_id == 1 ? -0.1f : 0.1f;
                data->field_1C = 0;
                data->rot_speed = init_data->rot_speed;
                mat4_translate_mult(&data->mat, &init_data->trans, &data->mat);
                mat4_rotate_x_mult(&data->mat, init_data->rot_x, &data->mat);
                mat4_translate_mult(&data->mat, 0.0f, 0.0f, init_data->offset, &data->mat);
                data->scale = init_data->scale;
                mat4_scale_rot(&data->mat, data->scale, &data->mat_scale);
                data->alive = true;
                data->gravity = init_data->gravity;
            }
            else
                data->alive = false;
        }
    }
    else if (data->alive) {
        if (data->remaining < 70.0f)
            data->rot_z += step * data->rot_speed * rot_speed;
        data->trans.x += (90.0f - data->remaining) * (float_t)(1.0 / 90.0)
            * data->gravity * 2.5f * step * rot_speed;
        data->trans.z -= (90.0f - data->remaining) * 0.000011f * step * rot_speed;

        mat4 m;
        mat4_translate_mult(&mat, &init_data->trans, &m);
        mat4_rotate_x_mult(&m, init_data->rot_x, &m);
        mat4_translate_mult(&m, 0.0f, 0.0f, init_data->offset, &m);
        mat4_rotate_y_mult(&m, init_data->rot_y, &m);
        mat4_translate_mult(&m, &data->trans, &m);
        mat4_rotate_z_mult(&m, data->rot_z, &m);
        mat4_scale_rot(&m, data->scale, &m);

        vec3 v43;
        mat4_get_translation(&m, &v43);

        vec3 v46;
        mat4_get_translation(&data->mat_scale, &v46);

        v43 = (v43 - v46) * min_def(step, 1.0f);

        float_t v28 = vec3::length(v43) * (1.0f - move_cancel);
        float_t v30 = 1.0f - (90.0f - data->remaining) * 0.000124f;

        vec2 v33 = vec2(v43.x, v43.z) * v30 * max_def(move_cancel, 0.18f);

        if (v28 > 1.0f)
            data->alive = false;
        else if (v28 > 0.2f)
            data->remaining -= step * 2.0f;

        vec3 v44;
        v44.x = v46.x + v33.x;
        v44.y = v46.y + v43.y;
        v44.z = v46.z + v33.y;

        mat4_set_translation(&m, &v44);

        data->mat_scale = m;

        if (data->remaining < 40.0f)
            data->scale = (1.0f - step * 0.02f) * data->scale;

        if (data->remaining < 10.0f)
            data->alpha = (1.0f - step * 0.9f) * data->remaining;
        else if (data->alpha < 1.0) {
            float_t alpha = step * 0.15f + data->alpha;
            data->alpha = min_def(alpha, 1.0f);
        }
    }
    data->remaining -= step;
    data->prev_parent_mat = mat;
}

void rob_chara_age_age::ctrl_skip() {
    frame = 0.0f;
    for (int32_t i = 0; i < 200; i++, frame += 1.0f)
        for (rob_chara_age_age_data& j : data)
            if (j.part_id == 1 || j.part_id == 2) {
                if (frame >= 200.0f)
                    frame = 0.0f;

                ctrl_data(&j, mat);
            }
}

void rob_chara_age_age::disp(render_context* rctx, size_t chara_id,
    bool npr, bool reflect, const vec3& a5, bool chara_color) {
    if (alpha >= 0.1f && this->visible)
        object.disp(rctx, chara_id, npr || this->npr, reflect, a5, chara_color);
}

void rob_chara_age_age::load(object_info obj_info, rob_chara_age_age_data* data, int32_t count) {
    reset();

    frame = 0.0f;
    field_9B4 = 1.0f;
    mat = mat4_identity;
    step = 1.0f;
    visible = true;
    skip = false;
    move_cancel = 0.0f;
    alpha = 1.0f;
    npr = false;
    rot_speed = 1.0f;
    step_full = false;

    count = min_def(count, 10);
    if (count < 1)
        return;

    memmove(this->data, data, sizeof(rob_chara_age_age_data) * count);
    object.load(obj_info, count);
}

void rob_chara_age_age::reset() {
    object.reset();

    for (rob_chara_age_age_data& i : data)
        i.reset();
}

void rob_chara_age_age::set_alpha(float_t value) {
    alpha = value;
}

void rob_chara_age_age::set_disp(bool value) {
    visible = value;
}

void rob_chara_age_age::set_move_cancel(float_t value) {
    move_cancel = value;
}

void rob_chara_age_age::set_npr(bool value) {
    npr = value;
}

void rob_chara_age_age::set_rot_speed(float_t value) {
    rot_speed = value;
}

void rob_chara_age_age::set_skip() {
    skip = true;
}

void rob_chara_age_age::set_step(float_t value) {
    step = max_def(value, 0.0f);
}

void rob_chara_age_age::set_step_full() {
    step_full = true;
}

rob_sleeve::rob_sleeve() : type(), l(), r() {

}

rob_sleeve::~rob_sleeve() {

}

rob_sleeve_handler::rob_sleeve_handler() : ready() {

}

rob_sleeve_handler::~rob_sleeve_handler() {
    clear();
}

void rob_sleeve_handler::clear() {
    ready = false;

    for (auto& i : sleeves) {
        delete i.second.l;
        delete i.second.r;
    }

    sleeves.clear();

    for (p_file_handler*& i : file_handlers)
        if (i) {
            delete i;
            i = 0;
        }
    file_handlers.clear();
}

void rob_sleeve_handler::get_sleeve_data(::chara_index chara_index,
    int32_t cos, rob_sleeve_data& l, rob_sleeve_data& r) {
    l = {};
    r = {};

    auto elem = sleeves.find({ chara_index, cos });
    if (elem != sleeves.end()) {
        if (elem->second.type & ROB_SLEEVE_L && elem->second.l)
            l = *elem->second.l;

        if (elem->second.type & ROB_SLEEVE_R && elem->second.r)
            r = *elem->second.r;
    }
}

bool rob_sleeve_handler::load() {
    if (ready)
        return false;

    for (p_file_handler*& i : file_handlers)
        if (i->check_not_ready())
            return true;

    for (p_file_handler*& i : file_handlers) {
        if (!i)
            continue;

        parse(i);

        delete i;
        i = 0;
    }
    file_handlers.clear();

    ready = true;
    return false;
}

void rob_sleeve_handler::parse(p_file_handler* pfhndl) {
    key_val kv;
    kv.parse(pfhndl->get_data(), pfhndl->get_size());

    int32_t count;
    if (!kv.read("sleeve", "length", count))
        return;

    for (int32_t i = 0; i < count; i++) {
        if (!kv.open_scope_fmt(i))
            continue;

        const char* chara;
        int32_t cos;
        const char* type_str;
        kv.read("chara", chara);
        kv.read("cos", cos);
        kv.read("type", type_str);

        rob_sleeve_type type = (rob_sleeve_type)0;
        if (!str_utils_compare(type_str, "L"))
            type = ROB_SLEEVE_L;
        else if (!str_utils_compare(type_str, "R"))
            type = ROB_SLEEVE_R;
        else if (!str_utils_compare(type_str, "LR"))
            type = ROB_SLEEVE_LR;

        ::chara_index chara_index = chara_index_get_from_chara_name(chara);

        rob_sleeve& sleeve = sleeves.insert({ { chara_index, cos - 1 }, {} }).first->second;
        switch (type) {
        case ROB_SLEEVE_L:
            parse_sleeve_data(kv, sleeve.l);
            break;
        case ROB_SLEEVE_R:
            parse_sleeve_data(kv, sleeve.r);
            break;
        case ROB_SLEEVE_LR:
            parse_sleeve_data(kv, sleeve.l);
            parse_sleeve_data(kv, sleeve.r);
            if (sleeve.l) {
                rob_sleeve_data* l = sleeve.l;
                rob_sleeve_data* r = sleeve.r;
                r->czofs = -l->czofs;
                r->zmin = -l->zmax;
                r->zmax = -l->zmin;
            }
            break;
        }
        sleeve.type = type;

        kv.close_scope();
    }

    kv.close_scope();
}

void rob_sleeve_handler::parse_sleeve_data(key_val& kv, rob_sleeve_data*& data) {
    if (data) {
        delete data;
        data = 0;
    }

    int32_t is_default;
    kv.read("is_default", is_default);
    if (is_default > 0)
        return;

    data = new rob_sleeve_data;
    kv.read("radius", data->radius);
    kv.read("cyofs", data->cyofs);
    kv.read("czofs", data->czofs);
    kv.read("ymin", data->ymin);
    kv.read("ymax", data->ymax);
    kv.read("zmin", data->zmin);
    kv.read("zmax", data->zmax);
    kv.read("mune_xofs", data->mune_xofs);
    kv.read("mune_yofs", data->mune_yofs);
    kv.read("mune_zofs", data->mune_zofs);
    kv.read("mune_rad", data->mune_rad);
}

void rob_sleeve_handler::read() {
    ready = false;

    data_struct* aft_data = &data_list[DATA_AFT];
    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string file;
        file.assign(i);
        file.append("rob_sleeve_data.txt");

        if (aft_data->check_file_exists("rom/", file.c_str())) {
            p_file_handler* pfhndl = new p_file_handler;
            pfhndl->read_file(aft_data, "rom/", file.c_str());
            file_handlers.push_back(pfhndl);
        }
    }
}

osage_set_motion::osage_set_motion() {
    motion_id = -1;
}

osage_set_motion::~osage_set_motion() {

}

void osage_set_motion::init_frame(uint32_t motion_id, float_t frame, int32_t stage_index) {
    frames.clear();
    this->motion_id = motion_id;
    frames.push_back(frame, stage_index);
}

skeleton_rotation_offset::skeleton_rotation_offset() : x(), y(), z() {

}

skeleton_rotation_offset::skeleton_rotation_offset(bool x, bool y, bool z, vec3 rotation) : x(x),
y(y), z(z), rotation(rotation) {

}

PvOsageManager::PvOsageManager() : state(), chara_id(), reset(), field_74(),
motion_index(), pv(), thread(), disp(), not_reset(), exit(), field_D4() {
    Reset();

    {
        std::unique_lock<std::mutex> u_lock(disp_mtx);
        disp = false;
    }

    {
        std::unique_lock<std::mutex> u_lock(not_reset_mtx);
        not_reset = false;
    }

    {
        std::unique_lock<std::mutex> u_lock(exit_mtx);
        exit = false;
    }

    thread = new std::thread(PvOsageManager::ThreadMain, this);
    if (thread) {
        wchar_t buf[0x80];
        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"PV Osage Manager #%d", pv_osage_manager_counter++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf);
    }
}

PvOsageManager::~PvOsageManager() {
    {
        std::unique_lock<std::mutex> u_lock(exit_mtx);
        exit = true;
    }

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
        sub_1404F8AA0();
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

void PvOsageManager::AddMotionFrameResetData(int32_t stage_index, uint32_t motion_id, float_t frame, int32_t iterations) {
    if (!CheckResetFrameNotFound(motion_id, frame))
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);

    struc_527 v17(rob_chr, stage_index, motion_id, frame, aft_bone_data, aft_mot_db);
    rob_chr->adjust_ctrl();
    rob_chr->rob_motion_modifier_ctrl();

    if (iterations < 0) {
        iterations = 60;
        if (rob_chr->data.field_1588.field_0.iterations > 0)
            iterations = rob_chr->data.field_1588.field_0.iterations;
    }

    rob_chara_add_motion_reset_data(rob_chr, motion_id, frame, iterations);

    reset_frames_list.insert({ frame, true });
    rob_chr->reset_osage();

    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        sub_140555120(rob_chr, (rob_osage_parts)i, false);
}

bool PvOsageManager::CheckResetFrameNotFound(uint32_t motion_id, float_t frame) {
    return reset_frames_list.find(frame) == reset_frames_list.end();
}

bool PvOsageManager::GetDisp() {
    std::unique_lock<std::mutex> u_lock(disp_mtx);
    return disp;
}

bool PvOsageManager::GetNotReset() {
    std::unique_lock<std::mutex> u_lock(not_reset_mtx);
    return not_reset;
}

void PvOsageManager::SetDisp(bool value) {
    std::unique_lock<std::mutex> u_lock(disp_mtx);
    disp = value;
}

void PvOsageManager::SetNotReset(bool value) {
    std::unique_lock<std::mutex> u_lock(not_reset_mtx);
    not_reset = value;
}

void PvOsageManager::SetPvId(int32_t pv_id, int32_t chara_id, bool reset) {
    if (!pv_set_motion.size())
        return;

    waitable_timer timer;
    while (GetDisp()) {
        SetNotReset(true);
        timer.sleep(1);
    }

    SetDisp(true);
    SetNotReset(false);
    this->reset = reset;
    this->chara_id = chara_id;

    sub_1404F77E0();

    if (pv_id > 0)
        pv = task_pv_db_get_pv(pv_id);

    cnd.notify_one();
}

void PvOsageManager::SetPvSetMotion(const std::vector<pv_data_set_motion>& set_motion) {
    pv_set_motion.assign(set_motion.begin(), set_motion.end());
}

void PvOsageManager::sub_1404F77E0() {
    quicksort_custom(pv_set_motion.data(), pv_set_motion.size(),
        sizeof(pv_data_set_motion), pv_data_set_motion_quicksort_compare_func);

    pv_data_set_motion* i_begin = pv_set_motion.data();
    pv_data_set_motion* i_end = pv_set_motion.data() + pv_set_motion.size();
    for (pv_data_set_motion* i = i_begin, *i_next = i_begin + 1; i != i_end && i_next != i_end; )
        if (i->motion_id == i_next->motion_id
            && i->frame_stage_index.first == i_next->frame_stage_index.first) {
            memmove(i, i_next, sizeof(pv_data_set_motion) * (i_end - i_next));
            pv_set_motion.pop_back();
            i_end--;
            continue;
        }
        else {
            i++;
            i_next++;
        }

    osage_set_motion.clear();

    uint32_t motion_id = -1;
    ::osage_set_motion* set_motion = 0;
    for (pv_data_set_motion& i : pv_set_motion) {
        if (motion_id != i.motion_id) {
            osage_set_motion.push_back({});
            set_motion = &osage_set_motion.back();
            set_motion->motion_id = i.motion_id;
        }

        if (set_motion)
            set_motion->frames.push_back(i.frame_stage_index);

        motion_id = i.motion_id;
    }
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
        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
        rob_chara* rob_chr = rob_chara_array_get(chara_id);
        rob_chr->set_motion_id(rob_chr->get_rob_cmn_mottbl_motion_id(0),
            0.0f, 0.0f, true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
        rob_chr->set_face_mottbl_motion(0, 6, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, true, aft_mot_db);
        rob_chr->set_hand_l_mottbl_motion(0, 192, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chr->set_hand_r_mottbl_motion(0, 192, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chr->set_mouth_mottbl_motion(0, 131, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chr->set_eyes_mottbl_motion(0, 165, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chr->set_eyelid_mottbl_motion_from_face(0, 0.0f, -1.0f, 0.0f, aft_mot_db);
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
        {
            std::unique_lock<std::mutex> u_lock(not_reset_mtx);
            if (not_reset)
                break;
        }

        sub_1404F8AA0();
    } while (sub_1404F7AF0());

    bool not_reset = false;
    {
        std::unique_lock<std::mutex> u_lock(not_reset_mtx);
        not_reset = this->not_reset;
    }

    sub_1404F7BD0(not_reset);
}

void PvOsageManager::sub_1404F83A0(::osage_set_motion* a2) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    float_t last_frame = (float_t)(int32_t)rob_chr->bone_data->get_frame_count() - 1.0f;
    if (!a2->frames.size())
        return;

    uint32_t motion_id = a2->motion_id;

    bool v32 = false;
    std::vector<float_t> v34;
    for (std::pair<float_t, int32_t>& i : a2->frames) {
        float_t frame = i.first;
        if (frame >= last_frame) {
            if (frame == last_frame)
                v32 = true;
            do
                frame -= last_frame;
            while (frame >= last_frame);
        }
        if (CheckResetFrameNotFound(motion_id, frame)) {
            v34.push_back(frame);
            reset_frames_list.insert({ frame, true });
        }
    }

    if (!v34.size() || v34.front() > last_frame)
        return;

    int32_t iterations = 60;
    if (rob_chr->data.field_1588.field_0.iterations > 0)
        iterations = rob_chr->data.field_1588.field_0.iterations;

    float_t frame = v34.front() - (float_t)iterations + 1.0f;
    while (frame < 0.0f)
        frame += last_frame;

    while (frame > last_frame)
        frame -= last_frame;

    float_t frame_1 = frame - 1.0f;
    if (frame_1 < 0.0f)
        frame_1 = last_frame - 1.0f;

    struc_527 v42(rob_chr, a2->frames.front().second, motion_id, frame_1, aft_bone_data, aft_mot_db);
    for (float_t& i : v34) {
        sub_14053E7B0(&v42, frame);
        sub_14053D360(&v42);

        float_t frame_1 = frame;

        int32_t frame_int = (int32_t)frame;
        if (frame != -0.0f && (float_t)frame_int != frame)
            frame = (float_t)(frame < 0.0f ? frame_int - 1 : frame_int);
        frame += 1.0f;

        if (iterations <= 1) {
            if (frame_1 == i) {
                if (frame_1 == 0.0f && v32)
                    rob_chara_add_motion_reset_data(rob_chr, motion_id, last_frame, 0);
                rob_chara_add_motion_reset_data(rob_chr, motion_id, frame_1, 0);
                continue;
            }
            if (frame_1 + 1.0f > i)
                frame = i;
        }
        else
            iterations--;

        if (frame >= last_frame) {
            frame = 0.0f;
            sub_14053D6C0(&v42, aft_mot_db);
        }
    }

    rob_chr->reset_osage();
    for (int32_t i = ROB_OSAGE_PARTS_LEFT; i < ROB_OSAGE_PARTS_MAX; i++)
        sub_140555120(rob_chr, (rob_osage_parts)i, false);
}

void PvOsageManager::sub_1404F88A0(uint32_t stage_index, uint32_t motion_id, float_t frame) {
    if (!CheckResetFrameNotFound(motion_id, frame))
        return;

    ::osage_set_motion v7;
    v7.init_frame(motion_id, frame, stage_index);
    sub_1404F83A0(&v7);
}

void PvOsageManager::sub_1404F8AA0() {
    if (!osage_set_motion.size())
        return;

    ::osage_set_motion& v4 = osage_set_motion.back();
    uint32_t motion_id = v4.motion_id;
    if (motion_id == -1)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    reset_frames_list.clear();
    rob_chr->set_motion_id(motion_id, 0.0f, 0.0f,
        true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
    if (pv)
        for (const pv_db_pv_osage_init& i : pv->osage_init) {
            int32_t osage_init_motion_id = aft_mot_db->get_motion_id(i.motion.c_str());
            if (osage_init_motion_id != -1 && osage_init_motion_id == motion_id) {
                if (rob_chr->data.field_1588.field_0.field_58 == 1)
                    sub_1404F88A0(i.stage, osage_init_motion_id, (float_t)i.frame);
                else
                    AddMotionFrameResetData(i.stage, osage_init_motion_id, (float_t)i.frame, -1);
            }
        }

    if (rob_chr->data.field_1588.field_0.field_58 == 1) {
        sub_1404F83A0(&v4);
    }
    else {
        bool v12 = true;
        const mothead_data* v13 = mothead_storage_get_mot_by_motion_id(motion_id, aft_mot_db)->data.data();
        if (v13 && v13->type >= MOTHEAD_DATA_TYPE_0) {
            mothead_data_type v14 = v13->type;
            while (v14 != MOTHEAD_DATA_WIND_RESET) {
                v13++;
                v14 = v13->type;
                if (v14 < MOTHEAD_DATA_TYPE_0)
                    goto LABEL_1;
            }

            while (v13->frame) {
                v13++;
                mothead_data_type v18 = v13->type;
                if (v18 < MOTHEAD_DATA_TYPE_0)
                    goto LABEL_1;

                while (v18 != MOTHEAD_DATA_WIND_RESET) {
                    v13++;
                    v18 = v13->type;
                    if (v18 < MOTHEAD_DATA_TYPE_0)
                        goto LABEL_1;
                }
            }
            v12 = false;
        }

    LABEL_1:
        for (std::pair<float_t, int32_t>& i : v4.frames)
            if (GetNotReset())
                break;
            else if (v12 || i.first != 0.0f)
                AddMotionFrameResetData(i.second, motion_id, i.first, -1);

        if (v12)
            AddMotionFrameResetData(0, motion_id, 0.0f, -1);

        const mothead_data* v23 = mothead_storage_get_mot_by_motion_id(motion_id, aft_mot_db)->data.data();
        if (v23 && v23->type >= MOTHEAD_DATA_TYPE_0) {
            mothead_data_type v24 = v23->type;
            while (v24 != MOTHEAD_DATA_OSAGE_RESET) {
                v23++;
                v24 = v23->type;
                if (v24 < MOTHEAD_DATA_TYPE_0)
                    return;
            }

            while (true) {
                int32_t iterations = 60;
                if (v23->data)
                    iterations = *(int32_t*)v23->data;

                AddMotionFrameResetData(0, motion_id, (float_t)v23->frame, iterations);

                v23++;
                mothead_data_type v29 = v23->type;
                if (v23->type < MOTHEAD_DATA_TYPE_0)
                    break;

                while (v29 != MOTHEAD_DATA_OSAGE_RESET) {
                    v23++;
                    v29 = v23->type;
                    if (v29 < MOTHEAD_DATA_TYPE_0)
                        return;
                }
            }
        }
    }
}

void PvOsageManager::ThreadMain(PvOsageManager* pv_osg_mgr) {
    std::unique_lock<std::mutex> u_lock(pv_osg_mgr->mtx);
    while (true) {
        pv_osg_mgr->cnd.wait(u_lock);

        {
            std::unique_lock<std::mutex> u_lock(pv_osg_mgr->exit_mtx);
            if (pv_osg_mgr->exit)
                break;
        }

        pv_osg_mgr->sub_1404F82F0();

        {
            std::unique_lock<std::mutex> u_lock(pv_osg_mgr->not_reset_mtx);
        }

        {
            std::unique_lock<std::mutex> u_lock(pv_osg_mgr->disp_mtx);
            pv_osg_mgr->disp = false;
        }
    }

    {
        std::unique_lock<std::mutex> u_lock(pv_osg_mgr->disp_mtx);
        pv_osg_mgr->disp = false;
    }
}

RobThreadParent::RobThreadParent() : exit(), thread() {
    thread = new std::thread(RobThreadParent::ThreadMain, this);
    if (thread) {
        wchar_t buf[0x80];
        swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"Rob Thread Parent #%d", rob_thread_parent_counter++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf);
    }
}

RobThreadParent::~RobThreadParent() {
    {
        std::unique_lock<std::mutex> u_lock(mtx);
        exit = true;
    }

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
}

void RobThreadHandler::sub_14054E3F0() {
    for (RobThreadParent*& i : arr)
        i->sub_14054E370();
}

void RobThreadParent::AppendRobThread(RobThread* thread) {
    std::unique_lock<std::mutex> u_lock(threads_mtx);
    threads.push_back(*thread);
}

bool RobThreadParent::CheckThreadsNotNull() {
    std::unique_lock<std::mutex> u_lock(threads_mtx);
    return !!threads.size();
}

void RobThreadParent::sub_14054E0D0() {
    bool threads_null = false;
    {
        std::unique_lock<std::mutex> u_lock(threads_mtx);
        threads.pop_front();
        threads_null = !threads.size();
    }

    if (threads_null) {
        std::unique_lock<std::mutex> u_lock(field_28);
        field_30.notify_one();
    }
}

void RobThreadParent::sub_14054E370() {
    std::unique_lock<std::mutex> u_lock(field_28);
    if (CheckThreadsNotNull())
        field_30.wait(u_lock);
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
        rob_chara* rob_chr = *i;
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

TaskRobBase::TaskRobBase() {

}

TaskRobBase::~TaskRobBase() {

}

bool TaskRobBase::Init() {
    return true;
}

bool TaskRobBase::Ctrl() {
    if (pv_osage_manager_array_get_disp())
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
        if (i && !(i->data.field_3 & 0x80))
            rob_base_rob_chara_ctrl(i);

    for (rob_chara*& i : free_chara)
        if (i)
            rob_base_rob_chara_free(i);
    free_chara.clear();
    return false;
}

bool TaskRobBase::Dest() {
    if (pv_osage_manager_array_get_disp()) {
        pv_osage_manager_array_set_not_reset_true();
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
            i->sub_140509D30();
            //sub_14050F4D0(i);
            //sub_14050E930(i);
        }

        /*for (rob_chara*& i : ctrl_chara)
            if (i && !(i->data.field_3 & 0x80))
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
    if (pv_osage_manager_array_get_disp())
        return false;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;
    for (rob_chara*& i : init_chara) {
        rob_disp_rob_chara_init(i, aft_bone_data, aft_data, aft_obj_db);
        AppendCtrlCharaList(i);
    }
    init_chara.clear();

    for (rob_chara*& i : ctrl_chara)
        if (i && !(i->data.field_3 & 0x80))
            rob_disp_rob_chara_ctrl(i);

    for (rob_chara*& i : ctrl_chara)
        if (i && !(i->data.field_3 & 0x80))
            rob_thread_handler->AppendRobCharaFunc(i->chara_id, i,
                rob_disp_rob_chara_ctrl_thread_main);

    rob_thread_handler->sub_14054E3F0();

    for (rob_chara*& i : free_chara)
        rob_disp_rob_chara_free(i);
    free_chara.clear();
    return false;
}

bool TaskRobDisp::Dest() {
    if (pv_osage_manager_array_get_disp()) {
        pv_osage_manager_array_set_not_reset_true();
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

        if (i->is_visible() && !(i->data.field_3 & 0x80))
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

    for (rob_chara*& i : ctrl_chara)
        if (i && !(i->data.field_3 & 0x80))
            i->rob_info_ctrl();

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
    if (pv_osage_manager_array_get_disp()) {
        pv_osage_manager_array_set_not_reset_true();
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

    for (auto i = load_req_data.end(); i != load_req_data.begin(); ) {
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

bool TaskRobLoad::AppendFreeReqDataObj(chara_index chara_index, const item_cos_data* cos) {
    if (chara_index < CHARA_MIKU || chara_index > CHARA_TETO)
        return false;

    for (auto i = load_req_data_obj.end(); i != load_req_data_obj.begin(); ) {
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

    for (auto i = free_req_data.end(); i != free_req_data.begin(); ) {
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

bool TaskRobLoad::AppendLoadReqDataObj(chara_index chara_index, const item_cos_data* cos) {
    if (chara_index < CHARA_MIKU || chara_index > CHARA_TETO)
        return false;

    for (auto i = free_req_data_obj.end(); i != free_req_data_obj.begin(); ) {
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

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    if (load_req_data_obj.size()) {
        for (ReqDataObj& i : load_req_data_obj) {
            AppendLoadedReqDataObj(&i);
            LoadCharaItemsParent(i.chara_index, &i.cos, aft_data, aft_obj_db);
            load_item_req_data_obj.push_back(i);
        }
        load_req_data_obj.clear();
        ret = 3;
    }

    if (load_req_data.size()) {
        for (ReqData& i : load_req_data) {
            AppendLoadedReqData(&i);
            LoadCharaObjSetMotionSet(i.chara_index, aft_data, aft_obj_db, aft_mot_db);
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
    for (auto i = loaded_req_data.begin(); i != loaded_req_data.end(); i++)
        if (i->chara_index == req_data->chara_index) {
            if (i->count > 0)
                i->count--;

            if (!i->count)
                i = loaded_req_data.erase(i);
            return;
        }
}

void TaskRobLoad::FreeLoadedReqDataObj(ReqDataObj* req_data_obj) {
    for (auto i = loaded_req_data_obj.begin(); i != loaded_req_data_obj.end(); i++)
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
    int32_t item_no, void* data, const object_database* obj_db) {
    if (!item_no)
        return;

    const std::vector<uint32_t>* item_objset
        = item_table_handler_array_get_item_objset(chara_index, item_no);
    if (!item_objset)
        return;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1)
            object_storage_load_set(data, obj_db, i);
}

bool TaskRobLoad::LoadCharaItemCheckNotRead(chara_index chara_index, int32_t item_no) {
    if (!item_no)
        return false;

    const std::vector<uint32_t>* item_objset
        = item_table_handler_array_get_item_objset(chara_index, item_no);
    if (!item_objset)
        return true;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1 && object_storage_load_obj_set_check_not_read(i))
            return true;
    return false;
}

void TaskRobLoad::LoadCharaItems(chara_index chara_index,
    item_cos_data* cos, void* data, const object_database* obj_db) {
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++)
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
    item_cos_data* cos, void* data, const object_database* obj_db) {
    LoadCharaItems(chara_index, cos, data, obj_db);
    if (cos->data.kami == 649)
        object_storage_load_set(data, obj_db, 3291);
}

void TaskRobLoad::LoadCharaObjSetMotionSet(chara_index chara_index,
    void* data, const object_database* obj_db, const motion_database* mot_db) {
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    object_storage_load_set(data, obj_db, chr_init_data->object_set);
    motion_set_load_motion(cmn_set_id, "", mot_db);
    motion_set_load_mothead(cmn_set_id, "", mot_db);
    motion_set_load_motion(chr_init_data->motion_set, "", mot_db);
}

bool TaskRobLoad::LoadCharaObjSetMotionSetCheck(chara_index chara_index) {
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    if (object_storage_load_obj_set_check_not_read(chr_init_data->object_set)
        || motion_storage_check_mot_file_not_ready(cmn_set_id)
        || mothead_storage_check_mhd_file_not_ready(cmn_set_id))
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

    const std::vector<uint32_t>* item_objset
        = item_table_handler_array_get_item_objset(chara_index, item_no);
    if (!item_objset)
        return;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1)
            object_storage_unload_set(i);
}

void TaskRobLoad::UnloadCharaItems(chara_index chara_index, item_cos_data* cos) {
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++)
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
    motion_set_unload_motion(cmn_set_id);
    motion_set_unload_mothead(cmn_set_id);
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
    task_rob_load_add_task();
    rob_manager_rob_impl* rob_impls1 = rob_manager_rob_impls1_get(this);
    for (; rob_impls1->task; rob_impls1++) {
        RobImplTask* task = rob_impls1->task;
        app::TaskWork::AddTask(task, rob_impls1->name);
        task->is_frame_dependent = task->IsFrameDependent();
        task->FreeCharaLists();
    }

    rob_manager_rob_impl* rob_impls2 = rob_manager_rob_impls2_get(this);
    for (; rob_impls2->task; rob_impls2++) {
        RobImplTask* task = rob_impls2->task;
        app::TaskWork::AddTask(task, rob_impls2->name);
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
            data_struct* aft_data = &data_list[DATA_AFT];
            bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
            motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

            for (rob_chara*& i : load_chara) {
                i->reset_data(&i->pv_data, aft_bone_data, aft_mot_db);
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
            task_rob_load_append_free_req_data(i->chara_index);
            task_rob_load_append_free_req_data_obj(i->chara_index, i->item_cos_data.get_cos());
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
            task_rob_load_append_load_req_data(i->chara_index);
            task_rob_load_append_load_req_data_obj(i->chara_index, i->item_cos_data.get_cos());
        }
        init_chara.clear();
        ctrl_state = 0;
    }
    return false;
}

bool TaskRobManager::Dest() {
    switch (dest_state) {
    case 0: {
        rob_manager_rob_impl* rob_impls1 = rob_manager_rob_impls1_get(this);
        for (; rob_impls1->task; rob_impls1++)
            rob_impls1->task->DelTask();

        rob_manager_rob_impl* rob_impls2 = rob_manager_rob_impls2_get(this);
        for (; rob_impls2->task; rob_impls2++)
            rob_impls2->task->DelTask();

        init_chara.clear();
        load_chara.clear();
        free_chara.clear();
        loaded_chara.clear();
        dest_state = 1;
    }
    case 1:
        if (!task_rob_load_del_task())
            return false;

        dest_state = 2;
        return true;
    default:
        return false;
    }
}

void TaskRobManager::Disp() {

}

void TaskRobManager::AppendFreeCharaList(rob_chara* rob_chr) {
    if (!rob_chr || rob_chr->chara_id >= ROB_CHARA_COUNT || free_chara.size() >= ROB_CHARA_COUNT)
        return;

    int32_t chara_id = rob_chr->chara_id;
    for (std::list<rob_chara*>::iterator i = init_chara.begin(); i != init_chara.end();)
        if ((*i)->chara_id == chara_id) {
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
    if (!app::TaskWork::CheckTaskReady(this)
        || rob_chr->chara_id < 0 || rob_chr->chara_id >= ROB_CHARA_COUNT)
        return false;

    int8_t chara_id = rob_chr->chara_id;
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
        if ((*i)->chara_id == *chara_id) {
            i = loaded_chara.erase(i);
            return;
        }
        else
            i++;
}

bool TaskRobManager::GetWait(rob_chara* rob_chr) {
    if (!app::TaskWork::CheckTaskReady(this))
        return false;

    int32_t chara_id = rob_chr->chara_id;
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return false;

    for (auto& i : init_chara)
        if (i->chara_id == chara_id)
            return true;

    for (auto& i : load_chara)
        if (i->chara_id == chara_id)
            return true;

    for (auto& i : loaded_chara)
        if (i->chara_id == chara_id)
            return true;

    for (auto& i : free_chara)
        if (i->chara_id == chara_id)
            return true;

    return false;
}

TaskRobMotionModifier::TaskRobMotionModifier() {

}

TaskRobMotionModifier::~TaskRobMotionModifier() {

}

bool TaskRobMotionModifier::Init() {
    return true;
}

static void sub_1405484A0(rob_chara* rob_chr) {
    rob_chr->data.motion.step_data.field_4 = rob_chr->data.motion.step_data.frame;

    if (rob_chr->data.motion.step_data.step < 0.0f)
        rob_chr->data.motion.step_data.frame = 1.0f;
    else
        rob_chr->data.motion.step_data.frame = rob_chr->data.motion.step_data.step;

    rob_chara_bone_data_set_step(rob_chr->bone_data, rob_chr->data.motion.step_data.frame);

    float_t step = rob_chr->data.motion.step;
    if (step < 0.0f)
        step = rob_chr->data.motion.step_data.frame;
    rob_chr->item_equip->set_osage_step(step);

    if (!(rob_chr->data.motion.field_28 & 0x40)) {
        if (rob_chr->data.field_8.field_B8.field_7C > 0.0f)
            rob_chr->data.motion.step_data.frame = rob_chr->data.field_8.field_B8.field_80;
        return;
    }

    if (!rob_chr->data.field_1588.field_0.field_298)
        return;

    if (!rob_chr->field_20 || rob_chr->data.motion.frame_data.frame >= rob_chr->data.field_1588.field_0.field_29C) {
        rob_chr->data.motion.step_data.frame = 1.0f;
        rob_chr->data.motion.field_28 &= ~0x40;
        return;
    }

    float_t v6 = 1.0f;
    /*size_t v7 = rob_chr->field_20;
    if (rob_chr->data.field_1588.field_0.field_298 & 0x200
        || (stru_140A2E410[*(int32_t*)(v7 + 6744)][rob_chr->data.field_1588.field_0.field_298 & 0xFF]
            && *(int32_t*)(v7 + 6736) & 0x02)) {
        float_t v11 = rob_chr->data.field_1588.field_0.field_2A4;
        switch (rob_chr->data.field_1588.field_0.field_2A0) {
        case 1:
            v11 += *(float_t*)(v7 + 6608);
            break;
        case 2:
            v11 += *(float_t*)(v7 + 6720);
            break;
        case 3:
            v11 += *(float_t*)(v7 + 6724);
            break;
        case 4:
            v11 += *(float_t*)(v7 + 6728);
            break;
        }

        float_t v12 = v11 - *(float_t*)(v7 + 1752);
        if (v12 > 0.0f)
            v6 = (rob_chr->data.field_1588.field_0.field_29C - rob_chr->data.motion.frame_data.frame) / v12;
    }*/

    if (v6 < rob_chr->data.field_1588.field_0.field_2A8)
        v6 = rob_chr->data.field_1588.field_0.field_2A8;

    if (v6 <= rob_chr->data.field_1588.field_0.field_2AC)
        rob_chr->data.motion.step_data.frame = v6;
    else
        rob_chr->data.motion.step_data.frame = rob_chr->data.field_1588.field_0.field_2AC;
    return;
}

bool TaskRobMotionModifier::Ctrl() {
    if (pv_osage_manager_array_get_disp())
        return false;

    for (rob_chara*& i : init_chara)
        if (i)
            AppendCtrlCharaList(i);
    init_chara.clear();

    for (rob_chara*& i : ctrl_chara)
        if (i)
            sub_1405484A0(i);

    for (rob_chara*& i : ctrl_chara)
        if (i)
            i->rob_motion_modifier_ctrl();

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
    rob_chr->bone_data->sub_14041DBA0();
    if (rob_chr->data.motion.field_28 & 0x80)
        rob_chr->bone_data->set_frame(rob_chr->data.motion.frame_data.last_set_frame);
    rob_chr->data.motion.frame_data.frame = rob_chr->bone_data->get_frame();
    sub_14053F300(&rob_chr->data.motion.field_150.face);
    sub_14053F300(&rob_chr->data.motion.field_150.hand_l);
    sub_14053F300(&rob_chr->data.motion.field_150.hand_r);
    sub_14053F300(&rob_chr->data.motion.field_150.mouth);
    sub_14053F300(&rob_chr->data.motion.field_150.eyes);
    sub_14053F300(&rob_chr->data.motion.field_150.eyelid);
    sub_14053F300(&rob_chr->data.motion.field_3B0.face);
    sub_14053F300(&rob_chr->data.motion.field_3B0.hand_l);
    sub_14053F300(&rob_chr->data.motion.field_3B0.hand_r);
    sub_14053F300(&rob_chr->data.motion.field_3B0.mouth);
    sub_14053F300(&rob_chr->data.motion.field_3B0.eyes);
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
    if (pv_osage_manager_array_get_disp())
        return false;

    for (rob_chara*& i : init_chara)
        if (i)
            AppendCtrlCharaList(i);
    init_chara.clear();

    for (rob_chara*& i : ctrl_chara)
        if (i)
            sub_1405484A0(i);

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
