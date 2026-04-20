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
#include "../../KKdLib/sort.hpp"
#include "../../KKdLib/str_utils.hpp"
#include "../../KKdLib/waitable_timer.hpp"
#include "../app_farc.hpp"
#include "../customize_item_table.hpp"
#include "../module_table.hpp"
#include "../data.hpp"
#include "../effect.hpp"
#include "../gl_state.hpp"
#include "../hand_item.hpp"
#include "../mdata_manager.hpp"
#include "../pv_db.hpp"
#include "../pv_expression.hpp"
#include "../random.hpp"
#include "../screen_param.hpp"
#include "../stage.hpp"
#include "../stage_param.hpp"
#include "../static_var.hpp"
#include "motion.hpp"
#include "skin_param.hpp"

#define OPD_MAKE_COUNT 4

enum rob_sleeve_type : uint8_t {
    ROB_SLEEVE_L  = 0x01,
    ROB_SLEEVE_R  = 0x02,

    ROB_SLEEVE_LR = 0x03,
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
        void ReadOpdiFiles(rob_chara* rob_chr, std::vector<uint32_t>& motions);
        void Reset();
    };

    rob_chara* rob_chr;
    const std::vector<uint32_t>* motion_ids;
    bool waiting;
    uint32_t motion_index;
    bool end;
    OpdMaker::Data* data;
    std::thread* thread;
    std::mutex waiting_mtx;
    std::mutex motion_mtx;
    std::mutex end_mtx;

    OpdMaker();
    virtual ~OpdMaker();

    bool CheckForFinalMotion();
    void Ctrl();
    bool GetEnd();
    bool InitThread(rob_chara* rob_chr, const std::vector<uint32_t>* motion_ids, OpdMaker::Data* data);
    bool IsWaiting();
    void Reset();
    bool SetOsagePlayInitData(uint32_t motion_id);
    void SetEnd();
    void SetWaiting(bool value);

    static void ThreadMain(OpdMaker* opd_maker);
};

struct OpdMakeWorker : public app::Task {
    int32_t state;
    int32_t chara_id;
    int32_t items[25];
    bool use_current_skp;
    OpdMaker::Data data;

    OpdMakeWorker(int32_t chara_id);
    virtual ~OpdMakeWorker() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    bool add_task(bool use_current_skp);
    bool del_task();
};

#if OPD_PLAY_GEN
struct OpdPlayGen {
    struct Object {
        uint64_t file_hash;
        bool generate;

        Object();
        Object(uint64_t file_hash);
    };

    std::atomic_int32_t state;
    std::thread* thread;
    std::map<uint32_t, std::vector<std::pair<object_info, Object>>> gen_data;
    std::vector<pv_data_set_motion> set_motion[ROB_ID_MAX];
    object_info obj_info[ROB_ID_MAX][RPK_ITEM_MAX];

    OpdPlayGen();
    ~OpdPlayGen();

    bool CheckState();
    void Ctrl();
    void Reset();

    static void ThreadMain(OpdPlayGen* opd_play);
};

bool operator==(const OpdPlayGen::Object& left, const OpdPlayGen::Object& right) {
    return left.file_hash == right.file_hash;
}

bool operator!=(const OpdPlayGen::Object& left, const OpdPlayGen::Object& right) {
    return left.file_hash != right.file_hash;
}

bool operator<(const OpdPlayGen::Object& left, const OpdPlayGen::Object& right) {
    return left.file_hash < right.file_hash;
}

bool operator>(const OpdPlayGen::Object& left, const OpdPlayGen::Object& right) {
    return left.file_hash > right.file_hash;
}

bool operator<=(const OpdPlayGen::Object& left, const OpdPlayGen::Object& right) {
    return left.file_hash <= right.file_hash;
}

bool operator>=(const OpdPlayGen::Object& left, const OpdPlayGen::Object& right) {
    return left.file_hash >= right.file_hash;
}
#endif

struct osage_play_data_init_header {
    uint32_t signature;
    std::pair<uint16_t, uint16_t> obj_info;
    uint32_t motion_id;
    uint16_t nodes_count;
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

    opd_file_data();

    void unload();
};

struct opd_vec3_data_vec {
    std::vector<float_t> x;
    std::vector<float_t> y;
    std::vector<float_t> z;

    opd_vec3_data_vec();
    ~opd_vec3_data_vec();
};

struct opd_chara_data {
    int32_t chara_id;
    bool init;
    uint32_t frame_index;
    uint32_t frame_count;
    uint32_t motion_id;
    uint64_t field_18;
    std::vector<std::vector<opd_vec3_data_vec>> opd_data[RPK_ITEM_MAX];
    p_farc_write opd[RPK_ITEM_MAX];
    p_farc_write opdi[RPK_ITEM_MAX];

    opd_chara_data();
    ~opd_chara_data();

    void add_frame_data();
    void encode_data();
    void encode_init_data(uint32_t motion_id);
    void fs_copy_file();
    void init_data(uint32_t motion_id);
    void open_opd_file();
    void open_opdi_file();
    void reset();
    void write_file();
};

class OsagePlayDataManager : public app::Task {
public:
    int32_t state;
    prj::vector_pair<object_info, uint32_t> req_data;
    std::list<p_file_handler*> file_handlers;
    std::map<std::pair<object_info, uint32_t>, opd_file_data> file_data;

    OsagePlayDataManager();
    virtual ~OsagePlayDataManager() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    bool add();
    void AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids);
    bool CheckTaskReady();
    void GetOpdFileData(object_info obj_info,
        uint32_t motion_id, const float_t*& data, uint32_t& count);
    void LoadOpdFile(p_file_handler* pfhndl);
    void LoadOpdFileList();
    void Reset();

    static std::string GetOpdFarcFilePath(const std::string& path);
};

struct osage_play_data_database_struct {
    std::map<std::string, size_t> map;

    osage_play_data_database_struct();
    ~osage_play_data_database_struct();

    size_t& find(const std::string& key);
    size_t& get_ver_by_name(size_t& ver, const std::string& name);
    void load(const std::string& file_path);
    void reset();
};

struct rob_osage_mothead_data {
    bool init;
    mothead_data_type type;
    const mothead_data* data;

    rob_osage_mothead_data(mothead_data_type type, uint32_t motion_id, const motion_database* mot_db);
    ~rob_osage_mothead_data();

    const mothead_data* find_next_data();
};

struct rob_osage_mothead {
    rob_chara* rob_chr;
    uint32_t motion_id;
    float_t frame;
    float_t last_frame;
    rob_osage_mothead_data* rob_parts_adjust;
    const mothead_data* rob_parts_adjust_data;
    rob_osage_mothead_data* rob_adjust_global;
    const mothead_data* rob_adjust_global_data;
    rob_osage_mothead_data* sleeve_adjust;
    const mothead_data* sleeve_adjust_data;
    rob_osage_mothead_data* disable_collision;
    const mothead_data* disable_collision_data;
    rob_osage_mothead_data* rob_chara_coli_ring;
    const mothead_data* rob_chara_coli_ring_data;
    int32_t field_68;

    rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index, uint32_t motion_id,
        float_t frame, const bone_database* bone_data, const motion_database* mot_db);
#if OPD_PLAY_GEN
    rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index,
        uint32_t motion_id, float_t frame, bool set_motion_reset_data,
        const bone_database* bone_data, const motion_database* mot_db);
#endif
    ~rob_osage_mothead();

    void ctrl();
    void init_data(const motion_database* mot_db);
    void reset();
    void reset_data();
    void set_coli_ring(const mothead_data* mhd_data);
    void set_disable_collision(const mothead_data* mhd_data);
    void set_frame(float_t value);
    void set_mothead_frame();
    void set_rob_parts_adjust(const mothead_data* mhd_data);
    void set_rob_adjust_global(const mothead_data* mhd_data);
    void set_sleeve_adjust(const mothead_data* mhd_data);
};

struct OpdMakeManager : app::Task {
    struct CharaCostume {
        std::vector<uint32_t> items[ITEM_SUB_MAX];

        CharaCostume();
        ~CharaCostume();
    };

    struct CharaData {
        size_t left;
        size_t count;
        CharaCostume chara_costumes[CN_MAX];

        CharaData();
        ~CharaData();

        void AddCostumes(const std::list<std::pair<CHARA_NUM, int32_t>>& costumes);
        void AddObjects(const std::vector<std::string>& customize_items);
        bool CheckNoItems(CHARA_NUM chara_num);
        void PopItems(CHARA_NUM chara_num, int32_t items[ITEM_SUB_MAX]);
        void Reset();
        void SortUnique();
    };

    int32_t mode;
    CharaData chara_data;
    CHARA_NUM chara;
    std::vector<uint32_t> motion_ids;
    std::vector<uint32_t> motion_set_ids;
    OpdMakeWorker* workers[OPD_MAKE_COUNT];
    OpdMakeManagerData data;
    bool use_current_skp;
    bool use_opdi;

    OpdMakeManager();
    virtual ~OpdMakeManager() override;

    bool init() override;
    bool ctrl() override;
    bool dest() override;
    void disp() override;

    void add_task(const OpdMakeManagerArgs& args);
    bool del_task();
    OpdMakeManagerData* GetData();
};

struct mothead_mot_func_data {
    rob_chara* rob_chara;
    rob_chara_data* rob_base;
    struc_223* field_10;
};

struct mothead_func_data {
    rob_chara* rob_chr;
    rob_chara_data* rob_base;
    rob_chara_data* field_10;
    rob_chara* field_18;
    rob_chara_data* field_20;
    struc_223* field_28;
};

typedef void(*mothead_func)(mothead_func_data*, const void*,
    const mothead_data*, int32_t, const motion_database*);
typedef void(*mothead_mot_func)(mothead_mot_func_data*, const void*, const mothead_mot_data*);

struct mothead_func_struct {
    mothead_func func;
    int32_t flags;
};

class ReqData {
public:
    CHARA_NUM chara_num;
    int32_t count;

    ReqData();
    ReqData(CHARA_NUM chara_num, int32_t count);
    virtual ~ReqData();

    virtual void Reset();
};

class ReqDataObj : public ReqData {
public:
    item_cos_data cos;

    ReqDataObj();
    ReqDataObj(CHARA_NUM chara_num, int32_t count);
    virtual ~ReqDataObj() override;

    virtual void Reset() override;
};

struct CmnMotTblBase {
    int32_t tbl;
    int32_t kamae_max;
};

struct CmnMotTblHeader {
    uint32_t cn_num;
    uint32_t mtp_num;
    uint32_t data_tbl;
};

struct rob_chara_age_age_init_data {
    vec3 pos;
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
    vec3 pos;
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
    ObjsetInfo* objset_info;
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
    vec3 pos[10];
    int32_t disp_count;
    int32_t count;
    bool field_C3C;

    rob_chara_age_age_object();

    void calc_vertex(rob_chara_age_age_object_vertex*& vtx_data,
        obj_mesh* mesh, const mat4& mat, float_t alpha);
    void disp(render_context* rctx, size_t chara_num,
        bool npr, bool reflect, const vec3& a5, bool chara_color);
    bool get_objset_info_obj_index(object_info obj_info);
    ::obj* get_obj_set_obj();
    std::vector<GLuint>& get_obj_set_texture();
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
    float_t speed;
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
    void set_speed(float_t value);
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
    std::map<std::pair<CHARA_NUM, int32_t>, rob_sleeve> sleeves;

    rob_sleeve_handler();
    ~rob_sleeve_handler();

    void clear();
    void get_sleeve_data(CHARA_NUM chara_num,
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
    bool display;
    bool not_reset;
    bool exit;
    int32_t field_D4;
    std::mutex mtx;
    std::mutex display_mtx;
    std::mutex not_reset_mtx;
    std::mutex exit_mtx;

    PvOsageManager();
    virtual ~PvOsageManager() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    void AddMotionFrameResetData(int32_t stage_index,
        uint32_t motion_id, float_t frame, int32_t iterations);
    bool CheckResetFrameNotFound(uint32_t motion_id, float_t frame);
    bool GetDisplay();
    bool GetNotReset();
    void Reset();
    void SetDisplay(bool value);
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
    RobThreadParent* arr[ROB_ID_MAX];

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

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    bool AppendFreeReqData(CHARA_NUM chara_num);
    bool AppendFreeReqDataObj(CHARA_NUM chara_num, const item_cos_data* cos);
    bool AppendLoadReqData(CHARA_NUM chara_num);
    bool AppendLoadReqDataObj(CHARA_NUM chara_num, const item_cos_data* cos);
    void AppendLoadedReqData(ReqData* req_data);
    void AppendLoadedReqDataObj(ReqDataObj* req_data_obj);
    int32_t CtrlFunc1();
    int32_t CtrlFunc2();
    int32_t CtrlFunc3();
    int32_t CtrlFunc4();
    void FreeLoadedReqData(ReqData* req_data);
    void FreeLoadedReqDataObj(ReqDataObj* req_data_obj);
    void LoadCharaItem(CHARA_NUM chara_num,
        int32_t item_no, void* data, const object_database* obj_db);
    bool LoadCharaItemCheckNotRead(CHARA_NUM chara_num, int32_t item_no);
    void LoadCharaItems(CHARA_NUM chara_num,
        item_cos_data* cos, void* data, const object_database* obj_db);
    bool LoadCharaItemsCheckNotRead(CHARA_NUM chara_num, item_cos_data* cos);
    bool LoadCharaItemsCheckNotReadParent(CHARA_NUM chara_num, item_cos_data* cos);
    void LoadCharaItemsParent(CHARA_NUM chara_num,
        item_cos_data* cos, void* data, const object_database* obj_db);
    void LoadCharaObjSetMotionSet(CHARA_NUM chara_num,
        void* data, const object_database* obj_db, const motion_database* mot_db);
    bool LoadCharaObjSetMotionSetCheck(CHARA_NUM chara_num);
    void ResetReqData();
    void ResetReqDataObj();
    void UnloadCharaItem(CHARA_NUM chara_num, int32_t item_no);
    void UnloadCharaItems(CHARA_NUM chara_num, item_cos_data* cos);
    void UnloadCharaItemsParent(CHARA_NUM chara_num, item_cos_data* cos);
    void UnloadCharaObjSetMotionSet(CHARA_NUM chara_num);
    void UnloadLoadedChara();
};

class RobImplTask : public app::Task {
public:
    std::list<rob_chara*> init_chara;
    std::list<rob_chara*> ctrl_chara;
    std::list<rob_chara*> free_chara;

    RobImplTask();
    virtual ~RobImplTask() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    virtual bool check_type(rob_chara_type type) = 0;
    virtual bool is_frame_dependent() = 0;

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

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(rob_chara_type type) override;
    virtual bool is_frame_dependent() override;
};

class TaskRobPrepareAction : public RobImplTask {
public:
    TaskRobPrepareAction();
    virtual ~TaskRobPrepareAction() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(rob_chara_type type) override;
    virtual bool is_frame_dependent() override;
};

class TaskRobBase : public RobImplTask {
public:
    TaskRobBase();
    virtual ~TaskRobBase() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(rob_chara_type type) override;
    virtual bool is_frame_dependent() override;
};

class TaskRobCollision : public RobImplTask {
public:
    TaskRobCollision();
    virtual ~TaskRobCollision() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(rob_chara_type type) override;
    virtual bool is_frame_dependent() override;
};

class TaskRobInfo : public RobImplTask {
public:
    TaskRobInfo();
    virtual ~TaskRobInfo() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(rob_chara_type type) override;
    virtual bool is_frame_dependent() override;
};

class TaskRobMotionModifier : public RobImplTask {
public:
    TaskRobMotionModifier();
    virtual ~TaskRobMotionModifier() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(rob_chara_type type) override;
    virtual bool is_frame_dependent() override;
};

class TaskRobDisp : public RobImplTask {
public:
    TaskRobDisp();
    virtual ~TaskRobDisp() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    virtual bool check_type(rob_chara_type type) override;
    virtual bool is_frame_dependent() override;
};

class TaskRobManager : public app::Task {
public:
    int32_t ctrl_state;
    int32_t dest_state;
    TaskRobPrepareControl rob_prepare_control;
    TaskRobPrepareAction rob_prepare_action;
    TaskRobBase rob_base;
    TaskRobCollision rob_collision;
    TaskRobInfo rob_info;
    TaskRobMotionModifier rob_motion_modifier;
    TaskRobDisp rob_disp;
    std::list<rob_chara*> init_chara;
    std::list<rob_chara*> load_chara;
    std::list<rob_chara*> free_chara;
    std::list<rob_chara*> loaded_chara;

    TaskRobManager();
    virtual ~TaskRobManager() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    void AppendFreeCharaList(rob_chara* rob_chr);
    void AppendInitCharaList(rob_chara* rob_chr);
    void AppendLoadedCharaList(rob_chara* rob_chr);
    bool CheckCharaLoaded(rob_chara* rob_chr);
    bool CheckHasRobCharaLoad(rob_chara* rob_chr);
    void CheckTypeAppendInitCharaLists(std::list<rob_chara*>* rob_chr_list);
    void FreeLoadedCharaList(int8_t* chara_id);
    bool GetFreeCharaListEmpty();
    bool GetWait(rob_chara* rob_chr);
};

struct rob_manager_rob_impl {
    RobImplTask* task;
    const char* name;
};

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data);
static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    const std::vector<BODYTYPE>* bones,
    const CHAINPOSRADIUS* motion_chain_pos, const CHAINPOSRADIUS* disp_chain_pos);
static void bone_data_parent_load_rob_chara(bone_data_parent* bone);

static void mot_interpolate(const mot* a1, float_t frame, float_t* value,
    mot_key_set* a4, uint32_t key_set_count, const struc_369* a6);
static uint32_t mot_load_last_key_calc(uint16_t keys_count);
static bool mot_load_file(mot* a1, const mot_data* a2);

static void mot_key_data_get_key_set_count_by_bone_database_bones(mot_key_data* a1,
    const std::vector<BODYTYPE>* a2);
static void mot_key_data_get_key_set_count(mot_key_data* a1, size_t motion_bone_count, size_t leaf_pos);
static void mot_key_data_init_key_sets(mot_key_data* a1, BONE_KIND type,
    size_t motion_bone_count, size_t leaf_pos);
static const mot_data* mot_key_data_load_file(mot_key_data* a1, uint32_t motion_id, const motion_database* mot_db);
static void mot_key_data_reserve_key_sets(mot_key_data* a1);

static void mothead_func_0(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_1(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_2(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_3(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_4(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_5(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_6(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_7(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_8(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_9(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_10(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_11(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_12(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_13(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_14(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_15(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_16(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_17(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_18(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_19(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_20(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_21(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_22(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_23(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_24(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_25(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_26(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_27(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_28(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_29(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_30(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_31(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_32(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_33(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_34(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_35(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_36(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_37(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_38(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_39(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_40(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_41(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_42(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_43(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_44(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_45(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_46(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_47(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_48(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_49(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_50_set_face_motion_id(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_51(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_52(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_53_set_face_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_54_set_hand_r_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_55_set_hand_l_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_56_set_mouth_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_57_set_eyes_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_58_set_eyelid_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_59_set_rob_chara_head_object(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_60_set_look_camera(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_61_set_eyelid_motion_from_face(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_62_rob_parts_adjust(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_63(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_64_osage_reset(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_65_motion_skin_param(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_66_osage_step(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_67_sleeve_adjust(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_68(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_69_motion_max_frame(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_70_camera_max_frame(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_71_osage_move_cancel(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_72(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_73_rob_hand_adjust(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_74_disable_collision(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_75_rob_adjust_global(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_76_rob_arm_adjust(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_77_disable_eye_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_78(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_79_rob_chara_coli_ring(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);
static void mothead_func_80_adjust_get_global_pos(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db);

static void mothead_mot_func_0(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_1(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_2(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_3(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_4(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_5(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_6(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_7(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_8(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_9(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_10(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_11(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_12(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_13(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_14(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_15(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_16(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_17(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_18(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_19(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_20(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_21(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_22(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_23(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_24(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_25(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_26(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_27(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_28(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_29(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_30(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_31(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_32(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_33(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_34(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_35(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_36(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_37(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_38(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_39(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_40(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_41(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_42(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_43(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_44(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_45(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_46(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_47(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_48(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_49(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_50(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_51(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_52(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_53(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_54(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_55(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_56(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_57(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_58(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_59(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_60(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_61(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_62(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_63(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_64(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_65(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_66(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_67(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);
static void mothead_mot_func_68(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data);

static PvOsageManager* pv_osage_manager_array_get(int32_t chara_id);

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

static void rob_chara_set_hand_l_object(rob_chara* rob_chr, object_info obj_info, int32_t type);
static void rob_chara_set_hand_r_object(rob_chara* rob_chr, object_info obj_info, int32_t type);

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
static void rob_chara_age_age_set_speed(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value);
static void rob_chara_age_age_set_skip(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id);
static void rob_chara_age_age_set_step(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value);
static void rob_chara_age_age_set_step_full(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id);

static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    const std::vector<BODYTYPE>* bones);
static void rob_chara_bone_data_get_ik_scale(
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data);
static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index);
static RobNode* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index);
static void rob_chara_bone_data_ik_scale_calculate(
    rob_chara_bone_data_ik_scale* ik_scale, prj::sys_vector<RobBlock>& bones,
    BONE_KIND base_skeleton_type, BONE_KIND skeleton_type, const bone_database* bone_data);
static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    BONE_KIND base_skeleton_type, BONE_KIND skeleton_type, const bone_database* bone_data);
static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
    BONE_KIND base_skeleton_type, BONE_KIND skeleton_type, const bone_database* bone_data);
static void rob_chara_bone_data_motion_blend_mot_free(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_list_free(rob_chara_bone_data* rob_bone_data,
    size_t last_index);
static void rob_chara_bone_data_motion_blend_mot_list_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data,
    uint32_t motion_id, MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db);
static void rob_chara_bone_data_reserve(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_set_look_anim_param(rob_chara_bone_data* rob_bone_data,
    const rob_chara_look_anim_eye_param* params, const eyes_adjust* eyes_adjust);
static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
    const std::vector<BODYTYPE>* bones, const std::string* motion_bones);
static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    const uint16_t* parent_indices);
static void rob_chara_bone_data_set_rot_y(rob_chara_bone_data* rob_bone_data, float_t rot_y);
static void rob_chara_bone_data_set_step(rob_chara_bone_data* rob_bone_data, float_t step);

static void rob_chara_age_age_ctrl(rob_chara* rob_chr, int32_t part_id, const char* name);
static object_info rob_chara_get_head_object(rob_chara* rob_chr, int32_t head_object_id);
static object_info rob_chara_get_object_info(rob_chara* rob_chr, ROB_PARTS_KIND rpk);
static void rob_chara_load_default_motion(rob_chara* rob_chr,
    const bone_database* bone_data, const motion_database* mot_db);
static void rob_chara_load_default_motion_sub(rob_chara* rob_chr, int32_t skeleton_select,
    uint32_t motion_id, const bone_database* bone_data, const motion_database* mot_db);

static void rob_chara_set_adjust(rob_chara* rob_chr, rob_chara_data_adjust* adjust_new,
    rob_chara_data_adjust* adjust, rob_chara_data_adjust* adjust_prev);
static void rob_chara_set_pv_data(rob_chara* rob_chr, int8_t chara_id,
    CHARA_NUM chara_num, int32_t cos_id, const rob_chara_pv_data& pv_data);

static void rob_cmn_mottbl_read(void* a1, const void* data, size_t size);

static void opd_chara_data_array_add_frame_data(int32_t chara_id);
static void opd_chara_data_array_encode_data(int32_t chara_id);
static void opd_chara_data_array_encode_init_data(int32_t chara_id, uint32_t motion_id);
static void opd_chara_data_array_fs_copy_file(int32_t chara_id);
static opd_chara_data* opd_chara_data_array_get(int32_t chara_id);
static void opd_chara_data_array_init_data(int32_t chara_id, uint32_t motion_id);
static void opd_chara_data_array_open_opd_file(int32_t chara_id);
static void opd_chara_data_array_open_opdi_file(int32_t chara_id);
static void opd_chara_data_array_write_file(int32_t chara_id);

static rob_manager_rob_impl* rob_manager_rob_impls1_get(TaskRobManager* rob_mgr);
static rob_manager_rob_impl* rob_manager_rob_impls2_get(TaskRobManager* rob_mgr);

rob_chara* rob_chara_array;
rob_chara_pv_data* rob_chara_pv_data_array;

rob_manager_rob_impl rob_manager_rob_impls1[2];
bool rob_manager_rob_impls1_init = false;
rob_manager_rob_impl rob_manager_rob_impls2[7];
bool rob_manager_rob_impls2_init = false;

opd_chara_data* opd_chara_data_array;
OpdChecker* opd_checker;
OpdMakeManager* opd_make_manager;
OpdMaker* opd_maker_array;
osage_play_data_database_struct* osage_play_data_database;
OsagePlayDataManager* osage_play_data_manager;
PvOsageManager* pv_osage_manager_array;
CmnMotTblHeader* common_mot_tbl_header;
rob_chara_age_age* rob_chara_age_age_array;
rob_sleeve_handler* rob_sleeve_handler_data;
RobThreadHandler* rob_thread_handler;
TaskRobLoad* task_rob_load;
TaskRobManager* task_rob_manager;

#if OPD_PLAY_GEN
OpdPlayGen* opd_play_gen;
std::atomic_bool opd_play_gen_run;
#endif

static int32_t opd_chara_data_counter = 0;
static int32_t opd_maker_counter = 0;
static int32_t osage_test_no_pause = 0;
static int32_t pv_osage_manager_counter = 0;
static int32_t rob_thread_parent_counter = 0;
static int32_t rob_chara_item_equip_object_disable_node_blocks = 0;

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
    { mothead_func_60_set_look_camera, 0 },
    { mothead_func_61_set_eyelid_motion_from_face, 0 },
    { mothead_func_62_rob_parts_adjust, 0 },
    { mothead_func_63, 0 },
    { mothead_func_64_osage_reset, 0 },
    { mothead_func_65_motion_skin_param, 0 },
    { mothead_func_66_osage_step, 0 },
    { mothead_func_67_sleeve_adjust, 0 },
    { mothead_func_68, 0 },
    { mothead_func_69_motion_max_frame, 0 },
    { mothead_func_70_camera_max_frame, 0 },
    { mothead_func_71_osage_move_cancel, 0 },
    { mothead_func_72, 0 },
    { mothead_func_73_rob_hand_adjust, 0 },
    { mothead_func_74_disable_collision, 0 },
    { mothead_func_75_rob_adjust_global, 0 },
    { mothead_func_76_rob_arm_adjust, 0 },
    { mothead_func_77_disable_eye_motion, 0 },
    { mothead_func_78, 0 },
    { mothead_func_79_rob_chara_coli_ring, 0 },
    { mothead_func_80_adjust_get_global_pos, 0 },
};

static const mothead_mot_func mothead_mot_func_array[] = {
    mothead_mot_func_0,
    mothead_mot_func_1,
    mothead_mot_func_2,
    mothead_mot_func_3,
    mothead_mot_func_4,
    mothead_mot_func_5,
    mothead_mot_func_6,
    mothead_mot_func_7,
    mothead_mot_func_8,
    mothead_mot_func_9,
    mothead_mot_func_10,
    mothead_mot_func_11,
    mothead_mot_func_12,
    mothead_mot_func_13,
    mothead_mot_func_14,
    mothead_mot_func_15,
    mothead_mot_func_16,
    mothead_mot_func_17,
    mothead_mot_func_18,
    mothead_mot_func_19,
    mothead_mot_func_20,
    mothead_mot_func_21,
    mothead_mot_func_22,
    mothead_mot_func_23,
    mothead_mot_func_24,
    mothead_mot_func_25,
    mothead_mot_func_26,
    mothead_mot_func_27,
    mothead_mot_func_28,
    mothead_mot_func_29,
    mothead_mot_func_30,
    mothead_mot_func_31,
    mothead_mot_func_32,
    mothead_mot_func_33,
    mothead_mot_func_34,
    mothead_mot_func_35,
    mothead_mot_func_36,
    mothead_mot_func_37,
    mothead_mot_func_38,
    mothead_mot_func_39,
    mothead_mot_func_40,
    mothead_mot_func_41,
    mothead_mot_func_42,
    mothead_mot_func_43,
    mothead_mot_func_44,
    mothead_mot_func_45,
    mothead_mot_func_46,
    mothead_mot_func_47,
    mothead_mot_func_48,
    mothead_mot_func_49,
    mothead_mot_func_40,
    mothead_mot_func_51,
    mothead_mot_func_52,
    mothead_mot_func_53,
    mothead_mot_func_54,
    mothead_mot_func_55,
    mothead_mot_func_56,
    mothead_mot_func_57,
    mothead_mot_func_58,
    mothead_mot_func_59,
    mothead_mot_func_60,
    mothead_mot_func_61,
    mothead_mot_func_62,
    mothead_mot_func_63,
    mothead_mot_func_64,
    mothead_mot_func_65,
    mothead_mot_func_66,
    mothead_mot_func_67,
    mothead_mot_func_68,
};

static const RobCollisionData MIKU_cb_tbl[] = {
    { { 0.02f , -0.005f,  0.0f   }, 0.09f , BONE_ID_KL_KOSI_ETC_WJ, 0 },
    { { 0.17f , -0.02f ,  0.03f  }, 0.09f , BONE_ID_CL_MUNE       , 1 },
    { { 0.17f , -0.02f , -0.03f  }, 0.09f , BONE_ID_CL_MUNE       , 1 },
    { { 0.0f  ,  0.02f ,  0.0f   }, 0.04f , BONE_ID_KL_KUBI       , 0 },
    { { 0.005f,  0.0f  ,  0.08f  }, 0.12f , BONE_ID_CL_KAO        , 1 },
    { { 0.03f ,  0.0f  ,  0.015f }, 0.055f, BONE_ID_KATA_R_WJ_CU  , 1 },
    { { 0.15f ,  0.0f  ,  0.01f  }, 0.05f , BONE_ID_KATA_R_WJ_CU  , 0 },
    { { 0.05f ,  0.0f  ,  0.01f  }, 0.05f , BONE_ID_UDE_R_WJ      , 0 },
    { { 0.17f ,  0.0f  ,  0.015f }, 0.06f , BONE_ID_UDE_R_WJ      , 0 },
    { { 0.08f ,  0.0f  ,  0.0f   }, 0.07f , BONE_ID_KL_TE_R_WJ    , 1 },
    { { 0.03f ,  0.0f  , -0.015f }, 0.055f, BONE_ID_KATA_L_WJ_CU  , 1 },
    { { 0.15f ,  0.0f  , -0.01f  }, 0.05f , BONE_ID_KATA_L_WJ_CU  , 0 },
    { { 0.05f ,  0.0f  , -0.01f  }, 0.05f , BONE_ID_UDE_L_WJ      , 0 },
    { { 0.17f ,  0.0f  , -0.015f }, 0.06f , BONE_ID_UDE_L_WJ      , 0 },
    { { 0.08f ,  0.0f  ,  0.0f   }, 0.07f , BONE_ID_KL_TE_L_WJ    , 1 },
    { { 0.04f , -0.02f , -0.02f  }, 0.13f , BONE_ID_J_MOMO_R_WJ   , 1 },
    { { 0.26f , -0.02f , -0.025f }, 0.09f , BONE_ID_J_MOMO_R_WJ   , 0 },
    { { 0.08f ,  0.0f  , -0.02f  }, 0.09f , BONE_ID_J_SUNE_R_WJ   , 0 },
    { { 0.28f ,  0.0f  , -0.02f  }, 0.09f , BONE_ID_J_SUNE_R_WJ   , 0 },
    { { 0.03f , -0.02f , -0.01f  }, 0.07f , BONE_ID_KL_ASI_R_WJ_CO, 0 },
    { { 0.02f , -0.02f , -0.02f  }, 0.035f, BONE_ID_KL_TOE_R_WJ   , 0 },
    { { 0.04f , -0.02f ,  0.02f  }, 0.13f , BONE_ID_J_MOMO_L_WJ   , 1 },
    { { 0.26f , -0.02f ,  0.025f }, 0.09f , BONE_ID_J_MOMO_L_WJ   , 0 },
    { { 0.09f ,  0.0f  ,  0.02f  }, 0.09f , BONE_ID_J_SUNE_L_WJ   , 0 },
    { { 0.28f ,  0.0f  ,  0.02f  }, 0.09f , BONE_ID_J_SUNE_L_WJ   , 0 },
    { { 0.03f , -0.02f ,  0.01f  }, 0.07f , BONE_ID_KL_ASI_L_WJ_CO, 0 },
    { { 0.02f , -0.02f ,  0.02f  }, 0.035f, BONE_ID_KL_TOE_L_WJ   , 0 },
};

static const RobCollisionData MIKU_cb_push_tbl[] = {
    { { -0.00391f, -0.08831f,  0.0f     }, 0.09f , BONE_ID_KL_KOSI_ETC_WJ, 0 },
    { {  0.17f   , -0.02f   ,  0.03f    }, 0.09f , BONE_ID_CL_MUNE       , 1 },
    { {  0.17f   , -0.02f   , -0.03f    }, 0.09f , BONE_ID_CL_MUNE       , 1 },
    { {  0.0f    ,  0.02f   ,  0.0f     }, 0.04f , BONE_ID_KL_KUBI       , 0 },
    { {  0.05025f,  0.0f    , -0.00181f }, 0.12f , BONE_ID_CL_KAO        , 1 },
    { {  0.03f   ,  0.0f    ,  0.015f   }, 0.055f, BONE_ID_KATA_R_WJ_CU  , 1 },
    { {  0.15f   ,  0.0f    ,  0.01f    }, 0.05f , BONE_ID_KATA_R_WJ_CU  , 0 },
    { {  0.05f   ,  0.0f    ,  0.01f    }, 0.05f , BONE_ID_UDE_R_WJ      , 0 },
    { {  0.17f   ,  0.0f    ,  0.015f   }, 0.06f , BONE_ID_UDE_R_WJ      , 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , BONE_ID_KL_TE_R_WJ    , 1 },
    { {  0.03f   ,  0.0f    , -0.015f   }, 0.055f, BONE_ID_KATA_L_WJ_CU  , 1 },
    { {  0.15f   ,  0.0f    , -0.01f    }, 0.05f , BONE_ID_KATA_L_WJ_CU  , 0 },
    { {  0.05f   ,  0.0f    , -0.01f    }, 0.05f , BONE_ID_UDE_L_WJ      , 0 },
    { {  0.17f   ,  0.0f    , -0.015f   }, 0.06f , BONE_ID_UDE_L_WJ      , 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , BONE_ID_KL_TE_L_WJ    , 1 },
    { {  0.04f   , -0.02f   , -0.02f    }, 0.13f , BONE_ID_J_MOMO_R_WJ   , 1 },
    { {  0.26f   , -0.02f   , -0.025f   }, 0.09f , BONE_ID_J_MOMO_R_WJ   , 0 },
    { {  0.08f   ,  0.0f    , -0.02f    }, 0.09f , BONE_ID_J_SUNE_R_WJ   , 0 },
    { {  0.28f   ,  0.0f    , -0.02f    }, 0.09f , BONE_ID_J_SUNE_R_WJ   , 0 },
    { {  0.03f   , -0.02f   , -0.01f    }, 0.07f , BONE_ID_KL_ASI_R_WJ_CO, 0 },
    { {  0.02f   , -0.02f   , -0.02f    }, 0.035f, BONE_ID_KL_TOE_R_WJ   , 0 },
    { {  0.04f   , -0.02f   ,  0.02f    }, 0.13f , BONE_ID_J_MOMO_L_WJ   , 1 },
    { {  0.26f   , -0.02f   ,  0.025f   }, 0.09f , BONE_ID_J_MOMO_L_WJ   , 0 },
    { {  0.08f   ,  0.0f    ,  0.02f    }, 0.09f , BONE_ID_J_SUNE_L_WJ   , 0 },
    { {  0.28f   ,  0.0f    ,  0.02f    }, 0.09f , BONE_ID_J_SUNE_L_WJ   , 0 },
    { {  0.03f   , -0.02f   ,  0.01f    }, 0.07f , BONE_ID_KL_ASI_L_WJ_CO, 0 },
    { {  0.02f   , -0.02f   ,  0.02f    }, 0.035f, BONE_ID_KL_TOE_L_WJ   , 0 },
};

static const RobCollisionData LUK_cb_tbl[] = {
    { { 0.02f    , -0.005f,  0.0f   }, 0.09f , BONE_ID_KL_KOSI_ETC_WJ, 0 },
    { { 0.165f   , -0.02f ,  0.015f }, 0.105f, BONE_ID_CL_MUNE       , 1 },
    { { 0.165f   , -0.02f , -0.015f }, 0.105f, BONE_ID_CL_MUNE       , 1 },
    { { 0.0f     ,  0.02f ,  0.0f   }, 0.04f , BONE_ID_KL_KUBI       , 0 },
    { { 0.005f   ,  0.0f  ,  0.08f  }, 0.12f , BONE_ID_CL_KAO        , 1 },
    { { 0.03f    ,  0.0f  ,  0.015f }, 0.055f, BONE_ID_KATA_R_WJ_CU  , 1 },
    { { 0.15f    ,  0.0f  ,  0.01f  }, 0.05f , BONE_ID_KATA_R_WJ_CU  , 0 },
    { { 0.05f    ,  0.0f  ,  0.01f  }, 0.05f , BONE_ID_UDE_R_WJ      , 0 },
    { { 0.17f    ,  0.0f  ,  0.015f }, 0.06f , BONE_ID_UDE_R_WJ      , 0 },
    { { 0.08f    ,  0.0f  ,  0.0f   }, 0.07f , BONE_ID_KL_TE_R_WJ    , 1 },
    { { 0.03f    ,  0.0f  , -0.015f }, 0.055f, BONE_ID_KATA_L_WJ_CU  , 1 },
    { { 0.15f    ,  0.0f  , -0.01f  }, 0.05f , BONE_ID_KATA_L_WJ_CU  , 0 },
    { { 0.05f    ,  0.0f  , -0.01f  }, 0.05f , BONE_ID_UDE_L_WJ      , 0 },
    { { 0.17f    ,  0.0f  , -0.015f }, 0.06f , BONE_ID_UDE_L_WJ      , 0 },
    { { 0.08f    ,  0.0f  ,  0.0f   }, 0.07f , BONE_ID_KL_TE_L_WJ    , 1 },
    { { 0.04f    , -0.02f , -0.02f  }, 0.13f , BONE_ID_J_MOMO_R_WJ   , 1 },
    { { 0.26f    , -0.02f , -0.025f }, 0.09f , BONE_ID_J_MOMO_R_WJ   , 0 },
    { { 0.08f    ,  0.0f  , -0.02f  }, 0.09f , BONE_ID_J_SUNE_R_WJ   , 0 },
    { { 0.28f    ,  0.0f  , -0.02f  }, 0.09f , BONE_ID_J_SUNE_R_WJ   , 0 },
    { { 0.03f    , -0.02f , -0.01f  }, 0.07f , BONE_ID_KL_ASI_R_WJ_CO, 0 },
    { { 0.02f    , -0.02f , -0.02f  }, 0.035f, BONE_ID_KL_TOE_R_WJ   , 0 },
    { { 0.04f    , -0.02f ,  0.02f  }, 0.13f , BONE_ID_J_MOMO_L_WJ   , 1 },
    { { 0.26f    , -0.02f ,  0.025f }, 0.09f , BONE_ID_J_MOMO_L_WJ   , 0 },
    { { 0.08f    ,  0.0f  ,  0.02f  }, 0.09f , BONE_ID_J_SUNE_L_WJ   , 0 },
    { { 0.28f    ,  0.0f  ,  0.02f  }, 0.09f , BONE_ID_J_SUNE_L_WJ   , 0 },
    { { 0.03f    , -0.02f ,  0.01f  }, 0.07f , BONE_ID_KL_ASI_L_WJ_CO, 0 },
    { { 0.02f    , -0.02f ,  0.02f  }, 0.035f, BONE_ID_KL_TOE_L_WJ   , 0 },
};

static const RobCollisionData LUK_cb_push_tbl[] = {
    { { -0.00391f, -0.08831f,  0.0f     }, 0.09f , BONE_ID_KL_KOSI_ETC_WJ, 0 },
    { {  0.165f  , -0.02f   ,  0.015f   }, 0.105f, BONE_ID_CL_MUNE       , 1 },
    { {  0.165f  , -0.02f   , -0.015f   }, 0.105f, BONE_ID_CL_MUNE       , 1 },
    { {  0.0f    ,  0.02f   ,  0.0f     }, 0.04f , BONE_ID_KL_KUBI       , 0 },
    { {  0.05025f,  0.0f    , -0.00181f }, 0.12f , BONE_ID_CL_KAO        , 1 },
    { {  0.03f   ,  0.0f    ,  0.015f   }, 0.055f, BONE_ID_KATA_R_WJ_CU  , 1 },
    { {  0.15f   ,  0.0f    ,  0.01f    }, 0.05f , BONE_ID_KATA_R_WJ_CU  , 0 },
    { {  0.05f   ,  0.0f    ,  0.01f    }, 0.05f , BONE_ID_UDE_R_WJ      , 0 },
    { {  0.17f   ,  0.0f    ,  0.015f   }, 0.06f , BONE_ID_UDE_R_WJ      , 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , BONE_ID_KL_TE_R_WJ    , 1 },
    { {  0.03f   ,  0.0f    , -0.015f   }, 0.055f, BONE_ID_KATA_L_WJ_CU  , 1 },
    { {  0.15f   ,  0.0f    , -0.01f    }, 0.05f , BONE_ID_KATA_L_WJ_CU  , 0 },
    { {  0.05f   ,  0.0f    , -0.01f    }, 0.05f , BONE_ID_UDE_L_WJ      , 0 },
    { {  0.17f   ,  0.0f    , -0.015f   }, 0.06f , BONE_ID_UDE_L_WJ      , 0 },
    { {  0.08f   ,  0.0f    ,  0.0f     }, 0.07f , BONE_ID_KL_TE_L_WJ    , 1 },
    { {  0.04f   , -0.02f   , -0.02f    }, 0.13f , BONE_ID_J_MOMO_R_WJ   , 1 },
    { {  0.26f   , -0.02f   , -0.025f   }, 0.09f , BONE_ID_J_MOMO_R_WJ   , 0 },
    { {  0.08f   ,  0.0f    , -0.02f    }, 0.09f , BONE_ID_J_SUNE_R_WJ   , 0 },
    { {  0.28f   ,  0.0f    , -0.02f    }, 0.09f , BONE_ID_J_SUNE_R_WJ   , 0 },
    { {  0.03f   , -0.02f   , -0.01f    }, 0.07f , BONE_ID_KL_ASI_R_WJ_CO, 0 },
    { {  0.02f   , -0.02f   , -0.02f    }, 0.035f, BONE_ID_KL_TOE_R_WJ   , 0 },
    { {  0.04f   , -0.02f   ,  0.02f    }, 0.13f , BONE_ID_J_MOMO_L_WJ   , 1 },
    { {  0.26f   , -0.02f   ,  0.025f   }, 0.09f , BONE_ID_J_MOMO_L_WJ   , 0 },
    { {  0.08f   ,  0.0f    ,  0.02f    }, 0.09f , BONE_ID_J_SUNE_L_WJ   , 0 },
    { {  0.28f   ,  0.0f    ,  0.02f    }, 0.09f , BONE_ID_J_SUNE_L_WJ   , 0 },
    { {  0.03f   , -0.02f   ,  0.01f    }, 0.07f , BONE_ID_KL_ASI_L_WJ_CO, 0 },
    { {  0.02f   , -0.02f   ,  0.02f    }, 0.035f, BONE_ID_KL_TOE_L_WJ   , 0 },
};

static const RobKamae kamae_data_tbl = {
    "COMMON_KAMAE",
    1,
    1,
    1,
    4,
    0,
};

static const RobKamae* kamae_tbl_miku[] = {
    &kamae_data_tbl,
};

static const RobData rob_data[] = {
    {
        0x0000, BONE_KIND_MIK,
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
        1, 0, MIKU_cb_tbl, MIKU_cb_push_tbl, kamae_tbl_miku,
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
        0x0107, BONE_KIND_RIN,
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
        9, 0, MIKU_cb_tbl, MIKU_cb_push_tbl, kamae_tbl_miku,
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
        0x0105, BONE_KIND_LEN,
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
        5, 0, MIKU_cb_tbl, MIKU_cb_push_tbl, kamae_tbl_miku,
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
        0x0106, BONE_KIND_LUK,
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
        6, 0, LUK_cb_tbl, LUK_cb_push_tbl, kamae_tbl_miku,
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
        0x0108, BONE_KIND_NER,
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
        8, 0, MIKU_cb_tbl, MIKU_cb_push_tbl, kamae_tbl_miku,
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
        0x0109, BONE_KIND_HAK,
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
        3, 0, LUK_cb_tbl, LUK_cb_push_tbl, kamae_tbl_miku,
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
        0x010D, BONE_KIND_KAI,
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
        4, 0, MIKU_cb_tbl, MIKU_cb_push_tbl, kamae_tbl_miku,
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
        0x010E, BONE_KIND_MEI,
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
        7, 0, LUK_cb_tbl, LUK_cb_push_tbl, kamae_tbl_miku,
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
        0x010F, BONE_KIND_SAK,
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
        10, 0, LUK_cb_tbl, LUK_cb_push_tbl, kamae_tbl_miku,
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
        0x063D, BONE_KIND_TET,
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
        467, 0, MIKU_cb_tbl, MIKU_cb_push_tbl, kamae_tbl_miku,
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

static const BONE_BLK rob_ik_hand_leg_bones[] = {
    BLK_C_KATA_R, BLK_C_KATA_L,
    BLK_CL_MOMO_R, BLK_CL_MOMO_L,
};

static const BONE_BLK dword_140A2DC90[] = {
    BLK_C_KATA_R, BLK_KL_TE_R_WJ, BLK_N_HITO_R_EX,
    BLK_NL_HITO_R_WJ, BLK_NL_HITO_B_R_WJ, BLK_NL_HITO_C_R_WJ,
    BLK_N_KO_R_EX, BLK_NL_KO_R_WJ, BLK_NL_KO_B_R_WJ,
    BLK_NL_KO_C_R_WJ, BLK_N_KUSU_R_EX, BLK_NL_KUSU_R_WJ,
    BLK_NL_KUSU_B_R_WJ, BLK_NL_KUSU_C_R_WJ, BLK_N_NAKA_R_EX,
    BLK_NL_NAKA_R_WJ, BLK_NL_NAKA_B_R_WJ, BLK_NL_NAKA_C_R_WJ,
    BLK_N_OYA_R_EX, BLK_NL_OYA_R_WJ, BLK_NL_OYA_B_R_WJ,
    BLK_NL_OYA_C_R_WJ, BLK_N_STE_R_WJ_EX, BLK_N_SUDE_R_WJ_EX,
    BLK_N_SUDE_B_R_WJ_EX, BLK_N_HIJI_R_WJ_EX, BLK_N_UP_KATA_R_EX,
    BLK_N_SKATA_R_WJ_CD_EX, BLK_N_SKATA_B_R_WJ_CD_CU_EX,
    BLK_N_SKATA_C_R_WJ_CD_CU_EX, BLK_DUMMY
};

static const BONE_BLK dword_140A2DD10[] = {
    BLK_C_KATA_L, BLK_KL_TE_L_WJ, BLK_N_HITO_L_EX,
    BLK_NL_HITO_L_WJ, BLK_NL_HITO_B_L_WJ, BLK_NL_HITO_C_L_WJ,
    BLK_N_KO_L_EX, BLK_NL_KO_L_WJ, BLK_NL_KO_B_L_WJ,
    BLK_NL_KO_C_L_WJ, BLK_N_KUSU_L_EX, BLK_NL_KUSU_L_WJ,
    BLK_NL_KUSU_B_L_WJ, BLK_NL_KUSU_C_L_WJ, BLK_N_NAKA_L_EX,
    BLK_NL_NAKA_L_WJ, BLK_NL_NAKA_B_L_WJ, BLK_NL_NAKA_C_L_WJ,
    BLK_N_OYA_L_EX, BLK_NL_OYA_L_WJ, BLK_NL_OYA_B_L_WJ,
    BLK_NL_OYA_C_L_WJ, BLK_N_STE_L_WJ_EX, BLK_N_SUDE_L_WJ_EX,
    BLK_N_SUDE_B_L_WJ_EX, BLK_N_HIJI_L_WJ_EX, BLK_N_UP_KATA_L_EX,
    BLK_N_SKATA_L_WJ_CD_EX, BLK_N_SKATA_B_L_WJ_CD_CU_EX,
    BLK_N_SKATA_C_L_WJ_CD_CU_EX, BLK_DUMMY
};

static const BONE_BLK dword_140A2DD90[] = {
    BLK_CL_MOMO_R, BLK_KL_ASI_R_WJ_CO, BLK_KL_TOE_R_WJ,
    BLK_N_HIZA_R_WJ_EX, BLK_N_MOMO_A_R_WJ_CD_EX,
    BLK_N_MOMO_B_R_WJ_EX, BLK_N_MOMO_C_R_WJ_EX, BLK_DUMMY
};

static const BONE_BLK dword_140A2DDB0[] = {
    BLK_CL_MOMO_L, BLK_KL_ASI_L_WJ_CO, BLK_KL_TOE_L_WJ,
    BLK_N_HIZA_L_WJ_EX, BLK_N_MOMO_A_L_WJ_CD_EX,
    BLK_N_MOMO_B_L_WJ_EX, BLK_N_MOMO_C_L_WJ_EX, BLK_DUMMY
};

static const BONE_ID dword_140A2DDD0[] = {
    BONE_ID_DUMMY, BONE_ID_KL_TE_R_WJ, BONE_ID_KL_TE_L_WJ, BONE_ID_KL_ASI_R_WJ_CO,
    BONE_ID_KL_ASI_L_WJ_CO, BONE_ID_J_SUNE_R_WJ, BONE_ID_J_SUNE_L_WJ,
    BONE_ID_UDE_R_WJ, BONE_ID_UDE_L_WJ, BONE_ID_CL_KAO, BONE_ID_DUMMY,
    BONE_ID_DUMMY, BONE_ID_DUMMY, BONE_ID_DUMMY, BONE_ID_DUMMY, BONE_ID_DUMMY,
    BONE_ID_DUMMY, BONE_ID_KL_TE_R_WJ, BONE_ID_KL_TE_L_WJ, BONE_ID_KL_ASI_R_WJ_CO,
    BONE_ID_KL_ASI_L_WJ_CO, BONE_ID_J_SUNE_R_WJ, BONE_ID_J_SUNE_L_WJ,
    BONE_ID_UDE_R_WJ, BONE_ID_UDE_L_WJ, BONE_ID_CL_KAO, BONE_ID_DUMMY,
};

static const BONE_BLK rob_motion_c_kata_bones[] = {
    BLK_C_KATA_L,
    BLK_C_KATA_R,
};

static const BONE_ID rob_kl_te_bones[] = {
    BONE_ID_KL_TE_L_WJ,
    BONE_ID_KL_TE_R_WJ
};

static const BONE_BLK* off_140C9E000[] = {
    dword_140A2DC90,
    dword_140A2DD10,
};

static const BONE_BLK* off_140C9E020[] = {
    dword_140A2DD10,
    dword_140A2DC90,
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

const RobData* get_rob_data(CHARA_NUM cn) {
    if (cn >= 0 && cn < CN_MAX)
        return &rob_data[cn];
    return 0;
}

int32_t rob_data_get_chara_size_index(CHARA_NUM cn) {
    const RobData* rob_data = get_rob_data(cn);
    if (rob_data)
        return rob_data->field_848.chara_size_index;
    return 1;
}

int32_t rob_data_get_swim_costume(CHARA_NUM cn) {
    const RobData* RobData = get_rob_data(cn);
    if (rob_data)
        return rob_data->field_848.swim_costume;
    return 0;
}

int32_t rob_data_get_swim_s_costume(CHARA_NUM cn) {
    const RobData* RobData = get_rob_data(cn);
    if (rob_data)
        return rob_data->field_848.swim_s_costume;
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

// 0x1405376C0
uint32_t get_common_rob_mot(CHARA_NUM cn, int32_t kamae_type, int32_t id) {
    if (common_mot_tbl_header
        && cn >= 0 && (uint32_t)cn < common_mot_tbl_header->cn_num
        && id >= 0 && (uint32_t)id < common_mot_tbl_header->mtp_num) {
        CmnMotTblBase* base = (CmnMotTblBase*)((uint8_t*)common_mot_tbl_header
            + common_mot_tbl_header->data_tbl);
        if (kamae_type >= 0 && kamae_type < base[cn].kamae_max)
            return ((uint32_t*)((uint8_t*)common_mot_tbl_header
                + *(uint32_t*)((uint8_t*)common_mot_tbl_header
                    + kamae_type * sizeof(uint32_t) + base[cn].tbl)))[id];
    }
    return -1;
}

const char* get_dev_ram_opdi_dir() {
    return "dev_ram/opdi";
}

const float_t get_gravity() {
    return 0.00299444468691945f;
}

const uint32_t* get_opd_motion_ids() {
    static const uint32_t opd_motion_ids[] = {
        195, (uint32_t)-1, // CMN_MRA00_13_01
    };

    return opd_motion_ids;
}

const uint32_t* get_opd_motion_set_ids() {
    static const uint32_t opd_motion_set_ids[] = {
        51, 194, 797, (uint32_t)-1, // EDT, EDT2, EDTF1
    };

    return opd_motion_set_ids;
}

const char* get_ram_osage_play_data_dir() {
    return "ram/osage_play_data";
}

const char* get_ram_osage_play_data_tmp_dir() {
    return "ram/osage_play_data_tmp";
}

const char* get_rom_osage_play_data_dir() {
    return "rom/osage_play_data";
}

const char* get_rom_osage_play_data_opdi_dir() {
    return "rom/osage_play_data/opdi";
}

OpdChecker* opd_checker_get() {
    return opd_checker;
}

bool opd_checker_check_state_not_3() {
    return opd_checker_get()->CheckStateNot3();
}

bool opd_checker_has_objects() {
    return !!opd_checker_get()->GetObjects().size();
}

void opd_checker_launch_thread() {
    opd_checker_get()->LaunchThread();
}

void opd_checker_terminate_thread() {
    opd_checker_get()->TerminateThread();
}

void opd_make_manager_add_task(const OpdMakeManagerArgs& args) {
    opd_make_manager->add_task(args);
}

bool opd_make_manager_check_task_ready() {
    return app::TaskWork::check_task_ready(opd_make_manager);
}

bool opd_make_manager_del_task() {
    return opd_make_manager->del_task();
}

OpdMakeManagerData* opd_make_manager_get_data() {
    return opd_make_manager->GetData();
}

void opd_make_start() {
    std::vector<std::string>& objects = opd_checker_get()->GetObjects();
    std::vector<int32_t> motion_ids;
    opd_make_start_get_motion_ids(motion_ids);

    OpdMakeManagerArgs args;
    args.modules = 0;
    args.use_current_skp = false;
    args.use_opdi = true;
    args.motion_ids = &motion_ids;
    args.objects = &objects;
    opd_make_manager->add_task(args);
}

void opd_make_start_get_motion_ids(std::vector<int32_t>& motion_ids) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    const uint32_t* opd_motion_set_ids = get_opd_motion_set_ids();
    while (*opd_motion_set_ids != -1) {
        const motion_set_info* set_info = aft_mot_db->get_motion_set_by_id(*opd_motion_set_ids);
        if (!set_info) {
            opd_motion_set_ids++;
            continue;
        }

        motion_ids.reserve(set_info->motion.size());
        for (const motion_info& i : set_info->motion)
            motion_ids.push_back(i.id);
        opd_motion_set_ids++;
    }

    /*const uint32_t* opd_motion_ids = get_opd_motion_ids();
    while (*opd_motion_ids != -1) {
        motion_ids.push_back(*opd_motion_ids);
        opd_motion_ids++;
    }*/
    motion_ids.push_back(195); // CMN_MRA00_13_01
    prj::sort_unique(motion_ids);
}

void opd_make_stop() {
    opd_checker_terminate_thread();
    opd_make_manager_del_task();
}

bool osage_play_data_manager_add_task() {
    return osage_play_data_manager->add();
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

void osage_play_database_load() {
    data_struct* aft_data = &data_list[DATA_AFT];

    for (const std::string& i : mdata_manager_get()->GetPrefixes()) {
        std::string path(get_rom_osage_play_data_dir());
        path.append("/");
        path.append(i);
        path.append("opd_db.txt");

        if (aft_data->check_file_exists(path.c_str()))
            osage_play_data_database->load(path.c_str());
    }
}

void pv_osage_manager_array_reset(int32_t chara_id) {
    pv_osage_manager_array_get(chara_id)->Reset();
}

void rob_init() {
    if (!opd_chara_data_array)
        opd_chara_data_array = new opd_chara_data[ROB_ID_MAX];

    if (!opd_checker)
        opd_checker = new OpdChecker;

    if (!opd_make_manager)
        opd_make_manager = new OpdMakeManager;

    if (!opd_maker_array)
        opd_maker_array = new OpdMaker[OPD_MAKE_COUNT];

    if (!osage_play_data_database)
        osage_play_data_database = new osage_play_data_database_struct;

    if (!osage_play_data_manager)
        osage_play_data_manager = new OsagePlayDataManager;

    if (!pv_osage_manager_array)
        pv_osage_manager_array = new PvOsageManager[ROB_ID_MAX];

    if (!rob_chara_age_age_array)
        rob_chara_age_age_array = new rob_chara_age_age[ROB_ID_MAX * 3];

    if (!rob_chara_array)
        rob_chara_array = new rob_chara[ROB_ID_MAX];

    if (!rob_chara_pv_data_array)
        rob_chara_pv_data_array = new rob_chara_pv_data[ROB_ID_MAX];

    {
        p_file_handler rob_mot_tbl_file_handler;
        rob_mot_tbl_file_handler.read_file(&data_list[DATA_AFT], "rom/rob/", "rob_mot_tbl.bin", prj::MemCSystem);
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

    free_def(common_mot_tbl_header);

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

    if (osage_play_data_database) {
        delete osage_play_data_database;
        osage_play_data_database = 0;
    }

    if (opd_maker_array) {
        delete[] opd_maker_array;
        opd_maker_array = 0;
    }

    if (opd_make_manager) {
        delete opd_make_manager;
        opd_make_manager = 0;
    }

    if (opd_checker) {
        delete opd_checker;
        opd_checker = 0;
    }

    if (opd_chara_data_array) {
        delete[] opd_chara_data_array;
        opd_chara_data_array = 0;
    }
}

static void rob_chara_item_equip_object_ctrl_step(rob_chara_item_equip_object* skin_disp, bool disable_ex_force) {
    for (int32_t i = 0; i < 6; i++)
        for (ExNodeBlock*& j : skin_disp->ex_node_block)
            j->CtrlStep(i, disable_ex_force);
}

static void rob_chara_item_equip_object_load_opd_data(rob_chara_item_equip_object* skin_disp) {
    if (!skin_disp->osage_blk.size() && !skin_disp->cloth.size())
        return;

#if OPD_PLAY_GEN
    rob_chara_item_equip* rob_disp = (rob_chara_item_equip*)skin_disp->rob_disp;
    size_t index = 0;
    for (opd_blend_data& i : rob_disp->opd_blend_data) {
#else
    const rob_chara_item_equip* rob_disp = skin_disp->rob_disp;
    size_t index = 0;
    for (const opd_blend_data& i : rob_disp->opd_blend_data) {
#endif
        const float_t* opd_data = 0;
        uint32_t opd_count = 0;
        osage_play_data_manager_get_opd_file_data(skin_disp->obj_uid, i.motion_id, opd_data, opd_count);
        if (!opd_data)
            break;

#if OPD_PLAY_GEN
        bool no_loop = opd_count & 0x80000000u;
        opd_count &= ~0x80000000u;

        i.no_loop = no_loop;
#endif

        for (ExOsageBlock*& j : skin_disp->osage_blk)
            opd_data = j->LoadOpdData(index, opd_data, opd_count);

        for (ExClothBlock*& j : skin_disp->cloth)
            opd_data = j->LoadOpdData(index, opd_data, opd_count);
        index++;
    }
}

static void rob_chara_item_equip_object_ctrl(rob_chara_item_equip_object* skin_disp) {
    if (rob_chara_item_equip_object_disable_node_blocks)
        return;

    if (skin_disp->init_cnt > 0) {
        skin_disp->pos_reset(skin_disp->init_cnt);
        skin_disp->init_cnt = 0;
    }

    if (!skin_disp->ex_node_block.size()) // Added
        return;

    for (ExNodeBlock*& i : skin_disp->ex_node_block)
        i->CtrlBegin();

    if (skin_disp->use_opd) {
        skin_disp->use_opd = false;
        rob_chara_item_equip_object_load_opd_data(skin_disp);
        for (ExNodeBlock*& i : skin_disp->ex_node_block)
            i->CtrlOsagePlayData();
    }
    else {
        if (skin_disp->osage_depends_on_others)
            rob_chara_item_equip_object_ctrl_step(skin_disp, false);

        for (ExNodeBlock*& i : skin_disp->ex_node_block)
            i->ctrl();
    }

    for (ExNodeBlock*& i : skin_disp->ex_node_block)
        i->CtrlEnd();
}

static void rob_chara_item_equip_ctrl(rob_chara_item_equip* rob_disp) {
    if (!osage_test_no_pause && !rob_disp->disable_update)
        for (int32_t i = rob_disp->disp_begin; i < rob_disp->disp_max; i++)
            rob_chara_item_equip_object_ctrl(&rob_disp->skin_disp[i]);
}

static void rob_chara_data_adjust_ctrl(rob_chara* rob_chr,
    rob_chara_data_adjust* adjust, rob_chara_data_adjust* adjust_prev) {
    float_t cycle = adjust->cycle;
    adjust->frame += rob_chr->data.motion.step_data.frame;
    if (rob_chr->data.motdata.field_0.field_58 == 1)
        cycle *= (60.0f / rob_chr->data.motdata.field_0.frame);
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

    adjust->curr_ex_force = (adjust->ex_force_cycle + cycle_val)
        * (adjust->ex_force * adjust->ex_force_cycle_strength) + adjust->ex_force;

    switch (adjust->type) {
    case 1: {
        mat4 mat = mat4_identity;
        mat4_mul_rotate_y(&mat, rob_chr->data.position.yang.get_rad(), &mat);
        mat4_transform_vector(&mat, &adjust->curr_ex_force, &adjust->curr_ex_force);
    }
    case 2: {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, BONE_NODE_KL_MUNE_B_WJ)->no_scale_mat;
        if (mat)
            mat4_transform_vector(mat, &adjust->curr_ex_force, &adjust->curr_ex_force);
    } break;
    case 3: {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, BONE_NODE_KL_KUBI)->no_scale_mat;
        if (mat)
            mat4_transform_vector(mat, &adjust->curr_ex_force, &adjust->curr_ex_force);
    } break;
    case 4: {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, BONE_NODE_FACE_ROOT)->no_scale_mat;
        if (mat)
            mat4_transform_vector(mat, &adjust->curr_ex_force, &adjust->curr_ex_force);
    } break;
    case 5: {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, BONE_NODE_KL_KOSI_ETC_WJ)->no_scale_mat;
        if (mat) {
            adjust->curr_ex_force.z = -adjust->curr_ex_force.z;
            mat4_transform_vector(mat, &adjust->curr_ex_force, &adjust->curr_ex_force);
        }
    } break;
    }

    if (adjust->ignore_gravity)
        adjust->curr_ex_force.y = get_gravity() + adjust->curr_ex_force.y;

    adjust->curr_force = adjust->force;
    adjust->curr_strength = adjust->strength;

    bool transition_frame_step = false;
    if (adjust->transition_duration > adjust->transition_frame
        && fabsf(adjust->transition_duration - adjust->transition_frame) > 0.000001f) {
        transition_frame_step = true;
        float_t blend = (adjust->transition_frame + 1.0f) / (adjust->transition_duration + 1.0f);
        adjust->curr_ex_force = vec3::lerp(adjust_prev->curr_ex_force,
            adjust->curr_ex_force, blend);
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

static void rob_chara_item_equip_object_set_parts_ex_force(
    rob_chara_item_equip_object* skin_disp, const rob_osage_parts_bit& parts_bits,
    const vec3* ex_force, const float_t& force, const float_t& gain) {
    for (ExOsageBlock*& i : skin_disp->osage_blk)
        if (i->osage_work.CheckPartsBits(parts_bits)) {
            i->osage_work.SetNodesExternalForce(ex_force, gain);
            i->osage_work.SetNodesForce(force);
        }
}

static void rob_chara_item_equip_set_parts_ex_force(rob_chara_item_equip* rob_disp,
    const rob_osage_parts& parts, const vec3* ex_force, const float_t& force, const float_t& strength) {
    ROB_PARTS_KIND rpk = (ROB_PARTS_KIND)(parts == ROB_OSAGE_PARTS_MUFFLER ? RPK_OUTER : RPK_KAMI);
    if (!osage_setting_data_obj_has_key(rob_disp->skin_disp[rpk].obj_uid))
        return;

    switch (parts) {
    case ROB_OSAGE_PARTS_SHORT_L:
    case ROB_OSAGE_PARTS_SHORT_R:
        rob_disp->parts_short = true;
        break;
    case ROB_OSAGE_PARTS_APPEND_L:
    case ROB_OSAGE_PARTS_APPEND_R:
        rob_disp->parts_append = true;
        break;
    case ROB_OSAGE_PARTS_WHITE_ONE_L:
        rob_disp->parts_white_one_l = true;
        break;
    }

    rob_osage_parts_bit parts_bits = (rob_osage_parts_bit)(1 << parts);
    if (!rob_disp->parts_short) {
        if (parts == ROB_OSAGE_PARTS_LEFT)
            enum_or(parts_bits, ROB_OSAGE_PARTS_SHORT_L_BIT);
        else if (parts == ROB_OSAGE_PARTS_RIGHT)
            enum_or(parts_bits, ROB_OSAGE_PARTS_SHORT_R_BIT);
    }

    if (!rob_disp->parts_append) {
        if (parts == ROB_OSAGE_PARTS_LEFT)
            enum_or(parts_bits, ROB_OSAGE_PARTS_APPEND_L_BIT);
        else if (parts == ROB_OSAGE_PARTS_RIGHT)
            enum_or(parts_bits, ROB_OSAGE_PARTS_APPEND_R_BIT);
    }

    if (!rob_disp->parts_white_one_l && parts == ROB_OSAGE_PARTS_LONG_C)
        enum_or(parts_bits, ROB_OSAGE_PARTS_WHITE_ONE_L_BIT);

    rob_chara_item_equip_object_set_parts_ex_force(
        &rob_disp->skin_disp[rpk], parts_bits, ex_force, force, strength);
}

static void rob_chara_item_equip_object_set_ex_force(
    rob_chara_item_equip_object* skin_disp, const vec3& f) {
    for (ExOsageBlock*& i : skin_disp->osage_blk)
        i->set_ex_force(f);

    for (ExClothBlock*& i : skin_disp->cloth)
        i->set_ex_force(f);
}

static void rob_chara_item_equip_set_ex_force(
    rob_chara_item_equip* rob_disp, const vec3& f) {
    for (int32_t i = rob_disp->disp_begin; i < rob_disp->disp_max; i++)
        rob_chara_item_equip_object_set_ex_force(&rob_disp->skin_disp[i], f);
}

void rob_chara::adjust_ctrl() {
    rob_chara_item_equip* rob_disp = this->rob_disp;

    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++) {
        rob_chara_data_adjust* parts_adjust = &data.motion.parts_adjust[i];
        if (parts_adjust->enable) {
            rob_chara_data_adjust_ctrl(this, parts_adjust, &data.motion.parts_adjust_prev[i]);
            rob_osage_parts parts = (rob_osage_parts)i;
            rob_chara_item_equip_set_parts_ex_force(rob_disp, parts,
                &parts_adjust->curr_ex_force, parts_adjust->curr_force, parts_adjust->curr_strength);
        }
    }

    rob_chara_data_adjust* adjust_global = &data.motion.adjust_global;
    if (adjust_global->enable) {
        rob_chara_data_adjust_ctrl(this, adjust_global, &data.motion.adjust_global_prev);
        rob_chara_item_equip_set_ex_force(rob_disp, adjust_global->curr_ex_force);
    }

    rob_disp->parts_short = false;
    rob_disp->parts_append = false;
    rob_disp->parts_white_one_l = false;
}

static BONE_BLK rob_motion_c_kata_bone_get(int32_t index) {
    switch (index) {
    case 0:
        return BLK_C_KATA_L;
    case 1:
        return BLK_C_KATA_R;
    default:
        return BLK_DUMMY;
    }
}

static void rob_chara_bone_data_set_motion_arm_length(rob_chara_bone_data* rob_bone_data,
    BONE_BLK block_id, float_t value) {
    rob_bone_data->motion_loaded.front()->set_arm_length(block_id, value);
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

object_info rob_chara::get_rob_data_face_object(int32_t index) {
    if (index >= 0 && index < 15)
        return rob_data->face_objects[index];
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
    if (id >= 214 && id <= 223)
        return pv_data.motion_face_ids[id - 214];
    else if (id == 224)
        return data.motion.motion_id;
    else if (id >= 226 && id <= 235)
        return pv_data.motion_face_ids[id - 226];
    return get_common_rob_mot(chara_num, data.action.kamae_type, id);
}

float_t rob_chara::get_pos_scale(int32_t bone, vec3& pos) {
    if (bone < 0 || bone > 26)
        return 0.0f;
    pos = data.field_1E68.field_DF8[bone].pos;
    return data.field_1E68.field_DF8[bone].scale;
}

static int16_t sub_14054FE90(rob_chara* rob_chr, bool a3) {
    int16_t v3 = rob_chr->data.position.yang.value;
    if (!a3 || !rob_chr->data.flag.bit.compel) {
        int16_t v4 = rob_chr->data.action.field_B8.field_8;
        if (v4)
            return v3 + v4;
    }

    if (rob_chr->data.motdata.field_0.field_20.field_0 & 0x100)
        return v3 + rob_chr->data.motdata.field_0.field_274;
    return v3;
}

static void sub_14041B9D0(rob_chara_bone_data* rob_bone_data) {
    rob_chara_sleeve_adjust& sleeve_adjust = rob_bone_data->sleeve_adjust;
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
    if (rob_chr->data.motdata.field_0.field_2C0 >= 0.0f)
        return rob_chr->data.motdata.field_0.field_2C0;
    else if (rob_chr->data.flag.bit.ringout)
        return 10.0;
    else if ((rob_chr->data.motdata.field_0.field_10.field_0 & 0x4000)
        || (rob_chr->data.motdata.field_0.field_20.field_8 & 0x08))
        return 5.0f;
    else if (rob_chr->data.motion.motion_id == rob_chr->data.motion.prev_motion_id)
        return 4.0f;
    else if (rob_chr->data.motdata.field_0.field_20.field_0 & 0x200) {
        if (rob_chr->data.flag.bit.old_fix_hara || rob_chr->data.flag.bit.land)
            return 1.0f;
    }
    else {
        if (!(rob_chr->data.motdata.field_0.field_20.field_0 & 0x8000)
            || !(rob_chr->data.motdata.field_0.field_20.field_0 & 0x01))
            return 7.0f;
    }
    return 3.0f;
}

static float_t sub_14054FEE0(rob_chara* a1) {
    if (a1->data.motdata.field_0.field_2C4 >= 0.0f)
        return a1->data.motdata.field_0.field_2C4;
    else if (a1->data.motdata.field_0.field_20.field_0 & 0x200)
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
        v3 = !(rob_chr->data.motdata.field_0.field_20.field_0 & 0x400)
            || !(rob_chr->data.motdata.field_0.field_10.field_0 & 0x40)
            || rob_chr->data.motdata.field_0.field_20.field_0 & 0x100000;
    }

    bool v6;
    int32_t v7;
    if (rob_chr->data.motdata.field_0.field_52) {
        v6 = 0;
        v7 = 2;
    }
    else if (rob_chr->data.motdata.field_0.field_20.field_0 & 0x8000) {
        v6 = 0;
        v7 = 2;
    }
    else {
        v6 = 1;
        v7 = 1;
    }

    if (rob_chr->data.motdata.field_0.field_52 != 3 && rob_chr->data.action.kamae_type) {
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

    float_t v12 = rob_chr->data.motdata.field_0.frame;
    float_t v13 = v12 - v9;
    if (v3 && v6 && (v13 < v11 || v13 <= v8)) {
        float_t v14 = rob_chr->data.motdata.field_0.frame;
        float_t v16 = prj::floorf((v12 - v8) * 0.5f) + v8;
        if (v11 > v16) {
            float_t v17;
            if (v13 < v16) {
                v13 = v16;
                v17 = v16;
                v9 = v12 - v16;
            }
            else
                v17 = rob_chr->data.motdata.field_0.frame - v9;
            v10 = v17 - v8;
        }
        else {
            v13 = v8 + v10;
            v9 = v12 - v11;
        }
    }
    rob_chr->bone_data->set_motion_blend_duration(v9, 1.0f, 1.0f);

    sub_14041D310(rob_chr->bone_data, v9, v13, v7);
    sub_14041D340(rob_chr->bone_data, !!(rob_chr->data.motdata.field_0.field_54 & 0x40));
    sub_14041D6C0(rob_chr->bone_data, (rob_chr->data.motdata.field_0.field_40.field_0 & 0x10)
        && (rob_chr->data.motdata.field_0.field_20.field_0 & 0x08));
    sub_14041D6F0(rob_chr->bone_data,
        !!(rob_chr->data.motdata.field_0.field_10.field_8 & 0x18000000));
    sub_14041D720(rob_chr->bone_data,
        !!(rob_chr->data.motdata.field_0.field_30.field_8 & 0x18000000));
    sub_14041D2A0(rob_chr->bone_data,
        (float_t)((double_t)rob_chr->data.motdata.field_0.field_276 * M_PI * (1.0 / 32768.0)));
    sub_14041D270(rob_chr->bone_data,
        (float_t)((double_t)rob_chr->data.motdata.field_0.field_274 * M_PI * (1.0 / 32768.0)));
}

// 0x140536D30
static void mothead_apply_mot_inner(mothead_mot_func_data* func_data, int32_t type, const mothead_mot_data* mhd_mot_data) {
    mothead_mot_func_array[type](func_data, mhd_mot_data->data, mhd_mot_data);
}

// 0x140537380
static void mothead_apply_mot(struc_223* a1, rob_chara* rob_chr, const mothead_mot_data* mhd_mot_data) {
    if (!mhd_mot_data || !mhd_mot_data->data)
        return;

    mothead_mot_func_data func_data;
    func_data.rob_chara = rob_chr;
    func_data.rob_base = &rob_chr->data;
    func_data.field_10 = &rob_chr->data.motdata;

    while (mhd_mot_data->data && mhd_mot_data->type >= 0) {
        mothead_apply_mot_inner(&func_data, mhd_mot_data->type, mhd_mot_data);
        mhd_mot_data++;
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

    const mothead_mot* mot = mothead_storage_get_mot_by_motion_id(motion_id, mot_db);
    if (!mot)
        return;

    a1->field_330.field_0.current = mot->data;
    a1->field_330.field_0.data = mot->data;
    a1->field_7A0 = mot->field_28;
    a1->motion_set_id = mot_db->get_motion_set_id_by_motion_id(motion_id);
    a1->field_0.field_10 = mot->field_0;
    a1->field_0.field_20 = mot->field_0;
    a1->field_0.field_20.field_0 &= ~0x4000;
    if (a1->field_0.field_10.field_0 & 0x100)
        a1->field_0.field_274 = (int16_t)0x8000;
    if (a1->field_0.field_10.field_0 & 0x200000)
        a1->field_0.field_2B8 = (int16_t)0x8000;

    a1->field_0.field_50 = mot->field_10;
    a1->field_0.field_52 = mot->field_12;
    if (mot->field_12 & 0x40) {
        a1->field_0.field_52 = mot->field_12 & ~0x40;
        a1->field_0.field_54 |= 0x40;
    }

    mothead_apply_mot(a1, rob_chr, mot->mot_data);

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

    const mothead_data* data = mot->data;
    if (!data)
        return;

    mothead_data_type type = data->type;
    if (data->type < 0)
        return;

    while (type != MOTHEAD_DATA_TYPE_11)
        if ((type = (data++)->type) < 0)
            return;

    const void* d = data->data;
    while (d) {
        a1->field_0.field_C += *(int16_t*)d;

        mothead_data_type type = (data++)->type;
        if (type < MOTHEAD_DATA_TYPE_0)
            break;

        while (type != MOTHEAD_DATA_TYPE_11)
            if ((type = (data++)->type) < 0)
                return;
        d = data->data;
    }
}

void rob_chara::load_body_parts_object_info(ROB_PARTS_KIND rpk, object_info obj_info,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    rob_disp->load_body_parts_object_info(rpk, obj_info, bone_data, data, obj_db);
}

void rob_chara::load_motion(uint32_t motion_id, bool a3, float_t frame,
    MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db) {
    data.position.yang = sub_14054FE90(this, false);
    if (!(data.motdata.field_0.field_20.field_0 & 0x10)) {
        data.position.pos.x = data.position.gpos.x;
        data.position.pos.z = data.position.gpos.z;
    }

    if (data.motdata.field_0.field_20.field_8 & 0x8000000)
        data.position.pos.y = data.position.gpos.y;

    if (a3)
        data.motion.field_28 ^= 0x04;

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
    data.adjust_data.get_global_pos = false;

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

    rob_disp->set_osage_move_cancel(0, 0.0f);
    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        rob_disp->set_disable_collision((rob_osage_parts)i, false);
    data.motdata.field_0.field_30 = data.motdata.field_0.field_10;
    data.motdata.field_0.field_40 = data.motdata.field_0.field_20;
    data.motdata.field_0.field_276 = data.motdata.field_0.field_274;
    data.motdata.reset();
    sub_14053A9C0(&data.motdata, this, motion_id,
        motion_storage_get_mot_data_frame_count(motion_id, mot_db), mot_db);
    if (data.motdata.field_0.field_40.field_C & 0x20)
        data.motdata.field_0.field_20.field_0 &= ~0x08;
    if (data.motdata.field_0.field_40.field_C & 0x40)
        data.motdata.field_0.field_20.field_8 |= 0x10000000;
    if (!(data.motdata.field_0.field_20.field_0 & 0x10) && frame > 0.0f)
        data.motdata.field_0.field_20.field_0 &= ~0x08;

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
    rob_disp->set_osage_step(step);

    rob_chara_bone_data_motion_load(this->bone_data, motion_id, blend_type, bone_data, mot_db);
    this->bone_data->set_motion_frame(data.motion.step_data.frame,
        data.motion.step_data.frame, data.motdata.field_0.frame_count);
    this->bone_data->set_motion_playback_state(MOT_PLAY_FRAME_DATA_PLAYBACK_FORWARD);
    if (data.motdata.field_0.field_58 == 1) {
        this->bone_data->set_motion_loop_state(MOT_PLAY_FRAME_DATA_LOOP_CONTINUOUS);
        this->bone_data->set_motion_loop(data.motdata.field_0.loop_begin,
            data.motdata.field_0.loop_count, -1.0f);
    }
    rob_chara_bone_data_set_rot_y(this->bone_data,
        (float_t)((double_t)data.motdata.field_0.field_2B8 * M_PI * (1.0 / 32768.0)));
    sub_14041BC40(this->bone_data, !!(data.motdata.field_0.field_20.field_8 & 0x20000000));
    sub_14041B9D0(this->bone_data);
    if (type == ROB_CHARA_TYPE_2) {
        sub_14041D340(this->bone_data, !!(data.motdata.field_0.field_54 & 0x40));
        sub_14041D6C0(this->bone_data, (data.motdata.field_0.field_40.field_0 & 0x10)
            && (data.motdata.field_0.field_20.field_0 & 0x08));
        sub_14041D6F0(this->bone_data,
            !!(data.motdata.field_0.field_10.field_8 & 0x18000000));
        sub_14041D720(this->bone_data,
            !!(data.motdata.field_0.field_30.field_8 & 0x18000000));
        sub_14041D2A0(this->bone_data,
            (float_t)((double_t)data.motdata.field_0.field_276 * M_PI * (1.0 / 32768.0)));
        sub_14041D270(this->bone_data,
            (float_t)((double_t)data.motdata.field_0.field_274 * M_PI * (1.0 / 32768.0)));
    }
    else
        sub_140555B00(this, blend_type >= MOTION_BLEND_FREEZE && blend_type <= MOTION_BLEND_CROSS);
    data.field_3DE0 = 0;
}

void rob_chara::load_outfit_object_info(ROB_PARTS_KIND rpk, object_info obj_info,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    rob_disp->load_outfit_object_info(rpk, obj_info, osage_reset, bone_data, data, obj_db);
}

static void rob_chara_head_adjust(rob_chara* rob_chr);
static bool rob_chara_hands_adjust(rob_chara* rob_chr);
static bool sub_14053B580(rob_chara* rob_chr, int32_t a2);
static void sub_14053B260(rob_chara* rob_chr);

static bool sub_14053B530(rob_chara* rob_chr) {
    return sub_14053B580(rob_chr, 2) | sub_14053B580(rob_chr, 3);
}

static void rob_chara_bone_data_set_left_hand_scale(rob_chara_bone_data* rob_bone_data, float_t scale) {
    mat4* kl_te_wj_mat = rob_bone_data->get_mats_mat(BONE_ID_KL_TE_L_WJ);
    if (!kl_te_wj_mat)
        return;

    vec3 pos;
    mat4_get_translation(kl_te_wj_mat, &pos);
    pos -= pos * scale;

    mat4 mat;
    mat4_scale(scale, scale, scale, &mat);
    mat4_set_translation(&mat, &pos);

    for (int32_t i = BONE_ID_KL_TE_L_WJ; i <= BONE_ID_NL_OYA_C_L_WJ; i++) {
        mat4* m = rob_bone_data->get_mats_mat(i);
        if (m)
            mat4_mul(m, &mat, m);
    }

    for (int32_t i = BONE_NODE_KL_TE_L_WJ; i <= BONE_NODE_NL_OYA_C_L_WJ; i++) {
        RobNode* node = rob_chara_bone_data_get_node(rob_bone_data, i);
        if (node)
            node->transform.scale = scale;
    }
}

static void rob_chara_bone_data_set_right_hand_scale(rob_chara_bone_data* rob_bone_data, float_t scale) {
    mat4* kl_te_wj_mat = rob_bone_data->get_mats_mat(BONE_ID_KL_TE_R_WJ);
    if (!kl_te_wj_mat)
        return;

    vec3 pos;
    mat4_get_translation(kl_te_wj_mat, &pos);
    pos -= pos * scale;

    mat4 mat;
    mat4_scale(scale, scale, scale, &mat);
    mat4_set_translation(&mat, &pos);

    for (int32_t i = BONE_ID_KL_TE_R_WJ; i <= BONE_ID_NL_OYA_C_R_WJ; i++) {
        mat4* m = rob_bone_data->get_mats_mat(i);
        if (m)
            mat4_mul(m, &mat, m);
    }

    for (int32_t i = BONE_NODE_KL_TE_R_WJ; i <= BONE_NODE_NL_OYA_C_R_WJ; i++) {
        RobNode* node = rob_chara_bone_data_get_node(rob_bone_data, i);
        if (node)
            node->transform.scale = scale;
    }
}

// 0x140550400
void rob_chara::pos_reset() {
    rob_disp->pos_reset(0);
}

void rob_chara::rob_info_ctrl() {
    data.field_3DA0 = data.field_3D9C;
    if (data.field_1E68.field_64 > 0.01f)
        data.flag.bit.wall_moved_mot = 1;

    //sub_140517CC0(this);
    //sub_140517B80(this);
    //sub_140517F20(this);

    /*int32_t v4 = sub_14013C8C0()->sub_1400E7910();
    if (v4 >= 4 && v4 != 8)
        sub_14054FF20(this);*/
}

void rob_chara::rob_motion_modifier_ctrl() {
    rob_chara_head_adjust(this);
    rob_chara_hands_adjust(this);

    if (!sub_14053B530(this))
        sub_14053B260(this);

    float_t left_hand_scale_default = data.adjust_data.left_hand_scale_default;
    float_t right_hand_scale_default = data.adjust_data.right_hand_scale_default;
    float_t left_hand_scale = data.adjust_data.left_hand_scale;
    float_t right_hand_scale = data.adjust_data.right_hand_scale;

    if (left_hand_scale_default > 0.0f)
        if (left_hand_scale <= 0.0f)
            left_hand_scale = data.adjust_data.left_hand_scale_default;
        else
            left_hand_scale *= left_hand_scale_default;

    if (right_hand_scale_default > 0.0f)
        if (right_hand_scale <= 0.0f)
            right_hand_scale = data.adjust_data.right_hand_scale_default;
        else
            right_hand_scale *= right_hand_scale_default;

    if (left_hand_scale > 0.0f)
        rob_chara_bone_data_set_left_hand_scale(bone_data, left_hand_scale);

    if (right_hand_scale > 0.0f)
        rob_chara_bone_data_set_right_hand_scale(bone_data, right_hand_scale);
}

static const struc_243& sub_140510550(CHARA_NUM chara_num) {
    static const struc_243 stru_1411AD680[] = {
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
        {
            (float_t)(-60.0 * DEG_TO_RAD), (float_t)(35.0 * DEG_TO_RAD),
            (float_t)(60.0 * DEG_TO_RAD), (float_t)(130.0 * DEG_TO_RAD), 0.0f, 0.0f,
            (float_t)(35.0 * DEG_TO_RAD), (float_t)(25.0 * DEG_TO_RAD), 0.3f, 0.95f,
        },
    };
    return stru_1411AD680[chara_num];
}

static void sub_14041C5C0(rob_chara_bone_data* rob_bone_data, const struc_243& a2) {
    rob_bone_data->look_anim.field_15C = a2;
}

static const rob_chara_look_anim_eye_param* rob_chara_look_anim_eye_param_array_get(CHARA_NUM chara_num) {
    static const rob_chara_look_anim_eye_param rob_chara_look_anim_eye_param_array[] = {
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.043633230f, 0.061086524f, -0.080285146f, 0.113446399f, },
            { 0.0149f, -0.0213f, 0.0f, }, 1.0f, 1.0f, -3.8f, 6.0f, -3.8f, 6.0f,
        },
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.075049161f, 0.104719758f, -0.073303826f, 0.125663697f, },
            { 0.0335f, -0.0112f, 0.0f, }, 1.0f, 1.0f, -5.5f, 10.0f, -5.5f, 6.5f,
        },
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.083775803f, 0.085521131f, -0.104719758f, 0.120427720f, },
            { 0.0167f, -0.0054f, 0.0f, }, 1.0f, 1.0f, -6.5f, 6.0f, -6.0f, 2.0f,
        },
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.034906584f, 0.069813169f, -0.087266460f, 0.113446399f, },
            { 0.0114f, 0.0081f, 0.0f, }, 1.0f, 1.0f, -3.5f, 6.0f, -3.5f, 6.0f,
        },
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.034906584f, 0.073303826f, -0.090757116f, 0.139626339f, },
            { 0.0151f, -0.044f, 0.0f, }, 1.0f, 1.0f, -2.6f, 6.0f, -2.6f, 6.0f,
        },
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.052359879f, 0.122173048f, -0.087266460f, 0.130899697f, },
            { 0.0222f, -0.0002f, 0.0f, }, 1.0f, 1.0f, -4.0f, 7.5f, -3.5f, 6.0f,
        },
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.073303826f, 0.101229101f, -0.078539818f, 0.113446399f, },
            { 0.025f, -0.0231f, 0.0f, }, 1.0f, 1.0f, -4.0f, 6.0f, -3.8f, 3.5f,
        },
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.066322512f, 0.059341195f, -0.078539818f, 0.113446399f, },
            { 0.0118f, -0.0051f, 0.0f, }, 1.0f, 1.0f, -5.6f, 6.0f, -5.6f, 5.5f,
        },
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.064577184f, 0.066322512f, -0.069813169f, 0.113446399f, },
            { 0.0223f, -0.0149f, 0.0f, }, 1.0f, 1.0f, -5.0f, 4.0f, -5.0f, 2.5f,
        },
        {
            { -0.061086524f, 0.104719758f, -0.080285146f, 0.113446399f, },
            { -0.069813169f, 0.069813169f, -0.113446399f, 0.122173048f, },
            { 0.0137f, -0.008f, 0.0f, }, 1.0f, 1.0f, -5.5f, 6.0f, -5.0f, 4.0f,
        },
    };

    return &rob_chara_look_anim_eye_param_array[chara_num];
}

static void sub_140514130(rob_chara_item_equip* rob_disp, ROB_PARTS_KIND rpk, bool a3) {
    if (a3)
        rob_disp->parts_attr[rpk] &= ~0x01;
    else
        rob_disp->parts_attr[rpk] |= 0x01;
}

static void sub_140514110(rob_chara_item_equip* rob_disp, ROB_PARTS_KIND rpk, bool a3) {
    if (a3)
        rob_disp->parts_attr[rpk] &= ~0x02;
    else
        rob_disp->parts_attr[rpk] |= 0x02;
}

static void sub_140513C60(rob_chara_item_equip* rob_disp, ROB_PARTS_KIND rpk, bool a3) {
    if (a3)
        rob_disp->parts_attr[rpk] &= ~0x04;
    else
        rob_disp->parts_attr[rpk] |= 0x04;
}

void rob_chara::reset_data(const rob_chara_pv_data* pv_data,
    const bone_database* bone_data, const motion_database* mot_db) {
    this->bone_data->reset();
    rob_chara_bone_data_init_data(this->bone_data,
        BONE_KIND_CMN, rob_data->skeleton_type, bone_data);
    data.reset();
    data_prev.reset();
    rob_disp->set_motion_node(rob_chara_bone_data_get_node(
        this->bone_data, BONE_NODE_N_HARA_CP), bone_data);
    type = pv_data->type;
    rob_chara_data* base = &data;
    base->position.yang = pv_data->yang;
    base->position.hara_yang = pv_data->yang;
    float_t scale = chara_size_table_get_value(this->pv_data.chara_size_index);
    base->adjust_data.scale = scale;
    base->adjust_data.item_scale = scale; // X
    base->adjust_data.height_adjust = this->pv_data.height_adjust;
    base->adjust_data.pos_adjust_y = chara_pos_adjust_y_table_get_value(this->pv_data.chara_size_index);
    base->action.kamae_data = rob_data->kamae_tbl[base->action.kamae_type];
    sub_14041C5C0(this->bone_data, sub_140510550(chara_num));
    rob_chara_bone_data_set_look_anim_param(this->bone_data,
        rob_chara_look_anim_eye_param_array_get(chara_num), &pv_data->eyes_adjust);
    this->bone_data->sleeve_adjust.sleeve_l = pv_data->sleeve_l;
    this->bone_data->sleeve_adjust.sleeve_r = pv_data->sleeve_r;
    this->bone_data->sleeve_adjust.enable1 = true;
    this->bone_data->sleeve_adjust.enable2 = false;
    rob_chara_bone_data_get_ik_scale(this->bone_data, bone_data);
    rob_chara_load_default_motion(this, bone_data, mot_db);
    base->action.field_4.field_0 = 1;
    base->action.field_4.motion_id = get_rob_cmn_mottbl_motion_id(0);
    base->action.field_0 = 0;
    base->action.field_4.field_10 = pv_data->field_5;
    load_motion(base->action.field_4.motion_id, pv_data->field_5, 0.0f, MOTION_BLEND, bone_data, mot_db);
    field_C = true;
    rob_disp->field_DC = 0;
    sub_140513C60(rob_disp, RPK_ATAMA, false);
    sub_140513C60(rob_disp, RPK_TE_R, false);
    sub_140513C60(rob_disp, RPK_TE_L, false);
    object_info head_obj = rob_chara_get_head_object(this, 0);
    base->motion.field_150.head_object = head_obj;
    base->motion.field_3B0.head_object = head_obj;
    set_parts_disp(RPK_MAX, true);
}

void rob_chara::reset_osage() {
    for (rob_chara_data_adjust& i : data.motion.parts_adjust)
        i.reset();

    for (rob_chara_data_adjust& i : data.motion.parts_adjust_prev)
        i.reset();

    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        rob_disp->reset_nodes_ex_force((rob_osage_parts)i);

    data.motion.adjust_global.reset();
    data.motion.adjust_global_prev.reset();

    rob_disp->reset_ex_force();
    rob_disp->parts_short = false;
    rob_disp->parts_append = false;
    rob_disp->parts_white_one_l = false;

    set_osage_step(-1.0f);
}

void rob_chara::set_bone_data_frame(float_t frame) {
    bone_data->set_frame(frame);
    bone_data->interpolate();
    bone_data->update(0);
}

void rob_chara::set_chara_color(bool value) {
    rob_disp->chara_color = value;
}

void rob_chara::set_chara_height_adjust(bool value) {
    data.adjust_data.height_adjust = value;
}

void rob_chara::set_chara_pos_adjust(const vec3& value) {
    data.adjust_data.pos_adjust = value;
}

void rob_chara::set_chara_pos_adjust_y(float_t value) {
    data.adjust_data.pos_adjust_y = value;
}

void rob_chara::set_chara_size(float_t value) {
    data.adjust_data.scale = value;
    data.adjust_data.item_scale = value; // X
}

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
    rob_chr->bone_data->set_eyelid_frame(motion->data.frame);
    rob_chr->bone_data->set_eyelid_step(motion->data.step);
    rob_chr->bone_data->set_eyelid_blend_duration(
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
    rob_chr->bone_data->set_eyes_frame(motion->data.frame);
    rob_chr->bone_data->set_eyes_step(motion->data.step);
    rob_chr->bone_data->set_eyes_blend_duration(
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
    rob_chr->bone_data->set_face_frame(motion->data.frame);
    rob_chr->bone_data->set_face_step(motion->data.step);
    rob_chr->bone_data->set_face_blend_duration(
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_hand_l_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db) {
    switch (type) {
    case 0:
    default:
        rob_chr->data.motion.field_150.hand_l.data = motion->data;
        if ((rob_chr->data.motion.field_29 & 0x08) || (rob_chr->data.motion.field_2A & 0x04))
            return;
        break;
    case 1:
        rob_chr->data.motion.hand_l.data = motion->data;
        if ((rob_chr->data.motion.field_29 & 0x08) || !(rob_chr->data.motion.field_2A & 0x04))
            return;
        break;
    case 2:
        rob_chr->data.motion.field_3B0.hand_l.data = motion->data;
        if (!(rob_chr->data.motion.field_29 & 0x08) || (rob_chr->data.motion.field_2A & 0x04))
            return;
        break;
    }

    rob_chr->bone_data->load_hand_l_motion(motion->data.motion_id, mot_db);
    rob_chr->bone_data->set_hand_l_frame(motion->data.frame);
    rob_chr->bone_data->set_hand_l_step(motion->data.step);
    rob_chr->bone_data->set_hand_l_blend_duration(
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_hand_r_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db) {
    switch (type) {
    case 0:
    default:
        rob_chr->data.motion.field_150.hand_r.data = motion->data;
        if ((rob_chr->data.motion.field_29 & 0x10) || (rob_chr->data.motion.field_2A & 0x08))
            return;
        break;
    case 1:
        rob_chr->data.motion.hand_r.data = motion->data;
        if ((rob_chr->data.motion.field_29 & 0x10) || !(rob_chr->data.motion.field_2A & 0x08))
            return;
        break;
    case 2:
        rob_chr->data.motion.field_3B0.hand_r.data = motion->data;
        if (!(rob_chr->data.motion.field_29 & 0x10) || (rob_chr->data.motion.field_2A & 0x08))
            return;
        break;
    }

    rob_chr->bone_data->load_hand_r_motion(motion->data.motion_id, mot_db);
    rob_chr->bone_data->set_hand_r_frame(motion->data.frame);
    rob_chr->bone_data->set_hand_r_step(motion->data.step);
    rob_chr->bone_data->set_hand_r_blend_duration(
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
    rob_chr->bone_data->set_mouth_frame(motion->data.frame);
    rob_chr->bone_data->set_mouth_step(motion->data.step);
    rob_chr->bone_data->set_mouth_blend_duration(
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

static vec3* rob_chara_bone_data_get_gblctr_pos(rob_chara_bone_data* rob_bone_data) {
    return &rob_bone_data->motion_loaded.front()->bone_data.gblctr_pos;
}

static void rob_chara_data_adjuct_set_pos(rob_chara_adjust_data* rob_chr_adj,
    const vec3& pos, bool pos_adjust, const vec3* gblctr_pos) {
    float_t scale = rob_chr_adj->scale;
    float_t item_scale = rob_chr_adj->item_scale; // X
    vec3 _offset = rob_chr_adj->offset;
    if (gblctr_pos)
        _offset.y += gblctr_pos->y;

    vec3 _pos = pos;
    vec3 _item_pos = pos;
    if (rob_chr_adj->height_adjust) {
        _pos.y += rob_chr_adj->pos_adjust_y;
        _item_pos.y += rob_chr_adj->pos_adjust_y; // X
    }
    else {
        vec3 temp = (_pos - _offset) * scale + _offset;
        vec3 arm_temp = (_item_pos - _offset) * item_scale + _offset;

        if (!rob_chr_adj->offset_x) {
            _pos.x = temp.x;
            _item_pos.x = arm_temp.x; // X
        }

        if (!rob_chr_adj->offset_y) {
            _pos.y = temp.y;
            _item_pos.y = arm_temp.y; // X
        }

        if (!rob_chr_adj->offset_z) {
            _pos.z = temp.z;
            _item_pos.z = arm_temp.z; // X
        }
    }

    if (pos_adjust) {
        _pos = rob_chr_adj->pos_adjust + _pos;
        _item_pos = rob_chr_adj->pos_adjust + _item_pos; // X
    }

    rob_chr_adj->pos = _pos - pos * scale;
    rob_chr_adj->item_pos = _item_pos - pos * item_scale; // X
}

void rob_chara::set_data_adjust_mat(rob_chara_adjust_data* rob_chr_adj, bool pos_adjust) {
    mat4* mat = bone_data->get_mats_mat(BONE_ID_N_HARA_CP);

    vec3 pos;
    mat4_get_translation(mat, &pos);

    vec3* gblctr_pos = 0;
    if (rob_chr_adj->get_global_pos)
        gblctr_pos = rob_chara_bone_data_get_gblctr_pos(bone_data);
    rob_chara_data_adjuct_set_pos(rob_chr_adj, pos, pos_adjust, gblctr_pos);

    float_t scale = rob_chr_adj->scale;
    mat4_scale(scale, scale, scale, &rob_chr_adj->mat);
    mat4_set_translation(&rob_chr_adj->mat, &rob_chr_adj->pos);

    float_t item_scale = rob_chr_adj->item_scale; // X
    mat4_scale(item_scale, item_scale, item_scale, &rob_chr_adj->item_mat);
    mat4_set_translation(&rob_chr_adj->item_mat, &rob_chr_adj->item_pos);
}

void rob_chara::set_base_position_pos(const vec3& value) {
    data.position.pos = value;
}

void rob_chara::set_base_position_yang(const RobAngle& value) {
    data.position.yang = value;
}

void rob_chara::set_disable_collision(rob_osage_parts parts, bool disable) {
    rob_disp->set_disable_collision(parts, disable);
}

void rob_chara::set_eyelid_mottbl_motion(int32_t type, int32_t mottbl_index,
    float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobEyelidMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.loop_state = loop_state;
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
            motion.data.playback_state = ROB_PARTIAL_MOTION_PLAYBACK_STOP;
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

void rob_chara::set_eyes_mottbl_motion(int32_t type, int32_t mottbl_index,
    float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobEyesMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.loop_state = loop_state;
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

void rob_chara::set_face_mottbl_motion(int32_t type, int32_t mottbl_index,
    float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, bool a11, const motion_database* mot_db) {
    RobFaceMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.loop_state = loop_state;
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
        if (rob_chara_get_object_info(this, RPK_ATAMA) != v17) {
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
    set_face_object(get_rob_data_face_object(index), 0);
}

void rob_chara::set_frame(float_t frame) {
    bone_data->set_frame(frame);
    data.motion.frame_data.frame = frame;
}

static void sub_140551870(rob_chara* rob_chr, float_t blend_duration,
    float_t blend_offset, const motion_database* aft_mot_db) {
    if (!(rob_chr->data.motion.field_2A & 0x04))
        return;

    rob_chara_set_hand_l_object(rob_chr, {}, 1);
    rob_chr->data.motion.field_2A &= ~0x04;

    RobHandMotion motion;
    motion.data = rob_chr->data.motion.field_150.hand_l.data;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    rob_chara_set_hand_l_motion(rob_chr, &motion, 0, aft_mot_db);
}

static void sub_140550B90(rob_chara* rob_chr, float_t blend_duration,
    float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->set_hand_l_mottbl_motion(1, 192, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
    sub_140551870(rob_chr, blend_duration, blend_offset, aft_mot_db);
}

static void sub_140554690(rob_chara* rob_chr, int32_t mottbl_index,
    float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->data.motion.field_2A |= 0x04;
    rob_chr->set_hand_l_mottbl_motion(1, mottbl_index, value, playback_state,
        blend_duration, play_duration, step, loop_state, blend_offset, aft_mot_db);
}

static void sub_1405519B0(rob_chara* rob_chr, float_t blend_duration,
    float_t blend_offset, const motion_database* aft_mot_db) {
    if (!(rob_chr->data.motion.field_2A & 0x08))
        return;

    rob_chara_set_hand_r_object(rob_chr, {}, 1);
    rob_chr->data.motion.field_2A &= ~0x08;

    RobHandMotion motion;
    motion.data = rob_chr->data.motion.field_150.hand_r.data;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    rob_chara_set_hand_r_motion(rob_chr, &motion, 0, aft_mot_db);
}

static void sub_140550C10(rob_chara* rob_chr, float_t blend_duration,
    float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->set_hand_r_mottbl_motion(1, 192, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
    sub_1405519B0(rob_chr, blend_duration, blend_offset, aft_mot_db);
}
static void sub_140554710(rob_chara* rob_chr, int32_t mottbl_index, float_t value,
    rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration, float_t step,
    rob_partial_motion_loop_state loop_state, float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->data.motion.field_2A |= 0x08;
    rob_chr->set_hand_r_mottbl_motion(1, mottbl_index, value, playback_state,
        blend_duration, play_duration, step, loop_state, blend_offset, aft_mot_db);
}

void rob_chara::set_hand_item(int32_t uid, float_t blend_duration) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    int32_t mottbl_index = 192;
    object_info obj_left = {};
    object_info obj_right = {};
    float_t left_hand_scale = -1.0f;
    float_t right_hand_scale = -1.0f;

    const hand_item* hand_item = hand_item_handler_data_get_hand_item(uid, chara_num);
    if (hand_item) {
        if (hand_item->obj_left.not_null()) {
            left_hand_scale = hand_item->hand_scale;
            obj_left = hand_item->obj_left;
        }

        if (hand_item->obj_right.not_null()) {
            right_hand_scale = hand_item->hand_scale;
            obj_right = hand_item->obj_right;
        }

        mottbl_index = hand_item->hand_mottbl_index;
    }

    if (obj_left.is_null())
        sub_140550B90(this, blend_duration, 0.0f, aft_mot_db);
    else {
        sub_140554690(this, mottbl_index, 1.0f, ROB_PARTIAL_MOTION_PLAYBACK_STOP, blend_duration,
            0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chara_set_hand_l_object(this, obj_left, 1);
    }

    if (obj_right.is_null())
        sub_140550C10(this, blend_duration, 0.0f, aft_mot_db);
    else {
        sub_140554710(this, mottbl_index, 1.0f, ROB_PARTIAL_MOTION_PLAYBACK_STOP, blend_duration,
            0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chara_set_hand_r_object(this, obj_right, 1);
    }

    data.adjust_data.left_hand_scale_default = left_hand_scale;
    data.adjust_data.right_hand_scale_default = right_hand_scale;
}

void rob_chara::set_hand_item_l(int32_t uid) {
    const hand_item* hand_item = hand_item_handler_data_get_hand_item(uid, chara_num);
    object_info obj_info = {};
    if (hand_item && hand_item->obj_left.not_null())
        obj_info = hand_item->obj_left;
    rob_chara_set_hand_l_object(this, obj_info, 0);
}

void rob_chara::set_hand_item_r(int32_t uid) {
    const hand_item* hand_item = hand_item_handler_data_get_hand_item(uid, chara_num);
    object_info obj_info = {};
    if (hand_item && hand_item->obj_right.not_null())
        obj_info = hand_item->obj_right;
    rob_chara_set_hand_r_object(this, obj_info, 0);
}

void rob_chara::set_hand_l_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.loop_state = loop_state;
    if (motion.data.motion_id != -1) {
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion.data.motion_id, mot_db);
        motion.data.frame = (motion.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_hand_l_motion(this, &motion, type, mot_db);
}

void rob_chara::set_hand_r_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.loop_state = loop_state;
    if (motion.data.motion_id != -1) {
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion.data.motion_id, mot_db);
        motion.data.frame = (motion.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_hand_r_motion(this, &motion, type, mot_db);
}

void rob_chara::set_left_hand_scale(float_t value) {
    data.adjust_data.left_hand_scale = value;
}

void rob_chara::set_look_camera(bool update_view_point, bool rotation_enable, float_t head_rot_strength,
    float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8, bool ft) {
    bone_data->set_look_anim(update_view_point, rotation_enable, head_rot_strength,
        eyes_rot_strength, blend_duration, eyes_rot_step, a8, ft);
    if (fabsf(blend_duration) <= 0.000001f)
        data.flag.bit.dmy_yokerare = 1;
}

void rob_chara::set_look_camera_new(bool rotation_enable, float_t head_rot_strength,
    float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8) {
    set_look_camera(true, rotation_enable, head_rot_strength,
        eyes_rot_strength, blend_duration, eyes_rot_step, a8, true);
}

void rob_chara::set_look_camera_old(bool rotation_enable, float_t head_rot_strength,
    float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8) {
    set_look_camera(true, rotation_enable, head_rot_strength,
        eyes_rot_strength, blend_duration, eyes_rot_step, a8, false);
}

bool rob_chara::set_motion_id(uint32_t motion_id,
    float_t frame, float_t blend_duration, bool blend, bool set_motion_reset_data,
    MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db) {
    if (!blend && data.motion.motion_id == motion_id)
        return false;

    if (blend_duration <= 0.0f)
        blend_type = MOTION_BLEND;
    load_motion(motion_id, false, frame, blend_type, bone_data, mot_db);
    this->bone_data->set_motion_blend_duration(blend_duration, 1.0f, 0.0f);
    data.motion.field_28 |= 0x80;
    data.motion.frame_data.last_set_frame = frame;
    set_motion_skin_param(motion_id, (float_t)(int32_t)frame);
    if (blend_duration == 0.0f) {
        if (set_motion_reset_data)
            rob_chara::set_motion_reset_data(motion_id, frame);

        rob_disp->skin_disp[RPK_TE_L].init_cnt = 60;
        rob_disp->skin_disp[RPK_TE_R].init_cnt = 60;

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
    if (data.motdata.field_0.field_58 != 1) {
        data.motdata.field_0.field_58 = 1;
        data.motdata.field_0.frame = data.motdata.field_0.frame_count - 1.0f;
    }
}

void rob_chara::set_motion_reset_data(uint32_t motion_id, float_t frame) {
    rob_disp->set_motion_reset_data(motion_id, frame);
}

void rob_chara::set_motion_skin_param(uint32_t motion_id, float_t frame) {
    rob_disp->set_motion_skin_param(chara_id, motion_id, (int32_t)prj::roundf(frame));
}

void rob_chara::set_motion_step(float_t value) {
    data.motion.step_data.step = value;
}

void rob_chara::set_mouth_mottbl_motion(int32_t type,
    int32_t mottbl_index, float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobMouthMotion motion;
    motion.data.motion_id = get_rob_cmn_mottbl_motion_id(mottbl_index);
    motion.data.mottbl_index = mottbl_index;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_data = &data.motion.frame_data;
        motion.data.step_data = &data.motion.step_data;
    }
    motion.data.loop_state = loop_state;
    if (motion.data.motion_id != -1) {
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion.data.motion_id, mot_db);
        motion.data.frame = (motion.data.frame_count - 1.0f) * value;
    }
    rob_chara_set_mouth_motion(this, &motion, type, mot_db);
}

void rob_chara::set_osage_move_cancel(uint8_t id, float_t mv_ccl) {
    rob_disp->set_osage_move_cancel(id, mv_ccl);
    if (id < 2) {
        rob_chara_age_age_array_set_move_cancel(chara_id, 1, mv_ccl);
        rob_chara_age_age_array_set_move_cancel(chara_id, 2, mv_ccl);
    }
}

void rob_chara::set_osage_reset() {
    rob_disp->set_osage_reset();
}

void rob_chara::set_osage_step(float_t value) {
    data.motion.step = value;
    if (value < 0.0f)
        value = data.motion.step_data.frame;
    rob_disp->set_osage_step(value);
}

void rob_chara::set_parts_disp(ROB_PARTS_KIND rpk, bool disp) {
    if (rpk < 0)
        return;
    else if (rpk < RPK_MAX)
        rob_disp->set_disp(rpk, disp);
    else if (rpk == RPK_MAX)
        for (int32_t i = RPK_DISP_BEGIN; i <= RPK_DISP_END; i++) {
            rob_disp->set_disp((ROB_PARTS_KIND)i, disp);
            if (i == RPK_ATAMA && check_for_ageageagain_module()) {
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
        rob_disp->shadow_flag |= 0x04;
    else
        rob_disp->shadow_flag &= ~0x04;
}

static void rob_chara_item_equip_object_set_ring(
    rob_chara_item_equip_object* skin_disp, const osage_ring_data& ring) {
    for (ExOsageBlock*& i : skin_disp->osage_blk)
        i->SetRing(ring);

    for (ExClothBlock*& i : skin_disp->cloth)
        i->SetRing(ring);
}

static void rob_chara_item_equip_set_ring(rob_chara_item_equip* rob_disp, const osage_ring_data& ring) {
    for (int32_t i = rob_disp->disp_begin; i < rob_disp->disp_max; i++)
        rob_chara_item_equip_object_set_ring(&rob_disp->skin_disp[i], ring);
}

void rob_chara::set_stage_data_ring(const int32_t& stage_index) {
    osage_ring_data ring;
    if (stage_index != -1)
        stage_param_data_coli_data_get_stage_index_data(stage_index, &ring);
    rob_chara_item_equip_set_ring(rob_disp, ring);
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
    rob_disp->set_osage_step(step);
}

void rob_chara::set_use_opd(bool value) {
    rob_disp->use_opd = value;
}

void rob_chara::set_visibility(bool value) {
    if (value) {
        data.flag.bit.disp = 1;
        data.flag.bit.yoketa = 1;
    }
    else {
        data.flag.bit.disp = 0;
        data.flag.bit.yoketa = 0;
    }

    if (check_for_ageageagain_module()) {
        rob_chara_age_age_array_set_disp(chara_id, 1, value);
        rob_chara_age_age_array_set_disp(chara_id, 2, value);
    }
}

void rob_chara::set_wind_strength(float_t value) {
    rob_disp->wind_strength = value;
}

static bool sub_140413710(mot_play_data* play_data) {
    if (play_data->loop_frames_enabled)
        return play_data->loop_frames <= 0.0f;
    else if (play_data->frame_data.loop_state >= MOT_PLAY_FRAME_DATA_LOOP_ONCE
        && play_data->frame_data.loop_state <= MOT_PLAY_FRAME_DATA_LOOP_REVERSE)
        return false;
    else if (play_data->frame_data.playback_state == MOT_PLAY_FRAME_DATA_PLAYBACK_FORWARD)
        return play_data->frame_data.last_frame < play_data->frame_data.frame;
    else if (play_data->frame_data.playback_state == MOT_PLAY_FRAME_DATA_PLAYBACK_BACKWARD)
        return play_data->frame_data.frame < 0.0f;
    else
        return false;
}

static bool sub_140419E90(rob_chara_bone_data* rob_bone_data) {
    return sub_140413710(&rob_bone_data->motion_loaded.front()->mot_play_data);
}

static void sub_140505310(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
    switch (rob_chr->data.motdata.field_0.field_58) {
    case -1: {
        rob_chr->data.motdata.field_0.field_20.field_0 &= ~0x81;
        rob_chr->data.motion.frame_data.frame = rob_chr->data.motdata.field_0.frame;
    } break;
    case 0: {
        uint32_t motion_id = rob_chr->data.motdata.field_0.field_5C;
        if (motion_id == -1)
            rob_chr->data.motdata.field_0.field_20.field_0 &= ~0x81;
        else
            rob_chr->load_motion(motion_id, !!(rob_chr->data.motdata.field_0.field_60 & 0x01),
                0.0f, MOTION_BLEND, bone_data, mot_db);
    } break;
    case 1: {
        struc_223* v1 = &rob_chr->data.motdata;
        int32_t loop_count = rob_chr->data.motdata.field_0.loop_count;
        if (loop_count > 0) {
            uint32_t motion_id = rob_chr->data.motdata.field_0.field_64;
            if (motion_id != -1) {
                if (rob_chr->data.motion.loop_index >= loop_count) {
                    int32_t v6 = mothead_storage_get_mot_by_motion_id(motion_id, mot_db)->field_0.field_0;
                    if (!(v6 & 0x80000) && !(v6 & 0x100000))
                        rob_chr->load_motion(motion_id, !!(rob_chr->data.motdata.field_0.field_68 & 0x01),
                            0.0f, MOTION_BLEND, bone_data, mot_db);
                }
                else if (v1->field_0.field_20.field_0 & 0x80000) {
                    int16_t* v8 = (int16_t*)rob_chr->data.action.field_148;
                    rob_chr->data.action.field_146 = v1->field_0.field_220;
                    if (v8)
                        v8[711] = v1->field_0.field_220;
                }
                else if (rob_chr->data.motdata.field_0.field_20.field_0 & 0x100000)
                    rob_chr->data.action.field_0 = 1;
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
        if (rob_chr->data.motdata.field_0.field_78 > rob_chr->data.motion.frame_data.frame)
            return;

        if (rob_chr->data.motdata.field_0.field_10.field_0 & 0x4000) {
            rob_chr->data.motdata.field_0.field_20.field_0 |= 0x4000;
            if ((rob_chr->data.motdata.field_0.field_10.field_0 & 0x20000) != 0)
                rob_chr->data.motdata.field_0.field_20.field_0 &= ~0x20000;
        }
        rob_chr->data.motdata.field_0.field_20.field_0 &= ~0x02;
        rob_chr->data.motion.field_24 = 1;
    }

    if (rob_chr->data.motdata.field_0.field_7C > rob_chr->data.motion.frame_data.frame)
        return;

    rob_chr->data.motdata.field_0.field_20.field_0 |= 4;
    rob_chr->data.motdata.field_0.field_20.field_8 &= ~0x400000;
    v3 = rob_chr->data.motdata.field_0.field_20.field_0;
    if (v3 & 0x4000) {
        rob_chr->data.motdata.field_0.field_20.field_0 = v3 & ~0x4000;
        rob_chr->data.motdata.field_0.field_88 &= ~0x02;
        if (rob_chr->data.motdata.field_0.field_88 & 0x04)
            rob_chr->data.motdata.field_0.field_20.field_0 &= ~0x8000;
    }
    else if (v3 & 0x2000000
        && (v3 & 0x01) != 0
        && !(rob_chr->data.motdata.field_0.field_20.field_4 & 0x10000000)
        && !(rob_chr->data.motdata.field_0.field_20.field_4 & 0x20000000))
        rob_chr->data.motdata.field_0.field_20.field_0 = v3 & ~0x01;

    v4 = rob_chr->data.motdata.field_0.field_10.field_0;
    if (v4 & 0x10000 && v4 & 0x20000)
        rob_chr->data.motdata.field_0.field_20.field_0 &= ~0x20000;
    rob_chr->data.motion.field_24 = 2;

LABEL_23:
    float_t v5 = rob_chr->data.motdata.field_0.field_80;
    if (v5 >= 0.0f && v5 <= rob_chr->data.motion.frame_data.frame) {
        if (v5 < (rob_chr->data.motdata.field_0.frame - 1.0f)
            || rob_chr->data.motdata.field_0.field_58 == -1
            || rob_chr->data.motdata.field_0.field_58 == 2) {
            rob_chr->data.motdata.field_0.field_20.field_0 &= ~0x2020081;
            rob_chr->data.flag.bit.mk_change_off = 1;
        }
        rob_chr->data.motion.field_24 = 3;
    }
}

// 0x1405333E0
static void mothead_apply_inner(mothead_func_data* func_data,
    int32_t func_id, const struc_377& a3, int32_t frame, const motion_database* mot_db) {
    if (func_id < 0 || func_id >= MOTHEAD_DATA_MAX || !mothead_func_array[func_id].func)
        return;

    if (!(mothead_func_array[func_id].flags & 0x01) || frame <= 0)
        mothead_func_array[func_id].func(func_data, a3.current->data, a3.current, frame, mot_db);
    else {
        frame -= a3.current->frame;
        if (frame <= 1)
            mothead_func_array[func_id].func(func_data, a3.current->data, a3.current, frame, mot_db);
    }
}

// 0x1405338F0
static void mothead_apply(struc_223* a1, rob_chara* rob_chr, float_t frame, const motion_database* mot_db) {
    a1->field_330.field_338 = 0;

    struc_377& v4 = a1->field_330.field_0;
    if (!v4.current)
        return;

    mothead_func_data func_data;
    func_data.rob_chr = rob_chr;
    func_data.rob_base = &rob_chr->data;
    if (rob_chr->field_20)
        func_data.field_10 = (rob_chara_data*)((size_t)rob_chr->field_20 + 0x440);
    else
        func_data.field_10 = 0;
    func_data.field_18 = rob_chr;
    func_data.field_28 = &rob_chr->data.motdata;
    func_data.field_20 = &rob_chr->data;

    int32_t frame_int = (int32_t)frame;
    while (v4.current->frame <= frame_int && v4.current->type >= MOTHEAD_DATA_TYPE_0) {
        mothead_apply_inner(&func_data, v4.current->type, v4, frame_int, mot_db);
        v4.current++;
    }
}

static void sub_1405044B0(rob_chara* rob_chr) {
    if (rob_chr->data.motdata.field_330.arm_adjust_duration > 0) { // X
        float_t blend = 1.0f;
        if (fabsf(rob_chr->data.motdata.field_330.arm_adjust_duration) > 0.000001f)
            blend = (rob_chr->data.motion.frame_data.frame
                - (float_t)rob_chr->data.motdata.field_330.arm_adjust_start_frame)
                / rob_chr->data.motdata.field_330.arm_adjust_duration;

        blend = clamp_def(blend, 0.0f, 1.0f);
        if (fabsf(blend - 1.0f) <= 0.000001f)
            rob_chr->data.motdata.field_330.arm_adjust_duration = -1.0f;
        rob_chr->data.arm_adjust_scale = lerp_def(rob_chr->data.motdata.field_330.arm_adjust_prev_value,
            rob_chr->data.motdata.field_330.arm_adjust_next_value, blend);

        float_t default_scale = chara_size_table_get_value(1);
        rob_chr->data.adjust_data.item_scale = default_scale
            + (rob_chr->data.adjust_data.scale - default_scale) * rob_chr->data.arm_adjust_scale;
    }

    float_t v2 = rob_chr->bone_data->ik_scale.ratio0;
    float_t v4 = v2;
    if (!rob_chr->data.motdata.field_330.field_31C) {
        rob_chr->data.motion.field_138 = v2;
        rob_chr->data.motion.field_13C = v2;
        rob_chr->data.motion.field_140 = 0.0f;
        return;
    }

    float_t v5 = rob_chr->data.motdata.field_330.field_324;
    float_t v6 = v2;
    float_t v7 = rob_chr->data.motdata.field_330.field_32C;
    float_t v8 = 0.0f;
    if (rob_chr->data.motdata.field_330.field_318 == 1) {
        switch (rob_chr->data.motdata.field_330.field_31C) {
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
            v6 = rob_chr->field_20->bone_data->ik_scale.ratio0;
            if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
                v6 *= v4;
        case 7:
            if (!rob_chr->field_20)
                break;
            v6 = rob_chr->field_20->bone_data->ik_scale.ratio1;
            if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
                v6 *= v4;
        case 8:
            if (!rob_chr->field_20)
                break;
            v6 = rob_chr->field_20->bone_data->ik_scale.ratio2;
            if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
                v6 *= v4;
        case 9:
            if (!rob_chr->field_20)
                break;
            v6 = rob_chr->field_20->bone_data->ik_scale.ratio3;
            if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.skeleton_select == 1)
                v6 *= v4;
            break;
        case 10:
            if (rob_chr->data.motdata.field_330.field_320 > 0.0f)
                v6 = rob_chr->data.motdata.field_330.field_320;
            break;
        }
        v8 = rob_chr->data.motdata.field_330.field_328;
    }

    float_t v15 = rob_chr->data.motion.frame_data.frame;
    float_t v16 = rob_chr->data.motdata.field_330.field_314;
    if (v15 < v16) {
        float_t v17 = rob_chr->data.motdata.field_330.field_310;
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
    mothead_apply(&data.motdata, this, data.motion.frame_data.frame, mot_db);
    sub_1405044B0(this);
}

void rob_chara::sub_140509D30() {
    struc_195* v39 = data.field_1E68.field_DF8;
    struc_195* v40 = data.field_1E68.field_1230;
    struc_195* v41 = data.field_1E68.field_1668;
    mat4* v42 = data.field_1E68.field_78;
    mat4* v43 = data.field_1E68.field_738;

    const RobCollisionData* colli = rob_data->colli_data;
    const RobCollisionData* push_colli = rob_data->push_colli_data;
    mat4* m;
    mat4 mat;
    for (int32_t i = 0; i < 27; i++) {
        m = bone_data->get_mats_mat(colli->bone);
        mat4_mul_translate(m, &colli->trans, &mat);
        *v42 = mat;

        m = bone_data->get_mats_mat(push_colli->bone);
        mat4_mul_translate(m, &push_colli->trans, &mat);
        *v43 = mat;

        float_t chara_scale = data.adjust_data.scale;

        vec3 v23 = *(vec3*)&v42->row3 * chara_scale + data.adjust_data.pos;
        *(vec3*)&v42->row3 = v23;

        vec3 v29 = *(vec3*)&v43->row3 * chara_scale + data.adjust_data.pos;
        *(vec3*)&v43->row3 = v29;

        float_t v20 = colli->scale * chara_scale;
        v39->scale = v20;
        v39->field_24 = v20;
        v39->prev_pos = v39->pos;
        v39->pos = v23;

        float_t v19 = push_colli->scale * chara_scale;
        v40->scale = v19;
        v40->field_24 = v19;
        v40->prev_pos = v40->pos;
        v40->pos = v29;

        v41->scale = v19;
        v41->field_24 = v19;
        v41->prev_pos = v41->pos;
        v41->pos = v29;

        v39++;
        v40++;
        v41++;
        v42++;
        v43++;
        colli++;
        push_colli++;
    }
}

void rob_chara::sub_1405163C0(int32_t index, mat4& mat) {
    if (index >= 0 && index <= 26)
        mat = data.field_1E68.field_78[index];
}

void rob_chara::sub_140551000() {
    struc_223* v1 = &data.motdata;
    v1->field_330.field_0.current = v1->field_330.field_0.data;
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

constexpr bool operator==(const pv_data_set_motion& left, const pv_data_set_motion& right) {
    return left.motion_id == right.motion_id && left.frame_stage_index == right.frame_stage_index;
}

constexpr bool operator!=(const pv_data_set_motion& left, const pv_data_set_motion& right) {
    return !(left == right);
}

constexpr bool operator<(const pv_data_set_motion& left, const pv_data_set_motion& right) {
    return left.motion_id < right.motion_id || (!(right.motion_id < left.motion_id)
        && left.frame_stage_index < right.frame_stage_index);
}

constexpr bool operator>(const pv_data_set_motion& left, const pv_data_set_motion& right) {
    return right < left;
}

constexpr bool operator<=(const pv_data_set_motion& left, const pv_data_set_motion& right) {
    return !(right < left);
}

constexpr bool operator>=(const pv_data_set_motion& left, const pv_data_set_motion& right) {
    return !(left < right);
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
    uint32_t motion_id, const std::string& dir, int32_t frame) {
    this->rob_chr = rob_chr;
    this->pv_id = pv_id;
    this->motion_id = motion_id;
    this->dir.assign(dir);
    this->frame = frame;
}

osage_init_data::~osage_init_data() {

}

OpdMakeManagerData::Worker::Worker() {
    mode = -1;
    progress = -1;
    chara = CN_NONE;
}

OpdMakeManagerData::Worker::~Worker() {

}

OpdMakeManagerData::OpdMakeManagerData() : left(), count() {
    mode = -1;
}

OpdMakeManagerData::~OpdMakeManagerData() {

}
OpdChecker::OpdChecker() : state(), field_8(), field_10(),
field_18(), index(), size(), terminated(), thread() {
    SetState(0);
    field_40 = -1;
}

OpdChecker::~OpdChecker() {

}

bool OpdChecker::CheckFileAdler32Checksum(const std::string& path) {
    bool ret = false;
    file_stream fs;
    fs.open(path.c_str(), "rb");
    if (fs.check_not_null()) {
        size_t length = fs.get_length();
        if (length) {
            uint8_t* data = prj::MemoryManager::alloc<uint8_t>(prj::MemCTemp, length, path.c_str());
            if (fs.read(data, length)) {
                adler_buf adler;
                adler.get_adler(data, length - 8);
                ret = *(uint32_t*)&data[length - 4] == adler.adler;
            }

            if (data)
                prj::MemoryManager::free(prj::MemCTemp, data);
        }
    }
    fs.close();
    return ret;
}

bool OpdChecker::CheckFileVersion(const std::string& path, uint32_t version) {
    bool ret = false;
    file_stream fs;
    fs.open(path.c_str(), "rb");
    if (fs.check_not_null()) {
        size_t length = fs.get_length();
        if (length) {
            static const size_t footer_len = 0x08;
            void* footer = prj::MemoryManager::alloc(prj::MemCTemp, footer_len, path.c_str());
            if (!fs.set_position(length - footer_len, SEEK_SET)
                && fs.read(footer, footer_len))
                ret = ((uint32_t*)footer)[0] == version;

            if (footer)
                prj::MemoryManager::free(prj::MemCTemp, footer);
        }
    }
    fs.close();
    return ret;
}

bool OpdChecker::CheckStateNot3() {
    if (GetState())
        return GetState() != 3;
    return false;
}

std::vector<std::string>& OpdChecker::GetObjects() {
    GetState();
    return objects;
}

void OpdChecker::GetIndexSize(int32_t& index, int32_t& size) {
    std::unique_lock<std::mutex> u_lock(index_mtx);
    index = this->index;
    size = this->size;
}

int32_t OpdChecker::GetState() {
    std::unique_lock<std::mutex> u_lock(state_mtx);
    return state;
}

bool OpdChecker::GetTerminated() {
    std::unique_lock<std::mutex> u_lock(terminated_mtx);
    return terminated;
}

void OpdChecker::LaunchThread() {
    SetState(1);

    thread = new std::thread(OpdChecker::ThreadMain, this);
    if (thread) {
        std::wstring buf = swprintf_s_string(L"OPD Checker");
        SetThreadDescription((HANDLE)thread->native_handle(), buf.c_str());
    }
}

void OpdChecker::SetIndexSize(int32_t index, int32_t size) {
    std::unique_lock<std::mutex> u_lock(index_mtx);
    this->index = index;
    this->size = size;
}

void OpdChecker::SetState(int32_t value) {
    std::unique_lock<std::mutex> u_lock(state_mtx);
    state = value;
}

void OpdChecker::SetTerminated() {
    std::unique_lock<std::mutex> u_lock(terminated_mtx);
    terminated = true;
}

void OpdChecker::TerminateThread() {
    SetTerminated();

    if (thread) {
        thread->join();
        delete thread;
    }

    thread = 0;
}

void OpdChecker::ThreadMain(OpdChecker* opd_checker) {
    opd_checker->terminated = 0;
    opd_checker->SetIndexSize(0, (int32_t)osage_play_data_database->map.size());
    opd_checker->sub_140471020();
}

void OpdChecker::sub_140471020() {
    data_struct* aft_data = &data_list[DATA_AFT];

    //bool v63 = sub_14066CC20(1);
    while (GetState() == 1) {
        std::vector<std::string> files = path_get_files(get_ram_osage_play_data_dir());
        for (std::string& i : files) {
            if (!i.size())
                continue;

            size_t pos = i.find(".farc");
            if (pos != -1)
                continue;

            std::string temp = string_to_upper(i.substr(0, pos));
            auto elem = osage_play_data_database->map.find(temp);
            if (elem != osage_play_data_database->map.end() && !temp.compare(elem->first))
                path_delete_file(sprintf_s_string("%s/%s", get_ram_osage_play_data_dir(), i.c_str()).c_str());
        }

        SetState(2);
    }

    int32_t index = 0;
    for (auto& i : osage_play_data_database->map) {
        if (GetTerminated())
            break;

        std::string file = string_to_lower(sprintf_s_string("/%s.farc", i.first.c_str()));

        std::string rom_path(get_rom_osage_play_data_dir());
        rom_path.append(file);

        std::string ram_path(get_ram_osage_play_data_dir());
        ram_path.append(file);

        bool v54 = false;
        if (aft_data->get_file_path(rom_path))
            v54 = CheckFileVersion(rom_path.c_str(), (uint32_t)i.second);

        if (v54) {
            if (path_check_file_exists(ram_path.c_str()))
                path_delete_file(ram_path.c_str());
        }
        else {
            if (!path_check_file_exists(ram_path.c_str())
                || /*!v63 && */!CheckFileAdler32Checksum(ram_path)
                || !CheckFileVersion(ram_path, (uint32_t)i.second)) {
                path_delete_file(ram_path.c_str());
                objects.push_back(i.first);
            }
        }

        SetIndexSize(++index, (int32_t)osage_play_data_database->map.size());
    }

    prj::sort_unique(objects);
    SetState(3);

    //if (!v63)
    //    sub_14066FE10(1);
}

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data) {
    const std::vector<BODYTYPE>* common_bones
        = bone_data->get_body_type(rob_bone_data->base_skeleton_type);
    const std::vector<CHAINPOSRADIUS>* motion_chain_pos
        = bone_data->get_chain_pos_rad(rob_bone_data->base_skeleton_type);
    const std::vector<CHAINPOSRADIUS>* disp_chain_pos
        = bone_data->get_chain_pos_rad(rob_bone_data->skeleton_type);
    if (!common_bones || !motion_chain_pos || !disp_chain_pos)
        return;

    bone->rob_bone_data = rob_bone_data;
    bone_data_parent_load_rob_chara(bone);
    bone_data_parent_load_bone_database(bone, common_bones, motion_chain_pos->data(), disp_chain_pos->data());
}

static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    const std::vector<BODYTYPE>* bones, const CHAINPOSRADIUS* motion_chain_pos, const CHAINPOSRADIUS* disp_chain_pos) {
    rob_chara_bone_data* rob_bone_data = bone->rob_bone_data;
    size_t chain_pos = 0;
    size_t node_count = 0;
    size_t leaf_pos = 0;

    RobBlock* block = bone->bones.data();
    for (const BODYTYPE& i : *bones ) {
        block->block_id = (BONE_BLK)(&i - bones->data());
        block->ik_type = i.ik_type;
        block->flip_block_id = (BONE_BLK)i.flip_block_id;
        block->inherit_mat_id = (BONE_BLK)i.inherit_mat_id;
        block->expression_id = i.expression_id;
        block->key_set_count = i.ik_type >= IKT_ROOT ? 6 : 3;
        block->chain_pos[0] = (*motion_chain_pos++).chain_pos;
        block->chain_pos[1] = (*disp_chain_pos++).chain_pos;
        block->node = &rob_bone_data->nodes[node_count];
        block->inherit_type = i.inherit_type;
        block->inherit_mat_ptr = i.inherit_type ? &rob_bone_data->mats[i.inherit_mat_id] : 0;

        block->up_vector_mat_ptr = 0;
        switch (i.ik_type) {
        case IKT_0:
        case IKT_0N:
        case IKT_0T:
            chain_pos++;
            node_count++;
            break;
        case IKT_ROOT:
            chain_pos++;
            leaf_pos++;
            node_count++;
            break;
        case IKT_1:
            block->len[0][0] = (*motion_chain_pos++).chain_pos.x;
            block->len[0][1] = (*disp_chain_pos++).chain_pos.x;

            chain_pos += 2;
            leaf_pos++;
            node_count += 3;
            break;
        case IKT_2:
        case IKT_2R:
            block->len[0][0] = (*motion_chain_pos++).chain_pos.x;
            block->len[0][1] = (*disp_chain_pos++).chain_pos.x;
            block->len[1][0] = (*motion_chain_pos++).chain_pos.x;
            block->len[1][1] = (*disp_chain_pos++).chain_pos.x;

            mat4* up_vector_mat = 0;
            if (i.up_vector_id)
                up_vector_mat = &rob_bone_data->mats[i.up_vector_id];
            block->up_vector_mat_ptr = up_vector_mat;

            chain_pos += 3;
            leaf_pos++;
            node_count += 4;
            break;
        }
        block++;
    }

    if (node_count != rob_bone_data->node_count)
        printf_debug_error("Node mismatch");
    if (leaf_pos != bone->leaf_pos)
        printf_debug_error("LeafPos mismatch");
    if (chain_pos != bone->chain_pos)
        printf_debug_error("ChainPos mismatch");
}

static void bone_data_parent_load_bone_indices_from_mot(bone_data_parent* a1,
    const mot_data* a2, const bone_database* bone_data, const motion_database* mot_db) {
    if (!a2)
        return;

    prj::sys_vector<uint16_t>& bone_indices = a1->bone_indices;
    bone_indices.clear();

    uint16_t key_set_count = a2->key_set_count - 1;
    if (!key_set_count)
        return;

    BONE_KIND skeleton_type = a1->rob_bone_data->base_skeleton_type;
    const std::string* bone_name = mot_db->bone_name.data();

    const mot_bone_info* bone_info = a2->bone_info_array;
    for (size_t key_set_offset = 0, i = 0; key_set_offset < key_set_count; i++) {
        BONE_BLK bone_index = (BONE_BLK)bone_data->get_block_index(
            skeleton_type, bone_name[bone_info[i].index].c_str());
        if (bone_index == -1) {
            i++;
            bone_index = (BONE_BLK)bone_data->get_block_index(
                skeleton_type, bone_name[bone_info[i].index].c_str());
            if (bone_index == -1)
                break;
        }
        bone_indices.push_back((uint16_t)bone_index);

        RobBlock* block = &a1->bones.data()[bone_index];
        block->key_set_offset = (int32_t)key_set_offset;
        block->frame = -1.0f;
        key_set_offset += block->ik_type >= IKT_ROOT ? 6 : 3;
    }
}

static void bone_data_parent_load_rob_chara(bone_data_parent* bone) {
    rob_chara_bone_data* rob_bone_data = bone->rob_bone_data;
    if (!rob_bone_data)
        return;

    bone->motion_bone_count = rob_bone_data->motion_bone_count;
    bone->leaf_pos = rob_bone_data->leaf_pos;
    bone->chain_pos = rob_bone_data->chain_pos;
    bone->bones.clear();
    bone->bones.resize(rob_bone_data->motion_bone_count);
    bone->global_key_set_count = 6;
    bone->bone_key_set_count = (uint32_t)((bone->motion_bone_count + bone->leaf_pos) * 3);
}

static void mot_interpolate(const mot* a1, float_t frame, float_t* value,
    mot_key_set* a4, uint32_t key_set_count, const struc_369* a6) {
    if (a6->field_0 == 4)
        frame = prj::floorf(frame);

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
    const std::vector<BODYTYPE>* a2) {
    size_t object_bone_count;
    size_t motion_bone_count;
    size_t node_count;
    size_t leaf_pos;
    size_t chain_pos;
    bone_database_bones_calculate_count(a2, object_bone_count,
        motion_bone_count, node_count, leaf_pos, chain_pos);
    mot_key_data_get_key_set_count(a1, motion_bone_count, leaf_pos);
}

static void mot_key_data_get_key_set_count(mot_key_data* a1, size_t motion_bone_count, size_t leaf_pos) {
    a1->key_set_count = (motion_bone_count + leaf_pos) * 3 + 16;
}

static void mot_key_data_init(mot_key_data* a1,
    BONE_KIND type, const bone_database* bone_data) {
    const std::vector<BODYTYPE>* bones = bone_data->get_body_type(type);
    if (!bones)
        return;

    mot_key_data_get_key_set_count_by_bone_database_bones(a1, bones);
    mot_key_data_reserve_key_sets(a1);
    a1->skeleton_type = type;
}

static void mot_key_data_init_key_sets(mot_key_data* a1, BONE_KIND type,
    size_t motion_bone_count, size_t leaf_pos) {
    mot_key_data_get_key_set_count(a1, motion_bone_count, leaf_pos);
    mot_key_data_reserve_key_sets(a1);
    a1->skeleton_type = type;
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
    a1->fc_value.clear();
    a1->fc_value.resize(a1->key_set_count);

    a1->mot.key_sets = a1->key_set.data();
    a1->key_sets_ready = true;
}

static void mothead_func_0(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_1(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_2(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_base->motdata.field_0.field_20.field_0 |= 0x100;
    rob_chara_data* rob_base = func_data->rob_base;
    if (rob_base->motion.field_28 & 0x08)
        rob_base->motdata.field_0.field_274 -= ((int16_t*)data)[0];
    else
        rob_base->motdata.field_0.field_274 += ((int16_t*)data)[0];
}

static void mothead_func_3(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    int32_t v4 = ((int32_t*)data)[0];
    rob_chara_data* rob_base = func_data->rob_base;
    (&rob_base->motdata.field_0.field_20.field_0)[v4 >> 5] |= 1 << (v4 & 0x1F);
}

static void mothead_func_4(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    int32_t v4 = ((int32_t*)data)[0];
    rob_chara_data* rob_base = func_data->rob_base;
    (&rob_base->motdata.field_0.field_20.field_0)[v4 >> 5] &= ~(1 << (v4 & 0x1F));
}

static void mothead_func_5(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_base->motion.field_28 ^= 0x04;
}

static void mothead_func_6(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_base->action.field_1B4 = 0;
    func_data->rob_base->motdata.field_0.field_20.field_0 |= 0x400;
    func_data->rob_base->motdata.field_0.field_20.field_0 |= 0x800;
}

static void sub_140551AF0(rob_chara* rob_chr) {
    if (!rob_chr->data.flag.bit.fix_hara) {
        rob_chr->data.position.pos.y = rob_chr->data.position.gpos.y;
        rob_chr->data.flag.bit.fix_hara = 1;
        if (!(rob_chr->data.motdata.field_0.field_1E8 & 0x01)) {
            rob_chr->data.position.pos.x = rob_chr->data.position.gpos.x;
            rob_chr->data.position.pos.z = rob_chr->data.position.gpos.z;
        }
    }
    rob_chr->data.motdata.field_0.field_20.field_0 |= 0x40;
}

static void mothead_func_7(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    int16_t v4 = ((int16_t*)data)[0];
    rob_chara_data* rob_base = func_data->rob_base;
    if (rob_base->motion.field_28 & 0x08)
        v4 = -v4;
    float_t v8 = (v4 + rob_base->motdata.field_0.field_2B8 + rob_base->position.yang).get_rad();
    float_t v9 = sinf(v8) * ((float_t*)data)[1];
    float_t v10 = get_gravity() * ((float_t*)data)[2];
    float_t v11 = cosf(v8) * ((float_t*)data)[1];
    rob_base->position.spd.x = v9;
    rob_base->position.spd.y = v10;
    rob_base->position.spd.z = v11;
    sub_140551AF0(func_data->rob_chr);
}

static void mothead_func_8(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    //sub_14036D130(0, &((int16_t*)data)[2]);
}

static void mothead_func_9(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_10(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_11(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_12(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_13(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    v5 = func_data->rob_base;
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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_15(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_16(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_17(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    v5 = func_data->rob_base;
    if (v4 & 0x02) {
        v8 = v5->field_1E68.field_1EB4[0].field_0;
        a1 = v5->position.field_2;
        if (v8 > 4.0)
            return;
        v9 = v8 - ((float_t*)data)[5];
        v10 = ((float_t*)data)[1];
        if (v9 < v10 && v10 >= 0.0f || (v10 = ((float_t*)data)[2], v9 > v10) && v10 >= 0.0f)
            v9 = v10;
    LABEL_8:
        v11 = &v5->motdata.field_330.field_20.x;
        sub_1405357B0(&a1, &v5->position.gpos, &v5->motdata.field_330.field_20, v9);
        v5->motdata.field_330.field_18 = 1;
        goto LABEL_27;
    }
    if (v4 & 0x04) {
        v12 = v5->field_1E68.field_1EB4[2].field_0;
        a1 = v5->position.field_2 + 0x8000;
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
    v5->motdata.field_330.field_18 = 2;
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
    v5->motdata.field_330.field_20.z = v17;
    v5->motdata.field_330.field_20.y = 0.0f;
    v11 = &v5->motdata.field_330.field_20.x;
    *(int64_t*)&v39.x = 0;
    v39.z = 0.0f;
    v5->motdata.field_330.field_20.x = v15;
    v19 = ((int16_t*)data)[0];
    *(int64_t*)&a3.x = 0;
    a3.z = 0.0f;
    sub_140533530(func_data, (float_t)v19, &a3);
    sub_140533530(func_data, func_data->rob_base->motion.frame_data.frame, &v39);
    v20 = &func_data->rob_base->position.field_2;
    a3.x = a3.x - v39.x;
    a3.z = a3.z - v39.z;
    sub_140535710(v20, &a3.x, &a3.x);
    a1 = func_data->field_10->position.field_2;
    sub_140535710(&a1, &v5->motdata.field_330.field_20.x, &v5->motdata.field_330.field_20.x);
    v21 = v5->motdata.field_330.field_20.x - a3.x;
    v22 = v5->motdata.field_330.field_20.z - a3.z;
    v5->motdata.field_330.field_20.x = v21;
    v5->motdata.field_330.field_20.z = v22;
    v23 = func_data->field_10;
    v24 = v23->position.gpos.y + v5->motdata.field_330.field_20.y;
    v25 = v23->position.gpos.z + v5->motdata.field_330.field_20.z;
    v5->motdata.field_330.field_20.x = v21 + v23->position.gpos.x;
    v26 = func_data->rob_base;
    v5->motdata.field_330.field_20.y = v24;
    v5->motdata.field_330.field_20.z = v25;
    v27 = v25 - v26->position.gpos.z;
    v28 = v5->motdata.field_330.field_20.x - v26->position.gpos.x;
    v5->motdata.field_330.field_20.y = v24 - v26->position.gpos.y;
    v5->motdata.field_330.field_20.z = v27;
    v5->motdata.field_330.field_20.x = v28;
    v5->motdata.field_330.field_20.y = 0.0f;
    v29 = (__m128)LODWORD(v5->motdata.field_330.field_20.x);
    v29.m128_f32[0] = (float_t)((float_t)(v29.m128_f32[0] * v29.m128_f32[0])
        + (float_t)(v5->motdata.field_330.field_20.y * v5->motdata.field_330.field_20.y))
        + (float_t)(v5->motdata.field_330.field_20.z * v5->motdata.field_330.field_20.z);
    if (_mm_sqrt_ps(v29).m128_f32[0] > v18) {
        vec3::normalize(&v5->motdata.field_330.field_20);
        v30 = v18 * *v11;
        v31 = v18 * v5->motdata.field_330.field_20.y;
        v5->motdata.field_330.field_20.z = v18 * v5->motdata.field_330.field_20.z;
        *v11 = v30;
        v5->motdata.field_330.field_20.y = v31;
    }
    v32 = func_data->rob_base;
    v33 = v32->position.gpos.z + v5->motdata.field_330.field_20.z;
    v34 = v32->position.gpos.y + v5->motdata.field_330.field_20.y;
    *v11 = v32->position.gpos.x + *v11;
    v5->motdata.field_330.field_20.y = v34;
    v5->motdata.field_330.field_20.z = v33;
LABEL_27:
    v35 = (float_t)((int16_t*)data)[0];
    v5->motdata.field_330.field_1C = v35;
    if (v4 & 0x08) {
        v36 = func_data->rob_base;
        if (v35 < v36->motdata.field_0.field_1F4)
            v35 = v36->motdata.field_0.field_1F4;
        v37 = v35 - v36->motion.frame_data.frame;
        if (v37 < 1.0)
            v37 = 1.0;
        v36->position.spd.x = (float_t)(*v11 - v36->position.gpos.x) * (float_t)(1.0 / v37);
        v36->position.spd.z = (float_t)(v5->motdata.field_330.field_20.z - v36->position.gpos.z)
            * (float_t)(1.0 / v37);
        v36->position.spd.y = get_gravity() * ((float_t*)data)[6];
        func_data->rob_base->field_8.field_B8.field_10.y = 0.0f;
        sub_140551AF0(func_data->rob_chr);
    }*/
}

static void mothead_func_18(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_19(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_20(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_21(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_22(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_23(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_24(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_25(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_26(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_27(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_28(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_29(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
            || (v9 = *(int32_t*)&v5[4 * ((uint64_t)(int32_t)func_data->rob_chr->chara_num >> 5)],
                _bittest(&v9, func_data->rob_chr->chara_num & 0x1F)))
        && (v8 != 1
            || (v10 = *(int32_t*)&v5[4 * ((uint64_t)(int32_t)func_data->rob_chr->chara_num >> 5)],
                !_bittest(&v10, func_data->rob_chr->chara_num & 0x1F)))) {
        v11 = func_data->rob_base;
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
        v11->motdata.field_330.field_33C[v23].field_0 = *((int16_t*)data + 2);
        v11->motdata.field_330.field_33C[v23].frame = (float_t)mhd_data->frame;
        v11->motdata.field_330.field_33C[v23].field_8 = (float_t)*(__int16*)data;
        if (((int16_t*)data)[2] != 1) {
            v11->motdata.field_330.field_33C[v23].field_C = v13;
            v24 = *((int16_t*)data + 10);
            v11->motdata.field_330.field_33C[v23].field_10.x = v14;
            v11->motdata.field_330.field_33C[v23].field_1C.x = v18;
            v11->motdata.field_330.field_33C[v23].field_28.x = v20;
            v11->motdata.field_330.field_33C[v23].field_10.y = v17;
            v11->motdata.field_330.field_33C[v23].field_E = v24;
            v11->motdata.field_330.field_33C[v23].field_1C.y = v19;
            v11->motdata.field_330.field_33C[v23].field_28.y = v21;
            v11->motdata.field_330.field_33C[v23].field_10.z = v15;
            v11->motdata.field_330.field_33C[v23].field_1C.z = v16;
            v11->motdata.field_330.field_33C[v23].field_28.z = v22;
        }
    }*/
}

static void mothead_func_30(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_base->position.spd.x = 0.0f;
    func_data->rob_base->position.spd.z = 0.0f;
}

static void mothead_func_31(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_data* rob_base = func_data->rob_base;
    struc_651& v2 = rob_base->motdata.field_330;

    float_t v8 = (float_t)((int16_t*)data)[0];
    int32_t v5 = ((int32_t*)data)[1];
    float_t v9 = ((float_t*)data)[2];
    int32_t v10 = ((int32_t*)data)[3];

    if (v8 == (int16_t)0xFA0C && v10 == 0xD62721C5) { // X
        float_t value = v9;
        float_t duration = (float_t)v5;
        v2.arm_adjust_next_value = value;
        v2.arm_adjust_prev_value = rob_base->arm_adjust_scale;
        v2.arm_adjust_start_frame = mhd_data->frame;
        v2.arm_adjust_duration = max_def(duration, 0.0f);
        return;
    }

    if (rob_base->motion.field_28 & 0x08)
        v5 = sub_140533440(v5);

    float_t v7 = rob_base->motdata.field_0.frame - 1.0f
        - rob_base->motion.frame_data.frame;
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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    uint32_t* v5 = (uint32_t*)((size_t)data + 16);
    if (sub_1405333D0(((uint32_t*)((size_t)data + 16))[0])
        && !(v5[(uint64_t)(int32_t)func_data->rob_chr->chara_num >> 5]
            & (1 << (func_data->rob_chr->chara_num & 0x1F))))
        return;

    uint32_t* v10 = (uint32_t*)func_data->rob_chr->field_20;
    if (sub_1405333D0(((uint32_t*)((size_t)data + 20))[0]) && !(func_data->rob_chr->field_20
        && (((uint32_t*)data + ((uint64_t)v10[4] >> 5))[5] & (1 << (v10[4] & 0x1F)))))
        return;

    struc_223* v8 = &func_data->rob_base->motdata;
    v8->field_330.field_310 = (float_t)mhd_data->frame;
    v8->field_330.field_314 = (float_t)((int16_t*)data)[0];
    v8->field_330.field_318 = ((uint8_t*)data)[2];
    v8->field_330.field_31C = ((int32_t*)data)[1];
    v8->field_330.field_320 = ((float_t*)data)[2];
    v8->field_330.field_324 = func_data->rob_base->motion.field_138;
    v8->field_330.field_328 = ((float_t*)data)[3];
    v8->field_330.field_32C = func_data->rob_base->motion.field_140.y;
}

static void mothead_func_34(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_35(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_36(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_37(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_38(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_39(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_40(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_41(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_42(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_43(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_44(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_45(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_46(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_47(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_48(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_49(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void rob_chara_set_face_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state, float_t blend_offset, const motion_database* mot_db);

static void mothead_func_50_set_face_motion_id(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_set_face_motion_id(func_data->rob_chr, ((int32_t*)data)[0],
        ((float_t*)data)[1], ROB_PARTIAL_MOTION_PLAYBACK_STOP, 0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void mothead_func_51(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_52(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_53_set_face_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
     func_data->rob_chr->set_face_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1],
         (rob_partial_motion_playback_state)((int32_t*)data)[3], ((float_t*)data)[2] * 6.0f,
         ((float_t*)data)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, false, mot_db);
}

static void mothead_func_54_set_hand_r_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_hand_r_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1],
        (rob_partial_motion_playback_state)((int32_t*)data)[3], ((float_t*)data)[2] * 12.0f,
        ((float_t*)data)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void mothead_func_55_set_hand_l_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_hand_l_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1],
        (rob_partial_motion_playback_state)((int32_t*)data)[3], ((float_t*)data)[2] * 12.0f,
        ((float_t*)data)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void mothead_func_56_set_mouth_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_mouth_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1],
        (rob_partial_motion_playback_state)((int32_t*)data)[3], ((float_t*)data)[2] * 6.0f,
        ((float_t*)data)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void mothead_func_57_set_eyes_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_eyes_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1],
        (rob_partial_motion_playback_state)((int32_t*)data)[3], ((float_t*)data)[2] * 6.0f,
        ((float_t*)data)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void mothead_func_58_set_eyelid_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_eyelid_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1],
        (rob_partial_motion_playback_state)((int32_t*)data)[3], ((float_t*)data)[2] * 6.0f,
        ((float_t*)data)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_eyelid_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state, float_t blend_offset, const motion_database* mot_db);
static void rob_chara_set_eyes_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state, float_t blend_offset, const motion_database* mot_db);

static void rob_chara_set_head_object(rob_chara* rob_chr, int32_t head_object_id, const motion_database* mot_db) {
    rob_chr->data.motion.field_150.head_object = rob_chara_get_head_object(rob_chr, head_object_id);
    rob_chara_set_face_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
    rob_chara_set_eyelid_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
    rob_chara_set_eyes_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
    rob_chr->bone_data->look_anim.disable = true;
}

static void mothead_func_59_set_rob_chara_head_object(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_set_head_object(func_data->rob_chr, ((int32_t*)data)[0], mot_db);
}

static void mothead_func_60_set_look_camera(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_look_camera(true, ((int32_t*)data)[0] != 0,
        ((float_t*)data)[1], ((float_t*)data)[2], ((float_t*)data)[3] * 6.0f, 0.25f, 0.0f, false);
}

static void mothead_func_61_set_eyelid_motion_from_face(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
        v16.ex_force.x = ((float_t*)data)[2];
        v16.ex_force.y = ((float_t*)data)[3];
        v16.ex_force.z = ((float_t*)data)[4];
        v16.ex_force_cycle_strength.x = ((float_t*)data)[5];
        v16.ex_force_cycle_strength.y = ((float_t*)data)[6];
        v16.ex_force_cycle_strength.z = ((float_t*)data)[7];
        v16.ex_force_cycle.x = ((float_t*)data)[8];
        v16.ex_force_cycle.y = ((float_t*)data)[9];
        v16.ex_force_cycle.z = ((float_t*)data)[10];
        v16.cycle = ((float_t*)data)[11];
        v16.force = ((float_t*)data)[13];
        v16.phase = ((float_t*)data)[12];
        v16.strength = ((float_t*)data)[14];
        v16.strength_transition = (float_t)((int32_t*)data)[15];
    }
    rob_chara_set_parts_adjust_by_index(rob_chr, (rob_osage_parts)((uint8_t*)data)[4], &v16);
}

static void mothead_func_63(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_64_osage_reset(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_osage_reset();
}

static void mothead_func_65_motion_skin_param(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;
    uint32_t motion_id = rob_chr->data.motion.motion_id;
    float_t _frame = (float_t)mhd_data->frame;
    rob_chr->set_motion_skin_param(motion_id, _frame);
    rob_chr->set_motion_reset_data(motion_id, _frame);
}

static void mothead_func_66_osage_step(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_osage_step(((float_t*)data)[0]);
}

static void mothead_func_67_sleeve_adjust(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    motion_blend_mot* mot = func_data->rob_chr->bone_data->motion_loaded.front();
    mot->mot_key_data.frame = -1.0f;
    mot->mot_key_data.field_68.field_0 = ((int32_t*)data)[0];
    mot->mot_key_data.field_68.field_4 = ((float_t*)data)[1];
}

static void mothead_func_69_motion_max_frame(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    float_t max_frame = (float_t)*(int32_t*)data;
    func_data->rob_chr->bone_data->set_motion_max_frame(max_frame);
    /*pv_game* v6 = pv_game_get();
    if (v6)
        pv_game::set_data_itmpv_max_frame(v6, func_data->rob_chr->chara_id, max_frame);*/
}

static void mothead_func_70_camera_max_frame(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_osage_move_cancel(((uint8_t*)data)[0], ((float_t*)data)[1]);
}

static void mothead_func_72(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_adjust_data* v6 = &func_data->rob_chr->data.adjust_data;
    v6->offset.x = ((float_t*)data)[0];
    v6->offset.y = ((float_t*)data)[1];
    v6->offset.z = ((float_t*)data)[2];
    v6->offset_x = !!((uint8_t*)data)[24];
    v6->offset_y = !!((uint8_t*)data)[25];
    v6->offset_z = !!((uint8_t*)data)[26];
}

static void mothead_func_73_rob_hand_adjust(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
        hand_adjust->iterations = hand_adjust_prev->iterations;
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
        hand_adjust->iterations = ((int32_t*)data)[11];
        break;
    }
}

static void mothead_func_74_disable_collision(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_disable_collision((rob_osage_parts)((uint8_t*)data)[0], !!((uint8_t*)data)[1]);
}

static void rob_chara_set_adjust_global(rob_chara* rob_chr, rob_chara_data_adjust* a2) {
    rob_chara_set_adjust(rob_chr, a2,
        &rob_chr->data.motion.adjust_global, &rob_chr->data.motion.adjust_global_prev);
}

static void mothead_func_75_rob_adjust_global(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
        v14.ex_force.x = ((float_t*)data)[2];
        v14.ex_force.y = ((float_t*)data)[3];
        v14.ex_force.z = ((float_t*)data)[4];
        v14.ex_force_cycle_strength.x = ((float_t*)data)[5];
        v14.ex_force_cycle_strength.y = ((float_t*)data)[6];
        v14.ex_force_cycle_strength.z = ((float_t*)data)[7];
        v14.ex_force_cycle.x = ((float_t*)data)[8];
        v14.ex_force_cycle.y = ((float_t*)data)[9];
        v14.ex_force_cycle.z = ((float_t*)data)[10];
        v14.cycle = ((float_t*)data)[11];
        v14.phase = ((float_t*)data)[12];
    }
    rob_chara_set_adjust_global(rob_chr, &v14);
}

static void mothead_func_76_rob_arm_adjust(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = func_data->rob_chr;

    rob_chara_data_arm_adjust* arm_adjust = &rob_chr->data.motion.arm_adjust[((int16_t*)data)[0]];
    arm_adjust->enable = true;
    arm_adjust->prev_value = arm_adjust->value;
    arm_adjust->value = 0.0f;
    arm_adjust->next_value = ((float_t*)data)[2];
    arm_adjust->duration = ((float_t*)data)[1];
}

static void mothead_func_77_disable_eye_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->bone_data->set_disable_eye_motion(((uint8_t*)data)[0] != 0);
}

static void mothead_func_78(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    static const int16_t word_140A2D430[] = {
        1, 0, 3, 2, 4, 0, 0, 0
    };

    rob_chara_data* rob_base = func_data->rob_base;
    uint16_t v5 = ((uint8_t*)data)[2];
    if (rob_base->motion.field_28 & 0x08)
        v5 = word_140A2D430[v5];

    struc_306* v7 = &rob_base->motdata.field_330.field_33C[v5];
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

static void rob_chara_set_coli_ring(rob_chara* rob_chr, int32_t mhd_id) {
    osage_ring_data ring;
    if (mhd_id >= 0)
        stage_param_data_coli_data_get_mhd_id_data(mhd_id, &ring);
    else {
        int32_t stage_index = task_stage_get_current_stage_index();
        if (stage_index != -1)
            stage_param_data_coli_data_get_stage_index_data(stage_index, &ring);
    }
    rob_chara_item_equip_set_ring(rob_chr->rob_disp, ring);
}

static void mothead_func_79_rob_chara_coli_ring(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_set_coli_ring(func_data->rob_chr, ((int8_t*)data)[0]);
}

static void mothead_func_80_adjust_get_global_pos(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_base->adjust_data.get_global_pos = ((uint8_t*)data)[0];
}

static void mothead_mot_func_0(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    if (func_data->field_10->field_0.field_58 != -1)
        return;

    std::list<std::pair<const void*, uint32_t>> v23;
    uint32_t v8 = 0;
    if (!mhd_mot_data)
        return;

    bool v9 = mhd_mot_data->type == 0;
    if (mhd_mot_data->type < 0)
        return;

    while (!v9) {
        mhd_mot_data++;
        v9 = mhd_mot_data->type == 0;
        if (mhd_mot_data->type < 0)
            return;
    }

    const void* v10 = mhd_mot_data->data;
    if (!v10)
        return;

LABEL_8:
    v8 += ((uint16_t*)v10)[11];
    v23.push_back({ v10, v8 });

    if (mhd_mot_data) {
        const mothead_mot_data* v13 = mhd_mot_data + 1;
        bool v9 = v13 == 0;
        if (!v9) {
            while (v13->type >= 0) {
                if (!v13->type) {
                    mhd_mot_data = v13;
                    v10 = v13->data;
                    if (v10)
                        goto LABEL_8;
                    break;
                }
                v13++;
            }
        }
    }

    if (!v8 || !v23.size())
        return;

    if (v23.size() > 1) {
        uint32_t v15 = rand_state_array_get_int(1) % v8;
        for (const std::pair<const void*, uint32_t>& i : v23)
            if (v15 < i.second) {
                data = i.first;
                break;
            }
    }

    v23.clear();

    struc_223* v19 = func_data->field_10;
    v19->field_0.field_58 = ((int16_t*)data)[0];
    v19->field_0.field_5C = ((int32_t*)data)[1];
    v19->field_0.field_60 = ((int32_t*)data)[2];
    v19->field_0.field_64 = ((int32_t*)data)[3];
    v19->field_0.field_68 = ((int32_t*)data)[4];
    v19->field_0.loop_count = ((int16_t*)data)[10];
    v19->field_0.loop_begin = (float_t)((int16_t*)data)[12];
    v19->field_0.loop_end = (float_t)((int16_t*)data)[13];
}

static void mothead_mot_func_1(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    func_data->field_10->field_0.field_78 = (float_t)((int16_t*)data)[0];
    if (((int16_t*)data)[1] >= 0)
        func_data->field_10->field_0.field_7C = (float_t)((int16_t*)data)[1];
    if (((int16_t*)data)[2] >= 0)
        func_data->field_10->field_0.field_80 = (float_t)((int16_t*)data)[2];
    func_data->field_10->field_0.field_20.field_0 |= 0x02;
    func_data->rob_base->motion.field_24 = 0;
}

static void mothead_mot_func_2(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    func_data->field_10->field_0.field_1E8 = ((int32_t*)data)[0];
    func_data->field_10->field_0.field_1EC = (float_t)((int16_t*)data)[2];
    func_data->field_10->field_0.field_1F0 = (float_t)((int16_t*)data)[3];
    func_data->field_10->field_0.field_1F4 = (float_t)((int16_t*)data)[4];
    func_data->field_10->field_0.field_1F8 = (float_t)((int16_t*)data)[5];
    func_data->field_10->field_0.field_1FC = ((float_t*)data)[3];
    func_data->field_10->field_0.field_200 += ((float_t*)data)[4];
    func_data->rob_base->action.field_B8.field_10.y = -(get_gravity() * ((float_t*)data)[4]);

    if (func_data->field_10->field_0.field_1F4 < func_data->field_10->field_0.field_1F0)
        func_data->field_10->field_0.field_1F4 = func_data->field_10->field_0.field_1F0;
}

static void mothead_mot_func_3(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_4(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_5(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_6(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_7(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_8(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_9(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_10(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_11(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_12(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_13(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_14(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_15(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_16(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_17(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_18(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_19(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_20(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_21(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_22(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_23(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_24(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_25(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_26(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_27(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_28(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_29(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_30(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_31(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_32(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_33(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_34(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_35(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_36(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_37(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_38(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_39(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_40(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    func_data->field_10->field_0.field_238 = ((int16_t*)data)[0];
    if (func_data->rob_base->motion.field_28 & 0x08)
        func_data->field_10->field_0.field_23C = -((float_t*)data)[1];
    else
        func_data->field_10->field_0.field_23C = ((float_t*)data)[1];
    func_data->field_10->field_0.field_240 = ((int32_t*)data)[2];
    func_data->rob_base->action.field_B8.field_10.y = -(get_gravity() * ((float_t*)data)[3]);
}

static void mothead_mot_func_41(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    if (func_data->field_10->field_0.field_244 <= 0)
        func_data->field_10->field_0.field_248 = mhd_mot_data;
    func_data->field_10->field_0.field_244++;
}

static void mothead_mot_func_42(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_43(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_44(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_45(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_46(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    if (func_data->field_10->field_0.field_10.field_0 & 0x100)
        if (func_data->rob_base->motion.field_28 & 0x08)
            func_data->field_10->field_0.field_274 = -((int16_t*)data)[0];
        else
            func_data->field_10->field_0.field_274 = ((int16_t*)data)[0];
}

static void mothead_mot_func_47(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_48(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_49(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_50(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_51(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    if (func_data->rob_base->motion.field_28 & 0x08)
        func_data->field_10->field_0.field_2B8 = -((int16_t*)data)[0];
    else
        func_data->field_10->field_0.field_2B8 = ((int16_t*)data)[0];
}

static void mothead_mot_func_52(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_53(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_54(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_55(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    func_data->field_10->field_0.field_2BC = ((int32_t*)data)[0];
    func_data->field_10->field_0.field_2C0 = ((float_t*)data)[1];
    func_data->field_10->field_0.field_2C4 = ((float_t*)data)[2];
}

static void mothead_mot_func_56(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_57(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_58(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_59(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_60(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_61(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_62(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_63(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_64(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_65(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    if (func_data->rob_base->motion.field_28 & 0x08)
        func_data->field_10->field_0.field_318.x = -((float_t*)data)[0];
    else
        func_data->field_10->field_0.field_318.x = ((float_t*)data)[0];
    func_data->field_10->field_0.field_318.y = ((float_t*)data)[1];
    func_data->field_10->field_0.field_318.z = ((float_t*)data)[2];
}

static void mothead_mot_func_66(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    func_data->field_10->field_0.field_324 = *(float_t*)data;

    float_t v4 = func_data->field_10->field_0.field_324;
    if (v4 < 0.0f)
        func_data->field_10->field_0.field_324 = v4 * -1.0f;
}

static void mothead_mot_func_67(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {

}

static void mothead_mot_func_68(mothead_mot_func_data* func_data,
    const void* data, const mothead_mot_data* mhd_mot_data) {
    int32_t iterations = *(int32_t*)data;
    if (iterations > 0)
        func_data->field_10->field_0.iterations = iterations;
}

static int16_t opd_data_encode_shift(float_t value, uint32_t shift) {
    int32_t val = (int32_t)((float_t)(1 << (uint8_t)shift) * value);
    if (val & 0x01)
        if (val & 0x8000)
            val++;
        else
            val--;
    return (int16_t)val;
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

static bool opd_decode(const osage_play_data_header* file_head, float_t*& opd_decod_buf, osage_play_data_header& head) {
    const osage_play_data_node_header* node = (osage_play_data_node_header*)&file_head[1];
    float_t* buf = prj::MemoryManager::alloc<float_t>(prj::MemCTemp,
        3ULL * file_head->frame_count * file_head->nodes_count, "OPD_DECORD_BUF");
    opd_decod_buf = buf;
    if (!buf)
        return false;

    for (size_t i = 0; i < file_head->nodes_count; i++)
        for (uint32_t i = 3; i; i--) {
            opd_data_decode(node->data, node->count, (uint8_t)node->shift, buf);
            buf += file_head->frame_count;
            node = (const osage_play_data_node_header*)((size_t)node
                + sizeof(const osage_play_data_node_header) + node->count * sizeof(uint16_t));
        }

    head = *file_head;
    return true;
}

static bool opd_decode_data(const void* data, float_t*& opd_decod_buf, osage_play_data_header& head) {
    if (!opd_decod_buf)
        return opd_decode((const osage_play_data_header*)data, opd_decod_buf, head);
    return false;
}

enum opd_value_range_flag : uint16_t {
    OPD_VALUE_RANGE_START      = 0x001,
    OPD_VALUE_RANGE_END        = 0x002,
    OPD_VALUE_RANGE_DIFF_SAME  = 0x004,
    OPD_VALUE_RANGE_DIFF_CHG   = 0x008,
    OPD_VALUE_RANGE_NEG_TO_POS = 0x010,
    OPD_VALUE_RANGE_POS_TO_NEG = 0x020,
    OPD_VALUE_RANGE_DIFF_LOSS  = 0x040,
    OPD_VALUE_RANGE_DIFF_GAIN  = 0x080,
    OPD_VALUE_RANGE_KEY        = 0x100,

    OPD_VALUE_RANGE_SIGN_CROSS = OPD_VALUE_RANGE_NEG_TO_POS | OPD_VALUE_RANGE_POS_TO_NEG,
};

struct opd_value_range {
    float_t value;
    float_t delta;
    uint16_t field_8;
    int16_t count;
    int32_t field_C;
    opd_value_range_flag flags;

    inline opd_value_range() : value(), delta(),
        field_8(), count(), field_C(), flags() {
    }

    inline opd_value_range(float_t value, int16_t count) : delta(),
        field_8(), field_C(), flags() {
        this->value = value;
        this->count = count;
    }
};

struct opd_encode_struct {
    std::vector<opd_value_range> vec;

    inline opd_encode_struct() {

    }

    inline ~opd_encode_struct() {

    }

    void find_range(size_t start, size_t end, const float_t epsilon) {
        while (true) {
            if (end - start <= 1)
                break;

            float_t max_diff = 0.0f;
            float_t start_value = vec.data()[start].value;
            float_t delta_value = vec.data()[end].value - start_value;
            int32_t start_count = vec.data()[start].count;
            float_t d = (float_t)vec.data()[end].count - vec.data()[start].count;

            bool sign_cross = false;
            const size_t count = end - start;
            size_t new_count = end - start;
            opd_value_range* val = &vec.data()[start + 1];
            for (size_t i = 1; i < count; i++, val++) {
                float_t diff = fabsf((float_t)(val->count - start_count)
                    * delta_value / d + start_value - val->value);
                if (diff > epsilon) {
                    bool _sign_cross = !!(val->flags & OPD_VALUE_RANGE_SIGN_CROSS);
                    if ((max_diff < diff && (!sign_cross || _sign_cross)) || (!sign_cross && _sign_cross)) {
                        max_diff = diff;
                        new_count = i;
                        sign_cross = _sign_cross;
                    }
                }
            }

            if (new_count == count)
                break;

            size_t new_start = start + new_count;
            if (new_count + 1 >= 4)
                find_range(start, new_start, epsilon);
            else if (start <= new_start) {
                opd_value_range* val = &vec.data()[start];
                for (size_t i = new_count + 1; i; i--, val++)
                    enum_or(val->flags, OPD_VALUE_RANGE_KEY);
            }

            if (end - new_start + 1 < 4) {
                opd_value_range* val = &vec.data()[new_start];
                for (size_t i = new_start; i <= end; i++, val++)
                    enum_or(val->flags, OPD_VALUE_RANGE_KEY);
                return;
            }
            start = new_start;
        }

        opd_value_range* val = &vec.data()[start];
        for (size_t i = start; i <= end; i++, val++)
            if (i == start || i == end)
                enum_or(val->flags, OPD_VALUE_RANGE_KEY);
            else
                enum_and(val->flags, ~OPD_VALUE_RANGE_KEY);
    }

    size_t get_next_index(size_t index) {
        size_t count = vec.size();
        if (index >= count)
            return count;

        opd_value_range* val = &vec.data()[index];
        while (!(val->flags & OPD_VALUE_RANGE_KEY)) {
            index++;
            val++;
            if (count <= index)
                return count;
        }
        return index;
    }
};

static uint32_t opd_encode_calculate_value_shift(float_t value) {
    value = fabsf(value);
    float_t max_value = 16384.0f;
    uint32_t shift = 0;
    for (; shift < 24; shift++, max_value *= 0.5f)
        if (value >= max_value)
            break;
    return shift;
}

static uint32_t opd_encode_calculate_shift(const float_t* data, size_t size) {
    uint32_t max_shift = 24;
    for (size_t i = size, j = 0; i; i--, j++)
        max_shift = min_def(max_shift, opd_encode_calculate_value_shift(data[j]));
    return max_shift;
}

static uint32_t opd_encode(const float_t* src_data, size_t src_size, const float_t epsilon, int16_t* dst_data, size_t& dst_size) {
    const uint32_t shift = opd_encode_calculate_shift(src_data, src_size);
    if (src_size < 4) {
        for (size_t i = src_size, j = 0; i; i--, j++)
            dst_data[j] = opd_data_encode_shift(src_data[j], shift);
        dst_size = src_size;
        return shift;
    }

    opd_encode_struct enc;
    enc.vec.reserve(src_size);

    for (size_t i = src_size, j = 0; i; i--, j++) {
        float_t value = src_data[j];
        opd_value_range val_range(value, (int16_t)j);
        if (!j) {
            val_range.delta = value;
            enum_or(val_range.flags, OPD_VALUE_RANGE_START);
            enum_or(val_range.flags, OPD_VALUE_RANGE_KEY);
            enc.vec.push_back(val_range);
            continue;
        }

        opd_value_range& prev_val = enc.vec.back();
        float_t delta = value - prev_val.value;
        val_range.delta = delta;
        if (j == src_size - 1) {
            enum_or(val_range.flags, OPD_VALUE_RANGE_END);
            enum_or(val_range.flags, OPD_VALUE_RANGE_KEY);
        }

        float_t prev_delta = prev_val.delta;
        if (fabs(delta - prev_delta) > 0.000001f) {
            if (delta > prev_delta) {
                if (j != 1 && delta >= 0.0f && prev_delta <= 0.0f)
                    enum_or(prev_val.flags, OPD_VALUE_RANGE_POS_TO_NEG);
                else if (prev_val.flags & OPD_VALUE_RANGE_DIFF_SAME)
                    enum_or(prev_val.flags, OPD_VALUE_RANGE_DIFF_GAIN);
            }
            else if (delta < prev_delta) {
                if (j != 1 && delta <= 0.0f && prev_delta >= 0.0f)
                    enum_or(prev_val.flags, OPD_VALUE_RANGE_NEG_TO_POS);
                else if (prev_val.flags & OPD_VALUE_RANGE_DIFF_SAME)
                    enum_or(prev_val.flags, OPD_VALUE_RANGE_DIFF_LOSS);
            }
        }
        else {
            enum_or(val_range.flags, OPD_VALUE_RANGE_DIFF_SAME);
            if (!(prev_val.flags & OPD_VALUE_RANGE_DIFF_SAME))
                enum_or(prev_val.flags, OPD_VALUE_RANGE_DIFF_CHG);
        }
        enc.vec.push_back(val_range);
    }

    enc.find_range(0, src_size - 1, epsilon);

    size_t data_offset = 0;
    opd_value_range* range_data = enc.vec.data();
    for (size_t i = 0; i < src_size; ) {
        size_t next_index = enc.get_next_index(i);
        if (next_index == src_size)
            break;

        if (next_index <= i + 1)
            dst_data[data_offset++] = opd_data_encode_shift(range_data[next_index].value, shift);
        else {
            dst_data[data_offset++] = (int16_t)((range_data[next_index].count << 1) | 0x0001);
            dst_data[data_offset++] = opd_data_encode_shift(range_data[next_index].value, shift);
        }
        i = next_index + 1;
    }
    dst_size = data_offset;
    return shift;
}

static void opd_encode_data(const std::vector<float_t>& src, uint8_t*& dst, size_t& dst_size, bool higher_accuracy) {
    const float_t epsilon = higher_accuracy ? 0.0001f : 0.001f;
    size_t count = src.size();
    uint32_t shift = opd_encode(src.data(), count, epsilon, (int16_t*)(dst + 4), count);
    osage_play_data_node_header* node = (osage_play_data_node_header*)dst;
    node->shift = (int16_t)shift;
    node->count = (uint16_t)count;
    dst_size = sizeof(osage_play_data_node_header) + sizeof(int16_t) * count;
}

static PvOsageManager* pv_osage_manager_array_get(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_ID_MAX)
        chara_id = 0;
    return &pv_osage_manager_array[chara_id];
}

static void rob_base_rob_chara_init(rob_chara* rob_chr) {
    //sub_14054F4A0(&rob_chr->rob_touch);
    //sub_14054F830(&rob_chr->rob_touch, rob_chr->pv_data.field_70 != 0);
}

static mat4* sub_140504E80(rob_chara* rob_chr) {
    mat4 mat;
    mat4_rotate_y(rob_chr->data.position.yang.get_rad(), &mat);
    mat4_set_translation(&mat, &rob_chr->data.position.pos);
    rob_chr->data.position.rob_mat = mat;
    return &rob_chr->data.position.rob_mat;
}

static void sub_140414900(struc_308* a1, const mat4* mat) {
    if (mat) {
        a1->field_8C = true;
        a1->mat = *mat;
    }
    else {
        a1->field_8C = false;
        a1->mat = mat4_identity;
    }
}

static void sub_14041DA50(rob_chara_bone_data* rob_bone_data, mat4* mat) {
    for (RobNode& i : rob_bone_data->nodes)
        mat4_mul(i.mat_ptr, mat, i.mat_ptr);

    sub_140414900(&rob_bone_data->motion_loaded.front()->field_4F8, mat);
}

static void sub_140507F60(rob_chara* rob_chr) {
    sub_14041DA50(rob_chr->bone_data, sub_140504E80(rob_chr));
    rob_chr->data.position.old_gpos = rob_chr->data.position.gpos;

    mat4* n_hara_mat = rob_chr->bone_data->get_mats_mat(BONE_ID_N_HARA);
    mat4_get_translation(n_hara_mat, &rob_chr->data.position.gpos);
    rob_chr->data.position.velocity = rob_chr->data.position.gpos
        - rob_chr->data.position.old_gpos;

    mat4* n_hara_cp_mat = rob_chr->bone_data->get_mats_mat(BONE_ID_N_HARA_CP);
    vec3 v10 = { 0.0f, 0.0f, 1.0f };
    mat4_transform_vector(n_hara_cp_mat, &v10, &v10);
    rob_chr->data.position.hara_yang.set_rad(atan2f(v10.x, v10.z));
}

static void sub_140505B20(rob_chara* rob_chr) {
    if (rob_chr->data.motdata.field_0.field_20.field_0 & 0x100)
        rob_chr->data.position.act_yang = rob_chr->data.position.yang + rob_chr->data.motdata.field_0.field_274;
    else
        rob_chr->data.position.act_yang = rob_chr->data.position.yang;

    if (rob_chr->data.flag.bit.fix_hara && rob_chr->data.position.spd.y > -0.000001f)
        rob_chr->data.flag.bit.jump_rise = 1;
    else
        rob_chr->data.flag.bit.jump_rise = 0;
}

static void rob_base_rob_chara_ctrl(rob_chara* rob_chr) {
    sub_140507F60(rob_chr);
    rob_chr->adjust_ctrl();
    sub_140505B20(rob_chr);
    //sub_14036D130(7, &rob_chr);
    rob_chr->set_data_adjust_mat(&rob_chr->data.adjust_data);
}

static void sub_140514680(rob_chara* rob_chr) {
    rob_chr->data.action.field_0 = 0;
    rob_chr->data.action.field_4 = rob_chr->data.action.field_2C;
}

static void sub_140514540(rob_chara* rob_chr) {
    rob_chr->data.action.field_18 = rob_chr->data.action.field_4;
    rob_chr->data.action.field_40 = rob_chr->data.action.field_2C;
    if (rob_chr->data.action.field_2C.field_0 == 1)
        sub_140514680(rob_chr);
    rob_chr->data.action.field_2C.field_0 = 0;
}

static void sub_1405145F0(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
    if (rob_chr->data.action.field_0)
        return;

    float_t frame;
    if (!(rob_chr->data.motion.field_28 & 0x80))
        frame = 0.0f;
    else
        frame = rob_chr->data.motion.frame_data.last_set_frame;
    rob_chr->load_motion(rob_chr->data.action.field_4.motion_id,
        rob_chr->data.action.field_4.field_10, frame, MOTION_BLEND, bone_data, mot_db);
    rob_chr->bone_data->set_motion_blend_duration(0.0f, 1.0f, 1.0f);
    rob_chr->set_motion_reset_data(rob_chr->data.action.field_4.motion_id, 0.0f);
    rob_chr->data.action.field_0 = 1;
}

static void sub_1405144C0(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
    if (rob_chr->data.action.field_4.field_0 == 1) {
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

static const void* sub_140551F60(rob_chara* rob_chr, uint32_t* a2) {
    int16_t v2 = rob_chr->data.motdata.field_0.field_244;
    const mothead_mot_data* v3 = rob_chr->data.motdata.field_0.field_248;
    if (!v2)
        return 0;

    while (v3) {
        if (v3->type == MOTHEAD_DATA_TYPE_41) {
            const void* v5 = v3->data;
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
    const void* v2 = sub_140551F60(rob_chr, &rob_chr->data.motion.field_30);
    const void* v3 = v2;
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

// 0x140553420
static void rob_chara_set_face_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobFaceMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_face_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_1405509D0(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->data.motion.field_150.face.CheckEnded())
        rob_chara_set_face_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            6.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_hand_l_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140553E30
static void rob_chara_set_hand_l_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_hand_l_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550A40(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->data.motion.field_150.hand_l.CheckEnded())
        rob_chara_set_hand_l_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            12.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_hand_r_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140554370
static void rob_chara_set_hand_r_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_hand_r_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550AB0(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->data.motion.field_150.hand_r.CheckEnded())
        rob_chara_set_hand_r_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            12.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_mouth_motion(rob_chara* rob_chr,
    RobMouthMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140554C40
static void rob_chara_set_mouth_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobMouthMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_mouth_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550B20(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->data.motion.field_150.mouth.CheckEnded())
        rob_chara_set_mouth_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            6.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_eyes_motion(rob_chara* rob_chr,
    RobEyesMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140552F10
static void rob_chara_set_eyes_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobEyesMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_eyes_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550960(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->data.motion.field_150.eyes.CheckEnded())
        rob_chara_set_eyes_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            6.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_eyelid_motion(rob_chara* rob_chr,
    RobEyelidMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140552A90
static void rob_chara_set_eyelid_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobEyelidMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motion_id != -1) {
        motion.data.motion_id = motion_id;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_count = motion_storage_get_mot_data_frame_count(motion_id, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_data = &rob_chr->data.motion.frame_data;
            motion.data.step_data = &rob_chr->data.motion.step_data;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_eyelid_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_1405508F0(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->data.motion.field_150.eyelid.CheckEnded())
        rob_chara_set_eyelid_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            6.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static bool sub_1404190E0(rob_chara_bone_data* rob_bone_data) {
    return rob_bone_data->eyelid.blend.field_8 || rob_bone_data->eyelid.blend.enable;
}

static void sub_140555F70(rob_chara* rob_chr, const motion_database* mot_db) {
    if (sub_1404190E0(rob_chr->bone_data) || (rob_chr->data.motion.field_29 & 0x80))
        return;

    int32_t v3 = rob_chr->data.motion.field_150.field_1C0;
    if (v3 == 1) {
        if (rob_chr->data.motion.field_29 & 0x02) {
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

static void sub_140415400(mot_play_data* play_data) {
    if (play_data->frame_data.playback_state != MOT_PLAY_FRAME_DATA_PLAYBACK_EXTERNAL)
        return;

    if (play_data->ext_frame)
        play_data->frame_data.frame = *play_data->ext_frame;
    if (play_data->ext_step)
        play_data->frame_data.step = *play_data->ext_step;
}

static void sub_14041DA00(rob_chara_bone_data* rob_bone_data) {
    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        sub_140415400(&i->mot_play_data);
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

static void sub_140419800(rob_chara_bone_data* rob_bone_data, vec3* position) {
    rob_bone_data->motion_loaded.front()->get_n_hara_cp_position(*position);
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
    if (rob_chr->data.motdata.field_0.field_20.field_0 & 0x08
        && !(rob_chr->data.motdata.field_0.field_20.field_8 & 0x10000000))
        return;

    vec3 v9 = 0.0f;
    sub_140419800(rob_chr->bone_data, &v9);
    float_t v3 = sub_1405503B0(rob_chr) * rob_chr->data.motion.step_data.frame;
    float_t v4 = sub_1405501F0(rob_chr) * rob_chr->data.motion.step_data.frame;
    v9.x -= v3 * rob_chr->data.motdata.field_0.field_318.x;
    v9.y -= v4 * rob_chr->data.motdata.field_0.field_318.y;
    v9.z -= v3 * rob_chr->data.motdata.field_0.field_318.z;

    mat4 mat;
    mat4_rotate_y(rob_chr->data.position.yang.get_rad(), &mat);
    mat4_transform_vector(&mat, &v9, &v9);

    if (!(rob_chr->data.motdata.field_0.field_20.field_0 & 0x08)) {
        rob_chr->data.motdata.field_0.field_20.field_0 |= 0x08;
        rob_chr->data.position.pos.x = rob_chr->data.position.gpos.x - v9.x;
        rob_chr->data.position.pos.z = rob_chr->data.position.gpos.z - v9.z;
    }

    if (rob_chr->data.motdata.field_0.field_20.field_8 & 0x10000000) {
        rob_chr->data.motdata.field_0.field_20.field_8 &= ~0x10000000;
        rob_chr->data.position.pos.y = rob_chr->data.position.gpos.y - v9.y;
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
    sub_14041C680(rob_chr->bone_data, !!rob_chr->data.flag.bit.fix_hara);
    sub_14041D270(rob_chr->bone_data,
        (float_t)((double_t)rob_chr->data.motdata.field_0.field_274 * M_PI * (1.0 / 32768.0)));
    sub_14041CA70(rob_chr->bone_data, rob_chr->data.motion.field_138);
    sub_14041CAB0(rob_chr->bone_data, rob_chr->data.motion.field_13C);
    sub_14041CA10(rob_chr->bone_data, &rob_chr->data.motion.field_140);
    sub_14041DA00(rob_chr->bone_data);

    RobFaceMotion* face = &rob_chr->data.motion.field_150.face;
    if ((rob_chr->data.motion.field_29 & 0x04))
        face = &rob_chr->data.motion.field_3B0.face;
    face->GetFrameStep();
    rob_chr->bone_data->set_face_frame(face->data.frame);
    rob_chr->bone_data->set_face_step(face->data.step);

    RobHandMotion* hand_l = &rob_chr->data.motion.field_150.hand_l;
    if (rob_chr->data.motion.field_29 & 0x08)
        hand_l = &rob_chr->data.motion.field_3B0.hand_l;
    else if (rob_chr->data.motion.field_2A & 0x04)
        hand_l = &rob_chr->data.motion.hand_l;
    hand_l->GetFrameStep();
    rob_chr->bone_data->set_hand_l_frame(hand_l->data.frame);
    rob_chr->bone_data->set_hand_l_step(hand_l->data.step);

    RobHandMotion* hand_r = &rob_chr->data.motion.field_150.hand_r;
    if (rob_chr->data.motion.field_29 & 0x10)
        hand_r = &rob_chr->data.motion.field_3B0.hand_r;
    else if (rob_chr->data.motion.field_2A & 0x08)
        hand_r = &rob_chr->data.motion.hand_r;
    hand_r->GetFrameStep();
    rob_chr->bone_data->set_hand_r_frame(hand_r->data.frame);
    rob_chr->bone_data->set_hand_r_step(hand_r->data.step);

    RobMouthMotion* mouth = &rob_chr->data.motion.field_150.mouth;
    if (rob_chr->data.motion.field_29 & 0x20)
        mouth = &rob_chr->data.motion.field_3B0.mouth;
    mouth->GetFrameStep();
    rob_chr->bone_data->set_mouth_frame(mouth->data.frame);
    rob_chr->bone_data->set_mouth_step(mouth->data.step);

    RobEyesMotion* eyes = &rob_chr->data.motion.field_150.eyes;
    if (rob_chr->data.motion.field_29 & 0x40)
        eyes = &rob_chr->data.motion.field_3B0.eyes;
    eyes->GetFrameStep();
    rob_chr->bone_data->set_eyes_frame(eyes->data.frame);
    rob_chr->bone_data->set_eyes_step(eyes->data.step);

    RobEyelidMotion* eyelid = &rob_chr->data.motion.field_150.eyelid;
    if (rob_chr->data.motion.field_29 & 0x80)
        eyelid = &rob_chr->data.motion.field_3B0.eyelid;
    eyelid->GetFrameStep();
    rob_chr->bone_data->set_eyelid_frame(eyelid->data.frame);
    rob_chr->bone_data->set_eyelid_step(eyelid->data.step);

    object_info v8 = sub_140550330(rob_chr);
    if (rob_chara_get_object_info(rob_chr, RPK_ATAMA) != v8)
        rob_chr->load_outfit_object_info(RPK_ATAMA,
            v8, false, bone_data, data, obj_db);
    object_info v9 = rob_chara_get_head_object(rob_chr, 1);
    object_info v10 = rob_chara_get_head_object(rob_chr, 7);

    bool v11 = v8 == v9 || v8 == v10;
    rob_chr->bone_data->eyes.disable = v11;
    rob_chr->bone_data->look_anim.disable = v11;

    object_info v12 = sub_140550350(rob_chr);
    if (rob_chara_get_object_info(rob_chr, RPK_TE_L) != v12)
        rob_chr->load_outfit_object_info(RPK_TE_L,
            v12, true, bone_data, data, obj_db);

    object_info v13 = sub_140550380(rob_chr);
    if (rob_chara_get_object_info(rob_chr, RPK_TE_R) != v13)
        rob_chr->load_outfit_object_info(RPK_TE_R,
            v13, true, bone_data, data, obj_db);

    object_info v14 = sub_140550310(rob_chr);
    if (rob_chara_get_object_info(rob_chr, RPK_HARA) != v14)
        rob_chr->load_outfit_object_info(RPK_HARA,
            v14, false, bone_data, data, obj_db);

    rob_chr->bone_data->interpolate();
    rob_chr->bone_data->update(0);
    sub_140504F00(rob_chr);

    rob_chr->data.motion.field_28 &= ~0x80;
}

static void sub_140504AC0(rob_chara* rob_chr) {
    vec3 v4 = rob_chr->data.position.spd;
    vec3 v20 = 0.0f;
    if (rob_chr->data.flag.bit.fix_hara) {
        v20 = v4 + rob_chr->data.position.adjust_spd + rob_chr->data.action.field_B8.field_10;
        v4.y = rob_chr->data.action.field_B8.field_10.y
            - get_gravity() * rob_chr->data.motdata.field_0.field_200;
    }
    else if (!rob_chr->data.flag.bit.jump_rise) {
        v20 = rob_chr->data.position.adjust_spd;
        v20.x += v4.x + rob_chr->data.action.field_B8.field_10.x;
        v20.z += v4.z + rob_chr->data.action.field_B8.field_10.z;

        float_t v7 = 0.4f;
        if (rob_chr->data.motdata.field_0.field_20.field_0 & 0xC00)
            v7 = 0.6f;

        float_t v13 = powf(v7, 1.0f);
        if (rob_chr->data.motdata.field_330.field_18)
            v13 = 1.0f;

        v4.x *= v13;
        v4.y = 0.0f;
        v4.z *= v13;

        rob_chr->data.action.field_B8.field_10.x *= v13;
        rob_chr->data.action.field_B8.field_10.z *= v13;

        if (rob_chr->data.flag.bit.ringout
            && rob_chr->data.motdata.field_0.field_1EC > rob_chr->data.motion.frame_data.frame) {
            v20.x += rob_chr->data.field_3DD4;
            v20.z += rob_chr->data.field_3DDC;
        }

        if (rob_chr->data.motdata.field_330.field_18 == 3) {
            v20 = 0.0f;
            if (rob_chr->data.motdata.field_330.field_1C >= rob_chr->data.motion.frame_data.frame)
                v20.y = rob_chr->data.motdata.field_330.field_20.y;
            else
                rob_chr->data.motdata.field_330.field_18 = 0;
        }
        else if (rob_chr->data.motdata.field_330.field_18) {
            v20.x = 0.0f;
            v20.z = 0.0f;
            if (rob_chr->data.motion.frame_data.frame > rob_chr->data.motdata.field_330.field_1C
                || rob_chr->data.field_1E68.field_1C && rob_chr->data.motdata.field_330.field_18 == 1)
                rob_chr->data.motdata.field_330.field_18 = 0;
            else {
                float_t v17 = rob_chr->data.motdata.field_330.field_1C
                    - rob_chr->data.motion.frame_data.frame;
                if (v17 < 0.0f)
                    v17 = 0.0f;

                float_t v18 = 1.0f / (v17 + 1.0f);
                v20.x = v18 * (rob_chr->data.motdata.field_330.field_20.x
                    - rob_chr->data.position.gpos.x);
                v20.z = v18 * (rob_chr->data.motdata.field_330.field_20.z
                    - rob_chr->data.position.gpos.z);
            }
        }
    }

    if (rob_chr->data.motdata.field_0.field_20.field_4 & 0x4000000
        && (rob_chr->data.field_3DA0 & 0x40 && rob_chr->data.field_3D9C & 0x20
            || rob_chr->data.field_3DA0 & 0x20 && rob_chr->data.field_3D9C & 0x40)) {
        v4.x = 0.0f;
        v4.z = 0.0f;
    }

    rob_chr->data.position.spd = v4;
    rob_chr->data.position.adjust_spd = 0.0f;
    rob_chr->data.position.pos = v20 + rob_chr->data.position.pos;
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

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;
    const RobData* rob_data = rob_chr->rob_data;
    RobNode* v3 = rob_chara_bone_data_get_node(rob_chr->bone_data, BONE_NODE_N_HARA_CP);
    rob_disp->reset_init_data(v3);
    rob_disp->set_one_skin(rob_chr->cos_id == 501);
    rob_disp->set_item({}, RPK_BODY, false, bone_data, data, obj_db);
    for (int32_t i = RPK_BASE_BEGIN; i <= RPK_BASE_END; i++)
        rob_disp->load_outfit_object_info((ROB_PARTS_KIND)i,
            rob_data->body_obj_uid[i], false, aft_bone_data, aft_data, aft_obj_db);
    rob_disp->set_shadow_group(rob_chr->chara_id);
    rob_chr->rob_disp->shadow_flag = 0x05;
    rob_chr->item_cos_data.reload_items(rob_chr->chara_id, bone_data, data, obj_db);
    if (rob_chr->item_cos_data.check_for_npr_flag())
        rob_chr->rob_disp->npr_flag = true;

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

    rob_chr->pos_reset();

    effect_fog_ring_data_reset();
    effect_splash_data_reset();
}

static void rob_disp_rob_chara_ctrl_thread_main(rob_chara* rob_chr) {
    if (rob_chr->bone_data->get_frame() < 0.0f)
        rob_chr->bone_data->set_frame(0.0f);

    float_t frame_count = rob_chr->bone_data->get_frame_count();
    if (rob_chr->bone_data->get_frame() > frame_count)
        rob_chr->bone_data->set_frame(frame_count);

    rob_chr->rob_disp->set_opd_blend_data(&rob_chr->bone_data->motion_loaded);

    vec3 pos = 0.0f;
    rob_chr->get_pos_scale(0, pos);
    rob_chr->rob_disp->position = pos;
    rob_chara_item_equip_ctrl(rob_chr->rob_disp);
    if (rob_chr->check_for_ageageagain_module()) {
        rob_chara_age_age_array_set_step(rob_chr->chara_id, 1, rob_chr->rob_disp->osage_step);
        rob_chara_age_age_ctrl(rob_chr, 1, "j_tail_l_006_wj");
        rob_chara_age_age_array_set_step(rob_chr->chara_id, 2, rob_chr->rob_disp->osage_step);
        rob_chara_age_age_ctrl(rob_chr, 2, "j_tail_r_006_wj");
    }
}

static void rob_disp_rob_chara_disp(rob_chara* rob_chr) {
    rob_chr->rob_disp->skin_color = 1.0f;
    rob_chr->rob_disp->mat = rob_chr->data.adjust_data.mat;
    rob_chr->rob_disp->disp(rob_chr->chara_id, rctx_ptr);
}

static void rob_disp_rob_chara_free(rob_chara* rob_chr) {
    rob_chara_age_age_array_reset(rob_chr->chara_id);
    rob_chr->rob_disp->reset();
}

static void sub_140409B70(rob_chara_look_anim* look_anim,
    vec3* target_view_point, mat4* adjust_mat, bool a4) {
    if (target_view_point) {
        if (look_anim->duration > 0.0f
            && look_anim->duration > look_anim->head_rot_frame
            && fabsf(look_anim->duration - look_anim->head_rot_frame) > 0.000001f) {
            look_anim->head_rot_frame += look_anim->step;
            look_anim->field_191 = true;
            look_anim->head_rot_blend = (look_anim->head_rot_frame
                + 1.0f) / (look_anim->duration + 1.0f);
        }
        else {
            look_anim->field_191 = false;
            look_anim->head_rot_blend = 1.0f;
        }

        look_anim->field_190 = true;

        mat4 mat;
        mat4_invert(adjust_mat, &mat);

        vec3 v24;
        mat4_transform_point(&mat, target_view_point, &v24);

        look_anim->field_184 = vec3::clamp(look_anim->field_184, v24 - 0.05f, v24 + 0.05f);

        look_anim->field_192 = a4;
        look_anim->field_193 = a4;
        look_anim->field_194 = a4;
        look_anim->field_195 = a4;
    }
    else {
        if ((look_anim->field_190 || look_anim->field_191)
            && look_anim->duration > 0.0f
            && look_anim->duration > look_anim->head_rot_frame
            && fabsf(look_anim->duration - look_anim->head_rot_frame) > 0.000001f) {
            look_anim->head_rot_frame += look_anim->step;
            look_anim->field_191 = true;
            look_anim->head_rot_blend = 1.0f - (look_anim->head_rot_frame
                + 1.0f) / (look_anim->duration + 1.0f);
        }
        else {
            look_anim->field_191 = false;
            look_anim->head_rot_blend = 0.0f;
        }

        look_anim->field_190 = false;
        look_anim->field_192 = false;
        look_anim->field_193 = false;
    }
}

static void sub_14041C620(rob_chara_bone_data* rob_bone_data,
    vec3* target_view_point, mat4* adjust_mat, bool a4) {
    sub_140409B70(&rob_bone_data->look_anim, target_view_point, adjust_mat, a4);
}

static float_t sub_14040ADE0(float_t a1, float_t a2) {
    float_t v1 = a1 - a2;
    if (v1 > (float_t)M_PI)
        return v1 - (float_t)(M_PI * 2.0);
    else if (v1 < (float_t)-M_PI)
        return v1 + (float_t)(M_PI * 2.0);
    return v1;
}

static void sub_140407280(rob_chara_look_anim* look_anim,
    prj::sys_vector<RobBlock>& bones, const mat4& parent_mat, float_t step) {
    vec3 v69;
    mat4_get_translation(bones[BLK_CL_KAO].node[2].mat_ptr, &v69);
    if (!look_anim->field_190 && !look_anim->field_191)
        return;

    float_t v14 = step * 0.25f;
    float_t v15 = look_anim->field_15C.field_8;
    float_t v16 = look_anim->field_15C.field_C;
    mat4* c_kata_r_mat = bones[BLK_C_KATA_R].node[0].mat_ptr;
    mat4* c_kata_l_mat = bones[BLK_C_KATA_L].node[0].mat_ptr;
    mat4* n_kao_mat = bones[BLK_N_KAO].node[0].mat_ptr;

    float_t v23 = atan2f(n_kao_mat->row0.x, n_kao_mat->row2.x);
    float_t v24 = atan2f(c_kata_r_mat->row3.x - c_kata_l_mat->row3.x, c_kata_r_mat->row3.z - c_kata_l_mat->row3.z);
    float_t v25 = sub_14040ADE0(v24, v23);

    float_t v18 = -v15;
    if (v25 < 0.0f)
        v15 = v25 + v15;
    else if (v25 > 0.0f)
        v18 = v25 - v15;

    vec3 v67;
    mat4_inverse_transform_point(n_kao_mat, &look_anim->field_184, &v67);
    if (fabsf(vec3::length(v67)) <= 0.000001f)
        return;

    vec3 v68;
    mat4_inverse_transform_point(n_kao_mat, &v69, &v68);
    float_t v28 = vec3::length(v68);
    if (fabsf(v28) <= 0.000001f)
        return;

    v67 = vec3::normalize(v67) * v28;

    float_t v38 = (float_t)(75.0 * DEG_TO_RAD);
    if (look_anim->type == 3)
        v38 -= (float_t)(30.0 * DEG_TO_RAD);

    bool v36;
    float_t v40;
    if (vec2::length(*(vec2*)&v67) >= vec3::length(v67) * cosf(v38)) {
        v40 = atan2f(-v67.x, v67.y);
        v36 = false;
    }
    else {
        v40 = look_anim->field_19C.field_0;
        v36 = true;
    }

    float_t v42 = v40;
    if (look_anim->field_190) {
        if (look_anim->type == 3)
            v16 -= (float_t)(100.0 * DEG_TO_RAD);

        if (v36 || v40 > v16 || v40 < -v16) {
            v40 = 0.0f;
            v14 *= 0.5f;
            look_anim->type = 3;
        }
        else if (v40 > v15) {
            v40 = v15;
            v14 *= 0.5f;
            look_anim->type = 2;
        }
        else if (v40 < v18) {
            v40 = v18;
            v14 *= 0.5f;
            look_anim->type = 1;
        }
        else
            look_anim->type = 0;
    }
    else
        look_anim->type = 0;

    float_t head_rot_strength = look_anim->head_rot_strength;
    if (look_anim->type != 3 && look_anim->ext_head_rotation) {
        if (!look_anim->type)
            v14 *= 0.5f;

        float_t v45 = look_anim->ext_head_rot_y_angle;
        if (fabsf(look_anim->ext_head_rot_y_angle) > 0.000001f) {
            float_t v46;
            if (v45 >= 0.0f) {
                v46 = v40 + (float_t)(50.0 * DEG_TO_RAD);
                if (v46 > v15)
                    v46 = v15;

                float_t v47 = v40 - (float_t)(50.0 * DEG_TO_RAD);
                if (v47 < v18)
                    v47 = v18;

                if (v46 < v45) {
                    if (v42 < (float_t)(-70.0 * DEG_TO_RAD)) {
                        v46 = 0.0f;
                        look_anim->type = 3;
                    }
                    else
                        v46 = look_anim->ext_head_rot_y_angle;
                }
                else if (v46 - (float_t)(30.0 * DEG_TO_RAD) < v40)
                    v46 = v47;
            }
            else {
                v46 = v40 - (float_t)(50.0 * DEG_TO_RAD);
                if (v46 < v18)
                    v46 = v18;

                float_t v47 = v40 + (float_t)(50.0 * DEG_TO_RAD);
                if (v47 > v15)
                    v47 = v15;

                if (v46 > v45) {
                    if (v42 > (float_t)(70.0 * DEG_TO_RAD)) {
                        v46 = 0.0f;
                        look_anim->type = 3;
                    }
                    else
                        v46 = look_anim->ext_head_rot_y_angle;
                }
                else if (v46 + (float_t)(30.0 * DEG_TO_RAD) > v40)
                    v46 = v47;
            }
            v40 = v46;
        }
        else if (v40 >= 0.0f) {
            v40 += (float_t)(-50.0 * DEG_TO_RAD);
            if (v40 < v18)
                v40 = v18;
        }
        else {
            v40 += (float_t)(50.0 * DEG_TO_RAD);
            if (v40 > v15)
                v40 = v15;
        }

        head_rot_strength = look_anim->ext_head_rot_strength;
        look_anim->ext_head_rot_y_angle = v40;
    }
    else
        look_anim->ext_head_rot_y_angle = 0.0f;

    float_t v48 = v40 * head_rot_strength;
    float_t v49 = v48;
    if (!look_anim->field_191) {
        if (look_anim->field_192)
            v48 = sub_14040ADE0(v48, look_anim->field_19C.field_0) * v14 + look_anim->field_19C.field_0;
        look_anim->field_19C.field_0 = v48;
        look_anim->field_1AC.field_0 = v48;
    }
    else if (look_anim->field_190) {
        look_anim->field_19C.field_0 = v48;
        look_anim->field_1AC.field_0 = v48;
    }
    else {
        look_anim->field_19C.field_0 = v48;
        v48 = look_anim->field_1AC.field_0;
    }

    mat4 cl_kao_mat = *bones[BLK_CL_KAO].node[1].mat_ptr;
    float_t head_rot_angle = (v48 - atan2f(-v68.x, v68.y)) * look_anim->head_rot_blend;
    vec3 head_rot_axis;
    head_rot_axis.x = n_kao_mat->row2.x * 0.5f;
    head_rot_axis.y = (n_kao_mat->row2.y + 1.0f) * 0.5f;
    head_rot_axis.z = n_kao_mat->row2.z * 0.5f;
    mat4_mul_rotation(&cl_kao_mat, &head_rot_axis, head_rot_angle, &cl_kao_mat);
    mat4_inverse_transform_point(&cl_kao_mat, &look_anim->field_184, &v67);

    float_t v52;
    if (v36)
        v52 = look_anim->field_19C.field_C;
    else
        v52 = atan2f(-v67.z, vec2::length(*(vec2*)&v67));

    if (look_anim->field_190) {
        float_t v55 = fabsf(v49);
        if (v52 >= 0.0f)
            v55 *= look_anim->field_15C.field_24;
        else
            v55 *= look_anim->field_15C.field_20;
        v52 *= (look_anim->field_15C.field_8 - v55) / look_anim->field_15C.field_8;
        v52 = clamp_def(v52, look_anim->field_15C.min, look_anim->field_15C.max);
    }

    if (look_anim->type == 3 || look_anim->ext_head_rotation)
        v52 = 0.0f;

    look_anim->ext_head_rot_x_angle = 0.0f;

    float_t v58 = v52 * look_anim->head_rot_strength;
    if (!look_anim->field_191) {
        if (look_anim->field_193)
            v58 = sub_14040ADE0(v58, look_anim->field_19C.field_4) * v14 + look_anim->field_19C.field_4;
        look_anim->field_19C.field_4 = v58;
        look_anim->field_1AC.field_4 = v58;
    }
    else if (look_anim->field_190) {
        look_anim->field_19C.field_4 = v58;
        look_anim->field_1AC.field_4 = v58;
    }
    else {
        look_anim->field_19C.field_4 = v58;
        v58 = look_anim->field_1AC.field_4;
    }

    mat4_mul_rotate_y(&cl_kao_mat, v58 * look_anim->head_rot_blend, &cl_kao_mat);

    mat4 cl_kao_mat_backup = bones[BLK_CL_KAO].chain_rot[1];

    mat4 v74;
    mat4_invert(bones[BLK_CL_KAO].node[0].mat_ptr, &v74);
    mat4_clear_trans(&v74, &v74);
    mat4_clear_trans(&cl_kao_mat, &cl_kao_mat);
    mat4_mul(&cl_kao_mat, &v74, &bones[BLK_CL_KAO].chain_rot[1]);

    for (int32_t i = BLK_CL_KAO; i <= BLK_N_KUBI_WJ_EX; i++)
        bones[i].mult_mat(parent_mat, bones.data(), true);

    bones[BLK_CL_KAO].chain_rot[1] = cl_kao_mat_backup;
}

static void sub_1404189A0(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
    sub_140407280(&rob_bone_data->look_anim, mot->bone_data.bones,
        mot->field_4F8.mat, mot->mot_play_data.frame_data.step);
}

static void sub_140406FC0(rob_chara_look_anim* look_anim, RobBlock* bone, mat4* eye_mat, vec3 pos,
    const vec3& rot_neg, const vec3& rot_pos, bool eyes_rot_anim, float_t eyes_rot_blend, float_t eyes_rot_step) {
    pos.z = fabsf(pos.z);

    float_t rot_x;
    float_t rot_y;
    float_t v17 = sqrtf(pos.x * pos.x + pos.z * pos.z);
    if (fabsf(v17) > 0.000001f) {
        rot_y = atan2f(pos.x, pos.z);

        rot_x = atanf(-(pos.y / v17));
        if (rot_x > 0.0f)
            rot_x *= look_anim->eyes_xrot_adjust_pos;
        else if (rot_x < 0.0f)
            rot_x *= look_anim->eyes_xrot_adjust_neg;

        rot_x = clamp_def(rot_x, rot_neg.x, rot_pos.x);
        rot_y = clamp_def(rot_y, rot_neg.y, rot_pos.y);
    }
    else {
        rot_x = pos.y > 0.0f ? rot_neg.x : rot_pos.x;
        rot_y = 0.0f;
    }

    if (look_anim->ext_head_rotation && fabsf(look_anim->ext_head_rot_strength) > 0.000001f) {
        if (fabsf(look_anim->ext_head_rot_x_angle) > 0.000001f)
            rot_x = look_anim->ext_head_rot_x_angle < 0.0f ? rot_neg.y : rot_pos.y;
        else
            rot_x = rot_x >= 0.0f ? rot_neg.y : rot_pos.y;

        if (fabsf(look_anim->ext_head_rot_y_angle) > 0.000001f)
            rot_y = look_anim->ext_head_rot_y_angle < 0.0f ? rot_neg.y : rot_pos.y;
        else
            rot_y = rot_y >= 0.0f ? rot_neg.y : rot_pos.y;
    }

    mat4 mat;
    mat4_rotate_y(rot_y, &mat);
    mat4_mul_rotate_x(&mat, rot_x, &mat);
    mat4_lerp_rotation(&mat4_identity, &mat, &mat, look_anim->eyes_rot_strength);
    if (eyes_rot_anim)
        mat4_lerp_rotation(&bone->chain_rot[0], &mat, &mat, eyes_rot_blend);
    else
        mat4_lerp_rotation(eye_mat, &mat, &mat, eyes_rot_step);
    *eye_mat = mat;
    bone->chain_rot[0] = mat;
}

static void sub_140409170(rob_chara_look_anim* look_anim, const mat4& adjust_mat,
    prj::sys_vector<RobBlock>& bones, const mat4& mat, float_t step) {
    if (look_anim->disable)
        return;

    look_anim->bones = &bones;
    look_anim->mat = mat;

    bool eyes_rot_anim = false;
    float_t eyes_rot_blend = 0.0f;
    if (look_anim->duration > 0.0f && look_anim->duration > look_anim->eyes_rot_frame
        && fabsf(look_anim->duration - look_anim->eyes_rot_frame) > 0.000001f) {
        eyes_rot_anim = true;
        eyes_rot_blend = (look_anim->eyes_rot_frame + 1.0f) / (look_anim->duration + 1.0f);
        if (!look_anim->eyes_rotation)
            eyes_rot_blend = 1.0f - eyes_rot_blend;
        look_anim->eyes_rot_frame += step;
    }

    float_t eyes_rot_step = look_anim->eyes_rot_step * step;
    if (fabsf(look_anim->eyes_rot_step - 1.0f) <= 0.000001f
        || eyes_rot_step > 1.0f || look_anim->init_eyes_rotation && !eyes_rot_anim
        || rctx_ptr->camera->fast_change_hist0 && rctx_ptr->camera->fast_change_hist1)
        eyes_rot_step = 1.0f;

    look_anim->init_head_rotation = false;
    look_anim->init_eyes_rotation = false;
    if (!(look_anim->eyes_rotation || look_anim->ext_head_rotation
        && fabsf(look_anim->ext_head_rot_strength) > 0.000001f || eyes_rot_anim)
        || !(look_anim->head_rot_strength <= 0.0f || look_anim->type != 3))
        return;

    look_anim->view_point = look_anim->target_view_point;
    mat4 v61;
    mat4_invert(&adjust_mat, &v61);
    mat4_transform_point(&v61, &look_anim->view_point, &look_anim->view_point);

    const mat4* kl_eye_l_parent_mat = bones[BLK_KL_EYE_L].inherit_mat_ptr;
    const mat4* kl_eye_r_parent_mat = bones[BLK_KL_EYE_R].inherit_mat_ptr;

    float_t yrot_neg_left;
    float_t yrot_pos_left;
    float_t yrot_neg_right;
    float_t yrot_pos_right;
    if (look_anim->ft) {
        yrot_neg_left = look_anim->param.ft.yrot_neg;
        yrot_pos_left = look_anim->param.ft.yrot_pos;
        yrot_neg_right = -look_anim->param.ft.yrot_pos;
        yrot_pos_right = -look_anim->param.ft.yrot_neg;
    }
    else {
        yrot_neg_left = look_anim->param.ac.yrot_neg;
        yrot_pos_left = look_anim->param.ac.yrot_pos;
        yrot_neg_right = -look_anim->param.ac.yrot_pos;
        yrot_pos_right = -look_anim->param.ac.yrot_neg;
    }

    if (look_anim->field_B0 > 0.0f) {
        mat4 v61 = *kl_eye_l_parent_mat;
        *(vec3*)&v61.row3 = (*(vec3*)&kl_eye_l_parent_mat->row3 + *(vec3*)&kl_eye_r_parent_mat->row3) * 0.5f;

        vec3 v54;
        mat4_inverse_transform_point(&v61, &look_anim->view_point, &v54);

        float_t v38 = vec3::length(v54);
        if (fabsf(v38) <= 0.000001f || v54.x * v54.x + v54.z * v54.z <= 0.01f) {
            yrot_neg_left = 0.0f;
            yrot_pos_right = 0.0f;
        }
        else if (v38 < look_anim->field_B0) {
            vec3 v53 = v54 * (look_anim->field_B0 / v38);
            vec3 v60;
            mat4_transform_point(&v61, &v53, &v60);

            vec3 v56;
            mat4_inverse_transform_point(kl_eye_l_parent_mat, &v60, &v56);

            float_t v41 = atan2f(v56.x - look_anim->param.pos.x, v56.z);
            if (yrot_neg_left < v41)
                yrot_neg_left = min_def(yrot_pos_left, v41);

            vec3 v55;
            mat4_inverse_transform_point(kl_eye_r_parent_mat, &v60, &v55);

            float_t v42 = atan2f(v55.x + look_anim->param.pos.x, v55.z);
            if (yrot_pos_right > v42)
                yrot_pos_right = max_def(-yrot_pos_left, v42);
        }
    }

    float_t xrot_neg;
    float_t xrot_pos;
    if (look_anim->ft) {
        xrot_neg = look_anim->param.ft.xrot_neg * -3.8f
            / look_anim->param.xrot_adjust_dir_neg * look_anim->eyes_xrot_adjust_neg;
        xrot_pos = look_anim->param.ft.xrot_pos * 6.0f
            / look_anim->param.xrot_adjust_dir_pos * look_anim->eyes_xrot_adjust_pos;
    }
    else {
        xrot_neg = look_anim->param.ac.xrot_neg * look_anim->eyes_xrot_adjust_neg;
        xrot_pos = look_anim->param.ac.xrot_pos * look_anim->eyes_xrot_adjust_pos;
    }

    vec3 rot_neg_left = vec3(xrot_neg, yrot_neg_left, 0.0f);
    vec3 rot_pos_left = vec3(xrot_pos, yrot_pos_left, 0.0f);
    vec3 pos_left;
    mat4_inverse_transform_point(kl_eye_l_parent_mat, &look_anim->view_point, &pos_left);
    sub_140406FC0(look_anim, &bones[BLK_KL_EYE_L],
        &look_anim->left_eye_mat, pos_left - look_anim->param.pos,
        rot_neg_left, rot_pos_left, eyes_rot_anim, eyes_rot_blend, eyes_rot_step);

    const vec3 _xor = vec3(-0.0f, 0.0f, 0.0f);
    vec3 rot_neg_right = vec3(xrot_neg, yrot_neg_right, 0.0f);
    vec3 rot_pos_right = vec3(xrot_pos, yrot_pos_right, 0.0f);
    vec3 pos_right;
    mat4_inverse_transform_point(kl_eye_r_parent_mat, &look_anim->view_point, &pos_right);
    sub_140406FC0(look_anim, &bones[BLK_KL_EYE_R],
        &look_anim->right_eye_mat, pos_right - (look_anim->param.pos ^ _xor),
        rot_neg_right, rot_pos_right, eyes_rot_anim, eyes_rot_blend, eyes_rot_step);

    for (int32_t i = BLK_KL_EYE_L; i <= BLK_KL_HIGHLIGHT_L_WJ; i++)
        bones[i].mult_mat(mat, bones.data(), true);

    for (int32_t i = BLK_KL_EYE_R; i <= BLK_KL_HIGHLIGHT_R_WJ; i++)
        bones[i].mult_mat(mat, bones.data(), true);
}

static void sub_14041A160(rob_chara_bone_data* rob_bone_data, const mat4& adjust_mat) {
    motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
    sub_140409170(&rob_bone_data->look_anim, adjust_mat, mot->bone_data.bones,
        mot->field_4F8.mat, mot->mot_play_data.frame_data.step);
}

static void rob_chara_head_adjust(rob_chara* rob_chr) {
    if (rob_chr->type != ROB_CHARA_TYPE_2)
        return;

    bool v4 = !rob_chr->data.flag.bit.dmy_yokerare;
    rob_chr->data.flag.bit.dmy_yokerare = 0;

    rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;
    vec3* target_view_point = 0;
    if (rob_bone_data->get_look_anim_head_rotation()
        || rob_bone_data->get_look_anim_ext_head_rotation()) {
        if (rob_bone_data->get_look_anim_update_view_point()) {
            vec3 view_point;
            rctx_ptr->camera->get_view_point(view_point);
            rob_bone_data->set_look_anim_target_view_point(view_point);
        }

        if (rob_bone_data->check_look_anim_head_rotation()
            || rob_bone_data->check_look_anim_ext_head_rotation())
            target_view_point = rob_bone_data->get_look_anim_target_view_point();
    }

    sub_14041C620(rob_bone_data, target_view_point, &rob_chr->data.adjust_data.mat, v4);
    sub_1404189A0(rob_bone_data);
    sub_14041A160(rob_bone_data, rob_chr->data.adjust_data.mat);
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

static void sub_14040AE10(mat4* mat, const vec3& target) {
    vec3 local_target;
    mat4_inverse_transform_point(mat, &target, &local_target);

    float_t target_len_xy_sq = vec2::length_squared(*(vec2*)&local_target);
    float_t target_len_sq = vec3::length_squared(local_target);
    float_t target_len_xy = sqrtf(target_len_xy_sq);
    float_t target_len = sqrtf(target_len_sq);
    if (fabsf(target_len_xy) > 0.000001f && fabsf(target_len) > 0.000001f) {
        mat4_mul_rotate_z(mat, (1.0f / target_len_xy) * local_target.y,
            (1.0f / target_len_xy) * local_target.x, mat);
        mat4_mul_rotate_y(mat, -(1.0f / target_len) * local_target.z,
            (1.0f / target_len) * target_len_xy, mat);
    }
}

static void sub_140406A70(struc_936* a1, prj::sys_vector<RobBlock>& bones, const mat4& mat, const vec3& target,
    const BONE_BLK* a5, float_t rotation_blend, float_t arm_length, bool solve_ik) {
    RobBlock* v14 = &bones[a5[0]];

    if (v14->up_vector_mat_ptr) {
        vec3 v36;
        mat4_get_translation(v14->up_vector_mat_ptr, &v36);

        mat4 v38 = *v14->node->mat_ptr;
        sub_14040AE10(&v38, v14->leaf_pos[1]);
        mat4_inverse_transform_point(&v38, &v36, &v36);

        v38 = *v14->node->mat_ptr;
        sub_14040AE10(&v38, target);

        vec3 v37;
        mat4_transform_point(&v38, &v36, &v37);
        mat4_set_translation((mat4*)v14->up_vector_mat_ptr, &v37);
    }

    v14->leaf_pos[1] = target;
    v14->chain_rot[1] = mat4_identity;
    v14->chain_rot[2] = mat4_identity;
    v14->get_mat_ik(1);

    if (a5[1] == BLK_DUMMY)
        return;

    RobBlock* v25 = &bones[a5[1]];
    if (rotation_blend > 0.0f) {
        mat4 v38;
        mat4 v39;
        mat4_clear_trans(v25->inherit_mat_ptr, &v38);
        mat4_clear_trans(v25->node->mat_ptr, &v39);
        mat4_transpose(&v38, &v38);
        mat4_mul(&v39, &v38, &v38);
        if (rotation_blend < 1.0f)
            mat4_lerp_rotation(&v25->chain_rot[0], &v38, &v38, rotation_blend);
        v25->chain_rot[0] = v38;
    }

    if (solve_ik) {
        a5++;
        while (*a5 != BLK_DUMMY)
            bones[*a5++].mult_mat(mat, bones.data(), true);
    }
    else
        v25->mult_mat(mat, bones.data(), false);
}

static void sub_140418A00(rob_chara_bone_data* rob_bone_data, const vec3& target,
    const BONE_BLK* a3, float_t rotation_blend, float_t arm_length, bool solve_ik) {
    motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
    mat4 m = mot->field_4F8.mat;
    sub_140406A70(&rob_bone_data->field_76C, mot->bone_data.bones,
        m, target, a3, rotation_blend, arm_length, solve_ik);
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
    if (rob_chr->data.motion.hand_adjust[hand].iterations > 0
        && fabsf(rotation_blend - 1.0f) > 0.000001f
        && (fabsf(rob_chr->data.motion.hand_adjust[hand].offset.x) > 0.000001f
            || fabsf(rob_chr->data.motion.hand_adjust[hand].offset.y) > 0.000001f
            || fabsf(rob_chr->data.motion.hand_adjust[hand].offset.z) > 0.000001f)) {
        v15 = rob_chr->data.motion.hand_adjust[hand].iterations;
        solve_ik = false;
    }

    mat4* v40 = rob_chr->bone_data->get_mats_mat(rob_kl_te_bones[hand]);
    mat4 v42;
    mat4_mul(v40, &adjust_data.mat, &v42);

    vec3 v37 = rob_chr->data.motion.hand_adjust[hand].offset * (1.0f - chara_scale / adjust_scale);

    vec3 v38;
    mat4_transform_point(&v42, &v37, &v38);

    vec3 v27 = 0.0f;
    if (rob_chr->data.motion.hand_adjust[hand].disable_x)
        v27.x = v40->row3.x - v38.x;
    if (rob_chr->data.motion.hand_adjust[hand].disable_y)
        v27.y = v40->row3.y - v38.y;
    if (rob_chr->data.motion.hand_adjust[hand].disable_z)
        v27.z = v40->row3.z - v38.z;

    vec3 target = v38 + v27;

    vec3 v18 = 0.0f;
    if (v15 > 0) {
        mat4_transform_point(&v42, &rob_chr->data.motion.hand_adjust[hand].offset, &v38);
        v18 = v38 + v27;
    }

    sub_140418A00(rob_chr->bone_data, target, off_140C9E020[hand], rotation_blend,
        rob_chr->data.motion.hand_adjust[hand].arm_length, solve_ik);

    while (v15 > 0) {
        mat4* v40 = rob_chr->bone_data->get_mats_mat(rob_kl_te_bones[hand]);
        mat4_set_translation(v40, &v18);

        vec3 v37 = -rob_chr->data.motion.hand_adjust[hand].offset;
        mat4_transform_point(v40, &v37, &target);
        if (!--v15) {
            rotation_blend = 1.0f;
            solve_ik = true;
        }

        sub_140418A00(rob_chr->bone_data, target, off_140C9E020[hand], rotation_blend,
            rob_chr->data.motion.hand_adjust[hand].arm_length, solve_ik);
    }

    rob_chr->data.motion.hand_adjust[hand].target = target;
    return true;
}

static void sub_140412860(motion_blend_mot* mot, BONE_BLK index, mat4* mat) {
    if (index >= mot->bone_data.bones.size())
        return;

    RobBlock* v4 = &mot->bone_data.bones.data()[index];
    if (!v4)
        return;

    switch (v4->ik_type) {
    case IKT_1:
        *mat = *v4->node[2].mat_ptr;
        break;
    case IKT_2:
    case IKT_2R:
        *mat = *v4->node[3].mat_ptr;
        break;
    }
}

static void sub_140419290(rob_chara_bone_data* rob_bone_data, BONE_BLK index, mat4* mat) {
    sub_140412860(rob_bone_data->motion_loaded.front(), index, mat);
}

static bool sub_14053B580(rob_chara* rob_chr, int32_t a2) {
    if (a2 < 0 || a2 > 3)
        return 0;

    struc_306& v0 = rob_chr->data.motdata.field_330.field_33C[a2];
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
        float_t ratio0 = rob_chr->field_20->bone_data->ik_scale.ratio0;
        v45 = v13 * ratio0;
    } break;
    case 11: {
        float_t ratio1 = rob_chr->field_20->bone_data->ik_scale.ratio1;
        v45 = v13 * ratio1;
    } break;
    case 12: {
        float_t ratio3 = rob_chr->field_20->bone_data->ik_scale.ratio3;
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
            mat4* v26 = rob_chr->field_20->bone_data->get_mats_mat(dword_140A2DDD0[v0.field_C]);
            mat4_get_translation(v26, &v45);
            v45 += v0.field_10;
        }
        else {
            mat4* v23 = rob_chr->field_20->bone_data->get_mats_mat(dword_140A2DDD0[v0.field_C]);

            mat4 v49;
            mat4_mul_translate(v23, &v0.field_10, &v49);
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

    sub_140418A00(rob_chr->bone_data, v48, off_140C9E000[a2], 0.0f, 0.0f, true);
    return true;
}

static void sub_1403F9A40(vec3* a1, vec3* a2, mat4* a3, float_t a4, float_t a5) {
    vec3 v10;
    mat4_inverse_transform_point(a3, a2, &v10);
    v10.y += a4;
    v10.z += a5;
    mat4_transform_point(a3, &v10, a1);
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
    mat4_inverse_transform_vector(a3, a1, &v50);

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
    mat4_transform_vector(a3, &v50, a1);
    *a8 = v50;
}

static void sub_1403FA770(rob_chara_sleeve_adjust* a1) {
    mat4* v4 = a1->bones->data()[BLK_C_KATA_L].node[3].mat_ptr;
    mat4* v5 = a1->bones->data()[BLK_C_KATA_R].node[3].mat_ptr;

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
            mat4_transform_vector(v4, &a1->field_5C, &v45);
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
            mat4_transform_vector(v5, &a1->field_68, &v44);
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
        mat4 mat = *a1->bones->data()[BLK_C_KATA_L].node[2].mat_ptr;
        sub_14040AE10(&mat, v42 + v45);
        *a1->bones->data()[BLK_C_KATA_L].node[2].mat_ptr = mat;
    }

    if (v19 || v18) {
        mat4 mat = *a1->bones->data()[BLK_C_KATA_R].node[2].mat_ptr;
        sub_14040AE10(&mat, v43 + v44);
        *a1->bones->data()[BLK_C_KATA_R].node[2].mat_ptr = mat;
    }
}

static void sub_1403F9B20(rob_chara_sleeve_adjust* a1, BONE_BLK block_id) {
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
    if (block_id == BLK_C_KATA_L) {
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
    else if (block_id == BLK_C_KATA_R) {
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

    RobBlock* v15 = a1->bones->data();
    RobNode* v18 = v15[block_id].node;
    mat4* v19 = v18[BLK_KL_HARA_ETC].mat_ptr;

    vec3 v39;
    mat4_get_translation(v19, &v39);

    vec3 v37;
    sub_1403F9A40(&v37, &v39, v19, sleeve_cyofs, sleeve_czofs);

    vec3 v40;
    mat4* v23 = v15[BLK_KL_MUNE_B_WJ].node->mat_ptr;
    mat4_transform_point(v23, &sleeve_mune_ofs, &v40);

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
        mat4_transform_vector(v19, v6, &v41);
    }

    mat4* v42 = v15[block_id].node[2].mat_ptr;
    sub_14040AE10(v42, v39 + v41);
}

static void sub_1403FAF30(rob_chara_sleeve_adjust* a1, prj::sys_vector<RobBlock>& bones, float_t step) {
    a1->step = step;
    a1->bones = &bones;
    if (a1->enable1)
        sub_1403FA770(a1);

    if (a1->enable2) {
        sub_1403F9B20(a1, BLK_C_KATA_L);
        sub_1403F9B20(a1, BLK_C_KATA_R);
    }
}

static void sub_14041AB50(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    sub_1403FAF30(&rob_bone_data->sleeve_adjust, v2->bone_data.bones, v2->mot_play_data.frame_data.step);
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

static float_t sub_140406E90(RobBlock* a1, RobBlock* a2, float_t a3, vec3* a4) {
    vec3 v18 = 0.0f;
    mat4_transform_point(a1->node[3].mat_ptr, &v18, &v18);
    *a4 = v18;

    vec3 v17;
    v17.x = 0.01f;
    v17.y = -0.05f;
    v17.z = 0.0f;
    mat4_transform_point(a2->node[0].mat_ptr, &v18, &v17);

    if (v18.y - a3 <= v17.y)
        return v18.y - a3;
    return  v17.y;
}

static void sub_140406920(struc_936* a1, RobBlock* a2, RobBlock* a3, float_t heel_height, vec3* a5, float_t step) {
    vec3 v18;
    float_t v7 = sub_140406E90(a2, a3, heel_height, &v18);
    float_t v8 = task_stage_current_get_floor_height(v18, heel_height);
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

    a2->leaf_pos[1].x = v18.x;
    a2->leaf_pos[1].y = v18.y + lerp_def(v12, v11, a5->z);
    a2->leaf_pos[1].z = v18.z;
}

static void sub_1404065B0(struc_936* a1, prj::sys_vector<RobBlock>* a2, const mat4& mat, float_t a4,
    BONE_KIND skeleton_type, const BONE_BLK* a6, const BONE_BLK* a7) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;

    const float_t* heel_height = aft_bone_data->get_heel_height(skeleton_type);
    if (!heel_height)
        return;

    sub_140406920(a1, &a2->data()[a6[0]], &a2->data()[BLK_KL_TOE_L_WJ], *heel_height, &a1->toe_l, a4);
    sub_140406920(a1, &a2->data()[a7[0]], &a2->data()[BLK_KL_TOE_R_WJ], *heel_height, &a1->toe_r, a4);

    RobBlock* v7 = a2->data();
    RobBlock* v17 = &v7[a6[0]];
    v17->chain_rot[1] = mat4_identity;
    v17->chain_rot[2] = mat4_identity;
    v17->get_mat_ik(1);

    RobBlock* v20 = &v7[a7[0]];
    v20->chain_rot[1] = mat4_identity;
    v20->chain_rot[2] = mat4_identity;
    v20->get_mat_ik(1);

    BONE_BLK v21 = a6[1];

    a6++;
    while (*a6 != BLK_DUMMY)
        v7[*a6++].mult_mat(mat, v7, true);

    a7++;
    while (*a7 != BLK_DUMMY)
        v7[*a7++].mult_mat(mat, v7, true);
}

static void sub_140418810(rob_chara_bone_data* rob_bone_data, const BONE_BLK* a6, const BONE_BLK* a7) {
    motion_blend_mot* v3 = rob_bone_data->motion_loaded.front();
    mat4 m = v3->field_4F8.mat;
    sub_1404065B0(&rob_bone_data->field_76C, &v3->bone_data.bones, m,
        v3->mot_play_data.frame_data.step, rob_bone_data->skeleton_type, a6, a7);
}

static void sub_14053B260(rob_chara* rob_chr) {
    if (rob_chr->data.motdata.field_0.field_20.field_C & 0x10000000)
        sub_140418810(rob_chr->bone_data, dword_140A2DDB0, dword_140A2DD90);
}

static void rob_chara_set_hand_l_object(rob_chara* rob_chr, object_info obj_info, int32_t type) {
    switch (type) {
    case 0:
    default:
        rob_chr->data.motion.field_150.hand_l_object = obj_info;
        break;
    case 1:
        rob_chr->data.motion.hand_l_object = obj_info;
        break;
    case 2:
        rob_chr->data.motion.field_3B0.hand_l_object = obj_info;
        break;
    }
}

static void rob_chara_set_hand_r_object(rob_chara* rob_chr, object_info obj_info, int32_t type) {
    switch (type) {
    case 0:
    default:
        rob_chr->data.motion.field_150.hand_r_object = obj_info;
        break;
    case 1:
        rob_chr->data.motion.hand_r_object = obj_info;
        break;
    case 2:
        rob_chr->data.motion.field_3B0.hand_r_object = obj_info;
        break;
    }
}

static void rob_chara_age_age_ctrl(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, mat4& mat) {
    arr[chara_id * 3 + part_id].ctrl(mat);
}

static void rob_chara_age_age_disp(rob_chara_age_age* arr,
    render_context* rctx, int32_t chara_id, bool reflect, bool chara_color) {
    bool npr = !!rctx->render_manager->npr_param;
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

static void rob_chara_age_age_set_speed(rob_chara_age_age* arr,
    int32_t chara_id, int32_t part_id, float_t value) {
    arr[chara_id * 3 + part_id].set_speed(value);
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
    const std::vector<BODYTYPE>* bones) {
    bone_database_bones_calculate_count(bones, rob_bone_data->object_bone_count,
        rob_bone_data->motion_bone_count, rob_bone_data->node_count,
        rob_bone_data->leaf_pos, rob_bone_data->chain_pos);
}

static void rob_chara_bone_data_get_ik_scale(
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data) {
    if (rob_bone_data->motion_loaded.size() < 0)
        return;

    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    rob_chara_bone_data_ik_scale_calculate(&rob_bone_data->ik_scale, v2->bone_data.bones,
        rob_bone_data->base_skeleton_type, rob_bone_data->skeleton_type, bone_data);
    float_t ratio0 = rob_bone_data->ik_scale.ratio0;
    v2->field_4F8.field_C0 = ratio0;
    v2->field_4F8.field_C4 = ratio0;
    v2->field_4F8.field_C8 = 0.0f;
}

static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index) {
    RobNode* node = rob_chara_bone_data_get_node(rob_bone_data, index);
    if (node)
        return node->mat_ptr;
    return 0;
}

static RobNode* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index) {
    if (index < rob_bone_data->nodes.size())
        return &rob_bone_data->nodes[index];
    return 0;
}

static void rob_chara_bone_data_ik_scale_calculate(
    rob_chara_bone_data_ik_scale* ik_scale, prj::sys_vector<RobBlock>& bones,
    BONE_KIND base_skeleton_type, BONE_KIND skeleton_type, const bone_database* bone_data) {
    const float_t* base_heel_height = bone_data->get_heel_height(base_skeleton_type);
    const float_t* heel_height = bone_data->get_heel_height(skeleton_type);
    if (!base_heel_height || !heel_height)
        return;

    RobBlock* b_cl_mune = &bones[BLK_CL_MUNE];
    RobBlock* b_kl_kubi = &bones[BLK_KL_KUBI];
    RobBlock* b_c_kata_l = &bones[BLK_C_KATA_L];
    RobBlock* b_cl_momo_l = &bones[BLK_CL_MOMO_L];

    float_t base_height = fabsf(b_cl_momo_l->chain_pos[0].y) + b_cl_momo_l->len[0][0]
        + b_cl_momo_l->len[1][0] + *base_heel_height;
    float_t height = fabsf(b_cl_momo_l->chain_pos[1].y) + b_cl_momo_l->len[0][1]
        + b_cl_momo_l->len[1][1] + *heel_height;
    ik_scale->ratio0 = height / base_height;
    ik_scale->ratio1 = (fabsf(b_kl_kubi->chain_pos[1].y) + b_cl_mune->len[0][1])
        / (fabsf(b_kl_kubi->chain_pos[0].y) + b_cl_mune->len[0][0]);
    ik_scale->ratio2 = (b_c_kata_l->len[0][1] + b_c_kata_l->len[1][1])
        / (b_c_kata_l->len[0][0] + b_c_kata_l->len[1][0]);
    ik_scale->ratio3 = (fabsf(b_kl_kubi->chain_pos[1].y) + b_cl_mune->len[0][1] + height)
        / (fabsf(b_kl_kubi->chain_pos[0].y) + b_cl_mune->len[0][0] + base_height);
}

static bool motion_blend_mot_check(BONE_BLK bone_index) {
    return true;
}

static bool partial_motion_blend_mot_face_check(BONE_BLK bone_index) {
    return bone_index >= BLK_FACE_ROOT && bone_index <= BLK_TL_TOOTH_UPPER_WJ;
}

static bool partial_motion_blend_mot_hand_l_check(BONE_BLK bone_index) {
    return bone_index >= BLK_N_HITO_L_EX && bone_index <= BLK_NL_OYA_C_L_WJ;
}

static bool partial_motion_blend_mot_hand_r_check(BONE_BLK bone_index) {
    return bone_index >= BLK_N_HITO_R_EX && bone_index <= BLK_NL_OYA_C_R_WJ;
}

static bool partial_motion_blend_mot_mouth_check(BONE_BLK bone_index) {
    return bone_index >= BLK_N_AGO && bone_index <= BLK_TL_TOOTH_UNDER_WJ
        || bone_index >= BLK_N_KUTI_D && bone_index <= BLK_TL_KUTI_U_R_WJ
        || bone_index >= BLK_N_TOOTH_UPPER && bone_index <= BLK_TL_TOOTH_UPPER_WJ;
}

static bool partial_motion_blend_mot_eyes_check(BONE_BLK bone_index) {
    return bone_index >= BLK_N_EYE_L && bone_index <= BLK_KL_HIGHLIGHT_R_WJ;
}

static bool partial_motion_blend_mot_eyelid_check(BONE_BLK bone_index) {
    if (bone_index >= BLK_N_MABU_L_D_A && bone_index <= BLK_TL_MABU_R_U_C_WJ
        || bone_index >= BLK_N_EYELID_L_A && bone_index <= BLK_TL_EYELID_R_B_WJ)
        return true;
    return partial_motion_blend_mot_eyes_check(bone_index);
}

static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    BONE_KIND base_type, BONE_KIND type, const bone_database* bone_data) {
    rob_chara_bone_data_init_skeleton(rob_bone_data, base_type, type, bone_data);
    for (motion_blend_mot*& i : rob_bone_data->motions)
        i->init(rob_bone_data, motion_blend_mot_check, bone_data);

    size_t motion_bone_count = rob_bone_data->motion_bone_count;
    rob_bone_data->face.init(base_type, partial_motion_blend_mot_face_check, motion_bone_count, bone_data);
    rob_bone_data->hand_l.init(base_type, partial_motion_blend_mot_hand_l_check, motion_bone_count, bone_data);
    rob_bone_data->hand_r.init(base_type, partial_motion_blend_mot_hand_r_check, motion_bone_count, bone_data);
    rob_bone_data->mouth.init(base_type, partial_motion_blend_mot_mouth_check, motion_bone_count, bone_data);
    rob_bone_data->eyes.init(base_type, partial_motion_blend_mot_eyes_check, motion_bone_count, bone_data);
    rob_bone_data->eyelid.init(base_type, partial_motion_blend_mot_eyelid_check, motion_bone_count, bone_data);
}

static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
    BONE_KIND base_skeleton_type, BONE_KIND skeleton_type, const bone_database* bone_data) {
    if (rob_bone_data->base_skeleton_type == base_skeleton_type
        && rob_bone_data->skeleton_type == skeleton_type)
        return;

    const char* name = bone_database_skeleton_type_to_string(base_skeleton_type);
    const std::vector<BODYTYPE>* bones = bone_data->get_body_type(name);
    const std::vector<uint16_t>* parent_indices = bone_data->get_parent_node(name);
    const std::vector<std::string>* motion_bones = bone_data->get_bone_node_name(name);
    if (!bones || !parent_indices || !motion_bones)
        return;

    rob_chara_bone_data_calculate_bones(rob_bone_data, bones);
    rob_chara_bone_data_reserve(rob_bone_data);
    rob_chara_bone_data_set_mats(rob_bone_data, bones, motion_bones->data());
    rob_chara_bone_data_set_parent_mats(rob_bone_data, parent_indices->data());
    rob_bone_data->base_skeleton_type = base_skeleton_type;
    rob_bone_data->skeleton_type = skeleton_type;
}

static void sub_14041B4E0(prj::sys_vector<bool>& vec) {
    for (uint32_t i = BLK_FACE_ROOT; i <= BLK_TL_TOOTH_UPPER_WJ; i++)
        vec[i] = false;
}

static void sub_14041B580(prj::sys_vector<bool>& vec) {
    for (uint32_t i = BLK_FACE_ROOT; i <= BLK_TL_TOOTH_UPPER_WJ; i++)
        vec[i] = false;

    for (uint32_t i = BLK_N_EYE_L; i <= BLK_KL_HIGHLIGHT_R_WJ; i++)
        vec[i] = true;
}

static void sub_14041B800(prj::sys_vector<bool>& vec) {
    for (uint32_t i = BLK_N_KUTI_D; i <= BLK_TL_KUTI_U_R_WJ; i++)
        vec[i] = false;

    for (uint32_t i = BLK_N_AGO; i <= BLK_TL_TOOTH_UNDER_WJ; i++)
        vec[i] = false;

    for (uint32_t i = BLK_N_TOOTH_UPPER; i <= BLK_TL_TOOTH_UPPER_WJ; i++)
        vec[i] = false;
}

static void sub_14041B440(prj::sys_vector<bool>& vec) {
    for (uint32_t i = BLK_N_EYE_L; i <= BLK_KL_HIGHLIGHT_R_WJ; i++)
        vec[i] = false;
}

static void sub_14041B1C0(prj::sys_vector<bool>& vec) {
    for (uint32_t i = BLK_N_MABU_L_D_A; i <= BLK_TL_MABU_R_U_C_WJ; i++)
        vec[i] = false;

    for (uint32_t i = BLK_N_EYELID_L_A; i <= BLK_TL_EYELID_R_B_WJ; i++)
        vec[i] = false;

    sub_14041B440(vec);
}

static void sub_14041B6B0(prj::sys_vector<bool>& vec) {
    for (uint32_t i = BLK_N_HITO_L_EX; i <= BLK_NL_OYA_C_L_WJ; i++)
        vec[i] = false;
}

static void sub_14041B750(prj::sys_vector<bool>& vec) {
    for (uint32_t i = BLK_N_HITO_R_EX; i <= BLK_NL_OYA_C_R_WJ; i++)
        vec[i] = false;
}

static void sub_14041B300(prj::sys_vector<bool>& vec) {
    for (uint32_t i = BLK_N_MABU_L_D_A; i <= BLK_TL_MABU_R_U_C_WJ; i++)
        vec[i] = false;

    for (uint32_t i = BLK_N_EYELID_L_A; i <= BLK_TL_EYELID_R_B_WJ; i++)
        vec[i] = false;
}

static void sub_14041AD90(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->mouth.mot_key_data.key_sets_ready
        && rob_bone_data->mouth.mot_key_data.mot_data && !rob_bone_data->mouth.disable)
        rob_bone_data->face.enabled_bones.set(sub_14041B800);
    if (rob_bone_data->eyelid.mot_key_data.key_sets_ready
        && rob_bone_data->eyelid.mot_key_data.mot_data && !rob_bone_data->eyelid.disable)
        rob_bone_data->face.enabled_bones.set(sub_14041B1C0);
    else if (rob_bone_data->disable_eye_motion || rob_bone_data->eyes.mot_key_data.key_sets_ready
        && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.disable)
        rob_bone_data->face.enabled_bones.set(sub_14041B440);
}

static void sub_14041AD50(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->disable_eye_motion || rob_bone_data->eyes.mot_key_data.key_sets_ready
        && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.disable)
        rob_bone_data->eyelid.enabled_bones.set(sub_14041B440);
}

static void sub_14041ABA0(rob_chara_bone_data* a1) {
    for (motion_blend_mot*& i : a1->motion_loaded) {
        if (a1->face.mot_key_data.key_sets_ready
            && a1->face.mot_key_data.mot_data && !a1->face.disable) {
            if (!a1->disable_eye_motion || a1->eyes.mot_key_data.key_sets_ready
                && a1->eyes.mot_key_data.mot_data && !a1->eyes.disable)
                i->enabled_bones.set(sub_14041B4E0);
            else
                i->enabled_bones.set(sub_14041B580);
        }
        else {
            if (a1->mouth.mot_key_data.key_sets_ready
                && a1->mouth.mot_key_data.mot_data && !a1->mouth.disable)
                i->enabled_bones.set(sub_14041B800);
            if (a1->eyelid.mot_key_data.key_sets_ready
                && a1->eyelid.mot_key_data.mot_data && !a1->eyelid.disable) {
                if (!a1->disable_eye_motion
                    || a1->eyes.mot_key_data.key_sets_ready
                    && a1->eyes.mot_key_data.mot_data && !a1->eyes.disable)
                    i->enabled_bones.set(sub_14041B1C0);
                else
                    i->enabled_bones.set(sub_14041B300);
            }
            else if (a1->eyes.mot_key_data.key_sets_ready
                && a1->eyes.mot_key_data.mot_data && !a1->eyes.disable)
                i->enabled_bones.set(sub_14041B440);
        }

        if (a1->hand_l.mot_key_data.key_sets_ready
            && a1->hand_l.mot_key_data.mot_data && !a1->hand_l.disable)
            i->enabled_bones.set(sub_14041B6B0);
        if (a1->hand_r.mot_key_data.key_sets_ready
            && a1->hand_r.mot_key_data.mot_data && !a1->hand_r.disable)
            i->enabled_bones.set(sub_14041B750);
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
    return !a1->blend || !a1->blend->enable;
}

static void sub_140415B30(partial_motion_blend_mot* a1) {
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
            v5->enabled_bones.check();
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
    rob_bone_data->mats2.resize(rob_bone_data->node_count - rob_bone_data->object_bone_count);

    for (mat4& i : rob_bone_data->mats2)
        i = mat4_identity;

    rob_bone_data->nodes.clear();
    rob_bone_data->nodes.resize(rob_bone_data->node_count);
}

static void sub_14041AE40(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot_enabled_bones* enabled_bones = &rob_bone_data->motion_loaded.front()->enabled_bones;
    if (!enabled_bones)
        return;

    if (rob_bone_data->face.mot_key_data.key_sets_ready
        && rob_bone_data->face.mot_key_data.mot_data && !rob_bone_data->face.disable) {
        if (!rob_bone_data->disable_eye_motion
            || rob_bone_data->eyes.mot_key_data.key_sets_ready
            && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.disable)
            enabled_bones->set(sub_14041B4E0);
        else
            enabled_bones->set(sub_14041B580);
    }
    else {
        if (rob_bone_data->mouth.mot_key_data.key_sets_ready
            && rob_bone_data->mouth.mot_key_data.mot_data && !rob_bone_data->mouth.disable)
            enabled_bones->set(sub_14041B800);
        if (rob_bone_data->eyelid.mot_key_data.key_sets_ready
            && rob_bone_data->eyelid.mot_key_data.mot_data && !rob_bone_data->eyelid.disable) {
            if (!rob_bone_data->disable_eye_motion
                || rob_bone_data->eyes.mot_key_data.key_sets_ready
                && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.disable)
                enabled_bones->set(sub_14041B1C0);
            else
                enabled_bones->set(sub_14041B300);
        }
        else
            if (rob_bone_data->eyes.mot_key_data.key_sets_ready
                && rob_bone_data->eyes.mot_key_data.mot_data && !rob_bone_data->eyes.disable)
                enabled_bones->set(sub_14041B440);
    }

    if (rob_bone_data->hand_l.mot_key_data.key_sets_ready
        && rob_bone_data->hand_l.mot_key_data.mot_data && !rob_bone_data->hand_l.disable)
        enabled_bones->set(sub_14041B6B0);
    if (rob_bone_data->hand_r.mot_key_data.key_sets_ready
        && rob_bone_data->hand_r.mot_key_data.mot_data && !rob_bone_data->hand_r.disable)
        enabled_bones->set(sub_14041B750);
}

static void sub_14041BA60(rob_chara_bone_data* rob_bone_data) {
    if (rob_bone_data->motion_loaded.front())
        rob_bone_data->motion_loaded.front()->enabled_bones.check();
}

static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data, uint32_t motion_id,
    MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db) {
    if (!rob_bone_data->motion_loaded.size())
        return;

    if (blend_type == MOTION_BLEND_FREEZE) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        rob_bone_data->motion_loaded.front()->load_file(
            motion_id, MOTION_BLEND_FREEZE, 1.0f, bone_data, mot_db);
        sub_14041BA60(rob_bone_data);
        return;
    }

    if (blend_type != MOTION_BLEND_CROSS) {
        if (blend_type == MOTION_BLEND_COMBINE)
            blend_type = MOTION_BLEND;
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        rob_bone_data->motion_loaded.front()->load_file(
            motion_id, blend_type, 1.0f, bone_data, mot_db);
        return;
    }
    rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 2);

    if (rob_bone_data->motion_indices.size()) {
        rob_chara_bone_data_motion_blend_mot_list_init(rob_bone_data);
        rob_bone_data->motion_loaded.front()->copy_rot_trans();
        rob_bone_data->motion_loaded.front()->load_file(
            motion_id, MOTION_BLEND_CROSS, 1.0f, bone_data, mot_db);
        return;
    }

    motion_blend_mot* mot = new motion_blend_mot();
    mot->init(rob_bone_data, mot->enabled_bones.func, bone_data);
    if (!mot) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        rob_bone_data->motion_loaded.front()->load_file(
            motion_id, MOTION_BLEND_FREEZE, 1.0f, bone_data, mot_db);
        sub_14041BA60(rob_bone_data);
        return;
    }

    rob_bone_data->motion_loaded.push_back(mot);
    rob_bone_data->motion_loaded_indices.push_back(rob_bone_data->motions.size());
    rob_bone_data->motion_loaded.front()->copy_rot_trans();
    rob_bone_data->motion_loaded.front()->load_file(
        motion_id, MOTION_BLEND_CROSS, 1.0f, bone_data, mot_db);
}

static void rob_chara_bone_data_set_look_anim_param(rob_chara_bone_data* rob_bone_data,
    const rob_chara_look_anim_eye_param* param, const eyes_adjust* eyes_adjust) {
    rob_bone_data->look_anim.param = *param;

    float_t v1_neg;
    float_t v1_pos;
    switch (eyes_adjust->base_adjust) {
    case EYES_BASE_ADJUST_DIRECTION:
    default:
        v1_neg = param->xrot_adjust_dir_neg * (float_t)(-1.0 / 3.8);
        v1_pos = param->xrot_adjust_dir_pos * (float_t)(1.0 / 6.0);
        break;
    case EYES_BASE_ADJUST_CLEARANCE:
        v1_neg = param->xrot_adjust_clear_neg * (float_t)(-1.0 / 3.8);
        v1_pos = param->xrot_adjust_clear_pos * (float_t)(1.0 / 6.0);
        break;
    case EYES_BASE_ADJUST_OFF:
        v1_neg = 1.0f;
        v1_pos = 1.0f;
        break;
    }

    float_t v2_pos = 1.0f;
    float_t v2_neg = 1.0f;
    if (eyes_adjust->xrot_adjust) {
        v2_neg = param->xrot_adjust_neg;
        v2_pos = param->xrot_adjust_pos;
    }

    if (eyes_adjust->neg >= 0.0f && eyes_adjust->pos >= 0.0f) {
        v2_neg = eyes_adjust->neg;
        v2_pos = eyes_adjust->pos;
    }

    float_t eyes_xrot_adjust_neg = v1_neg * v2_neg;
    float_t eyes_xrot_adjust_pos = v1_pos * v2_pos;
    for (motion_blend_mot*& i : rob_bone_data->motions) {
        RobBlock* data = i->bone_data.bones.data();
        data[BLK_KL_EYE_L].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        data[BLK_KL_EYE_L].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
        data[BLK_KL_EYE_R].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        data[BLK_KL_EYE_R].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
    }

    rob_bone_data->look_anim.set_eyes_xrot_adjust(eyes_xrot_adjust_neg, eyes_xrot_adjust_pos);
}

static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
    const std::vector<BODYTYPE>* bones, const std::string* motion_bones) {
    size_t node = 0;
    size_t matrix = 0;
    size_t matrix2 = 0;
    size_t leaf_pos = 0;
    size_t chain_pos = 0;

    RobNode* nodes = rob_bone_data->nodes.data();
    for (const BODYTYPE& i : *bones) {
        chain_pos++;

        switch (i.ik_type) {
        case IKT_0:
        case IKT_0N:
        case IKT_0T:
            nodes->mat_ptr = &rob_bone_data->mats[matrix++];
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;
            break;
        case IKT_ROOT:
            nodes->mat_ptr = &rob_bone_data->mats[matrix++];
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            leaf_pos++;
            break;
        case IKT_1:
            nodes->mat_ptr = &rob_bone_data->mats2[matrix2++];
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat_ptr = &rob_bone_data->mats[matrix++];
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat_ptr = &rob_bone_data->mats2[matrix2++];
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            chain_pos++;
            leaf_pos++;
            node += 2;
            break;
        case IKT_2:
        case IKT_2R:
            nodes->mat_ptr = &rob_bone_data->mats[matrix++];
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat_ptr = &rob_bone_data->mats[matrix++];
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat_ptr = &rob_bone_data->mats[matrix++];
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            nodes->mat_ptr = &rob_bone_data->mats2[matrix2++];
            nodes->name = motion_bones->c_str();
            nodes++;
            motion_bones++;

            chain_pos += 2;
            leaf_pos++;
            node += 3;
            break;
        }
        node++;
    }

    for (RobNode& i : rob_bone_data->nodes)
        i.no_scale_mat = i.mat_ptr;

    if (node != rob_bone_data->node_count)
        printf_debug_error("Node mismatch");
    if (matrix != rob_bone_data->object_bone_count)
        printf_debug_error("Matrix mismatch");
    if (matrix2 != rob_bone_data->node_count - rob_bone_data->object_bone_count)
        printf_debug_error("Matrix2 mismatch");
    if (leaf_pos != rob_bone_data->leaf_pos)
        printf_debug_error("LeafPos mismatch");
    if (chain_pos != rob_bone_data->chain_pos)
        printf_debug_error("ChainPos mismatch");
    rob_bone_data->field_1 = true;
}

static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    const uint16_t* parent_indices) {
    if (rob_bone_data->nodes.size() < 1)
        return;

    parent_indices++;
    RobNode* i_begin = rob_bone_data->nodes.data();
    RobNode* i_begin_1 = rob_bone_data->nodes.data() + 1;
    RobNode* i_end = rob_bone_data->nodes.data() + rob_bone_data->nodes.size();
    for (RobNode* i = i_begin_1; i != i_end; i++)
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
    for (RobBlock& i : a1->bone_data.bones)
        i.get_mat(skeleton_select);
}

static void sub_14040FBF0(motion_blend_mot* a1, float_t a2) {
    RobBlock* b_n_hara_cp = &a1->bone_data.bones.data()[BLK_N_HARA_CP];
    RobBlock* b_kg_hara_y = &a1->bone_data.bones.data()[BLK_KG_HARA_Y];
    RobBlock* b_kl_hara_xz = &a1->bone_data.bones.data()[BLK_KL_HARA_XZ];
    RobBlock* b_kl_hara_etc = &a1->bone_data.bones.data()[BLK_KL_HARA_ETC];
    a1->field_4F8.field_90 = 0.0f;

    mat4 chain_rot = b_n_hara_cp->chain_rot[0];
    mat4 mat = b_kg_hara_y->chain_rot[0];
    mat4_mul(&mat, &chain_rot, &chain_rot);
    mat = b_kl_hara_xz->chain_rot[0];
    mat4_mul(&mat, &chain_rot, &chain_rot);
    mat = b_kl_hara_etc->chain_rot[0];
    mat4_mul(&mat, &chain_rot, &chain_rot);
    b_n_hara_cp->chain_rot[0] = chain_rot;
    b_kg_hara_y->chain_rot[0] = mat4_identity;
    b_kl_hara_xz->chain_rot[0] = mat4_identity;
    b_kl_hara_etc->chain_rot[0] = mat4_identity;

    float_t v8 = a1->field_4F8.field_C0;
    float_t v9 = a1->field_4F8.field_C4;
    vec3 v10 = a1->field_4F8.field_C8;
    a1->field_4F8.field_A8 = b_n_hara_cp->leaf_pos[0];
    if (!a1->mot_key_data.skeleton_select) {
        if (a2 != v9) {
            b_n_hara_cp->leaf_pos[0].x = (b_n_hara_cp->leaf_pos[0].x - v10.x) * v9 + v10.x;
            b_n_hara_cp->leaf_pos[0].z = (b_n_hara_cp->leaf_pos[0].z - v10.z) * v9 + v10.z;
        }
        b_n_hara_cp->leaf_pos[0].y = ((b_n_hara_cp->leaf_pos[0].y - v10.y) * v8) + v10.y;
    }
    else {
        if (a2 != v9) {
            v9 /= a2;
            b_n_hara_cp->leaf_pos[0].x = (b_n_hara_cp->leaf_pos[0].x - v10.x) * v9 + v10.x;
            b_n_hara_cp->leaf_pos[0].z = (b_n_hara_cp->leaf_pos[0].z - v10.z) * v9 + v10.z;
        }

        if (a2 != v8) {
            v8 /= a2;
            b_n_hara_cp->leaf_pos[0].y = (b_n_hara_cp->leaf_pos[0].y - v10.y) * v8 + v10.y;
        }
    }
}

static void sub_140410A40(motion_blend_mot* a1, prj::sys_vector<RobBlock>* a2, prj::sys_vector<RobBlock>* a3) {
    MotionBlend* v4 = a1->blend;
    if (!v4 || !v4->enable)
        return;

    v4->Field_20(a2, a3);

    for (RobBlock& i : *a2) {
        if (!a1->enabled_bones.arr[i.block_id])
            continue;

        RobBlock* v8 = 0;
        if (a3)
            v8 = &a3->data()[i.block_id];
        v4->Blend(&a2->data()[i.block_id], v8);
    }
}

static void sub_140410B70(motion_blend_mot* a1, prj::sys_vector<RobBlock>* a2) {
    MotionBlend* v3 = a1->blend;
    if (!v3 || !v3->enable)
        return;

    v3->Field_20(&a1->bone_data.bones, a2);

    for (RobBlock& i : a1->bone_data.bones) {
        if (!a1->enabled_bones.arr[i.block_id])
            continue;

        RobBlock* v6 = 0;
        if (a2)
            v6 = &a2->data()[i.block_id];
        v3->Blend(&a1->bone_data.bones.data()[i.block_id], v6);
    }
}

static void sub_140410CB0(partial_motion_blend_mot* a1, prj::sys_vector<RobBlock>* a2) {
    if (!a1->blend.enable)
        return;

    for (RobBlock& i : *a2)
        if (a1->enabled_bones.arr[i.block_id])
            a1->blend.Blend(&a2->data()[i.block_id], 0);
}

static void sub_1404182B0(rob_chara_bone_data* rob_bone_data) {
    if (!rob_bone_data->motion_loaded.size())
        return;

    motion_blend_mot* v3 = rob_bone_data->motion_loaded.front();

    auto i_begin = rob_bone_data->motion_loaded.rbegin();
    auto i_end = rob_bone_data->motion_loaded.rend();
    for (auto i = i_begin; i != i_end; i++) {
        if (!sub_1404136B0(*i))
            continue;

        auto v4 = i;
        v4++;
        sub_140410A40(*i, &(*v4)->bone_data.bones, &(*i)->bone_data.bones);
    }

    auto j_begin = rob_bone_data->motion_loaded.rbegin();
    auto j_end = rob_bone_data->motion_loaded.rend();
    for (auto j = j_begin; j != j_end; j++) {
        if (sub_1404136B0(*j))
            continue;

        prj::sys_vector<RobBlock>* bones = 0;
        if (j != rob_bone_data->motion_loaded.rbegin()) {
            auto v5 = j;
            v5--;
            bones = &(*v5)->bone_data.bones;
        }
        else if ((*j)->get_type() != MOTION_BLEND_FREEZE)
            continue;

        sub_140410B70(*j, bones);
    }

    sub_140410CB0(&rob_bone_data->face, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->hand_l, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->hand_r, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->mouth, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->eyes, &v3->bone_data.bones);
    sub_140410CB0(&rob_bone_data->eyelid, &v3->bone_data.bones);

    RobBlock* v7 = &v3->bone_data.bones.data()[0];
    v3->field_4F8.field_9C = v7->leaf_pos[0];
    if (sub_140413790(&v3->field_4F8)) { // WTF???
        v3->field_4F8.field_90 = v7->leaf_pos[0];
        v7->leaf_pos[0] -= v7->leaf_pos[0];
    }
}

void sub_14041B9F0(rob_chara_bone_data* rob_bone_data) {
    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        i->enabled_bones.check();

    rob_bone_data->face.enabled_bones.check();
    rob_bone_data->eyelid.enabled_bones.check();
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
    a1->kamae_type = 0;
    a1->kamae_data = 0;
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

static float_t sub_140512F60(rob_chara_item_equip* rob_disp) {
    const mat4* motion_matrix = rob_disp->motion_matrix;
    float_t y = 1.0f;
    y = min_def(y, motion_matrix[BONE_ID_N_HARA_CP     ].row3.y);
    y = min_def(y, motion_matrix[BONE_ID_KL_ASI_L_WJ_CO].row3.y);
    y = min_def(y, motion_matrix[BONE_ID_KL_ASI_R_WJ_CO].row3.y);
    y = min_def(y, motion_matrix[BONE_ID_KL_TE_L_WJ    ].row3.y);
    y = min_def(y, motion_matrix[BONE_ID_KL_TE_R_WJ    ].row3.y);
    return y;
}

static void rob_chara_add_motion_reset_data(rob_chara* rob_chr,
    uint32_t motion_id, float_t frame, int32_t iterations) {
    if (motion_id != -1)
        rob_chr->rob_disp->add_motion_reset_data(motion_id, frame, iterations);
}

static void rob_chara_age_age_ctrl(rob_chara* rob_chr, int32_t part_id, const char* name) {
    mat4 mat = *rob_chr->rob_disp->get_ex_data_bone_node_mat(RPK_KAMI, name);
    if (vec3::length(rob_chr->data.adjust_data.pos_adjust) > 0.000001f) {
        mat4 temp;
        mat4_translate(&rob_chr->data.adjust_data.pos_adjust, &temp);
        mat4_mul(&mat, &temp, &mat);
    }
    rob_chara_age_age_array_ctrl(rob_chr->chara_id, part_id, mat);
}

static object_info rob_chara_get_head_object(rob_chara* rob_chr, int32_t head_object_id) {
    if (head_object_id < 0 || head_object_id > 8)
        return {};
    object_info obj_info = rob_chr->item_cos_data.get_head_object_replace(head_object_id);
    if (obj_info.not_null())
        return obj_info;
    return rob_chr->rob_data->head_objects[head_object_id];
}

static object_info rob_chara_get_object_info(rob_chara* rob_chr, ROB_PARTS_KIND rpk) {
    return rob_chr->rob_disp->get_object_info(rpk);
}

static void rob_chara_load_default_motion(rob_chara* rob_chr,
    const bone_database* bone_data, const motion_database* mot_db) {
    uint32_t motion_id = rob_chr->get_rob_cmn_mottbl_motion_id(0);
    rob_chara_load_default_motion_sub(rob_chr, 1, motion_id, bone_data, mot_db);
}

static void sub_140419820(rob_chara_bone_data* rob_bone_data, int32_t skeleton_select) {
    rob_bone_data->motion_loaded.front()->store_curr_rot_trans(skeleton_select);
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
    rob_chr->bone_data->set_motion_blend_duration(0.0f, 1.0f, 1.0f);
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
        && left->ex_force.x == right->ex_force.x
        && left->ex_force.y == right->ex_force.y
        && left->ex_force.z == right->ex_force.z
        && left->ex_force_cycle_strength.x == right->ex_force_cycle_strength.x
        && left->ex_force_cycle_strength.y == right->ex_force_cycle_strength.y
        && left->ex_force_cycle_strength.z == right->ex_force_cycle_strength.z
        && left->ex_force_cycle.x == right->ex_force_cycle.x
        && left->ex_force_cycle.y == right->ex_force_cycle.y
        && left->ex_force_cycle.z == right->ex_force_cycle.z
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
    CHARA_NUM chara_num, int32_t cos_id, const rob_chara_pv_data& pv_data) {
    rob_chr->chara_id = chara_id;
    rob_chr->chara_num = chara_num;
    rob_chr->cos_id = cos_id;
    rob_chr->pv_data = pv_data;
    rob_chr->rob_data = get_rob_data(chara_num);
    if (!check_cos_id_is_501(cos_id)) {
        const item_cos_data* cos = item_table_handler_array_get_item_cos_data(chara_num, cos_id);
        if (cos)
            rob_chr->item_cos_data.set_chara_num_item_nos(chara_num, cos->arr);
    }
    rob_chr->item_cos_data.set_item_array(pv_data.item);
}

static void rob_cmn_mottbl_read(void* a1, const void* data, size_t size) {
    free_def(common_mot_tbl_header);

    farc f;
    f.read(data, size, true);
    if (f.files.size()) {
        farc_file* ff = f.read_file("rob_cmn_mottbl.bin");
        if (ff && ff->data) {
            common_mot_tbl_header = (CmnMotTblHeader*)ff->data;
            ff->data = 0;
        }
    }
}

static void opd_chara_data_array_add_frame_data(int32_t chara_id) {
    opd_chara_data_array_get(chara_id)->add_frame_data();
}

static void opd_chara_data_array_encode_data(int32_t chara_id) {
    opd_chara_data_array_get(chara_id)->encode_data();
}

static void opd_chara_data_array_encode_init_data(int32_t chara_id, uint32_t motion_id) {
    opd_chara_data_array_get(chara_id)->encode_init_data(motion_id);
}

static void opd_chara_data_array_fs_copy_file(int32_t chara_id) {
    opd_chara_data_array_get(chara_id)->fs_copy_file();
}

static opd_chara_data* opd_chara_data_array_get(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_ID_MAX)
        chara_id = 0;
    return &opd_chara_data_array[chara_id];
}

static void opd_chara_data_array_init_data(int32_t chara_id, uint32_t motion_id) {
    opd_chara_data_array_get(chara_id)->init_data(motion_id);
}

static void opd_chara_data_array_open_opd_file(int32_t chara_id) {
    opd_chara_data_array_get(chara_id)->open_opd_file();
}

static void opd_chara_data_array_open_opdi_file(int32_t chara_id) {
    opd_chara_data_array_get(chara_id)->open_opdi_file();
}

static void opd_chara_data_array_write_file(int32_t chara_id) {
    opd_chara_data_array_get(chara_id)->write_file();
}

static rob_manager_rob_impl* rob_manager_rob_impls1_get(TaskRobManager* rob_mgr) {
    if (!rob_manager_rob_impls1_init) {
        rob_manager_rob_impls1_init = true;
        rob_manager_rob_impls1[0].task = &rob_mgr->rob_prepare_control;
        rob_manager_rob_impls1[0].name = "ROB_PREPARE_CONTROL";
        rob_manager_rob_impls1[1].task = 0;
        rob_manager_rob_impls1[1].name = "";
    }
    return rob_manager_rob_impls1;
}

static rob_manager_rob_impl* rob_manager_rob_impls2_get(TaskRobManager* rob_mgr) {
    if (!rob_manager_rob_impls2_init) {
        rob_manager_rob_impls2_init = true;
        rob_manager_rob_impls2[0].task = &rob_mgr->rob_prepare_action;
        rob_manager_rob_impls2[0].name = "ROB_PREPARE_ACTION";
        rob_manager_rob_impls2[1].task = &rob_mgr->rob_base;
        rob_manager_rob_impls2[1].name = "ROB_BASE";
        rob_manager_rob_impls2[2].task = &rob_mgr->rob_motion_modifier;
        rob_manager_rob_impls2[2].name = "ROB_MOTION_MODIFIER";
        rob_manager_rob_impls2[3].task = &rob_mgr->rob_collision;
        rob_manager_rob_impls2[3].name = "ROB_COLLISION";
        rob_manager_rob_impls2[4].task = &rob_mgr->rob_info;
        rob_manager_rob_impls2[4].name = "ROB_INFO";
        rob_manager_rob_impls2[5].task = &rob_mgr->rob_disp;
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
    mdl::ObjFlags obj_flags = rctx->disp_manager->get_obj_flags();
    rob_chara_age_age_disp(rob_chara_age_age_array, rctx, chara_id, reflect, chara_color);
    rctx->disp_manager->set_obj_flags(obj_flags);
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
    int32_t npr, int32_t speed, int32_t skip, int32_t disp) {
    if (!npr || npr == 1)
        rob_chara_age_age_array_set_npr(chara_id, part_id, npr);
    if (speed != -1)
        rob_chara_age_age_array_set_speed(chara_id, part_id, (float_t)speed * 0.001f);
    if (skip != -1)
        rob_chara_age_age_array_set_skip(chara_id, part_id);
    if (!disp || disp == 1)
        rob_chara_age_age_array_set_disp(chara_id, part_id, !!disp);
}

void rob_chara_age_age_array_set_speed(int32_t chara_id, int32_t part_id, float_t value) {
    rob_chara_age_age_set_speed(rob_chara_age_age_array, chara_id, part_id, value);
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
    if (chara_id < 0 || chara_id >= ROB_ID_MAX)
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

rob_chara_item_equip* rob_chara_array_get_rob_disp(int32_t chara_id) {
    return rob_chara_array[chara_id].rob_disp;
}

int32_t rob_chara_array_init_chara_num(CHARA_NUM chara_num,
    const rob_chara_pv_data& pv_data, int32_t cos_id, bool can_set_default) {
    if (!app::TaskWork::check_task_ready(task_rob_manager)
        || pv_data.type < ROB_CHARA_TYPE_0|| pv_data.type > ROB_CHARA_TYPE_3)
        return -1;

    int32_t chara_id = 0;
    while (rob_chara_pv_data_array[chara_id].type != ROB_CHARA_TYPE_NONE) {
        chara_id++;
        if (chara_id >= ROB_ID_MAX)
            return -1;
    }

    if (can_set_default && (cos_id < 0 || cos_id > 498))
        cos_id = 0;
    rob_chara_pv_data_array[chara_id] = pv_data;
    rob_chara_set_pv_data(&rob_chara_array[chara_id], chara_id, chara_num, cos_id, pv_data);
    task_rob_manager->AppendInitCharaList(&rob_chara_array[chara_id]);
    return chara_id;
}

void rob_chara_array_free_chara_id(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_ID_MAX)
        return;

    task_rob_manager->AppendFreeCharaList(&rob_chara_array[chara_id]);
    rob_chara_pv_data_array[chara_id].type = ROB_CHARA_TYPE_NONE;
}

void rob_chara_array_reset_pv_data(int32_t chara_id) {
    rob_chara_pv_data_array[chara_id].type = ROB_CHARA_TYPE_NONE;
}

void rob_chara_array_reset_bone_data_item_equip(int32_t chara_id) {
    rob_chara_item_equip* rob_disp = rob_chara_array[chara_id].rob_disp;
    rob_disp->reset_init_data(rob_chara_bone_data_get_node(
        rob_chara_array[chara_id].bone_data, BONE_NODE_N_HARA_CP));
    rob_disp->set_shadow_group(chara_id);
    rob_disp->shadow_flag = 0x05;
}

void rob_chara_array_set_alpha_obj_flags(int32_t chara_id, float_t alpha, mdl::ObjFlags flags) {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chr->rob_disp->set_alpha_obj_flags(alpha, flags);

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
    for (int32_t i = 0; i < ROB_ID_MAX; i++)
        disp |= pv_osage_manager_array_get(i)->GetDisplay();
    return disp;
}

bool pv_osage_manager_array_get_disp(int32_t chara_id) {
    return pv_osage_manager_array_get(chara_id)->GetDisplay();
}

void pv_osage_manager_array_set_motion_ids(
    int32_t chara_id, const std::vector<uint32_t>& motion_ids) {
    std::vector<pv_data_set_motion> set_motion;
    for (const uint32_t& i : motion_ids)
        set_motion.push_back({ i });
    pv_osage_manager_array_get(chara_id)->SetPvSetMotion(set_motion);
}

void pv_osage_manager_array_set_not_reset_true() {
    for (int32_t i = 0; i < ROB_ID_MAX; i++)
        pv_osage_manager_array_get(i)->SetNotReset(true);
}

void pv_osage_manager_array_set_pv_id(int32_t chara_id, int32_t pv_id, bool reset) {
    pv_osage_manager_array_get(chara_id)->SetPvId(pv_id, chara_id, reset);
}

void pv_osage_manager_array_set_pv_set_motion(
    int32_t chara_id, const std::vector<pv_data_set_motion>& set_motion) {
    pv_osage_manager_array_get(chara_id)->SetPvSetMotion(set_motion);
}

bool rob_chara_check_for_ageageagain_module(CHARA_NUM chara_num, int32_t cos_id) {
    return chara_num == CN_MIKU && cos_id == 148;
}

bool rob_chara_pv_data_array_check_chara_id(int32_t chara_id) {
    return rob_chara_pv_data_array[chara_id].type != ROB_CHARA_TYPE_NONE;
}

void rob_sleeve_handler_data_get_sleeve_data(
    CHARA_NUM chara_num, int32_t cos, rob_sleeve_data& l, rob_sleeve_data& r) {
    rob_sleeve_handler_data->get_sleeve_data(chara_num, cos, l, r);
}

bool rob_sleeve_handler_data_load() {
    return rob_sleeve_handler_data->load();
}

void rob_sleeve_handler_data_read() {
    rob_sleeve_handler_data->read();
}

bool task_rob_load_add_task() {
    return app::TaskWork::add_task(task_rob_load, 0, "ROB LOAD", 0);
}

bool task_rob_load_append_free_req_data(CHARA_NUM chara_num) {
    return task_rob_load->AppendFreeReqData(chara_num);
}

bool task_rob_load_append_free_req_data_obj(CHARA_NUM chara_num, const item_cos_data* cos) {
    return task_rob_load->AppendFreeReqDataObj(chara_num, cos);
}

bool task_rob_load_append_load_req_data(CHARA_NUM chara_num) {
    return task_rob_load->AppendLoadReqData(chara_num);
}

bool task_rob_load_append_load_req_data_obj(CHARA_NUM chara_num, const item_cos_data* cos) {
    return task_rob_load->AppendLoadReqDataObj(chara_num, cos);
}

bool task_rob_load_check_load_req_data() {
    if (task_rob_load->field_F0 == 2 && !task_rob_load->load_req_data_obj.size())
        return task_rob_load->load_req_data.size() != 0;
    return true;
}

bool task_rob_load_del_task() {
    task_rob_load->del();
    return true;
}

bool task_rob_manager_add_task() {
    return app::TaskWork::add_task(task_rob_manager, "ROB_MANAGER TASK");
}

bool task_rob_manager_check_chara_loaded(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_ID_MAX)
        return false;
    return task_rob_manager->CheckCharaLoaded(&rob_chara_array[chara_id]);
}

bool task_rob_manager_check_task_ready() {
    return app::TaskWork::check_task_ready(task_rob_manager);
}

void task_rob_manager_free_all_chara() {
    for (int32_t i = 0; i < ROB_ID_MAX; i++)
        if (rob_chara_array[i].type != ROB_CHARA_TYPE_NONE) {
            task_rob_manager->AppendFreeCharaList(&rob_chara_array[i]);
            rob_chara_array[i].type = ROB_CHARA_TYPE_NONE;
        }
}

bool task_rob_manager_get_free_chara_list_empty() {
    return task_rob_manager->GetFreeCharaListEmpty();
}

bool task_rob_manager_get_wait(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_ID_MAX)
        return false;

    return task_rob_manager->GetWait(&rob_chara_array[chara_id]);
}

bool task_rob_manager_hide_task() {
    return task_rob_manager->hide();
}

bool task_rob_manager_run_task() {
    return task_rob_manager->run();
}

bool task_rob_manager_del_task() {
    if (!app::TaskWork::check_task_ready(task_rob_manager))
        return true;

    task_rob_manager->del();
    return false;
}

RobTransform::RobTransform() {
    pos = 0.0f;
    rot = 0.0f;
    scale = 1.0f;
    hsc = 1.0f;
}

void RobTransform::CalcMatrixHS(const vec3& hsc, mat4& mat, mat4& dsp_mat) {
    vec3 pos = this->pos * hsc;
    mat4_mul_translate(&mat, &pos, &mat);
    mat4_mul_rotate_zyx(&mat, &rot, &mat);
    this->hsc = scale * hsc;
    dsp_mat = mat;
    mat4_scale_rot(&dsp_mat, &this->hsc, &dsp_mat);
}

void RobTransform::init(float_t px, float_t py, float_t pz,
    float_t rx, float_t ry, float_t rz) {
    pos.x = px;
    pos.y = py;
    pos.z = pz;
    rot.x = rx;
    rot.z = ry;
    rot.y = rz;
    scale = 1.0f;
    hsc = 1.0f;
}

void RobTransform::init(const vec3& p, const vec3& r) {
    pos = p;
    rot = r;
    scale = 1.0f;
    hsc = 1.0f;
}

void RobTransform::reset() {
    pos = 0.0f;
    rot = 0.0f;
    scale = 1.0f;
    hsc = 1.0f;
}

void RobTransform::reset_scale() {
    scale = 1.0f;
    hsc = 1.0f;
}

RobNode::RobNode() : name(), mat_ptr(), parent(), no_scale_mat() {

}

float_t* RobNode::get_transform_component(size_t index, Expr_type& type) {
    switch (index) {
    case 0:
        type = Expr_variable;
        return &transform.pos.x;
    case 1:
        type = Expr_variable;
        return &transform.pos.y;
    case 2:
        type = Expr_variable;
        return &transform.pos.z;
    case 3:
        type = Expr_variable_rad;
        return &transform.rot.x;
    case 4:
        type = Expr_variable_rad;
        return &transform.rot.y;
    case 5:
        type = Expr_variable_rad;
        return &transform.rot.z;
    case 6:
        type = Expr_variable;
        return &transform.scale.x;
    case 7:
        type = Expr_variable;
        return &transform.scale.y;
    case 8:
        type = Expr_variable;
        return &transform.scale.z;
    default:
        return 0;
    }
}

const float_t* RobNode::get_transform_component(size_t index, Expr_type& type) const {
    switch (index) {
    case 0:
        type = Expr_variable;
        return &transform.pos.x;
    case 1:
        type = Expr_variable;
        return &transform.pos.y;
    case 2:
        type = Expr_variable;
        return &transform.pos.z;
    case 3:
        type = Expr_variable_rad;
        return &transform.rot.x;
    case 4:
        type = Expr_variable_rad;
        return &transform.rot.y;
    case 5:
        type = Expr_variable_rad;
        return &transform.rot.z;
    case 6:
        type = Expr_variable;
        return &transform.scale.x;
    case 7:
        type = Expr_variable;
        return &transform.scale.y;
    case 8:
        type = Expr_variable;
        return &transform.scale.z;
    default:
        return 0;
    }
}

void RobNode::set_name_mat_no_scale_mat(const char* name, mat4* mat, mat4* no_scale_mat) {
    this->name = name;

    this->mat_ptr = mat;
    if (mat)
        *mat = mat4_identity;

    parent = 0;
    transform.reset();

    this->no_scale_mat = no_scale_mat;
    if (no_scale_mat)
        *no_scale_mat = mat4_identity;
}

mot_key_data::mot_key_data() : key_sets_ready(), key_set_count(), key_set(),
mot(), fc_value(), mot_data(), skeleton_type(), skeleton_select(), field_68() {
    motion_id = -1;
    frame = -1.0f;
}

mot_key_data::~mot_key_data() {

}

void mot_key_data::interpolate(float_t frame, uint32_t key_set_offset, uint32_t key_set_count) {
    mot_interpolate(&mot, frame, &fc_value.data()[key_set_offset],
        &key_set.data()[key_set_offset], key_set_count, &field_68);
}

void mot_key_data::reset() {
    key_sets_ready = 0;
    skeleton_type = BONE_KIND_NONE;
    frame = -1.0f;
    key_set_count = 0;
    mot_data = 0;
    skeleton_select = 0;
    field_68 = {};

    key_set.clear();
    fc_value.clear();
    motion_id = -1;
}

eyes_adjust::eyes_adjust() : xrot_adjust(), base_adjust() {
    neg = -1.0f;
    pos = -1.0f;
}

RobBlock::RobBlock() : ik_type(), inherit_type(), block_id(), flip_block_id(),
inherit_mat_id(), expression_id(), key_set_offset(), key_set_count(), frame(),
up_vector_mat_ptr(), inherit_mat_ptr(), node(), len(), arm_length() {
    eyes_xrot_adjust_neg = 1.0f;
    eyes_xrot_adjust_pos = 1.0f;
}

RobBlock::~RobBlock() {

}

void RobBlock::copy_rot_trans(const RobBlock& other) {
    if (check_expression_id_not_null())
        return;

    switch (ik_type) {
    case IKT_0N:
    case IKT_0T:
    case IKT_ROOT:
        leaf_pos[0] = other.leaf_pos[0];
        smooth_pos[0] = other.smooth_pos[0];
        break;
    }

    switch (ik_type) {
    case IKT_0:
    case IKT_0N:
    case IKT_0T:
    case IKT_ROOT:
        chain_rot[0] = other.chain_rot[0];
        smooth_rot[0][0] = other.smooth_rot[0][0];
        break;
    case IKT_1:
        chain_rot[0] = other.chain_rot[0];
        chain_rot[1] = other.chain_rot[1];
        smooth_rot[0][0] = other.smooth_rot[0][0];
        smooth_rot[1][0] = other.smooth_rot[1][0];
        break;
    case IKT_2:
    case IKT_2R:
        chain_rot[0] = other.chain_rot[0];
        chain_rot[1] = other.chain_rot[1];
        chain_rot[2] = other.chain_rot[2];
        smooth_rot[0][0] = other.smooth_rot[0][0];
        smooth_rot[1][0] = other.smooth_rot[1][0];
        smooth_rot[2][0] = other.smooth_rot[2][0];
        break;
    }
}

bool RobBlock::check_expression_id_not_null() {
    return !!expression_id;
}

bool RobBlock::get_constraint_ik(const RobBlock* bones) {
    vec3 target;

    switch (block_id) {
    case BLK_N_SKATA_L_WJ_CD_EX:
        mat4_get_translation(bones[BLK_C_KATA_L].node[2].mat_ptr, &target);
        orient_x(target);
        break;
    case BLK_N_SKATA_R_WJ_CD_EX:
        mat4_get_translation(bones[BLK_C_KATA_R].node[2].mat_ptr, &target);
        orient_x(target);
        break;
    case BLK_N_SKATA_B_L_WJ_CD_CU_EX:
        mat4_get_translation(bones[BLK_N_UP_KATA_L_EX].node[0].mat_ptr, &target);
        orient_x_cns(target, 0.333f);
        break;
    case BLK_N_SKATA_B_R_WJ_CD_CU_EX:
        mat4_get_translation(bones[BLK_N_UP_KATA_R_EX].node[0].mat_ptr, &target);
        orient_x_cns(target, 0.333f);
        break;
    case BLK_N_SKATA_C_L_WJ_CD_CU_EX:
        mat4_get_translation(bones[BLK_N_UP_KATA_L_EX].node[0].mat_ptr, &target);
        orient_x_cns(target, 0.5f);
        break;
    case BLK_N_SKATA_C_R_WJ_CD_CU_EX:
        mat4_get_translation(bones[BLK_N_UP_KATA_R_EX].node[0].mat_ptr, &target);
        orient_x_cns(target, 0.5f);
        break;
    case BLK_N_MOMO_A_L_WJ_CD_EX:
        mat4_get_translation(bones[BLK_CL_MOMO_L].node[2].mat_ptr, &target);
        mat4_inverse_transform_point(node[0].mat_ptr, &target, &target);
        orient_y(-target);
        break;
    case BLK_N_MOMO_A_R_WJ_CD_EX:
        mat4_get_translation(bones[BLK_CL_MOMO_R].node[2].mat_ptr, &target);
        mat4_inverse_transform_point(node[0].mat_ptr, &target, &target);
        orient_y(-target);
        break;
    case BLK_N_HARA_CD_EX:
        mat4_get_translation(bones[BLK_KL_MUNE_B_WJ].node[0].mat_ptr, &target);
        mat4_inverse_transform_point(node[0].mat_ptr, &target, &target);
        orient_y(target);
        break;
    default:
        return false;
    }
    return true;
}

bool RobBlock::get_ex_rotation(RobTransform& transform, const RobBlock* bones) {
    RobNode* node;

    switch (block_id) {
    case BLK_N_EYE_L_WJ_EX:
        transform.rot.x = -bones[BLK_KL_EYE_L].node[0].transform.rot.x;
        transform.rot.y = bones[BLK_KL_EYE_L].node[0].transform.rot.y * (float_t)(-1.0 / 2.0);
        break;
    case BLK_N_EYE_R_WJ_EX:
        transform.rot.x = -bones[BLK_KL_EYE_R].node[0].transform.rot.x;
        transform.rot.y = bones[BLK_KL_EYE_R].node[0].transform.rot.y * (float_t)(-1.0 / 2.0);
        break;
    case BLK_N_KUBI_WJ_EX: {
        mat4 mat = bones[BLK_CL_KAO].chain_rot[0];
        mat4_mul(&bones[BLK_CL_KAO].chain_rot[1], &mat, &mat);

        vec3 rotation;
        mat4_get_rotation_zyx(&mat, &rotation);

        float_t rot = rotation.z;
        if (rot < (float_t)-M_PI_2)
            rot += (float_t)(M_PI * 2.0);
        transform.rot.y = (rot - (float_t)M_PI_2) * 0.2f;
    } break;
    case BLK_N_HITO_L_EX:
    case BLK_N_HITO_R_EX:
        transform.rot.z = (float_t)(-9.0 * DEG_TO_RAD);
        break;
    case BLK_N_KO_L_EX:
        transform.rot.x = (float_t)(-8.0 * DEG_TO_RAD);
        transform.rot.z = (float_t)(16.0 * DEG_TO_RAD);
        break;
    case BLK_N_KUSU_L_EX:
    case BLK_N_KUSU_R_EX:
        transform.rot.z = (float_t)(9.0 * DEG_TO_RAD);
        break;
    case BLK_N_NAKA_L_EX:
    case BLK_N_NAKA_R_EX:
        transform.rot.z = (float_t)(-1.0 * DEG_TO_RAD);
        break;
    case BLK_N_OYA_L_EX:
        transform.rot.x = (float_t)(75.0 * DEG_TO_RAD);
        transform.rot.y = (float_t)(12.0 * DEG_TO_RAD);
        transform.rot.z = (float_t)(-24.0 * DEG_TO_RAD);
        break;
    case BLK_N_STE_L_WJ_EX:
        transform.rot.x = bones[BLK_KL_TE_L_WJ].node[0].transform.rot.x;
        break;
    case BLK_N_SUDE_L_WJ_EX:
    case BLK_N_SUDE_B_L_WJ_EX:
        node = &bones[BLK_KL_TE_L_WJ].node[0];
        transform.rot.x = RobBlock::limit_angle(node->transform.rot.x) * (float_t)(1.0 / 3.0);
        break;
    case BLK_N_SUDE_R_WJ_EX:
    case BLK_N_SUDE_B_R_WJ_EX:
        node = &bones[BLK_KL_TE_R_WJ].node[0];
        transform.rot.x = RobBlock::limit_angle(node->transform.rot.x) * (float_t)(1.0 / 3.0);
        break;
    case BLK_N_HIJI_L_WJ_EX:
        transform.rot.z = bones[BLK_C_KATA_L].node[2].transform.rot.z * (float_t)(1.0 / 2.0);
        break;
    case BLK_N_UP_KATA_L_EX:
    case BLK_N_UP_KATA_R_EX:
        break;
    case BLK_N_KO_R_EX:
        transform.rot.x = (float_t)(8.0 * DEG_TO_RAD);
        transform.rot.z = (float_t)(16.0 * DEG_TO_RAD);
        break;
    case BLK_N_OYA_R_EX:
        transform.rot.x = (float_t)(-75.0 * DEG_TO_RAD);
        transform.rot.y = (float_t)(-12.0 * DEG_TO_RAD);
        transform.rot.z = (float_t)(-24.0 * DEG_TO_RAD);
        break;
    case BLK_N_STE_R_WJ_EX:
        transform.rot.x = bones[BLK_KL_TE_R_WJ].node[0].transform.rot.x;
        break;
    case BLK_N_HIJI_R_WJ_EX:
        transform.rot.z = bones[BLK_C_KATA_R].node[2].transform.rot.z * (float_t)(1.0 / 2.0);
        break;
    case BLK_N_HIZA_L_WJ_EX:
        transform.rot.z = bones[BLK_CL_MOMO_L].node[2].transform.rot.z * (float_t)(1.0 / 2.0);
        break;
    case BLK_N_HIZA_R_WJ_EX:
        transform.rot.z = bones[BLK_CL_MOMO_R].node[2].transform.rot.z * (float_t)(1.0 / 2.0);
        break;
    case BLK_N_MOMO_B_L_WJ_EX:
    case BLK_N_MOMO_C_L_WJ_EX:
        node = &bones[BLK_CL_MOMO_L].node[0];
        transform.rot.y = RobBlock::limit_angle(node->transform.rot.y
            + node->transform.rot.x) * (float_t)(1.0 / 3.0);
        break;
    case BLK_N_MOMO_B_R_WJ_EX:
    case BLK_N_MOMO_C_R_WJ_EX:
        node = &bones[BLK_CL_MOMO_R].node[0];
        transform.rot.y = RobBlock::limit_angle(node->transform.rot.y
            + node->transform.rot.x) * (float_t)(1.0 / 3.0);
        break;
    case BLK_N_HARA_B_WJ_EX:
        transform.rot.y = bones[BLK_CL_MUNE].node[0].transform.rot.y * (float_t)(1.0 / 3.0)
            + bones[BLK_KL_KOSI_Y].node[0].transform.rot.y * (float_t)(2.0 / 3.0);
        break;
    case BLK_N_HARA_C_WJ_EX:
        transform.rot.y = bones[BLK_CL_MUNE].node[0].transform.rot.y * (float_t)(2.0 / 3.0)
            + bones[BLK_KL_KOSI_Y].node[0].transform.rot.y * (float_t)(1.0 / 3.0);
        break;
    default:
        return false;
    }
    return true;
}

void RobBlock::get_mat(int32_t skeleton_select) {
    if (check_expression_id_not_null())
        return;

    mat4 mat = inherit_type ? *inherit_mat_ptr : mat4_identity;

    if (ik_type == IKT_0T) {
        mat4_mul_translate(&mat, &leaf_pos[0], &mat);
        chain_rot[0] = mat4_identity;
    }
    else if (ik_type == IKT_0N) {
        mat4_inverse_transform_point(&mat, &leaf_pos[0], &leaf_pos[0]);
        mat4_mul_translate(&mat, &leaf_pos[0], &mat);
        chain_rot[0] = mat4_identity;
    }
    else {
        if (block_id == BLK_KL_EYE_L || block_id == BLK_KL_EYE_R) {
            if (chain_ang.x > 0.0f)
                chain_ang.x *= eyes_xrot_adjust_pos;
            else if (chain_ang.x < 0.0f)
                chain_ang.x *= eyes_xrot_adjust_neg;
        }

        mat4 chain_rot;
        if (ik_type == IKT_ROOT)
            mat4_mul_rotate_zyx(&this->chain_rot[0], &chain_ang, &chain_rot);
        else {
            leaf_pos[0] = chain_pos[skeleton_select];
            mat4_rotate_zyx(&chain_ang, &chain_rot);
        }

        mat4_mul_translate(&mat, &leaf_pos[0], &mat);
        mat4_mul(&chain_rot, &mat, &mat);
        this->chain_rot[0] = chain_rot;
    }

    *node[0].mat_ptr = mat;

    get_mat_ik(skeleton_select);
}

void RobBlock::get_mat_ik(int32_t skeleton_select) {
    if (ik_type < IKT_1)
        return;

    mat4 mat = *node[0].mat_ptr;

    vec3 local_target;
    mat4_inverse_transform_point(&mat, &leaf_pos[1], &local_target);

    float_t target_len_xy_sq = vec2::length_squared(*(vec2*)&local_target);
    float_t target_len_sq = vec3::length_squared(local_target);
    float_t target_len_xy = sqrtf(target_len_xy_sq);
    float_t target_len = sqrtf(target_len_sq);
    mat4 chain_rot;
    if (fabsf(target_len_xy) > 0.000001f && fabsf(target_len_sq) > 0.000001f) {
        mat4_rotate_z((1.0f / target_len_xy) * local_target.y,
            (1.0f / target_len_xy) * local_target.x, &chain_rot);
        mat4_mul_rotate_y(&chain_rot, -(1.0f / target_len) * local_target.z,
            (1.0f / target_len) * target_len_xy, &chain_rot);
        mat4_mul(&chain_rot, &mat, &mat);
    }
    else
        chain_rot = mat4_identity;

    if (up_vector_mat_ptr) {
        vec3 up_vector;
        mat4_get_translation(up_vector_mat_ptr, &up_vector);
        mat4_inverse_transform_point(&mat, &up_vector, &up_vector);
        float_t up_vector_length = vec2::length(*(vec2*)&up_vector.y);
        if (up_vector_length > 0.000001f) {
            up_vector *= 1.0f / up_vector_length;
            float_t rot_cos = up_vector.y;
            float_t rot_sin = up_vector.z;
            mat4_mul_rotate_x(&mat, rot_sin, rot_cos, &mat);
            mat4_mul_rotate_x(&chain_rot, rot_sin, rot_cos, &chain_rot);
        }
    }

    if (ik_type == IKT_1) {
        this->chain_rot[1] = chain_rot;
        *node[1].mat_ptr = mat;
        mat4_mul_translate(&mat, len[0][skeleton_select], 0.0f, 0.0f, &mat);

        *node[2].mat_ptr = mat;
        return;
    }

    float_t len0 = len[0][skeleton_select];
    float_t len1 = len[1][skeleton_select];
    float_t rot_sin;
    float_t rot_cos;
    float_t rot_2nd_sin;
    float_t rot_2nd_cos;
    if (target_len_sq > 0.000001f) {
        if (arm_length > 0.0001f) {
            float_t max_reach = (len0 + len1) * arm_length;
            if (target_len > max_reach) {
                target_len = max_reach;
                target_len_sq = max_reach * max_reach;
            }
        }

        float_t proj_length = (target_len_sq - len1 * len1) / len0;
        rot_cos = (proj_length + len0) / (2.0f * target_len);
        rot_2nd_cos = (proj_length - len0) / (2.0f * len1);

        rot_cos = clamp_def(rot_cos, -1.0f, 1.0f);
        rot_2nd_cos = clamp_def(rot_2nd_cos, -1.0f, 1.0f);

        rot_sin = sqrtf(1.0f - rot_cos * rot_cos);
        rot_2nd_sin = sqrtf(1.0f - rot_2nd_cos * rot_2nd_cos);
        if (ik_type == IKT_2R)
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

    mat4_mul_rotate_z(&mat, rot_sin, rot_cos, &mat);
    *node[1].mat_ptr = mat;
    mat4_mul_rotate_z(&chain_rot, rot_sin, rot_cos, &chain_rot);
    this->chain_rot[1] = chain_rot;
    mat4_mul_translate(&mat, len0, 0.0f, 0.0f, &mat);

    mat4_mul_rotate_z(&mat, rot_2nd_sin, rot_2nd_cos, &mat);
    *node[2].mat_ptr = mat;
    mat4_rotate_z(rot_2nd_sin, rot_2nd_cos, &chain_rot);
    this->chain_rot[2] = chain_rot;
    mat4_mul_translate(&mat, len1, 0.0f, 0.0f, &mat);

    *node[3].mat_ptr = mat;
}

void RobBlock::mult_mat(const mat4& parent_mat, const RobBlock* bones, bool solve_ik) {
    mat4 mat = inherit_type ? *inherit_mat_ptr : parent_mat;

    if (ik_type != IKT_0N && ik_type != IKT_0T) {
        if (ik_type != IKT_ROOT)
            leaf_pos[0] = chain_pos[1];

        mat4_mul_translate(&mat, &leaf_pos[0], &mat);
        if (solve_ik) {
            node[0].transform.rot = 0.0f;
            if (!check_expression_id_not_null())
                mat4_get_rotation_zyx(&chain_rot[0], &node[0].transform.rot);
            else if (get_ex_rotation(node[0].transform, bones))
                mat4_rotate_zyx(&node[0].transform.rot, &chain_rot[0]);
            else {
                *node[0].mat_ptr = mat;

                if (get_constraint_ik(bones)) {
                    mat4 chain_rot;
                    mat4_invert_rotation_fast(&mat, &chain_rot);
                    mat4_mul(node[0].mat_ptr, &chain_rot, &chain_rot);
                    mat4_clear_trans(&chain_rot, &chain_rot);
                    mat4_get_rotation_zyx(&chain_rot, &node[0].transform.rot);
                    this->chain_rot[0] = chain_rot;
                }
                else
                    chain_rot[0] = mat4_identity;
            }
        }

        mat4_mul(&chain_rot[0], &mat, &mat);
    }
    else {
        mat4_mul_translate(&mat, &leaf_pos[0], &mat);
        if (solve_ik)
            node[0].transform.rot = 0.0f;
    }

    *node[0].mat_ptr = mat;

    if (solve_ik) {
        node[0].transform.pos = leaf_pos[0];
        node[0].transform.reset_scale();
    }

    if (ik_type < IKT_1)
        return;

    mat4_mul(&chain_rot[1], &mat, &mat);
    *node[1].mat_ptr = mat;

    if (ik_type == IKT_1) {
        mat4_mul_translate(&mat, len[0][1], 0.0f, 0.0f, &mat);
        *node[2].mat_ptr = mat;

        if (solve_ik) {
            node[1].transform.pos = 0.0f;
            mat4_get_rotation_zyx(&chain_rot[1], &node[1].transform.rot);
            node[1].transform.reset_scale();

            node[2].transform.init(len[0][1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
    else {
        mat4_mul_translate(&mat, len[0][1], 0.0f, 0.0f, &mat);
        mat4_mul(&chain_rot[2], &mat, &mat);
        *node[2].mat_ptr = mat;

        mat4_mul_translate(&mat, len[1][1], 0.0f, 0.0f, &mat);
        *node[3].mat_ptr = mat;

        if (solve_ik) {
            node[1].transform.pos = 0.0f;
            mat4_get_rotation_zyx(&chain_rot[1], &node[1].transform.rot);
            node[1].transform.reset_scale();

            node[2].transform.pos = { len[0][1], 0.0f, 0.0f };
            mat4_get_rotation_zyx(&chain_rot[2], &node[2].transform.rot);
            node[2].transform.reset_scale();

            node[3].transform.init(len[1][1], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
}

void RobBlock::orient_x(const vec3& target) {
    vec3 x_axis;
    mat4_inverse_transform_point(node[0].mat_ptr, &target, &x_axis);

    float_t len;
    len = vec3::length_squared(x_axis);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        x_axis *= 1.0f / len;

    // Gram-Schmidt process, but hacky
    vec3 z_axis;
    z_axis.x = x_axis.y * 0.0f - x_axis.x * x_axis.z - x_axis.z;
    z_axis.y = x_axis.z * -x_axis.y - 0.0f * x_axis.x;
    z_axis.z = x_axis.x * x_axis.x - -x_axis.y * x_axis.y + x_axis.x;

    // Written other way
    //vec3 z_axis = vec3::cross(x_axis, vec3(-x_axis.y, x_axis.x, 0.0f)) + vec3(-x_axis.z, 0.0f, x_axis.x);

    // Expanded
    //vec3 z_axis = vec3::cross(x_axis, vec3::cross(vec3(0.0f, 0.0f, 1.0f), x_axis))
    //    + vec3::cross(x_axis, vec3(0.0f, 1.0f, 0.0f));

    len = vec3::length_squared(z_axis);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        z_axis *= 1.0f / len;

    vec3 y_axis = vec3::cross(z_axis, x_axis);

    mat4 chain_rot;
    mat4_set_row(&chain_rot, 0, x_axis.x, x_axis.y, x_axis.z, 0.0f);
    mat4_set_row(&chain_rot, 1, y_axis.x, y_axis.y, y_axis.z, 0.0f);
    mat4_set_row(&chain_rot, 2, z_axis.x, z_axis.y, z_axis.z, 0.0f);
    mat4_set_row(&chain_rot, 3, 0.0f, 0.0f, 0.0f, 1.0f);

    mat4_mul(&chain_rot, node[0].mat_ptr, node[0].mat_ptr);
}

void RobBlock::orient_x_cns(const vec3& target, float_t weight) {
    vec3 local_target;
    mat4_inverse_transform_point(node[0].mat_ptr, &target, &local_target);
    local_target.x = 0.0f;

    float_t len = vec2::length(*(vec2*)&local_target.y);
    if (fabsf(len) > 0.000001f) {
        float_t angle = atan2f((1.0f / len) * local_target.z, (1.0f / len) * local_target.y);
        mat4_mul_rotate_x(node[0].mat_ptr, angle * weight, node[0].mat_ptr);
    }
}

void RobBlock::orient_y(vec3 y_axis) {
    float_t len;
    len = vec3::length_squared(y_axis);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        y_axis *= 1.0f / len;

    // Gram-Schmidt process, but hacky
    vec3 z_axis;
    z_axis.x = -y_axis.x * y_axis.z - y_axis.y * 0.0f;
    z_axis.y = 0.0f * y_axis.x - y_axis.z * y_axis.y - y_axis.z;
    z_axis.z = y_axis.y * y_axis.y - y_axis.x * -y_axis.x + y_axis.y;

    // Written other way
    //vec3 z_axis = vec3::cross(vec3(y_axis.y, -y_axis.x, 0.0f), y_axis) + vec3(0.0f, -y_axis.z, y_axis.y);

    // Expanded
    //vec3 z_axis = vec3::cross(vec3::cross(y_axis, vec3(0.0f, 0.0f, 1.0f)), y_axis)
    //    + vec3::cross(vec3(1.0f, 0.0f, 0.0f), y_axis);

    len = vec3::length_squared(z_axis);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        z_axis *= 1.0f / len;

    vec3 x_axis = vec3::cross(y_axis, z_axis);

    mat4 chain_rot;
    mat4_set_row(&chain_rot, 0, x_axis.x, x_axis.y, x_axis.z, 0.0f);
    mat4_set_row(&chain_rot, 1, y_axis.x, y_axis.y, y_axis.z, 0.0f);
    mat4_set_row(&chain_rot, 2, z_axis.x, z_axis.y, z_axis.z, 0.0f);
    mat4_set_row(&chain_rot, 3, 0.0f, 0.0f, 0.0f, 1.0f);

    mat4_mul(&chain_rot, node[0].mat_ptr, node[0].mat_ptr);
}

const vec3* RobBlock::set_global_leaf_sub(const vec3* val, BONE_KIND kind, bool get_data, bool flip_x) {
    if (ik_type == IKT_ROOT) {
        if (get_data) {
            leaf_pos[0] = *val;
            if (flip_x)
                leaf_pos[0].x = -leaf_pos[0].x;
        }
        val++;
    }
    else if (ik_type == IKT_1 || ik_type == IKT_2 || ik_type == IKT_2R) {
        if (get_data) {
            leaf_pos[1] = *val;
            if (flip_x)
                leaf_pos[1].x = -leaf_pos[1].x;
        }
        val++;
    }

    if (get_data) {
        if (ik_type == IKT_0N || ik_type == IKT_0T) {
            leaf_pos[0] = *val;
            if (flip_x)
                leaf_pos[0].x = -leaf_pos[0].x;
        }
        else if (!expression_id) {
            chain_ang = *val;
            if (flip_x) {
                const skeleton_rotation_offset* rot_off = skeleton_rotation_offset_array;
                size_t index = block_id;
                if (rot_off[index].x)
                    chain_ang.x = rot_off[index].rotation.x - chain_ang.x;
                if (rot_off[index].y)
                    chain_ang.y = rot_off[index].rotation.y - chain_ang.y;
                if (rot_off[index].z)
                    chain_ang.z = rot_off[index].rotation.z - chain_ang.z;
            }
        }
    }
    val++;
    return val;
}

void RobBlock::store_curr_rot_trans(int32_t skeleton_select) {
    if (check_expression_id_not_null())
        return;

    switch (ik_type) {
    case IKT_0N:
    case IKT_0T:
    case IKT_ROOT:
        smooth_pos[skeleton_select] = leaf_pos[0];
        break;
    }

    switch (ik_type) {
    case IKT_0:
        smooth_rot[0][skeleton_select] = chain_rot[0];
        break;
    case IKT_0N:
    case IKT_0T:
    case IKT_ROOT:
        smooth_rot[0][skeleton_select] = chain_rot[0];
        break;
    case IKT_1:
        smooth_rot[0][skeleton_select] = chain_rot[0];
        smooth_rot[1][skeleton_select] = chain_rot[1];
        break;
    case IKT_2:
    case IKT_2R:
        smooth_rot[0][skeleton_select] = chain_rot[0];
        smooth_rot[1][skeleton_select] = chain_rot[1];
        smooth_rot[2][skeleton_select] = chain_rot[2];
        break;
    }
}

float_t RobBlock::limit_angle(float_t angle) {
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

// 0x1401EB410
void RobBlock::orient_to_target(mat4& mat, vec3 target, vec3 source) {
    target = vec3::normalize(target);
    source = vec3::normalize(source);

    vec3 axis = vec3::cross(source, target);

    float_t c = clamp_def(vec3::dot(source, target), -1.0f, 1.0f);
    float_t s = sqrtf(clamp_def(1.0f - c * c, 0.0f, 1.0f));
    mat4_set(&axis, -s, c, &mat);
}

bone_data_parent::bone_data_parent() : rob_bone_data(),
motion_bone_count(), leaf_pos(), chain_pos(),
bone_key_set_count(), global_key_set_count(), rot_y() {

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

MotionBlend::MotionBlend() : field_8(), enable(), duration(), frame(), offset(), blend() {
    step = 1.0f;
}

MotionBlend::~MotionBlend() {

}

void MotionBlend::Reset() {
    field_8 = false;
    enable = false;
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

MotionBlendCross::MotionBlendCross() : trans_xz(), trans_y() {
    rot_y_mat = mat4_identity;
    field_64 = mat4_identity;
    field_A4 = mat4_identity;
    field_E4 = mat4_identity;
}

MotionBlendCross::~MotionBlendCross() {

}

void MotionBlendCross::Reset() {
    MotionBlend::Reset();
    trans_xz = false;
    trans_y = false;
    rot_y_mat = mat4_identity;
    field_64 = mat4_identity;
    field_A4 = mat4_identity;
    field_E4 = mat4_identity;
}

void MotionBlendCross::Field_10(float_t a2, float_t a3, int32_t a4) {

}

void MotionBlendCross::Step(struc_400* a2) {
    trans_xz = a2->field_0;
    trans_y = true;
    if (duration <= 0.0f || duration <= frame || fabsf(duration - frame) <= 0.000001f) {
        field_8 = false;
        enable = false;
    }
    else {
        enable = true;
        blend = (frame + offset) / (duration + offset);
        frame += step;
    }

    if (enable)
        mat4_rotate_y(-a2->prev_rot_y, &rot_y_mat);
    else
        rot_y_mat = mat4_identity;
}

void MotionBlendCross::Field_20(prj::sys_vector<RobBlock>* bones_curr, prj::sys_vector<RobBlock>* bones_prev) {
    if (!bones_curr || !bones_prev)
        return;

    RobBlock& n_hara_cp_curr = (*bones_curr)[BLK_N_HARA_CP];
    RobBlock& n_hara_curr = (*bones_curr)[BLK_N_HARA];
    RobBlock& n_hara_cp_prev = (*bones_prev)[BLK_N_HARA_CP];
    RobBlock& n_hara_prev = (*bones_prev)[BLK_N_HARA];
    mat4_mul(&n_hara_curr.chain_rot[0], &n_hara_cp_curr.chain_rot[0], &field_64);
    mat4_mul(&n_hara_cp_prev.chain_rot[0], &rot_y_mat, &field_A4);
    mat4_blend_rotation(&field_A4, &n_hara_cp_curr.chain_rot[0], &n_hara_cp_curr.chain_rot[0], blend);
    mat4_mul(&n_hara_prev.chain_rot[0], &field_A4, &field_A4);
    mat4_blend_rotation(&n_hara_prev.chain_rot[0], &n_hara_curr.chain_rot[0], &n_hara_curr.chain_rot[0], blend);
    mat4_mul(&n_hara_curr.chain_rot[0], &n_hara_cp_curr.chain_rot[0], &field_E4);
    mat4_transpose(&field_E4, &field_E4);
}

void MotionBlendCross::Blend(RobBlock* curr, RobBlock* prev) {
    if (!curr || !prev || curr->check_expression_id_not_null())
        return;

    mat4 prev_mat;
    mat4 mat;
    mat4 ik_mat;

    switch (curr->ik_type) {
    case IKT_0:
    default:
        if (curr->block_id == BLK_KL_KOSI_Y) {
            mat4_mul(&prev->chain_rot[0], &field_A4, &prev_mat);
            mat4_mul(&curr->chain_rot[0], &field_64, &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
            mat4_mul(&mat, &field_E4, &curr->chain_rot[0]);
        }
        else if (curr->block_id < 0 || curr->block_id > BLK_N_HARA)
            mat4_blend_rotation(&prev->chain_rot[0], &curr->chain_rot[0], &curr->chain_rot[0], blend);
        break;
    case IKT_0N:
    case IKT_0T:
        curr->leaf_pos[0] = vec3::lerp(prev->leaf_pos[0], curr->leaf_pos[0], blend);
        break;
    case IKT_ROOT:
        if (trans_xz) {
            curr->leaf_pos[0].x = lerp_def(prev->leaf_pos[0].x, curr->leaf_pos[0].x, blend);
            curr->leaf_pos[0].z = lerp_def(prev->leaf_pos[0].z, curr->leaf_pos[0].z, blend);
        }

        if (trans_y)
            curr->leaf_pos[0].y = lerp_def(prev->leaf_pos[0].y, curr->leaf_pos[0].y, blend);
        break;
    case IKT_1:
        if (curr->block_id == BLK_CL_MUNE) {
            mat4_mul(&prev->chain_rot[0], &field_A4, &prev_mat);
            mat4_mul(&prev->chain_rot[1], &prev_mat, &prev_mat);
            mat4_mul(&curr->chain_rot[0], &field_64, &mat);
            mat4_mul(&curr->chain_rot[1], &mat, &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
            mat4_mul(&mat, &field_E4, &mat);
        }
        else {
            mat4_mul(&prev->chain_rot[1], &prev->chain_rot[0], &prev_mat);
            mat4_mul(&curr->chain_rot[1], &curr->chain_rot[0], &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
        }
        mat4_transpose(&curr->chain_rot[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->chain_rot[1]);
        break;
    case IKT_2:
    case IKT_2R:
        mat4_blend_rotation(&prev->chain_rot[2], &curr->chain_rot[2], &curr->chain_rot[2], blend);
        mat4_mul(&prev->chain_rot[1], &prev->chain_rot[0], &prev_mat);
        mat4_mul(&curr->chain_rot[1], &curr->chain_rot[0], &mat);
        mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
        mat4_transpose(&curr->chain_rot[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->chain_rot[1]);
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
    trans_xz = a2->field_0;
    trans_y = true;
    if (enable)
        mat4_rotate_y(-a2->prev_rot_y, &rot_y_mat);
    else
        rot_y_mat = mat4_identity;
}

bool MotionBlendCombine::Field_30() {
    return true;
}

MotionBlendFreeze::MotionBlendFreeze() : trans_xz(),
trans_y(), field_24(), field_28(), field_2C(), field_30() {
    rot_y_mat = mat4_identity;
    field_74 = mat4_identity;
    field_B4 = mat4_identity;
    field_F4 = mat4_identity;
}

MotionBlendFreeze::~MotionBlendFreeze() {

}

void MotionBlendFreeze::Reset() {
    MotionBlend::Reset();
    trans_xz = false;
    trans_y = false;
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
            field_8 = false;
            enable = false;
        }
        else {
            enable = true;
            field_24 = field_30;
            blend = 1.0f - (v9 + offset) / (field_28 + offset);
            if (a2->field_4)
                field_24 = field_30 + 1;
            trans_xz = a2->field_2;
            trans_y = a2->field_3;
        }
    }
    else {
        enable = true;
        field_24 = 0;
        blend = (frame + offset) / (duration + offset);
        frame += step;
        trans_xz = a2->field_0;
        trans_y = a2->field_1;
    }

    if (enable) {
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

void MotionBlendFreeze::Field_20(prj::sys_vector<RobBlock>* bones_curr, prj::sys_vector<RobBlock>* bones_prev) {
    if (!bones_curr)
        return;

    RobBlock& n_hara_cp = (*bones_curr)[BLK_N_HARA_CP];
    RobBlock& n_hara = (*bones_curr)[BLK_N_HARA];
    mat4_mul(&n_hara.chain_rot[0], &n_hara_cp.chain_rot[0], &field_74);
    mat4_mul(&n_hara_cp.smooth_rot[0][field_24], &rot_y_mat, &field_B4);
    mat4_blend_rotation(&field_B4, &n_hara_cp.chain_rot[0], &n_hara_cp.chain_rot[0], blend);
    mat4_mul(&n_hara.smooth_rot[0][field_24], &field_B4, &field_B4);
    mat4_blend_rotation(&n_hara.smooth_rot[0][field_24], &n_hara.chain_rot[0], &n_hara.chain_rot[0], blend);
    mat4_mul(&n_hara.chain_rot[0], &n_hara_cp.chain_rot[0], &field_F4);
    mat4_transpose(&field_F4, &field_F4);
}

void MotionBlendFreeze::Blend(RobBlock* curr, RobBlock* prev) {
    if (!curr || curr->check_expression_id_not_null())
        return;

    mat4 prev_mat;
    mat4 mat;
    mat4 ik_mat;

    switch (curr->ik_type) {
    case IKT_0:
    default:
        if (curr->block_id == BLK_KL_KOSI_Y) {
            mat4_mul(&curr->smooth_rot[0][field_24], &field_B4, &prev_mat);
            mat4_mul(&curr->chain_rot[0], &field_74, &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
            mat4_mul(&mat, &field_F4, &curr->chain_rot[0]);
        }
        else if (curr->block_id < 0 || curr->block_id > BLK_N_HARA)
            mat4_blend_rotation(&curr->smooth_rot[0][field_24], &curr->chain_rot[0], &curr->chain_rot[0], blend);
        break;
    case IKT_0N:
    case IKT_0T:
        curr->leaf_pos[0] = vec3::lerp(curr->smooth_pos[field_24], curr->leaf_pos[0], blend);
        break;
    case IKT_ROOT:
        if (trans_xz) {
            curr->leaf_pos[0].x = lerp_def(curr->smooth_pos[field_24].x, curr->leaf_pos[0].x, blend);
            curr->leaf_pos[0].z = lerp_def(curr->smooth_pos[field_24].z, curr->leaf_pos[0].z, blend);
        }

        if (trans_y)
            curr->leaf_pos[0].y = lerp_def(curr->smooth_pos[field_24].y, curr->leaf_pos[0].y, blend);
        break;
    case IKT_1:
        if (curr->block_id == BLK_CL_MUNE) {
            mat4_mul(&curr->smooth_rot[0][field_24], &field_B4, &prev_mat);
            mat4_mul(&curr->smooth_rot[1][field_24], &prev_mat, &prev_mat);
            mat4_mul(&curr->chain_rot[0], &field_74, &mat);
            mat4_mul(&curr->chain_rot[1], &mat, &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
            mat4_mul(&mat, &field_F4, &mat);
        }
        else {
            mat4_mul(&curr->smooth_rot[1][field_24], &curr->smooth_rot[0][field_24], &prev_mat);
            mat4_mul(&curr->chain_rot[1], &curr->chain_rot[0], &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
        }
        mat4_transpose(&curr->chain_rot[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->chain_rot[1]);
        break;
    case IKT_2:
    case IKT_2R:
        mat4_blend_rotation(&curr->smooth_rot[2][field_24], &curr->chain_rot[2], &curr->chain_rot[2], blend);
        mat4_mul(&curr->smooth_rot[1][field_24], &curr->smooth_rot[0][field_24], &prev_mat);
        mat4_mul(&curr->chain_rot[1], &curr->chain_rot[0], &mat);
        mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
        mat4_transpose(&curr->chain_rot[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->chain_rot[1]);
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
        field_8 = false;
        enable = false;
    }
    else {
        enable = true;
        frame += step;
        blend = (offset + frame) / (offset + duration);
    }
}

void PartialMotionBlendFreeze::Field_20(
    prj::sys_vector<RobBlock>* bones_curr, prj::sys_vector<RobBlock>* bones_prev) {

}

void PartialMotionBlendFreeze::Blend(RobBlock* curr, RobBlock* prev) {
    if (!curr || curr->check_expression_id_not_null())
        return;

    mat4 prev_mat;
    mat4 mat;
    mat4 ik_mat;

    switch (curr->ik_type) {
    case IKT_0:
    default:
        mat4_lerp_rotation(&curr->smooth_rot[0][0], &curr->chain_rot[0], &curr->chain_rot[0], blend);
        break;
    case IKT_0T:
    case IKT_0N:
        curr->leaf_pos[0] = vec3::lerp(curr->smooth_pos[0], curr->leaf_pos[0], blend);
        break;
    case IKT_ROOT:
        mat4_lerp_rotation(&curr->smooth_rot[0][0], &curr->chain_rot[0], &curr->chain_rot[0], blend);
        curr->leaf_pos[0] = vec3::lerp(curr->smooth_pos[0], curr->leaf_pos[0], blend);
        break;
    case IKT_2:
    case IKT_2R:
        mat4_lerp_rotation(&curr->smooth_rot[2][0], &curr->chain_rot[2], &curr->chain_rot[2], blend);
    case IKT_1: {
        mat4_mul(&curr->smooth_rot[1][0], &curr->smooth_rot[0][0], &prev_mat);
        mat4_mul(&curr->chain_rot[1], &curr->chain_rot[0], &mat);
        mat4_lerp_rotation(&prev_mat, &mat, &mat, blend);
        mat4_transpose(&curr->chain_rot[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->chain_rot[1]);
    } break;
    }
}

motion_blend_mot_enabled_bones::motion_blend_mot_enabled_bones() : func(), arr(0, false), count() {

}

motion_blend_mot_enabled_bones::~motion_blend_mot_enabled_bones() {

}

// 0x1404146F0
void motion_blend_mot_enabled_bones::check() {
    int32_t bone_index = BLK_N_HARA_CP;
    for (auto i = arr.begin(); i != arr.end(); i++) {
        if (func((BONE_BLK)bone_index))
            *i = true;
        else
            *i = false;
        bone_index++;
    }
}

// 0x140413350
void motion_blend_mot_enabled_bones::init(PFNMOTIONBONECHECKFUNC func, size_t count) {
    this->func = func;
    this->count = count;
    arr.clear();
    arr.resize(count, false);
    check();
}

void motion_blend_mot_enabled_bones::reset() {
    func = 0;
    arr.clear();
    count = 0;
}

// 0x140413EA0
void motion_blend_mot_enabled_bones::set(void(*func)(prj::sys_vector<bool>&)) {
    func(arr);
}

motion_blend_mot::motion_blend_mot() : bone_data(),
mot_key_data(), mot_play_data(), field_4F8(), field_5CC(), blend() {
    mot_key_data.reset();
    mot_play_data.reset();
    field_4F8.field_C0 = 1.0f;
    field_4F8.field_C4 = 1.0f;
}

motion_blend_mot::~motion_blend_mot() {

}

// 0x140415430
void motion_blend_mot::apply_global_transform() {
    mat4 mat;
    mat4_rotate_y(bone_data.rot_y, &mat);
    mat4_mul_translate(&mat, &bone_data.gblctr_pos, &mat);
    mat4_mul_rotate_zyx(&mat, &bone_data.gblctr_rot, &mat);

    for (RobBlock& i : bone_data.bones)
        switch (i.ik_type) {
        case IKT_0N:
            mat4_transform_point(&mat, &i.leaf_pos[0], &i.leaf_pos[0]);
            break;
        case IKT_ROOT: {
            mat4 chain_rot;
            mat4_clear_trans(&mat, &chain_rot);
            i.chain_rot[0] = chain_rot;
            mat4_transform_point(&mat, &i.leaf_pos[0], &i.leaf_pos[0]);
        } break;
        case IKT_1:
        case IKT_2:
        case IKT_2R:
            mat4_transform_point(&mat, &i.leaf_pos[1], &i.leaf_pos[1]);
            break;
        }
}

void motion_blend_mot::copy_rot_trans() {
    copy_rot_trans(bone_data.bones);
}

// 0x140412BB0
void motion_blend_mot::copy_rot_trans(const prj::sys_vector<RobBlock>& bones) {
    for (RobBlock& i : bone_data.bones)
        i.copy_rot_trans(bones[i.block_id]);
}

bool motion_blend_mot::get_blend_enable() {
    if (blend && !blend->Field_30())
        return blend->enable;
    return false;
}

// 0x140412DA0
void motion_blend_mot::get_n_hara_cp_position(vec3& position) {
    position = bone_data.bones[BLK_N_HARA_CP].leaf_pos[0];
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

void motion_blend_mot::init(rob_chara_bone_data* rob_bone_data,
    PFNMOTIONBONECHECKFUNC check_func, const bone_database* bone_data) {
    bone_data_parent_data_init(&this->bone_data, rob_bone_data, bone_data);
    mot_key_data_init_key_sets(
        &mot_key_data,
        this->bone_data.rob_bone_data->base_skeleton_type,
        this->bone_data.motion_bone_count,
        this->bone_data.leaf_pos);
    enabled_bones.init(check_func, this->bone_data.motion_bone_count);
}

void motion_blend_mot::interpolate() {
    const vec3* fc_value = (const vec3*)mot_key_data.fc_value.data();
    bool flip = motion_blend_mot::interpolate_get_flip(field_4F8);
    float_t frame = mot_play_data.frame_data.frame;

    BONE_KIND skeleton_type = bone_data.rob_bone_data->base_skeleton_type;
    RobBlock* bones_data = bone_data.bones.data();
    for (uint16_t& i : bone_data.bone_indices) {
        RobBlock* data = &bones_data[i];
        bool get_data = enabled_bones.arr[data->block_id];
        if (flip && data->flip_block_id != 0xFF)
            data = &bones_data[data->flip_block_id];

        if (get_data && frame != data->frame) {
            mot_key_data.interpolate(frame, data->key_set_offset, data->key_set_count);
            data->frame = frame;
        }

        fc_value = data->set_global_leaf_sub(fc_value, skeleton_type, get_data, flip);
    }

    uint32_t bone_key_set_count = bone_data.bone_key_set_count;
    if (frame != mot_key_data.frame) {
        mot_key_data.interpolate(frame, bone_key_set_count, bone_data.global_key_set_count);
        mot_key_data.frame = frame;
    }

    const vec3* gbl_fc_value = (const vec3*)&mot_key_data.fc_value.data()[bone_key_set_count];
    bone_data.gblctr_pos = flip ? -gbl_fc_value[0] : gbl_fc_value[0];
    bone_data.gblctr_rot = gbl_fc_value[1];

    apply_global_transform();
}

void motion_blend_mot::load_file(uint32_t motion_id, MotionBlendType blend_type, float_t blend,
    const bone_database* bone_data, const motion_database* mot_db) {
    set_blend(blend_type, blend);
    const mot_data* v6 = mot_key_data_load_file(&mot_key_data, motion_id, mot_db);
    bone_data_parent* v7 = &this->bone_data;
    if (v6) {
        bone_data_parent_load_bone_indices_from_mot(v7, v6, bone_data, mot_db);
        RobBlock* bone = this->bone_data.bones.data();
        for (uint16_t& i : this->bone_data.bone_indices)
            bone[i].frame = -1.0f;
    }
    else {
        v7->rob_bone_data->set_mats_identity();
        this->bone_data.bone_indices.clear();
    }

    field_4F8.field_8C = false;
    field_4F8.field_4C = field_4F8.mat;
    field_4F8.mat = mat4_identity;
}

void motion_blend_mot::mult_mat(const mat4* mat) {
    sub_140414900(&field_4F8, mat);

    mat4& m = field_4F8.mat;
    RobBlock* bones = bone_data.bones.data();
    for (RobBlock& bone : bone_data.bones)
        bone.mult_mat(m, bones, true);
}

void motion_blend_mot::reset() {
    mot_key_data.reset();
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

inline void motion_blend_mot::set_arm_length(BONE_BLK block_id, float_t value) {
    bone_data.bones.data()[block_id].arm_length = value;
}

void motion_blend_mot::set_blend(MotionBlendType blend_type, float_t blend) {
    switch (blend_type) {
    case MOTION_BLEND:
    default:
        this->blend = 0;
        break;
    case MOTION_BLEND_FREEZE:
        freeze.Reset();
        this->blend = &freeze;
        store_curr_rot_trans(0);
        break;
    case MOTION_BLEND_CROSS:
        cross.Reset();
        this->blend = &cross;
        break;
    case MOTION_BLEND_COMBINE:
        combine.Reset();
        this->blend = &combine;

        this->blend->blend = clamp_def(blend, 0.0f, 1.0f);
        this->blend->field_8 = true;
        this->blend->enable = true;
        break;
    }
}

void motion_blend_mot::set_blend_duration(float_t duration, float_t step, float_t offset) {
    if (blend)
        blend->SetDuration(duration, step, offset);
}

void motion_blend_mot::set_step(float_t step) {
    mot_play_data.frame_data.step_prev = mot_play_data.frame_data.step;
    mot_play_data.frame_data.step = step;
}

// 0x140412E10
void motion_blend_mot::store_curr_rot_trans(int32_t skeleton_select) {
    for (RobBlock& i : bone_data.bones)
        if (enabled_bones.arr[i.block_id]) {
            i.store_curr_rot_trans(skeleton_select);
            if (i.ik_type == IKT_ROOT && (field_4F8.field_0 & 0x02))
                i.smooth_pos[skeleton_select] += field_4F8.field_90;
        }
}

bool motion_blend_mot::interpolate_get_flip(struc_308& a1) {
    return a1.field_0 & 0x01 && !(a1.field_0 & 0x08) || !(a1.field_0 & 0x01) && a1.field_0 & 0x08;
}

rob_chara_bone_data_ik_scale::rob_chara_bone_data_ik_scale() {
    ratio0 = 1.0f;
    ratio1 = 1.0f;
    ratio2 = 1.0f;
    ratio3 = 1.0f;
}

partial_motion_blend_mot::partial_motion_blend_mot() : disable(), mot_key_data(), mot_play_data() {

}

partial_motion_blend_mot::~partial_motion_blend_mot() {

}

void partial_motion_blend_mot::init(BONE_KIND type,
    PFNMOTIONBONECHECKFUNC check_func, size_t motion_bone_count, const bone_database* bone_data) {
    mot_key_data_init(&mot_key_data, type, bone_data);
    enabled_bones.init(check_func, motion_bone_count);
}

void partial_motion_blend_mot::interpolate(prj::sys_vector<RobBlock>& bones,
    const prj::sys_vector<uint16_t>* bone_indices, BONE_KIND skeleton_type) {
    if (!mot_key_data.key_sets_ready || !mot_key_data.mot_data || disable)
        return;

    float_t frame = mot_play_data.frame_data.frame;
    const vec3* fc_value = (const vec3*)mot_key_data.fc_value.data();
    for (const uint16_t& i : *bone_indices) {
        RobBlock* data = &bones[i];
        bool get_data = enabled_bones.arr[data->block_id];
        if (get_data) {
            mot_key_data.interpolate(frame, data->key_set_offset, data->key_set_count);
            data->frame = frame;
        }
        fc_value = data->set_global_leaf_sub(fc_value, skeleton_type, get_data, false);
    }
}

void partial_motion_blend_mot::load_motion(uint32_t motion_id, const motion_database* mot_db) {
    mot_key_data_load_file(&mot_key_data, motion_id, mot_db);
    blend.Reset();
}

void partial_motion_blend_mot::set_blend_duration(float_t duration, float_t step, float_t offset) {
    blend.SetDuration(duration, step, offset);
}

void partial_motion_blend_mot::set_frame(float_t frame) {
    mot_play_data.frame_data.set_frame(frame);
}

void partial_motion_blend_mot::set_step(float_t step) {
    mot_play_data.frame_data.step_prev = mot_play_data.frame_data.step;
    mot_play_data.frame_data.step = step;
}

// 0x140412F20
void partial_motion_blend_mot::store_curr_rot_trans(prj::sys_vector<RobBlock>& bones) {
    for (RobBlock& i : bones)
        if (enabled_bones.arr[i.block_id])
            i.store_curr_rot_trans(0);
}

void partial_motion_blend_mot::reset() {
    mot_key_data.reset();
    mot_play_data.reset();
    blend.Reset();
    enabled_bones.reset();
    disable = false;
}

rob_chara_look_anim_eye_param::rob_chara_look_anim_eye_param() : ac(), ft(),
xrot_adjust_neg(), xrot_adjust_pos(), xrot_adjust_dir_neg(), xrot_adjust_dir_pos(),
xrot_adjust_clear_neg(), xrot_adjust_clear_pos() {

}

rob_chara_look_anim_eye_param::rob_chara_look_anim_eye_param(rob_chara_look_anim_eye_param_limits ac,
    rob_chara_look_anim_eye_param_limits ft,
    vec3 pos, float_t xrot_adjust_neg, float_t xrot_adjust_pos,
    float_t xrot_adjust_dir_neg, float_t xrot_adjust_dir_pos,
    float_t xrot_adjust_clear_neg, float_t xrot_adjust_clear_pos) : ac(ac), ft(ft), pos(pos),
    xrot_adjust_neg(xrot_adjust_neg), xrot_adjust_pos(xrot_adjust_pos),
    xrot_adjust_dir_neg(xrot_adjust_dir_neg), xrot_adjust_dir_pos(xrot_adjust_dir_pos),
    xrot_adjust_clear_neg(xrot_adjust_clear_neg), xrot_adjust_clear_pos(xrot_adjust_clear_pos) {

}

void rob_chara_look_anim_eye_param::reset() {
    ac = {};
    ft = {};
    pos = 0.0f;
    xrot_adjust_neg = 1.0f;
    xrot_adjust_pos = 1.0f;
    xrot_adjust_dir_neg = -3.8f;
    xrot_adjust_dir_pos = 6.0f;
    xrot_adjust_clear_neg = -3.8f;
    xrot_adjust_clear_pos = 6.0f;
}

struc_936::struc_936() {

}

void struc_936::reset() {
    toe_l = 0.0f;
    toe_r = 0.0f;
}

rob_chara_look_anim::rob_chara_look_anim() : bones(), update_view_point(), init_head_rotation(),
head_rotation(), init_eyes_rotation(), eyes_rotation(), disable(), head_rot_strength(),
eyes_rot_strength(), eyes_rot_step(), duration(), eyes_rot_frame(), step(), head_rot_frame(), field_B0(),
ext_head_rotation(), ext_head_rot_strength(), ext_head_rot_y_angle(), ext_head_rot_x_angle(),
field_15C(), field_190(), field_191(), field_192(), field_193(), field_194(), field_195(), head_rot_blend(),
field_19C(), field_1AC(), eyes_xrot_adjust_neg(), eyes_xrot_adjust_pos(), ft(), type() {
    reset();
}

void rob_chara_look_anim::reset() {
    bones = 0;
    mat = mat4_identity;
    param.reset();
    update_view_point = false;
    init_head_rotation = false;
    head_rotation = false;
    init_eyes_rotation = false;
    eyes_rotation = false;
    disable = false;
    head_rot_strength = 0.0f;
    eyes_rot_strength = 1.0f;
    eyes_rot_step = 1.0f;
    duration = 0.0f;
    eyes_rot_frame = 0.0f;
    step = 0.0f;
    head_rot_frame = 0.0f;
    field_B0 = 0.0f;
    target_view_point = 0.0f;
    view_point = 0.0f;
    left_eye_mat = mat4_identity;
    right_eye_mat = mat4_identity;
    ext_head_rotation = false;
    ext_head_rot_strength = 0.0f;
    ext_head_rot_y_angle = 0.0f;
    ext_head_rot_x_angle = 0.0f;
    field_15C = {};
    field_184 = 0.0f;
    field_190 = false;
    field_191 = false;
    field_192 = false;
    field_193 = false;
    field_194 = false;
    field_195 = false;
    head_rot_blend = 0.0f;
    field_19C = {};
    field_1AC = {};
    eyes_xrot_adjust_neg = 1.0f;
    eyes_xrot_adjust_pos = 1.0f;
    ft = false;
    type = 0;
}

void rob_chara_look_anim::set(bool update_view_point, bool rotation_enable, float_t head_rot_strength,
    float_t eyes_rot_strength, float_t duration, float_t eyes_rot_step, float_t a8, bool ft) {
    this->update_view_point = update_view_point;
    const bool prev_head_rotation = head_rotation;
    if (!prev_head_rotation && rotation_enable)
        init_head_rotation = true;
    head_rotation = rotation_enable;
    if (!eyes_rotation && rotation_enable)
        init_eyes_rotation = true;
    eyes_rotation = rotation_enable;
    this->head_rot_strength = head_rot_strength >= 0.0f ? head_rot_strength : 1.0f;
    this->eyes_rot_strength = eyes_rot_strength >= 0.0f ? eyes_rot_strength : 1.0f;
    this->eyes_rot_step = eyes_rot_step >= 0.0f ? eyes_rot_step : 1.0f;
    this->duration = (!prev_head_rotation && rotation_enable || prev_head_rotation && !rotation_enable)
        && duration >= 0.0f ? duration : 0.0f;
    eyes_rot_frame = 0.0f;
    step = 1.0f;
    head_rot_frame = 0.0f;
    field_B0 = a8;
    this->ft = ft;
}

void rob_chara_look_anim::set_eyes_xrot_adjust(float_t neg, float_t pos) {
    eyes_xrot_adjust_neg = neg;
    eyes_xrot_adjust_pos = pos;
}

void rob_chara_look_anim::set_target_view_point(const vec3& value) {
    target_view_point = value;
}

rob_chara_sleeve_adjust::rob_chara_sleeve_adjust() : sleeve_l(), sleeve_r(),
enable1(), enable2(), field_5C(), field_68(), field_74(), field_80(), radius(), bones(), step() {
    reset();
}

void rob_chara_sleeve_adjust::reset() {
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

rob_chara_bone_data::rob_chara_bone_data() : field_0(), field_1(), object_bone_count(), node_count(),
motion_bone_count(), leaf_pos(), chain_pos(), disable_eye_motion(), look_anim() {
    base_skeleton_type = BONE_KIND_NONE;
    skeleton_type = BONE_KIND_NONE;
}

rob_chara_bone_data::~rob_chara_bone_data() {
    for (motion_blend_mot*& i : motions) {
        delete i;
        i = 0;
    }
}

bool rob_chara_bone_data::check_look_anim_head_rotation() {
    if (look_anim.head_rotation)
        return look_anim.head_rot_strength > 0.0f;
    return false;
}

bool rob_chara_bone_data::check_look_anim_ext_head_rotation() {
    if (look_anim.ext_head_rotation)
        return fabsf(look_anim.ext_head_rot_strength) > 0.000001f;
    return false;
}

float_t rob_chara_bone_data::get_frame() {
    return motion_loaded.front()->mot_play_data.frame_data.frame;
}

float_t rob_chara_bone_data::get_frame_count() {
    return (float_t)motion_loaded.front()->mot_key_data.mot.frame_count;
}

vec3* rob_chara_bone_data::get_look_anim_target_view_point() {
    return &look_anim.target_view_point;
}

bool rob_chara_bone_data::get_look_anim_ext_head_rotation() {
    return look_anim.ext_head_rotation;
}

bool rob_chara_bone_data::get_look_anim_head_rotation() {
    if (look_anim.duration > 0.0f && look_anim.duration > look_anim.eyes_rot_frame
        && fabsf(look_anim.duration - look_anim.eyes_rot_frame) > 0.000001f)
        return true;
    return look_anim.head_rotation;
}

bool rob_chara_bone_data::get_look_anim_update_view_point() {
    return look_anim.update_view_point;
}

mat4* rob_chara_bone_data::get_mats_mat(size_t index) {
    if (index < mats.size())
        return &mats[index];
    return 0;
}

bool rob_chara_bone_data::get_motion_has_looped() {
    return motion_loaded.front()->mot_play_data.frame_data.looped;
}

void rob_chara_bone_data::interpolate() {
    if (motion_loaded.size() < 0)
        return;

    sub_14041ABA0(this);
    sub_14041DAC0(this);

    for (motion_blend_mot*& i : motion_loaded)
        i->interpolate();

    BONE_KIND skeleton_type = base_skeleton_type;
    motion_blend_mot* v5 = motion_loaded.front();
    prj::sys_vector<RobBlock>& bones = v5->bone_data.bones;
    prj::sys_vector<uint16_t>* bone_indices = &v5->bone_data.bone_indices;
    face.interpolate(bones, bone_indices, skeleton_type);
    hand_l.interpolate(bones, bone_indices, skeleton_type);
    hand_r.interpolate(bones, bone_indices, skeleton_type);
    mouth.interpolate(bones, bone_indices, skeleton_type);
    eyes.interpolate(bones, bone_indices, skeleton_type);
    eyelid.interpolate(bones, bone_indices, skeleton_type);
}

void rob_chara_bone_data::load_eyelid_motion(uint32_t motion_id, const motion_database* mot_db) {
    sub_14041AD50(this);
    eyelid.store_curr_rot_trans(motion_loaded.front()->bone_data.bones);
    eyelid.enabled_bones.check();
    eyelid.load_motion(motion_id, mot_db);
}

void rob_chara_bone_data::load_eyes_motion(uint32_t motion_id, const motion_database* mot_db) {
    eyes.store_curr_rot_trans(motion_loaded.front()->bone_data.bones);
    eyes.load_motion(motion_id, mot_db);
}

void rob_chara_bone_data::load_face_motion(uint32_t motion_id, const motion_database* mot_db) {
    sub_14041AD90(this);
    face.store_curr_rot_trans(motion_loaded.front()->bone_data.bones);
    face.enabled_bones.check();
    face.load_motion(motion_id, mot_db);
}

void rob_chara_bone_data::load_hand_l_motion(uint32_t motion_id, const motion_database* mot_db) {
    hand_l.store_curr_rot_trans(motion_loaded.front()->bone_data.bones);
    hand_l.load_motion(motion_id, mot_db);
}

void rob_chara_bone_data::load_hand_r_motion(uint32_t motion_id, const motion_database* mot_db) {
    hand_r.store_curr_rot_trans(motion_loaded.front()->bone_data.bones);
    hand_r.load_motion(motion_id, mot_db);
}

void rob_chara_bone_data::load_mouth_motion(uint32_t motion_id, const motion_database* mot_db) {
    mouth.store_curr_rot_trans(motion_loaded.front()->bone_data.bones);
    mouth.load_motion(motion_id, mot_db);
}

// 0x1404156B0
static void mot_play_data_step(mot_play_data* play_data) {
    mot_play_frame_data* frame_data = &play_data->frame_data;
    frame_data->looped = false;
    if (play_data->loop_frames_enabled && play_data->loop_frames >= 0.0f)
        play_data->loop_frames -= frame_data->step;

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

            if (frame_data->loop_count > 0 && play_data->loop_index >= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            float_t frame = frame_data->frame - last_frame;
            float_t loop_range = last_frame - frame_data->loop_begin;
            if (frame >= loop_range && loop_range > 0.0f)
                frame = fmodf(frame, loop_range);
            frame_data->frame = frame + frame_data->loop_begin;
            frame_data->looped = true;
            play_data->loop_index++;
        } break;
        case MOT_PLAY_FRAME_DATA_LOOP_RESET: {
            if (frame_data->loop_end >= 0.0f && frame_data->loop_end <= last_frame)
                last_frame = frame_data->loop_end;

            if (last_frame >= frame_data->frame)
                break;

            if (frame_data->loop_count > 0 && play_data->loop_index >= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            frame_data->frame = frame_data->loop_begin;
            frame_data->looped = true;
            play_data->loop_index++;
        } break;
        case MOT_PLAY_FRAME_DATA_LOOP_REVERSE: {
            if (frame_data->frame <= last_frame)
                return;

            if (frame_data->loop_count > 0 && play_data->loop_index <= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            float_t frame = frame_data->frame - last_frame;
            if (frame >= last_frame && last_frame > 0.0f)
                frame = fmodf(frame, last_frame);
            frame_data->frame = last_frame - frame;
            frame_data->playback_state = MOT_PLAY_FRAME_DATA_PLAYBACK_BACKWARD;
            frame_data->looped = true;
            play_data->loop_index++;
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

            if (frame_data->loop_count > 0 && play_data->loop_index >= frame_data->loop_count) {
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
            play_data->loop_index++;
        } break;
        case MOT_PLAY_FRAME_DATA_LOOP_RESET: {
            float_t loop_begin = 0.0f;
            if (frame_data->loop_begin >= 0.0f)
                loop_begin = frame_data->loop_begin;

            if (frame_data->frame >= loop_begin)
                break;

            if (frame_data->loop_count > 0 && play_data->loop_index >= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            if (frame_data->loop_end >= 0.0f && frame_data->loop_end <= last_frame)
                last_frame = frame_data->loop_end;
            frame_data->frame = last_frame;
            frame_data->looped = true;
            play_data->loop_index++;
        } break;
        case MOT_PLAY_FRAME_DATA_LOOP_REVERSE: {
            if (frame_data->frame >= 0.0f)
                return;

            if (frame_data->loop_count > 0 && play_data->loop_index >= frame_data->loop_count) {
                frame_data->loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
                return;
            }

            float_t frame = -frame_data->frame;
            if (last_frame > 0.0f && frame >= last_frame)
                frame = fmodf(frame, last_frame);
            frame_data->frame = frame;
            frame_data->playback_state = MOT_PLAY_FRAME_DATA_PLAYBACK_FORWARD;
            frame_data->looped = true;
            play_data->loop_index++;
        } break;
        }
    } break;
    case MOT_PLAY_FRAME_DATA_PLAYBACK_EXTERNAL: {
        if (play_data->ext_frame)
            frame_data->frame = *play_data->ext_frame;
        if (play_data->ext_step)
            frame_data->step = *play_data->ext_step;
    } break;
    }
}

// 0x14041DBA0
void rob_chara_bone_data::motion_step() {
    for (motion_blend_mot*& i : motion_loaded)
        mot_play_data_step(&i->mot_play_data);
}

void rob_chara_bone_data::reset() {
    object_bone_count = 0;
    motion_bone_count = 0;
    node_count = 0;
    leaf_pos = 0;
    chain_pos = 0;
    mats.clear();
    mats2.clear();
    nodes.clear();
    base_skeleton_type = BONE_KIND_NONE;
    skeleton_type = BONE_KIND_NONE;

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
    field_76C.reset();
    look_anim.reset();
    sleeve_adjust.reset();
}

void rob_chara_bone_data::set_disable_eye_motion(bool value) {
    disable_eye_motion = value;
}

void rob_chara_bone_data::set_eyelid_blend_duration(float_t duration, float_t step, float_t offset) {
    eyelid.set_blend_duration(duration, step, offset);
}

void rob_chara_bone_data::set_eyelid_frame(float_t frame) {
    eyelid.set_frame(frame);
}

void rob_chara_bone_data::set_eyelid_step(float_t step) {
    eyelid.set_step(step);
}

void rob_chara_bone_data::set_eyes_blend_duration(float_t duration, float_t step, float_t offset) {
    eyes.set_blend_duration(duration, step, offset);
}

void rob_chara_bone_data::set_eyes_frame(float_t frame) {
    eyes.set_frame(frame);
}

void rob_chara_bone_data::set_eyes_step(float_t step) {
    eyes.set_step(step);
}

void rob_chara_bone_data::set_face_blend_duration(float_t duration, float_t step, float_t offset) {
    face.set_blend_duration(duration, step, offset);
}

void rob_chara_bone_data::set_face_frame(float_t frame) {
    face.set_frame(frame);
}

void rob_chara_bone_data::set_face_step(float_t step) {
    face.set_step(step);
}

void rob_chara_bone_data::set_frame(float_t frame) {
    motion_loaded.front()->mot_play_data.frame_data.set_frame(frame);
}

void rob_chara_bone_data::set_hand_l_blend_duration(float_t duration, float_t step, float_t offset) {
    hand_l.set_blend_duration(duration, step, offset);
}

void rob_chara_bone_data::set_hand_l_frame(float_t frame) {
    hand_l.set_frame(frame);
}

void rob_chara_bone_data::set_hand_l_step(float_t step) {
    hand_l.set_step(step);
}

void rob_chara_bone_data::set_hand_r_blend_duration(float_t duration, float_t step, float_t offset) {
    hand_r.set_blend_duration(duration, step, offset);
}

void rob_chara_bone_data::set_hand_r_frame(float_t frame) {
    hand_r.set_frame(frame);
}

void rob_chara_bone_data::set_hand_r_step(float_t step) {
    hand_r.set_step(step);
}

void rob_chara_bone_data::set_look_anim(bool update_view_point, bool rotation_enable, float_t head_rot_strength,
    float_t eyes_rot_strength, float_t duration, float_t eyes_rot_step, float_t a8, bool ft) {
    look_anim.set(update_view_point, rotation_enable, head_rot_strength,
        eyes_rot_strength, duration, eyes_rot_step, a8, ft);
}

void rob_chara_bone_data::set_look_anim_target_view_point(const vec3& value) {
    look_anim.set_target_view_point(value);
}

void rob_chara_bone_data::set_mats_identity() {
    for (mat4& i : mats)
        i = mat4_identity;

    for (mat4& i : mats2)
        i = mat4_identity;
}

void rob_chara_bone_data::set_motion_blend_duration(float_t duration, float_t step, float_t offset) {
    motion_loaded.front()->set_blend_duration(duration, step, offset);
}

void rob_chara_bone_data::set_motion_frame(float_t frame, float_t step, float_t frame_count) {
    mot_play_data* play_data = &motion_loaded.front()->mot_play_data;
    play_data->frame_data.frame = frame;
    play_data->frame_data.step_prev = play_data->frame_data.step;
    play_data->frame_data.step = step;
    play_data->frame_data.frame_count = frame_count;
    play_data->frame_data.last_frame = frame_count - 1.0f;
    play_data->frame_data.max_frame = -1.0f;
    play_data->frame_data.playback_state = MOT_PLAY_FRAME_DATA_PLAYBACK_NONE;
    play_data->frame_data.loop_state = MOT_PLAY_FRAME_DATA_LOOP_NONE;
    play_data->frame_data.loop_begin = 0.0f;
    play_data->frame_data.loop_end = -1.0f;
    play_data->frame_data.looped = 0;
    play_data->frame_data.loop_count = -1;
    play_data->loop_index = 0;
    play_data->loop_frames_enabled = false;
    play_data->loop_frames = 0.0f;
    play_data->ext_frame = 0;
    play_data->ext_step = 0;
}

void rob_chara_bone_data::set_motion_loop(float_t loop_begin, int32_t loop_count, float_t loop_end) {
    motion_blend_mot* mot = motion_loaded.front();
    mot->mot_play_data.frame_data.loop_begin = loop_begin;
    mot->mot_play_data.frame_data.loop_end = loop_end;
    mot->mot_play_data.frame_data.loop_count = loop_count;
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

void rob_chara_bone_data::set_mouth_blend_duration(float_t duration, float_t step, float_t offset) {
    mouth.set_blend_duration(duration, step, offset);
}

void rob_chara_bone_data::set_mouth_frame(float_t frame) {
    mouth.set_frame(frame);
}

void rob_chara_bone_data::set_mouth_step(float_t step) {
    mouth.set_step(step);
}

void rob_chara_bone_data::update(const mat4* mat) {
    if (!motion_loaded.size())
        return;

    for (motion_blend_mot*& i : motion_loaded) {
        sub_1404117F0(i);
        sub_14040FBF0(i, ik_scale.ratio0);
    }

    sub_1404182B0(this);
    sub_14041B9F0(this);

    motion_loaded.front()->mult_mat(mat);
}

rob_chara_pv_data::rob_chara_pv_data() : type(), field_4(), field_5(),
field_6(), yang(), field_16(), sleeve_l(), sleeve_r(), field_70(),
motion_face_ids(), chara_size_index(), height_adjust(), item() {
    reset();
}

void rob_chara_pv_data::reset() {
    type = ROB_CHARA_TYPE_2;
    field_4 = true;
    field_5 = false;
    field_6 = false;
    field_8 = 0.0f;
    yang = 0;
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

RobSkinOfs::RobSkinOfs() : flag() {

}

rob_chara_item_equip_object::rob_chara_item_equip_object() : index(), motion_matrix(),
obj_uid(), obj_uid_sub(), alpha(), obj_flags(), can_disp(), bone_kind(),
mat(), init_cnt(), motion_node(), field_138(), osage_depends_on_others(),
osage_nodes_count(), use_opd(), skin_ex_data(), skin(), rob_disp() {
    init_members(0x12345678);
}

rob_chara_item_equip_object::~rob_chara_item_equip_object() {
    init_members(0xDDDDDDDD);
}

void rob_chara_item_equip_object::add_motion_reset_data(
    uint32_t motion_id, float_t frame, int32_t iterations) {
    if (iterations > 0)
        pos_reset(iterations);

    for (ExOsageBlock*& i : osage_blk)
        i->AddMotionResetData(motion_id, frame);

    for (ExClothBlock*& i : cloth)
        i->AddMotionResetData(motion_id, frame);
}

void rob_chara_item_equip_object::check_no_opd(std::vector<opd_blend_data>& opd_blend_data) {
    use_opd = true;
    if (!osage_blk.size() && !cloth.size() || !opd_blend_data.size())
        return;

    for (::opd_blend_data& i : opd_blend_data) {
        const float_t* opd_data = 0;
        uint32_t opd_count = 0;
        osage_play_data_manager_get_opd_file_data(obj_uid, i.motion_id, opd_data, opd_count);
        if (!opd_data) {
            use_opd = false;
            break;
        }
    }
}

void rob_chara_item_equip_object::clear_ex_data() {
    for (ExNullBlock*& i : null_blk) {
        i->dest();
        delete i;
    }
    null_blk.clear();

    for (ExOsageBlock*& i : osage_blk) {
        i->dest();
        delete i;
    }
    osage_blk.clear();

    for (ExConstraintBlock*& i : constraint) {
        i->dest();
        delete i;
    }
    constraint.clear();

    for (ExExpressionBlock*& i : expression) {
        i->dest();
        delete i;
    }
    expression.clear();

    for (ExClothBlock*& i : cloth) {
        i->dest();
        delete i;
    }
    cloth.clear();

    ex_node_block.clear();
}

void rob_chara_item_equip_object::disp(const mat4& mat, render_context* rctx) {
    if (obj_uid.is_null())
        return;

    mdl::ObjFlags flags = rctx->disp_manager->get_obj_flags();
    mdl::ObjFlags chara_flags = flags;
    if (fabsf(alpha - 1.0f) > 0.000001f)
        enum_or(chara_flags, obj_flags);
    else
        enum_and(chara_flags, ~mdl::OBJ_ALPHA_ORDER);
    rctx->disp_manager->set_obj_flags(chara_flags);
    if (can_disp) {
        rctx->disp_manager->entry_obj_by_object_info_object_skin(obj_uid,
            &texchg_vec, &skn_col, alpha, motion_matrix, matrix.data(), 0, mat);

        for (ExNodeBlock*& i : ex_node_block)
            i->disp(mat, rctx);
    }
    rctx->disp_manager->set_obj_flags(flags);
}

int32_t rob_chara_item_equip_object::get_node_index(const char* name, const bone_database* bone_data) const {
    int32_t node_index = bone_data->get_bone_node_index(BONE_KIND_CMN, name);
    if (node_index == -1)
        for (auto& i : node_name_map)
            if (!str_utils_compare(name, i.first))
                return 0x8000 | i.second;
    return node_index;
}

const RobNode* rob_chara_item_equip_object::get_node(int32_t index) const {
    if (!(index & 0x8000))
        return &motion_node[index & 0x7FFF];
    else if ((index & 0x7FFF) < ex_node.size())
        return &ex_node[index & 0x7FFF];
    return 0;
}

const RobNode* rob_chara_item_equip_object::get_node(const char* name, const bone_database* bone_data) const {
    return get_node(get_node_index(name, bone_data));
}

const mat4* rob_chara_item_equip_object::get_ex_data_bone_node_mat(const char* name) {
    if (!name || !ex_node.size())
        return &mat4_identity;

    for (RobNode& i : ex_node)
        if (!str_utils_compare(i.name, name))
            return i.mat_ptr;

    return &mat4_identity;
}

RobJointNode* rob_chara_item_equip_object::get_normal_ref_osage_node(const std::string& str, size_t* index) {
    if (!str.size())
        return 0;

    size_t pos = str.find(',', 0);
    if (pos == -1)
        return 0;

    std::string name = str.substr(0, pos);
    std::string node_idx_str = str.substr(pos + 1);
    size_t node_idx = atoi(node_idx_str.c_str());
    if (index)
        *index = node_idx;

    RobJointNode* node = 0;
    for (ExOsageBlock*& i : osage_blk)
        if (!name.compare(i->name)) {
            if (node_idx < i->osage_work.joint_node_vec.size())
                node = i->osage_work.get_joint_node(node_idx + 1);
            break;
        }
    return node;
}

void rob_chara_item_equip_object::set_motion_node(const RobNode* mot_node, const bone_database* bone_data) {
    motion_node = mot_node;
    motion_matrix = mot_node->mat_ptr;
    for (ExNodeBlock*& i : ex_node_block)
        i->parent = get_node(i->parent_name.c_str(), bone_data);
}

void rob_chara_item_equip_object::init_members(size_t index) {
    this->index = index;
    obj_uid = {};
    obj_uid_sub = {};
    motion_matrix = 0;
    texchg_vec.clear();
    skn_col.type = -1;
    skn_col.blend_color = 1.0f;
    skn_col.offset_color = 0.0f;
    skn_col.blend_specular = 1.0f;
    skn_col.offset_specular = 0.0f;
    alpha = 1.0f;
    obj_flags = mdl::OBJ_ALPHA_ORDER_POST_GLITTER;
    skn_ofs.flag = false;
    can_disp = true;
    bone_kind = 0;
    mat = 0;
    motion_node = 0;
    clear_ex_data();
    ex_node.clear();
    matrix.clear();
    no_scale_matrix.clear();
    node_name_map.clear();
    osage_depends_on_others = false;
    use_opd = false;
    osage_nodes_count = 0;
}

void rob_chara_item_equip::load_outfit_object_info(ROB_PARTS_KIND rpk, object_info obj_info,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (rpk < RPK_BASE_BEGIN && rpk > RPK_BASE_END)
        obj_info = {};
    set_item(obj_info, rpk, osage_reset, bone_data, data, obj_db);
}

// 0x1405F4820
void rob_chara_item_equip_object::pos_reset(int32_t init_cnt) {
    if (!ex_node_block.size())
        return;

    auto pos_init = &ExNodeBlock::pos_init;
    auto pos_init_cont = &ExNodeBlock::pos_init_cont;

    for (ExNodeBlock*& i : ex_node_block)
        (i->*pos_init)();

    for (; init_cnt; init_cnt--) {
        if (osage_depends_on_others)
            rob_chara_item_equip_object_ctrl_step(this, true);

        for (ExNodeBlock*& i : ex_node_block)
            (i->*pos_init_cont)();
    }
}

void rob_chara_item_equip_object::reset_ex_force() {
    for (ExOsageBlock*& i : osage_blk)
        i->reset_ex_force();
    for (ExClothBlock*& i : cloth)
        i->reset_ex_force();
}

void rob_chara_item_equip_object::reset_nodes_ex_force(rob_osage_parts_bit parts_bits) {
    for (ExOsageBlock*& i : osage_blk)
        if (i->osage_work.CheckPartsBits(parts_bits)) {
            i->osage_work.SetNodesExternalForce(0, 1.0f);
            i->osage_work.SetNodesForce(1.0f);
        }
}

void rob_chara_item_equip_object::set(object_info objuid, const RobNode* mot_node,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    obj_uid = objuid;
    motion_node = mot_node;
    motion_matrix = mot_node->mat_ptr;
    ex_node.clear();
    node_name_map.clear();
    matrix.clear();
    clear_ex_data();

    obj_skin* skin = objset_info_storage_get_obj_skin(obj_uid);
    if (!skin)
        return;

    this->skin = skin;
    if (!skin->ex_data)
        return;

    skin_ex_data = skin->ex_data;
    set_ex_node(skin->ex_data);
    set_ex_node_block(skin->ex_data, bone_data, data, obj_db);

    if (osage_reset && osage_blk.size())
        init_cnt = 60;
}

void rob_chara_item_equip_object::set_alpha_obj_flags(float_t alpha, int32_t flags) {
    obj_flags = (mdl::ObjFlags)flags;
    this->alpha = clamp_def(alpha, 0.0f, 1.0f);
}

bool rob_chara_item_equip_object::set_boc(
    const skin_param_osage_root& skp_root, ExOsageBlock* osg) {
    RobOsage& osage_work = osg->osage_work;
    osage_work.dest_boc();

    bool has_boc_node = false;
    for (const skin_param_osage_root_boc& i : skp_root.boc)
        for (ExOsageBlock*& j : osage_blk) {
            if (i.ed_root.compare(j->name)
                || i.ed_node + 1ULL >= osage_work.joint_node_vec.size()
                || i.st_node + 1ULL >= j->osage_work.joint_node_vec.size())
                continue;

            RobJointNode* ed_node = osage_work.get_joint_node(i.ed_node + 1ULL);
            RobJointNode* st_node = j->osage_work.get_joint_node(i.st_node + 1ULL);
            ed_node->data_ptr->boc.push_back(st_node);
            has_boc_node = true;
            break;
        }
    return has_boc_node;
}

void rob_chara_item_equip_object::set_collision_target_osage(
    const skin_param_osage_root& skp_root, skin_param* skp) {
    if (!skp_root.colli_tgt_osg.size())
        return;

    const char* colli_tgt_osg = skp_root.colli_tgt_osg.c_str();
    for (ExOsageBlock*& i : osage_blk)
        if (!str_utils_compare(colli_tgt_osg, i->name)) {
            skp->colli_tgt_osg = &i->osage_work.joint_node_vec;
            break;
        }
}

void rob_chara_item_equip_object::set_disable_collision(rob_osage_parts_bit parts_bits, bool disable) {
    for (ExOsageBlock*& i : osage_blk)
        if (i->osage_work.CheckPartsBits(parts_bits))
            i->SetDisableCollision(disable);
}

void rob_chara_item_equip_object::set_ex_node(obj_skin_ex_data* ex_data) {
    ex_node.clear();
    matrix.clear();
    no_scale_matrix.clear();

    int32_t nb_node_name = ex_data->nb_node_name;
    ex_node.resize(nb_node_name);
    matrix.resize(nb_node_name);
    no_scale_matrix.resize(nb_node_name);

    RobNode* motion_node = ex_node.data();
    mat4* mat = matrix.data();
    mat4* no_scale_mat = no_scale_matrix.data();
    for (int32_t i = 0; i < nb_node_name; i++)
        motion_node[i].mat_ptr = &mat[i];

    if (ex_data->ex_node_name) {
        node_name_map.clear();
        const char** ex_node_name = ex_data->ex_node_name;
        for (int32_t i = 0; i < nb_node_name; i++) {
            motion_node[i].set_name_mat_no_scale_mat(ex_node_name[i], &mat[i], &no_scale_mat[i]);
            node_name_map.push_back(ex_node_name[i], i);
        }
    }
}

void rob_chara_item_equip_object::set_ex_node_block(obj_skin_ex_data* ex_data,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!ex_data->ex_node_table)
        return;

    prj::vector_pair<uint32_t, RobJointNode*> joint_node_list;
    std::map<std::string, ExNodeBlock*> ex_node_list;
    clear_ex_data();

    size_t constraint_count = 0;
    size_t expression_count = 0;
    size_t osage_count = 0;
    size_t cloth_count = 0;
    size_t null_count = 0;
    const char** ex_node_name = ex_data->ex_node_name;
    obj_skin_ex_node* block = ex_data->ex_node_table;
    int32_t num_ex_node = ex_data->num_ex_node;
    for (int32_t i = 0; i < num_ex_node; i++, block++) {
        ExNodeBlock* ex_node;
        switch (block->type) {
        case OBJ_SKIN_EX_NODE_CLOTH: {
            if (cloth_count >= 0x08)
                continue;

            ExClothBlock* cls = new ExClothBlock;
            ex_node = cls;
            cloth.push_back(cls);
            cls->set_data(this, block->cloth, 0, bone_data);
            cls->block_idx = cloth_count + osage_count;
            cls->name = block->cloth->omote_name;
            cloth_count++;
        } break;
        case OBJ_SKIN_EX_NODE_CONSTRAINT: {
            if (constraint_count >= 0x40)
                continue;

            ExConstraintBlock* cns = new ExConstraintBlock;
            ex_node = cns;
            constraint.push_back(cns);
            cns->set_data(this, block->constraint,
                ex_node_name[block->constraint->node_name & 0x7FFF], bone_data);
            constraint_count++;
        } break;
        case OBJ_SKIN_EX_NODE_EXPRESSION: {
            if (expression_count >= 0x50)
                continue;

            ExExpressionBlock* exp = new ExExpressionBlock;
            ex_node = exp;
            expression.push_back(exp);
            exp->set_data(this, block->expression,
                ex_node_name[block->expression->node_name & 0x7FFF], obj_uid, index, bone_data);
            expression_count++;
        } break;
        case OBJ_SKIN_EX_NODE_OSAGE: {
            if (osage_count >= 0x100)
                continue;

            ExOsageBlock* osg = new ExOsageBlock;
            ex_node = osg;
            osage_blk.push_back(osg);
            osg->set_data(this, block->osage, ex_node_name[block->osage->root_idx & 0x7FFF],
                &skin_ex_data->osage_joint[block->osage->joint_ofs],
                motion_node, this->ex_node.data(), skin);
            osg->get_node_list(block->osage,
                &skin_ex_data->osage_joint[block->osage->joint_ofs], joint_node_list, ex_node_list);
            osg->block_idx = osage_count;
            osage_count++;
        } break;
        default: {
            if (null_count >= 0x04)
                continue;

            ExNullBlock* null = new ExNullBlock;
            ex_node = null;
            null_blk.push_back(null);
            null->set_data(this, block->constraint,
                ex_node_name[block->constraint->node_name & 0x7FFF], bone_data);
            null_count++;
        } break;
        }

        obj_skin_ex_node_transform* b_transform = block->transform;
        RobTransform transform;
        transform.pos = b_transform->position;
        transform.rot = b_transform->rotation;
        transform.scale = b_transform->scale;
        transform.hsc = 1.0f;
        ex_node->parent_name = b_transform->parent_name;

        const RobNode* parent = get_node(b_transform->parent_name, bone_data);
        ex_node->parent = parent;
        if (ex_node->dst_node) {
            ex_node->dst_node->transform = transform;
            ex_node->dst_node->parent = parent;
        }

        if (parent) {
            auto elem = ex_node_list.find(parent->name);
            if (elem != ex_node_list.end())
                ex_node->parent_node = elem->second;
        }

        ex_node_list.insert({ ex_node->name, ex_node });
        ex_node_block.push_back(ex_node);
    }

    for (ExNodeBlock*& i : ex_node_block) {
        ExNodeBlock* parent_node = i->parent_node;
        if (parent_node) {
            parent_node->has_children_node = true;
            if ((parent_node->type & ~0x03) || parent_node->type == EX_NODE_TYPE_OSAGE
                || !parent_node->is_parent)
                continue;
        }
        i->is_parent = true;
    }

    for (ExOsageBlock*& i : osage_blk) {
        ExOsageBlock* osg = i;
        const ExNodeBlock* parent_node = osg->parent_node;
        const RobNode* parent = 0;
        if (!parent_node || osg->is_parent)
            parent = osg->parent;
        else {
            while (!parent_node->is_parent) {
                parent_node = parent_node->parent_node;
                if (!parent_node)
                    break;
            }

            if (parent_node)
                parent = parent_node->parent;
        }

        if (parent && parent->no_scale_mat) {
            osg->osage_work.root_matrix_ptr = parent->no_scale_mat;
            osg->osage_work.root_matrix_prev = *parent->no_scale_mat;
        }
    }

    if (ex_data->osage_constraint_tbl) {
        obj_skin_osage_constraint_info* sibling_info = ex_data->osage_constraint_tbl;
        for (int32_t i = 0; i < ex_data->num_osage_constraint; i++, sibling_info++) {
            uint32_t dst_joint = sibling_info->dst_joint;
            uint32_t src_joint = sibling_info->src_joint;

            auto node_elem = joint_node_list.begin();
            for (; node_elem != joint_node_list.end(); node_elem++)
                if (node_elem->first == dst_joint)
                    break;

            auto distance_elem = joint_node_list.begin();
            for (; distance_elem != joint_node_list.end(); distance_elem++)
                if (distance_elem->first == src_joint)
                    break;

            if (node_elem != joint_node_list.end() && distance_elem != joint_node_list.end()) {
                RobJointNode* node = node_elem->second;
                node->distance = distance_elem->second;
                node->length_dist = sibling_info->length;
            }
        }
    }

    if (osage_count || cloth_count)
        skp_load_file(data, bone_data, obj_db);

    size_t osage_nodes_count = 0;
    for (ExOsageBlock*& i : osage_blk)
        osage_nodes_count += i->osage_work.joint_node_vec.size() - 1;
    for (ExClothBlock*& i : cloth)
        osage_nodes_count += i->cloth_work.vtxarg.size() - i->cloth_work.width;
    this->osage_nodes_count = osage_nodes_count;
}

void rob_chara_item_equip_object::set_motion_reset_data(uint32_t motion_id, float_t frame) {
    for (ExOsageBlock*& i : osage_blk)
        i->SetMotionResetData(motion_id, frame);

    for (ExClothBlock*& i : cloth)
        i->SetMotionResetData(motion_id, frame);
}

void rob_chara_item_equip_object::set_motion_skin_param(int8_t chara_id, uint32_t motion_id, int32_t frame) {
    if (obj_uid.is_null())
        return;

    std::vector<skin_param_file_data>* skp_file_data
        = skin_param_manager_get_skin_param_file_data(chara_id, obj_uid, motion_id, frame);
    if (!skp_file_data)
        return;

    osage_depends_on_others = false;
    skin_param_file_data* j = skp_file_data->data();
    for (ExOsageBlock*& i : osage_blk) {
        osage_depends_on_others |= j->depends_on_others;
        i->SetSkinParam(j++);
    }

    for (ExClothBlock*& i : cloth) {
        osage_depends_on_others |= j->depends_on_others;
        i->SetSkinParam(j++);
    }
}

void rob_chara_item_equip_object::set_null_blocks_expression_data(
    const vec3& position, const vec3& rotation, const vec3& scale) {
    const vec3 pos = position;
    const vec3 rot = rotation * DEG_TO_RAD_FLOAT;
    const vec3 sc = scale;

    skn_ofs.flag = true;
    for (ExNullBlock*& i : null_blk) {
        if (!i || !i->dst_node)
            continue;

        RobTransform& transform = i->dst_node->transform;
        transform.pos = pos;
        transform.rot = rot;
        transform.scale = sc;
    }
}

void rob_chara_item_equip_object::set_osage_play_data_init(const float_t* opdi_data) {
    for (ExOsageBlock*& i : osage_blk)
        opdi_data = i->SetOsagePlayDataInit(opdi_data);

    for (ExClothBlock*& i : cloth)
        opdi_data = i->SetOsagePlayDataInit(opdi_data);
}

void rob_chara_item_equip_object::set_osage_reset() {
    for (ExOsageBlock*& i : osage_blk)
        i->SetOsageReset();

    for (ExClothBlock*& i : cloth)
        i->SetOsageReset();
}

void rob_chara_item_equip_object::set_osage_move_cancel(const float_t& mv_ccl) {
    for (ExOsageBlock*& i : osage_blk)
        i->set_move_cancel(mv_ccl);

    for (ExClothBlock*& i : cloth)
        i->set_move_cancel(mv_ccl);
}

void rob_chara_item_equip_object::set_texture_pattern(texture_pattern_struct* tex_pat, size_t count) {
    texchg_vec.clear();
    if (count && tex_pat)
        for (size_t i = 0; i < count; i++)
            texchg_vec.push_back(tex_pat[i]);
}

void rob_chara_item_equip_object::skp_load(void* kv, const bone_database* bone_data) {
    key_val* _kv = (key_val*)kv;
    if (_kv->key.size() < 1)
        return;

    osage_depends_on_others = false;
    for (ExOsageBlock*& i : osage_blk) {
        ExOsageBlock* osg = i;
        skin_param_osage_root root;
        osg->osage_work.LoadSkinParam(_kv, osg->name, root, obj_uid, bone_data);
        set_collision_target_osage(root, osg->osage_work.skin_param_ptr);
        osage_depends_on_others |= set_boc(root, osg);
        osage_depends_on_others |= root.coli_type != SkinParam::RootCollisionTypeEnd;
        osage_depends_on_others |= skp_load_normal_ref(root, 0);
    }

    for (ExClothBlock*& i : cloth) {
        ExClothBlock* cls = i;
        cls->cloth_work.LoadSkinParam(_kv, cls->name, bone_data);
    }
}

void rob_chara_item_equip_object::skp_load(const skin_param_osage_root& skp_root,
    std::vector<skin_param_osage_node>& vec, skin_param_file_data* skp_file_data, const bone_database* bone_data) {
    set_collision_target_osage(skp_root, &skp_file_data->skin_param);
    skp_file_data->depends_on_others |= skp_file_data->skin_param.coli_type > SkinParam::RootCollisionTypeEnd;

    skin_param_osage_node* j = vec.data();
    size_t k = 0;
    for (RobJointNodeData& i : skp_file_data->nodes_data)
        i.SetForce(skp_root, j++, k++);

    skp_file_data->depends_on_others |= skp_load_boc(skp_root, &skp_file_data->nodes_data);
    skp_file_data->depends_on_others |= skp_load_normal_ref(skp_root, &skp_file_data->nodes_data);
}

bool rob_chara_item_equip_object::skp_load_boc(
    const skin_param_osage_root& skp_root, std::vector<RobJointNodeData>* node_data) {
    bool has_boc_node = false;
    for (const skin_param_osage_root_boc& i : skp_root.boc)
        for (ExOsageBlock*& j : osage_blk) {
            if (i.ed_root.compare(j->name)
                || i.ed_node >= node_data->size()
                || i.st_node >= j->osage_work.joint_node_vec.size())
                continue;

            RobJointNodeData& ed_node = node_data->data()[i.ed_node];
            RobJointNode* st_node = j->osage_work.get_joint_node(i.st_node + 1ULL);
            ed_node.boc.push_back(st_node);
            has_boc_node = true;
            break;
        }
    return has_boc_node;
}

void rob_chara_item_equip_object::skp_load_file(void* data,
    const bone_database* bone_data, const object_database* obj_db) {
    key_val* kv_ptr = skin_param_storage_get_key_val(obj_uid);
    if (kv_ptr) {
        skp_load(kv_ptr, bone_data);
        return;
    }

    std::string buf = string_to_lower(sprintf_s_string(
        "ext_skp_%s.txt", obj_db->get_object_name(obj_uid)));

    key_val kv;
    ((data_struct*)data)->load_file(&kv, "rom/skin_param/", buf.c_str(), key_val::load_file);
    skp_load(&kv, bone_data);
}

bool rob_chara_item_equip_object::skp_load_normal_ref(
    const skin_param_osage_root& skp_root, std::vector<RobJointNodeData>* node_data) {
    if (!skp_root.normal_ref.size())
        return false;

    for (const skin_param_osage_root_normal_ref& i : skp_root.normal_ref) {
        size_t index = 0;
        RobJointNode* n = get_normal_ref_osage_node(i.n, &index);
        if (!n)
            continue;

        RobJointNodeData* data = node_data ? &(*node_data)[index] : n->data_ptr;
        data->normal_ref.n = n;
        data->normal_ref.u = get_normal_ref_osage_node(i.u, 0);
        data->normal_ref.d = get_normal_ref_osage_node(i.d, 0);
        data->normal_ref.l = get_normal_ref_osage_node(i.l, 0);
        data->normal_ref.r = get_normal_ref_osage_node(i.r, 0);
        data->normal_ref.Load();
    }
    return true;
}

rob_chara_item_equip::rob_chara_item_equip() : motion_node(), motion_matrix(), skin_disp(),
parts_attr(), one_skin(), disp_begin(), disp_max(), shadow_flag(), shadow_group(), position(),
hyoutan_rpk(), disable_update(), field_DC(), skin_color(), wet(), wind_strength(), chara_color(),
npr_flag(), shadow_skin(), use_opd(), parts_short(), parts_append(), parts_white_one_l() {
    skin_disp = new rob_chara_item_equip_object[RPK_MAX];
    wind_strength = 1.0f;
    chara_color = true;
    osage_step = 1.0f;

    for (int32_t i = 0; i < RPK_MAX; i++)
        skin_disp[i].rob_disp = this;
}

rob_chara_item_equip::~rob_chara_item_equip() {
    reset();

    if (skin_disp) {
        delete[] skin_disp;
        skin_disp = 0;
    }
}

void rob_chara_item_equip::add_motion_reset_data(uint32_t motion_id, float_t frame, int32_t iterations) {
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].add_motion_reset_data(motion_id, frame, iterations);
}

static void sub_140512C20(rob_chara_item_equip* rob_disp, render_context* rctx) {
    if (rob_disp->hyoutan_obj.is_null() || rob_disp->hyoutan_rpk == RPK_NONE)
        return;

    mat4 mat = mat4_identity;
    const char* name;
    if (rob_disp->field_DC == 1) {
        name = "kl_te_r_wj";
        mat4_translate(0.0f, 0.0f, 0.082f, &mat);
        mat4_mul_rotate_zyx(&mat, (float_t)(-90.9 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    if (rob_disp->field_DC == 2) {
        name = "kl_te_l_wj";
        mat4_translate(0.0f, 0.0015f, -0.0812f, &mat);
        mat4_mul_rotate_zyx(&mat, (float_t)(-34.5 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    else
        name = "j_1_hyoutan_000wj";

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;

    const RobNode* node = rob_disp->skin_disp[rob_disp->hyoutan_rpk].get_node(name, aft_bone_data);
    if (!node || !node->mat_ptr)
        return;

    mat4_mul(node->mat_ptr, &mat, &mat);
    int32_t num_texchg = (int32_t)rob_disp->hyoutan_texchg_list.size();
    if (num_texchg)
        rctx->disp_manager->set_texture_pattern(num_texchg, rob_disp->hyoutan_texchg_list.data());
    rctx->disp_manager->entry_obj_by_object_info(mat, rob_disp->hyoutan_obj);
    if (num_texchg)
        rctx->disp_manager->set_texture_pattern();
}

void rob_chara_item_equip::disp(int32_t chara_id, render_context* rctx) {
    extern bool reflect_full;
    extern bool reflect_draw;
    reflect_draw = reflect_full;

    mdl::ObjFlags flags = (mdl::ObjFlags)0;
    if (rctx->chara_reflect)
        enum_or(flags, mdl::OBJ_CHARA_REFLECT);
    if (rctx->chara_refract)
        enum_or(flags, mdl::OBJ_REFRACT);

    mdl::DispManager& disp_manager = *rctx->disp_manager;
    if (shadow_group != -1) {
        if (shadow_flag & 0x04) {
            vec3 pos = position;
            pos.y -= 0.2f;
            shadow_ptr_get()->positions[shadow_group].push_back(pos);

            float_t v9;
            if (sub_140512F60(this) <= -0.2f)
                v9 = -0.5f;
            else
                v9 = 0.05f;
            shadow_ptr_get()->field_1C0[shadow_group] = v9;
            disp_manager.set_shadow_group(shadow_group);
            enum_or(flags, mdl::OBJ_SHADOW);
        }

        if (shadow_flag & 0x01)
            enum_or(flags, mdl::OBJ_4);
    }
    disp_manager.set_obj_flags(flags);
    disp_manager.set_chara_color(chara_color);

    vec4 temp_texture_color_coeff;
    disp_manager.get_texture_color_coeff(temp_texture_color_coeff);

    disp_manager.set_texture_color_coefficients(skin_color);
    disp_manager.set_wet_param(wet);
    rctx->render_manager->npr_mask |= npr_flag;
    sub_140512C20(this, rctx);
    rob_chara_age_age_array_disp(rctx, chara_id, rctx->chara_reflect, chara_color);

    if (one_skin)
        for (int32_t i = disp_begin; i < disp_max; i++)
            skin_disp[i].disp(mat, rctx);
    else {
        for (int32_t i = RPK_DISP_BEGIN; i <= RPK_DISP_END; i++) {
            mdl::ObjFlags chara_flags = (mdl::ObjFlags)0;
            if (!(parts_attr[i] & 0x02)) {
                if (rctx->chara_reflect)
                    enum_or(chara_flags, mdl::OBJ_CHARA_REFLECT);
                if (rctx->chara_refract)
                    enum_or(chara_flags, mdl::OBJ_REFRACT);
            }

            mdl::ObjFlags flags = (mdl::ObjFlags)(mdl::OBJ_4 | mdl::OBJ_SHADOW);
            if (i == RPK_HARA)
                flags = (mdl::ObjFlags)0;

            if (!(shadow_flag & 0x04))
                enum_and(flags, ~mdl::OBJ_SHADOW);

            disp_manager.set_obj_flags((mdl::ObjFlags)(chara_flags | flags | mdl::OBJ_SSS));
            skin_disp[i].disp(mat, rctx);
        }
    }
    disp_manager.set_texture_color_coefficients(temp_texture_color_coeff);
    disp_manager.set_wet_param();
    disp_manager.set_chara_color();
    disp_manager.set_obj_flags();
    disp_manager.set_shadow_group();

    reflect_draw = false;
}

rob_chara_item_equip_object* rob_chara_item_equip::get_skin_disp(ROB_PARTS_KIND rpk) {
    if (rpk >= 0 && rpk < RPK_MAX)
        return &skin_disp[rpk];
    return 0;
}

object_info rob_chara_item_equip::get_object_info(ROB_PARTS_KIND rpk) {
    if (rpk >= 0 && rpk < RPK_MAX)
        return skin_disp[rpk].obj_uid;
    return {};
}

void rob_chara_item_equip::set_motion_node(const RobNode* mot_node, const bone_database* bone_data) {
    motion_node = mot_node;
    motion_matrix = mot_node->no_scale_mat;
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].set_motion_node(mot_node, bone_data);
}

const mat4* rob_chara_item_equip::get_ex_data_bone_node_mat(ROB_PARTS_KIND rpk, const char* name) {
    return skin_disp[rpk].get_ex_data_bone_node_mat(name);
}

void rob_chara_item_equip::load_body_parts_object_info(ROB_PARTS_KIND rpk, object_info obj_info,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (rpk < RPK_ITEM_BEGIN || rpk > RPK_ITEM_END)
        obj_info = {};
    set_item(obj_info, rpk, false, bone_data, data, obj_db);
}

// 0x1405135E0
void rob_chara_item_equip::pos_reset(uint8_t init_cnt) {
    for (int32_t i = 0; i < RPK_MAX; i++)
        skin_disp[i].pos_reset(init_cnt);
}

void rob_chara_item_equip::reset() {
    motion_node = 0;
    motion_matrix = 0;
    for (int32_t i = 0; i < RPK_MAX; i++)
        skin_disp[i].init_members();
    one_skin = false;
    shadow_group = (SHADOW_GROUP)-1;
    shadow_flag = 0x05;
    hyoutan_texchg_list.clear();
    hyoutan_obj = {};
    hyoutan_rpk = RPK_NONE;
    disable_update = false;
    parts_short = false;
    parts_append = false;
    parts_white_one_l = false;
    osage_step = 1.0f;
    use_opd = false;
    disp_begin = RPK_DISP_BEGIN;
    disp_max = RPK_DISP_END;
}

void rob_chara_item_equip::reset_ex_force() {
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].reset_ex_force();
}

void rob_chara_item_equip::reset_init_data(RobNode* mot_node) {
    reset();
    motion_node = mot_node;
    motion_matrix = mot_node->mat_ptr;

    mat4* v7 = parts_matrix;
    int32_t* v8 = parts_attr;
    for (int32_t i = 0; i < RPK_MAX; i++) {
        skin_disp[i].init_members(i);
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

void rob_chara_item_equip::reset_nodes_ex_force(rob_osage_parts parts) {
    skin_disp[parts == ROB_OSAGE_PARTS_MUFFLER ? RPK_OUTER : RPK_KAMI]
        .reset_nodes_ex_force((rob_osage_parts_bit)(1 << parts));
}

void rob_chara_item_equip::set_alpha_obj_flags(float_t alpha, mdl::ObjFlags flags) {
    if (one_skin)
        for (int32_t i = disp_begin; i < disp_max; i++)
            skin_disp[i].set_alpha_obj_flags(alpha, flags);
    else
        for (int32_t i = RPK_DISP_BEGIN; i <= RPK_DISP_END; i++)
            skin_disp[i].set_alpha_obj_flags(alpha, flags);
}

void rob_chara_item_equip::set_disable_collision(rob_osage_parts parts, bool disable) {
    skin_disp[parts == ROB_OSAGE_PARTS_MUFFLER ? RPK_OUTER : RPK_KAMI]
        .set_disable_collision((rob_osage_parts_bit)(1 << parts), disable);
}

void rob_chara_item_equip::set_disp(ROB_PARTS_KIND rpk, bool value) {
    skin_disp[rpk].can_disp = value;
}

void rob_chara_item_equip::set_item(object_info  obj_uid, ROB_PARTS_KIND rpk,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (rpk < 0 || rpk >= RPK_MAX || !motion_node)
        return;

    skin_disp[rpk].set(obj_uid, motion_node, osage_reset, bone_data, data, obj_db);
    set_disp(rpk, true);
}

void rob_chara_item_equip::set_motion_reset_data(uint32_t motion_id, float_t frame) {
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].set_motion_reset_data(motion_id, frame);

    task_wind->stage_wind.wind->reset();
}

void rob_chara_item_equip::set_motion_skin_param(int8_t chara_id, uint32_t motion_id, int32_t frame) {
    for (int32_t i = RPK_ITEM_BEGIN; i <= RPK_ITEM_END; i++)
        skin_disp[i].set_motion_skin_param(chara_id, motion_id, frame);
}

void rob_chara_item_equip::set_null_blocks_expression_data(ROB_PARTS_KIND rpk,
    const vec3& position, const vec3& rotation, const vec3& scale) {
    skin_disp[rpk].set_null_blocks_expression_data(position, rotation, scale);
}

void rob_chara_item_equip::set_object_texture_pattern(ROB_PARTS_KIND rpk,
    texture_pattern_struct* tex_pat, size_t count) {
    skin_disp[rpk].set_texture_pattern(tex_pat, count);
}

void rob_chara_item_equip::set_one_skin(bool value) {
    one_skin = value;
    if (value) {
        disp_begin = RPK_1SKIN_BEGIN;
        disp_max = RPK_1SKIN_MAX;
    }
    else {
        disp_begin = RPK_DISP_BEGIN;
        disp_max = RPK_DISP_END;
    }
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
        v11.use_blend = false;
#if OPD_PLAY_GEN
        v11.no_loop = false;
#endif
        v11.type = MOTION_BLEND_NONE;
        v11.blend = 0.0f;

        if (i->get_blend_enable()) {
            v11.type = i->get_type();
            v11.use_blend = true;
            v11.blend = i->blend->blend;
        }
        opd_blend_data.push_back(v11);
    }

    if (opd_blend_data.size()) {
        ::opd_blend_data* v9 = &opd_blend_data.back();
        if (v9->use_blend && v9->type != MOTION_BLEND_FREEZE)
            v9->use_blend = false;
    }

    if (use_opd)
        for (int32_t i = RPK_ITEM_BEGIN; i <= RPK_ITEM_END; i++)
            skin_disp[i].check_no_opd(opd_blend_data);
}

void rob_chara_item_equip::set_osage_reset() {
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].set_osage_reset();

    task_wind->stage_wind.wind->reset();
}

void rob_chara_item_equip::set_osage_step(float_t value) {
    osage_step = value;
}

void rob_chara_item_equip::set_osage_move_cancel(uint8_t id, const float_t& mv_ccl) {
    switch (id) {
    case 0:
        for (int32_t i = disp_begin; i < disp_max; i++)
            skin_disp[i].set_osage_move_cancel(mv_ccl);
        break;
    case 1:
        skin_disp[RPK_KAMI].set_osage_move_cancel(mv_ccl);
        break;
    case 2:
        skin_disp[RPK_OUTER].set_osage_move_cancel(mv_ccl);
        break;
    }
}

void rob_chara_item_equip::set_osage_play_data_init(ROB_PARTS_KIND rpk, const float_t* opdi_data) {
    if (rpk >= RPK_ITEM_BEGIN && rpk <= RPK_ITEM_END)
        skin_disp[rpk].set_osage_play_data_init(opdi_data);
}

void rob_chara_item_equip::set_shadow_group(int32_t chara_id) {
    if (!chara_id)
        shadow_group = SHADOW_GROUP_CHARA;
    else
        shadow_group = SHADOW_GROUP_STAGE;
}

void rob_chara_item_equip::set_texture_pattern(texture_pattern_struct* tex_pat, size_t count) {
    hyoutan_texchg_list.clear();
    if (count && tex_pat)
        for (size_t i = 0; i < count; i++)
            hyoutan_texchg_list.push_back(tex_pat[i]);
}

void rob_chara_item_equip::skp_load(ROB_PARTS_KIND rpk, const skin_param_osage_root& skp_root,
    std::vector<skin_param_osage_node>& vec, skin_param_file_data* skp_file_data, const bone_database* bone_data) {
    if (rpk >= 0 && rpk < RPK_MAX)
        skin_disp[rpk].skp_load(skp_root, vec, skp_file_data, bone_data);
}

item_cos_texture_change_tex::item_cos_texture_change_tex() : org(), chg(), changed() {

}

item_cos_texture_change_tex::~item_cos_texture_change_tex() {

}

rob_chara_item_cos_data::rob_chara_item_cos_data() : curr_chara_num(), chara_num(), curr_cos(), cos() {

}

rob_chara_item_cos_data::~rob_chara_item_cos_data() {

}

bool rob_chara_item_cos_data::check_for_npr_flag() {
    for (int32_t i = ITEM_SUB_ZUJO; i < ITEM_SUB_MAX; i++) {
        const item_table_item* item = item_table_handler_array_get_item(chara_num, cos.arr[i]);
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
        const item_table_item* item = item_table_handler_array_get_item(chara_num, cos.arr[i]);
        if (item && max_face_depth < item->face_depth)
            max_face_depth = item->face_depth;
    }
    return max_face_depth;
}

static void sub_140526FD0(rob_chara_item_cos_data* item_cos_data,
    int32_t item_no, const item_table_item* item) {
    if (!(item->attr & 0x0C))
        return;

    if (!(item->attr & 0x08)) {
        std::vector<item_cos_texture_change_tex> tex_chg_vec;
        for (const item_table_item_data_tex& i : item->data.tex) {
            item_cos_texture_change_tex chg_tex;
            chg_tex.org = texture_manager_get_texture(i.org);
            chg_tex.chg = texture_manager_get_texture(i.chg);
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
        texture* tex = texture_manager_get_texture(i);
        if (!tex) {
            index++;
            continue;
        }

        bool changed = false;
        if (item->data.col[j].flag & 0x01) {
            tex = texture_create_copy_texture_apply_color_tone(
                texture_manager_get_copy_id(0x30), tex, &item->data.col[j].col_tone);
            changed = true;
        }

        item_cos_texture_change_tex chg_tex;
        chg_tex.org = texture_manager_get_texture(item->data.col[j].tex_id);
        chg_tex.chg = tex;
        chg_tex.changed = changed;
        tex_chg_vec.push_back(chg_tex);
    }
    item_cos_data->texture_change.insert({ item_no, tex_chg_vec });
}

static void sub_140527280(rob_chara_item_cos_data* item_cos_data,
    int32_t item_no, const item_table_item* item) {
    if (!(item->attr & 0x20) || !(item->attr & 0x0C))
        return;

    for (const item_table_item_data_obj& i : item->data.obj) {
        if (i.rpk == RPK_NONE)
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

        const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_num, item_no);
        if (!item)
            continue;

        sub_140526FD0(item_cos_data, item_no, item);
        sub_140527280(item_cos_data, item_no, item);
    }
}

static void sub_140522A30(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_disp,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    const RobData* rob_data = get_rob_data(item_cos_data->chara_num);
    for (int32_t i = RPK_BASE_BEGIN; i <= RPK_BASE_END; i++) {
        rob_disp->load_outfit_object_info((ROB_PARTS_KIND)i,
            rob_data->body_obj_uid[i], false, bone_data, data, obj_db);
        auto elem = item_cos_data->item_change.find(i);
        if (elem != item_cos_data->item_change.end())
            item_cos_data->set_texture_pattern(rob_disp, elem->second, (ROB_PARTS_KIND)i, false);
        else
            item_cos_data->set_texture_pattern(rob_disp, (ROB_PARTS_KIND)i, false);
    }

    for (int32_t i = RPK_ITEM_BEGIN; i <= RPK_ITEM_END; i++) {
        rob_disp->load_body_parts_object_info((ROB_PARTS_KIND)i, {}, bone_data, data, obj_db);
        item_cos_data->set_texture_pattern(rob_disp, (ROB_PARTS_KIND)i, false);
    }
    item_cos_data->set_texture_pattern(rob_disp, RPK_NONE, true);

    item_cos_data->field_F0.clear();
    item_cos_data->field_100.clear();
    item_cos_data->head_replace.clear();
}

static ROB_PARTS_KIND sub_140525B90(rob_chara_item_cos_data* item_cos_data, item_sub_id sub_id) {
    const ROB_PARTS_KIND v3[] = {
        RPK_ZUJO, RPK_KAMI, RPK_NONE, RPK_NONE,
        RPK_MEGANE, RPK_NONE, RPK_KUCHI, RPK_NONE,
        RPK_KUBI, RPK_NONE, RPK_OUTER, RPK_NONE,
        RPK_NONE, RPK_NONE, RPK_NONE, RPK_NONE,
        RPK_JOHA_USHIRO, RPK_NONE, RPK_NONE, RPK_PANTS,
        RPK_NONE, RPK_NONE, RPK_NONE, RPK_NONE,
        RPK_NONE,
    };

    return v3[sub_id];
}

static ROB_PARTS_KIND sub_140512EF0(rob_chara_item_equip* rob_disp, ROB_PARTS_KIND rpk) {
    int32_t _rpk = rpk;
    if (rpk < 0 || rpk >= RPK_DISP_END)
        return RPK_NONE;

    for (rob_chara_item_equip_object* i = &rob_disp->skin_disp[rpk];
        i->obj_uid.not_null(); i++) {
        if (++_rpk >= RPK_DISP_END)
            return RPK_NONE;
    }
    return (ROB_PARTS_KIND)_rpk;
}

static void sub_140513B90(rob_chara_item_equip* rob_disp, ROB_PARTS_KIND rpk, object_info a3, object_info a4,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (rpk <= RPK_BODY || rpk >= RPK_MAX) {
        a4 = {};
        a3 = {};
    }
    rob_disp->hyoutan_obj = a4;
    rob_disp->hyoutan_rpk = rpk;
    rob_disp->set_item(a3, rpk, false, bone_data, data, obj_db);
    rob_disp->set_disp(rpk, false);
}

static void sub_14052C560(rob_chara_item_cos_data* item_sub_data,
    rob_chara_item_equip* rob_disp, int32_t item_no, const item_table_item* item, item_sub_id sub_id,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    for (const item_table_item_data_obj& i : item->data.obj) {
        if (i.obj_info.is_null())
            break;

        if (item->type == 1) {
            if (i.rpk != RPK_NONE) {
                rob_disp->load_outfit_object_info(i.rpk,
                    i.obj_info, false, bone_data, data, obj_db);
                if (item->attr & 0x0C)
                    item_sub_data->set_texture_pattern(rob_disp, item_no, i.rpk, false);
            }
            continue;
        }

        else if (item->type)
            continue;

        ROB_PARTS_KIND rpk = sub_140525B90(item_sub_data, sub_id);
        if (rpk != RPK_NONE) {
            rob_disp->load_body_parts_object_info(rpk, i.obj_info, bone_data, data, obj_db);
            item_sub_data->field_F0.insert({ i.obj_info, rpk });
            if (item->attr & 0x0C)
                item_sub_data->set_texture_pattern(rob_disp, item_no, rpk, false);
        }
        else {
            ROB_PARTS_KIND rpk = sub_140512EF0(rob_disp, RPK_ITEM09);
            if (rpk != RPK_NONE && (!(item->attr & 0x10) || &i - item->data.obj.data())) {
                bool v8 = false;
                if (item->attr & 0x10 && &i - item->data.obj.data() == 1) {
                    sub_140513B90(rob_disp, rpk, item->data.obj[1].obj_info,
                        item->data.obj[0].obj_info, bone_data, data, obj_db);
                    v8 = true;
                }
                else
                    rob_disp->load_body_parts_object_info(rpk, i.obj_info, bone_data, data, obj_db);
                item_sub_data->field_F0.insert({ i.obj_info, rpk });
                if (item->attr & 0x0C)
                    item_sub_data->set_texture_pattern(rob_disp, item_no, rpk, v8);
            }
        }
    }

    item_sub_data->head_replace.clear();
    if (!(item->attr & 0x800))
        return;

    const char* chara_name = get_chara_name(item_sub_data->chara_num);
    for (int32_t i = 0; i < 9; i++) {
        std::string buf = sprintf_s_string("%sITM%03d_ATAM_HEAD_%02d_SP__DIVSKN", chara_name, item_no, i);
        object_info obj_info = obj_db->get_object_info(buf.c_str());
        if (obj_info.not_null())
            item_sub_data->head_replace.insert({ i, obj_info });
    }
}

static void sub_140522C60(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_disp,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!item_table_handler_array_get_table(item_cos_data->chara_num))
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
            const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_num, item_no);
            if (item && item->type != 3 && item->attr & 0x01)
                sub_14052C560(item_cos_data, rob_disp, item_no, item, *v6, bone_data, data, obj_db);
        }
        v6++;
    }
}

static void sub_14052C8C0(rob_chara_item_cos_data* item_cos_data,
    rob_chara_item_equip* rob_disp, int32_t item_no, const item_table_item* item,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    for (const item_table_item_data_obj& i : item->data.obj) {
        object_info v11 = i.obj_info;
        ROB_PARTS_KIND rpk = i.rpk;
        if (item->type == 1) {
            if (v11.is_null()) {
                if (rpk == RPK_NONE)
                    continue;
            }
            else if (rpk == RPK_NONE) {
            }
            else if (i.rpk < 100 || i.rpk > 105) {
                rob_disp->load_outfit_object_info(rpk,
                    v11, false, bone_data, data, obj_db);
            }
            else {
                ROB_PARTS_KIND v13 = sub_140512EF0(rob_disp, RPK_KAMI);
                if (v13 == RPK_NONE)
                    continue;

                rob_disp->load_body_parts_object_info(v13, i.obj_info, bone_data, data, obj_db);
                item_cos_data->field_F0.insert({ i.obj_info, v13 });
                item_cos_data->field_100.insert({ rpk, v13 });
                rpk = v13;
            }
        }
        else if (!item->type) {
            rpk = sub_140512EF0(rob_disp, RPK_ITEM09);
            if (rpk != RPK_NONE) {
                rob_disp->load_body_parts_object_info(rpk, i.obj_info, bone_data, data, obj_db);
                item_cos_data->field_F0.insert({ i.obj_info, rpk });
                continue;
            }
        }
        else
            continue;

        if (item->attr & 0x0C)
            item_cos_data->set_texture_pattern(rob_disp, item_no, rpk, false);
    }
}

static void sub_140522D00(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_disp,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!item_table_handler_array_get_table(item_cos_data->chara_num))
        return;

    const item_sub_id dword_140A2BBC8[] = {
        ITEM_SUB_TE, ITEM_SUB_ASI, ITEM_SUB_MAX,
    };

    const item_sub_id* v6 = dword_140A2BBC8;
    while (*v6 != ITEM_SUB_MAX) {
        int32_t item_no = item_cos_data->cos.arr[*v6];
        if (item_no) {
            const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_num, item_no);
            if (item && item->type != 3 && item->attr & 0x01)
                sub_14052C8C0(item_cos_data, rob_disp, item_no, item, bone_data, data, obj_db);
        }
        v6++;
    }
}

static void sub_140525D90(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_disp,
    int32_t a3, const bone_database* bone_data, void* data, const object_database* obj_db) {
    auto elem = item_cos_data->field_100.find(a3);
    if (elem != item_cos_data->field_100.end())
        rob_disp->load_body_parts_object_info(elem->second, {}, bone_data, data, obj_db);
}

static void sub_140522D90(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_disp,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!item_table_handler_array_get_table(item_cos_data->chara_num))
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

        const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_num, item_no);
        if (!item || item->type != 1)
            continue;

        for (const item_table_item_data_obj& i : item->data.obj) {
            if (i.obj_info.is_null())
                continue;

            ROB_PARTS_KIND rpk = sub_140525B90(item_cos_data, *v6);
            rob_disp->load_body_parts_object_info(rpk, i.obj_info, bone_data, data, obj_db);
            item_cos_data->field_F0.insert({ i.obj_info, rpk });
            if (item->attr & 0x0C)
                item_cos_data->set_texture_pattern(rob_disp, item_no, rpk, false);

            if (i.rpk == RPK_NONE)
                continue;

            rob_disp->load_outfit_object_info(i.rpk,
                {}, false, bone_data, data, obj_db);
            switch (i.rpk) {
            case RPK_UDE_R:
                sub_140525D90(item_cos_data, rob_disp, 102, bone_data, data, obj_db);
                break;
            case RPK_UDE_L:
                sub_140525D90(item_cos_data, rob_disp, 100, bone_data, data, obj_db);
                break;
            case RPK_TE_R:
                sub_140525D90(item_cos_data, rob_disp, 103, bone_data, data, obj_db);
                break;
            case RPK_TE_L:
                sub_140525D90(item_cos_data, rob_disp, 101, bone_data, data, obj_db);
                break;
            case RPK_SUNE:
                sub_140525D90(item_cos_data, rob_disp, 104, bone_data, data, obj_db);
                break;
            case RPK_ASI:
                sub_140525D90(item_cos_data, rob_disp, 105, bone_data, data, obj_db);
                break;
            }
        }
        v6++;
    }
}

static void sub_140522F90(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_disp) {
    if (!item_table_handler_array_get_table(item_cos_data->chara_num))
        return;

    int32_t item_no = item_cos_data->cos.arr[ITEM_SUB_HADA];
    if (!item_no)
        return;

    const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_num, item_no);
    if (!item || item->data.col.size())
        return;

    vec3 blend_color = item->data.col[0].col_tone.blend;
    vec3 offset_color = item->data.col[0].col_tone.offset;
    vec3 blend_specular = 1.0f;
    vec3 offset_specular = 0.0f;
    if (item->data.col.size() > 1) {
        blend_specular = item->data.col[1].col_tone.blend;
        offset_specular = item->data.col[1].col_tone.offset;
    }

    for (int32_t i = RPK_ATAMA; i <= RPK_ITEM16; i++) {
        if (rob_disp->get_object_info((ROB_PARTS_KIND)i).is_null())
            continue;

        rob_chara_item_equip_object* skin_disp = &rob_disp->skin_disp[i];
        skin_disp->skn_col.type = 0;
        skin_disp->skn_col.blend_color = blend_color;
        skin_disp->skn_col.offset_color = offset_color;
        skin_disp->skn_col.blend_specular = blend_specular;
        skin_disp->skn_col.offset_specular = offset_specular;
    }
}

static void sub_140523230(rob_chara_item_cos_data* item_cos_data,
    rob_chara_item_equip* rob_disp, int32_t item_no) {
    if (!item_no)
        return;

    const item_table_item* item = item_table_handler_array_get_item(item_cos_data->chara_num, item_no);
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
                    rob_disp->set_null_blocks_expression_data(
                        elem->second, i.position, i.rotation, i.scale);
            }
            break;
        }

    for (const item_table_item_data_obj& i : item->data.obj) {
        if (i.obj_info.is_null())
            break;

        auto elem = item_cos_data->field_F0.find(i.obj_info);
        if (elem != item_cos_data->field_F0.end()) {
            ROB_PARTS_KIND rpk = elem->second;
            sub_140514130(rob_disp, rpk, !(item->attr & 0x80));
            sub_140514110(rob_disp, rpk, !(item->attr & 0x100));
            sub_140513C60(rob_disp, rpk, !(item->attr & 0x400));
        }
    }
}

static void sub_1405231D0(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_disp) {
    if (!item_table_handler_array_get_table(item_cos_data->chara_num))
        return;

    for (int32_t i = 0; i < ITEM_SUB_MAX; i++)
        sub_140523230(item_cos_data, rob_disp, item_cos_data->cos.arr[i]);
}

static void sub_1405234E0(rob_chara_item_cos_data* item_cos_data) {
    item_cos_data->item_change.clear();
    item_cos_data->field_F0.clear();
    item_cos_data->field_100.clear();
}

void rob_chara_item_cos_data::reload_items(int32_t chara_id,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    rob_chara_item_equip* rob_disp = rob_chara_array_get_rob_disp(chara_id);
    sub_140522990(this);
    sub_140522A30(this, rob_disp, bone_data, data, obj_db);
    sub_140522C60(this, rob_disp, bone_data, data, obj_db);
    sub_140522D00(this, rob_disp, bone_data, data, obj_db);
    sub_140522D90(this, rob_disp, bone_data, data, obj_db);
    sub_140522F90(this, rob_disp);
    sub_1405231D0(this, rob_disp);
    sub_1405234E0(this);
}

void rob_chara_item_cos_data::set_chara_num(CHARA_NUM chara_num) {
    this->chara_num = chara_num;
}

void rob_chara_item_cos_data::set_chara_num_item(CHARA_NUM chara_num, int32_t item_no) {
    set_chara_num(chara_num);
    set_item(item_no);
}

void rob_chara_item_cos_data::set_chara_num_item_nos(CHARA_NUM chara_num, const int32_t* items) {
    set_chara_num(chara_num);
    set_item_nos(items);
}

void rob_chara_item_cos_data::set_chara_num_item_zero(CHARA_NUM chara_num, int32_t item_no) {
    set_chara_num(chara_num);
    set_item_zero(item_no);
}

void rob_chara_item_cos_data::set_item(int32_t item_no) {
    if (!item_no)
        return;

    const item_table_item* item = item_table_handler_array_get_item(chara_num, item_no);
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
    const item_table_item* item = item_table_handler_array_get_item(chara_num, item_no);
    if (item)
        cos.arr[item->sub_id] = 0;
}

void rob_chara_item_cos_data::set_texture_pattern(rob_chara_item_equip* rob_disp,
    ROB_PARTS_KIND rpk, bool tex_pat_for_all) {
    if (tex_pat_for_all)
        rob_disp->set_texture_pattern(0, 0);
    else
        rob_disp->set_object_texture_pattern(rpk, 0, 0);
}

void rob_chara_item_cos_data::set_texture_pattern(rob_chara_item_equip* rob_disp,
    uint32_t item_no, ROB_PARTS_KIND rpk, bool tex_pat_for_all) {
    if (rpk < 0 || rpk >= RPK_MAX)
        return;

    auto elem = texture_change.find(item_no);
    if (elem == texture_change.end())
        return;

    for (item_cos_texture_change_tex& i : elem->second)
        if (i.org && i.chg)
            texture_pattern[rpk].push_back({ i.org->id, i.chg->id });

    if (tex_pat_for_all)
        rob_disp->set_texture_pattern(texture_pattern[rpk].data(), texture_pattern[rpk].size());
    else
        rob_disp->set_object_texture_pattern(rpk,
            texture_pattern[rpk].data(), texture_pattern[rpk].size());
}

void rob_chara_item_cos_data::set_texture_pattern(rob_chara_item_equip* rob_disp,
    const std::vector<uint32_t>& item_nos, ROB_PARTS_KIND rpk, bool tex_pat_for_all) {
    if (rpk < 0 || rpk >= RPK_MAX)
        return;

    for (const uint32_t& i : item_nos) {
        auto elem = texture_change.find(i);
        if (elem == texture_change.end())
            continue;

        for (item_cos_texture_change_tex& j : elem->second)
            if (j.org && j.chg)
                texture_pattern[rpk].push_back({ j.org->id, j.chg->id });
    }

    if (tex_pat_for_all)
        rob_disp->set_texture_pattern(
            texture_pattern[rpk].data(), texture_pattern[rpk].size());
    else
        rob_disp->set_object_texture_pattern(rpk,
            texture_pattern[rpk].data(), texture_pattern[rpk].size());
}

void rob_chara_item_cos_data::texture_change_clear() {
    for (auto i : texture_change)
        for (item_cos_texture_change_tex& j : i.second)
            if (j.changed) {
                texture_release(j.chg);
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
field_190(), field_194(), field_198(), field_19C(), field_1A0(), kamae_type(), kamae_data(), field_1B0(), field_1B4(),
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

RobPartialMotion::RobPartialMotion() {

}

RobPartialMotion::~RobPartialMotion() {

}

// 0x14053F1B0
bool RobPartialMotion::CheckEnded() {
    if (!CheckPlaybackStateValid())
        return false;

    if (CheckPlaybackStateForward()) {
        if (data.loop_state >= ROB_PARTIAL_MOTION_LOOP_MAX && (data.frame_count - 1.0f) < data.frame)
            return true;
    }
    else if (CheckPlaybackStateBackward()) {
        if (data.loop_state >= ROB_PARTIAL_MOTION_LOOP_MAX && data.frame < 0.0f)
            return true;
    }
    else if (CheckPlaybackStateDuration()) {
        return data.play_duration <= 0.0f;
    }
    return false;
}

// 0x14053F270
bool RobPartialMotion::CheckPlaybackStateBackward() {
    return data.playback_state == ROB_PARTIAL_MOTION_PLAYBACK_BACKWARD;
}

// 0x14053F290
bool RobPartialMotion::CheckPlaybackStateCharaMotion() {
    return data.playback_state == ROB_PARTIAL_MOTION_PLAYBACK_CHARA_MOTION;
}

// 0x14053F2A0
bool RobPartialMotion::CheckPlaybackStateDuration() {
    return data.playback_state == ROB_PARTIAL_MOTION_PLAYBACK_DURATION;
}

// 0x14053F280
bool RobPartialMotion::CheckPlaybackStateForward() {
    return data.playback_state == ROB_PARTIAL_MOTION_PLAYBACK_FORWARD;
}

// 0x14053F260
bool RobPartialMotion::CheckPlaybackStateStop() {
    return data.playback_state == ROB_PARTIAL_MOTION_PLAYBACK_STOP;
}

// 0x14053F2B0
bool RobPartialMotion::CheckPlaybackStateValid() {
    return data.playback_state < ROB_PARTIAL_MOTION_PLAYBACK_MAX;
}

// 0x14053F2C0
void RobPartialMotion::GetFrameStep() {
    if (!CheckPlaybackStateCharaMotion())
        return;

    if (data.frame_data)
        data.frame = data.frame_data->frame;

    if (data.step_data)
        data.step = data.step_data->frame;
}

// 0x14053F300
void RobPartialMotion::Step() {
    if (!CheckPlaybackStateValid())
        return;

    if (CheckPlaybackStateForward()) {
        float_t frame = data.frame + data.step;
        float_t last_frame = data.frame_count - 1.0f;
        switch (data.loop_state) {
        case ROB_PARTIAL_MOTION_LOOP_ONCE:
            if (frame > last_frame)
                frame = last_frame;
            break;
        case ROB_PARTIAL_MOTION_LOOP_CONTINUOUS:
            if (frame >= last_frame)
                frame = fmodf(frame - last_frame, last_frame);
            break;
        case ROB_PARTIAL_MOTION_LOOP_RESET:
            if (frame > last_frame)
                frame = 0.0f;
            break;
        case ROB_PARTIAL_MOTION_LOOP_REVERSE:
            if (frame > last_frame) {
                data.playback_state = ROB_PARTIAL_MOTION_PLAYBACK_BACKWARD;
                frame = last_frame - fmodf(frame - last_frame, last_frame);
            }
            break;
        }
        data.frame = frame;
    }
    else if (CheckPlaybackStateBackward()) {
        float_t frame = data.frame - data.step;
        float_t last_frame = data.frame_count - 1.0f;
        switch (data.loop_state) {
        case ROB_PARTIAL_MOTION_LOOP_ONCE:
            if (frame < 0.0f)
                frame = 0.0f;
            break;
        case ROB_PARTIAL_MOTION_LOOP_CONTINUOUS:
            if (frame <= 0.0f)
                frame = last_frame - fmodf(-frame, last_frame);
            break;
        case ROB_PARTIAL_MOTION_LOOP_RESET:
            if (frame < 0.0f)
                frame = last_frame;
            break;
        case ROB_PARTIAL_MOTION_LOOP_REVERSE:
            if (frame < 0.0f) {
                data.playback_state = ROB_PARTIAL_MOTION_PLAYBACK_FORWARD;
                frame = fmodf(-frame, last_frame);
            }
            break;
        }
        data.frame = frame;
    }
    else if (CheckPlaybackStateDuration()) {
        if (data.play_duration >= 0.0f)
            data.play_duration -= data.step;
    }
    else if (CheckPlaybackStateCharaMotion()) {
        if (data.frame_data)
            data.frame = data.frame_data->frame;
        if (data.step_data)
            data.step = data.step_data->frame;
    }
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
    curr_ex_force = 0.0f;
    curr_force = 1.0f;
    curr_strength = 1.0f;
    motion_id = -1;
    set_frame = 0.0f;
    transition_duration = 0.0f;
    type = 6;
    cycle_type = 0;
    ignore_gravity = true;
    ex_force = 0.0f;
    ex_force_cycle_strength = 0.0f;
    ex_force_cycle = 0.0f;
    cycle = 1.0f;
    phase = 0.0f;
    force = 1.0f;
    strength = 1.0f;
    strength_transition = 0.0f;
}

rob_chara_data_hand_adjust::rob_chara_data_hand_adjust() : enable(), scale_select(), type(),
current_scale(), scale(), duration(), current_time(), rotation_blend(), scale_blend(),
enable_scale(), disable_x(), disable_y(), disable_z(), arm_length(), iterations() {
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
    target = 0.0f;
    arm_length = 0.0f;
    iterations = 0;
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
    mot_data = 0;
    data = 0;
    field_28 = 0;
}

mothead::mothead() {
    mot_set_id = -1;
    first_mot_id = 0;
    last_mot_id = 0;
    mots = 0;
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

rob_chara_data_miku_rot::rob_chara_data_miku_rot() : revise_type(), ground_pos(), ground_last() {
    reset();
}

void rob_chara_data_miku_rot::reset() {
    yang = 0;
    act_yang = 0;
    hara_xang = 0;
    hara_yang = 0;
    hara_zang = 0;
    revise_tag_yang = 0;
    revise_type = (ReviseType)0;
    ground_pos = 0.0f;
    ground_last = 0.0f;
    pos = 0.0f;
    gpos = 0.0f;
    spd = 0.0f;
    adjust_spd = 0.0f;
    old_gpos = 0.0f;
    velocity = 0.0f;
    pos_camera_old = 0.0f;
    rob_mat = mat4_identity;
}

rob_chara_adjust_data::rob_chara_adjust_data() : scale(), height_adjust(), pos_adjust_y(),
offset_x(), offset_y(), offset_z(), get_global_pos(), left_hand_scale(), right_hand_scale(),
left_hand_scale_default(), right_hand_scale_default(), item_scale() {
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
    get_global_pos = false;
    mat4_translate(&pos, &mat);
    left_hand_scale = -1.0f;
    right_hand_scale = -1.0f;
    left_hand_scale_default = -1.0f;
    right_hand_scale_default = -1.0f;
    mat4_translate(&item_pos, &item_mat); // X
    item_scale = 1.0f; // X
}

struc_195::struc_195() : scale(), field_1C(), field_20(), field_24() {

}

pos_scale::pos_scale() : scale() {

}

float_t pos_scale::get_screen_pos_scale(const mat4& mat,
    const vec3& pos, float_t scale, bool apply_offset) {
    vec4 v19;
    *(vec3*)&v19 = pos;
    v19.w = 1.0f;
    mat4_transform_vector(&mat, &v19, &v19);

    if (fabsf(v19.w) >= 1.0e-10f) {
        float_t v11 = v19.y * (1.0f / v19.w);
        float_t v12 = v19.x * (1.0f / v19.w) + 1.0f;
        ScreenParam& render_screen_param = get_render_screen_param();
        float_t v14 = (float_t)(v12 * 0.5f) * (float_t)render_screen_param.width;
        float_t v15 = (float_t)((1.0f - v11) * 0.5f) * (float_t)render_screen_param.height;
        if (apply_offset) {
            ScreenParam& screen_param = get_screen_param();
            v14 += (float_t)render_screen_param.xoffset;
            v15 += (float_t)(screen_param.height - render_screen_param.yoffset - render_screen_param.height);
        }
        this->pos.x = v14;
        this->pos.y = v15;
        this->scale = -v19.w;
        return fabsf(1.0f / v19.w) * (rctx_ptr->camera->depth * scale);
    }
    else {
        this->pos = 0.0f;
        this->scale = 0.0f;
        return 0.0f;
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

rob_chara_data::rob_chara_data() : flag(),
adjust_data(), field_3D90(), field_3D94(), field_3D98(), field_3D9A(), field_3D9C(), field_3DA0(),
field_3DA4(), field_3DA8(), field_3DB0(), field_3DB8(), field_3DBC(), field_3DC0(), field_3DC4(), field_3DC8(),
field_3DCC(), field_3DD0(), field_3DD4(), field_3DD8(), field_3DDC(), field_3DE0(), arm_adjust_scale() {

}

rob_chara_data::~rob_chara_data() {

}

void rob_chara_data::reset() {
    sub_140505C90(&action);
    rob_sub_action.Reset();
    motion.reset();
    motdata.reset();
    position.reset();
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
chara_num(), cos_id(), rob_data(), field_20() {
    frame_speed = 1.0f;
    bone_data = new rob_chara_bone_data();
    rob_disp = new rob_chara_item_equip();
}

rob_chara::~rob_chara() {
    delete bone_data;
    bone_data = 0;
    delete rob_disp;
    rob_disp = 0;
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

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;
    for (int32_t i = RPK_KAMI; i < RPK_MAX; i++) {
        rob_chara_item_equip_object* skin_disp = rob_disp->get_skin_disp((ROB_PARTS_KIND)i);
        object_info obj_info = skin_disp->obj_uid;
        if (obj_info.not_null() && skin_disp->osage_nodes_count) {
            size_t data = (size_t)GetOpdiFileData(obj_info, motion_id);
            if (!data
                || *((uint16_t*)(data + 0x04)) != (uint16_t)obj_info.id
                || *((uint16_t*)(data + 0x06)) != (uint16_t)obj_info.set_id
                || *((int32_t*)(data + 0x08)) != motion_id
                || *((uint16_t*)(data + 0x0C)) != skin_disp->osage_nodes_count)
                return false;
        }
    }
    return true;
}

void OpdMaker::Data::ReadOpdiFiles(rob_chara* rob_chr, std::vector<uint32_t>& motions) {
    if (!empty || !rob_chr)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;
    for (int32_t i = RPK_KAMI; i < RPK_MAX; i++) {
        rob_chara_item_equip_object* skin_disp = rob_disp->get_skin_disp((ROB_PARTS_KIND)i);
        if (skin_disp->obj_uid.is_null() || !skin_disp->osage_nodes_count)
            continue;

        object_info obj_info = skin_disp->obj_uid;
        const char* object_name = objset_info_storage_get_obj_name(obj_info);
        if (!object_name)
            continue;

        for (uint32_t& i : motions) {
            auto elem = opdi_files.insert({ { obj_info, i }, 0 }).first;

            const char* motion_name = aft_mot_db->get_motion_name(i);
            if (!motion_name)
                continue;

            std::string farc_path = string_to_lower(sprintf_s_string(
                "%s_%s.opdi", get_rom_osage_play_data_opdi_dir(), object_name));
            std::string file = string_to_lower(sprintf_s_string(
                "%s_%s.opdi", object_name, motion_name));

            p_file_handler* file_handler = new p_file_handler;
            file_handler->read_file(aft_data, farc_path.c_str(), file.c_str(), prj::MemCTemp, true);
            elem->second = file_handler;
        }
    }
    empty = false;
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

bool OpdMaker::CheckForFinalMotion() {
    std::unique_lock<std::mutex> u_lock(motion_mtx);
    return ++motion_index < motion_ids->size();
}

void OpdMaker::Ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    motion_index = 0;
    int32_t chara_id = rob_chr->chara_id;

    opd_chara_data_array_open_opd_file(chara_id);

    do {
        if (GetEnd()) {
            SetWaiting(false);
            return;
        }

        uint32_t motion_id = (*motion_ids)[motion_index];
        if (motion_id == -1)
            break;

        rob_chr->set_motion_id(motion_id, 0.0f, 0.0f,
            true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);

        float_t frame_count = (float_t)(int32_t)rob_chr->bone_data->get_frame_count() - 1.0f;
        int32_t iterations = 60;
        if (rob_chr->data.motdata.field_0.iterations > 0)
            iterations = rob_chr->data.motdata.field_0.iterations;

        float_t start_frame = 0.0f;
        bool no_opdi = true;
        if (data && data->IsValidOpdiFile(rob_chr, motion_id)) {
            no_opdi = false;

            int32_t frame_count_int = (int32_t)frame_count;
            int32_t iteration = frame_count_int;
            start_frame = frame_count - 1.0f;
            while (iteration < iterations) {
                iteration += frame_count_int;
                start_frame = (float_t)(iteration - 1);
            }
        }

        rob_osage_mothead osg_mhd(rob_chr, 0, motion_id, start_frame, aft_bone_data, aft_mot_db);
        if (no_opdi) {
            uint64_t frame = 0;
            int32_t iteration = 0;
            while (true) {
                osg_mhd.set_frame((float_t)(int64_t)frame);
                osg_mhd.ctrl();

                frame++;
                iteration++;
                if ((float_t)(int64_t)frame >= frame_count) {
                    if (iteration > iterations)
                        break;

                    frame = 0;
                    osg_mhd.init_data(aft_mot_db);
                }
            }
        }
        else {
            osg_mhd.ctrl();
            SetOsagePlayInitData(motion_id);
        }

        osg_mhd.init_data(aft_mot_db);

        opd_chara_data_array_init_data(chara_id, motion_id);

        uint64_t frame_int = 0;
        float_t frame = 0.0f;
        while (frame < frame_count) {
            osg_mhd.set_frame(frame);
            osg_mhd.ctrl();
            opd_chara_data_array_add_frame_data(chara_id);
            frame = (float_t)(int64_t)++frame_int;
        }

        opd_chara_data_array_encode_data(chara_id);
    } while (CheckForFinalMotion());

    opd_chara_data_array_write_file(chara_id);

    const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();

    std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, chara_id);

    std::vector<std::string> files = path_get_files(chara_dir.c_str());
    for (std::string& i : files) {
        if (!i.size())
            continue;

        std::string tmp_path = sprintf_s_string("%s/%s", chara_dir.c_str(), i.c_str());
        std::string ram_path = sprintf_s_string("%s/%s", ram_osage_play_data_tmp_path, i.c_str());

        if (path_check_file_exists(ram_path.c_str()))
            path_delete_file(ram_path.c_str());

        if (!path_check_file_exists(tmp_path.c_str()))
            continue;

        size_t pos = i.find(".farc");
        if (pos == -1)
            continue;

        std::string name(i.substr(0, pos));
        for (char& c : name)
            if (c >= 'a' && c <= 'z')
                c -= 0x20;

        size_t ver = 0;
        osage_play_data_database->get_ver_by_name(ver, name);

        uint32_t footer[2] = {};
        footer[0] = (uint32_t)ver;

        file_stream fs;
        fs.open(tmp_path.c_str(), "rb");
        if (fs.check_null())
            continue;

        size_t file_length = fs.get_length();
        fs.close();

        void* file_data = prj::MemoryManager::alloc(prj::MemCTemp, file_length, tmp_path.c_str());
        if (!file_data)
            continue;

        fs.open(tmp_path.c_str(), "rb");
        size_t read_length = fs.read(file_data, file_length);
        fs.close();

        if (!read_length) {
            prj::MemoryManager::free(prj::MemCTemp, file_data);
            continue;
        }

        adler_buf adler;
        adler.get_adler(file_data, file_length);
        footer[1] = adler.adler;

        fs.open(tmp_path.c_str(), "ab");
        if (fs.check_not_null())
            fs.write(footer, sizeof(footer));
        fs.close();
        prj::MemoryManager::free(prj::MemCTemp, file_data);
    }

    rob_chr = 0;
    motion_index = 0;
    SetWaiting(false);
}

bool OpdMaker::GetEnd() {
    std::unique_lock<std::mutex> u_lock(end_mtx);
    return end;
}

bool OpdMaker::InitThread(rob_chara* rob_chr, const std::vector<uint32_t>* motion_ids, OpdMaker::Data* data) {
    bool waiting = IsWaiting();
    if (waiting || this->rob_chr)
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
        std::wstring buf = swprintf_s_string(L"OPD Maker #%d", opd_maker_counter++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf.c_str());
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
    motion_index = 0;
    end = false;
    data = 0;
    thread = 0;
}

bool OpdMaker::SetOsagePlayInitData(uint32_t motion_id) {
    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;
    for (int32_t i = RPK_KAMI; i < RPK_MAX; i++) {
        rob_chara_item_equip_object* skin_disp = rob_disp->get_skin_disp((ROB_PARTS_KIND)i);
        if (skin_disp->obj_uid.is_null() || !skin_disp->osage_nodes_count)
            continue;

        size_t data = (size_t)this->data->GetOpdiFileData(skin_disp->obj_uid, motion_id);
        if (data)
            rob_disp->set_osage_play_data_init((ROB_PARTS_KIND)i, (float_t*)(data + 0x10));
    }
    return true;
}

void OpdMaker::SetEnd() {
    std::unique_lock<std::mutex> u_lock(end_mtx);
    end = true;
}

void OpdMaker::SetWaiting(bool value) {
    std::unique_lock<std::mutex> u_lock(waiting_mtx);
    waiting = value;
}

void OpdMaker::ThreadMain(OpdMaker* opd_maker) {
    opd_maker->Ctrl();
}

OpdMakeWorker::OpdMakeWorker(int32_t chara_id) : state(), items(), use_current_skp() {
    this->chara_id = chara_id;
}

OpdMakeWorker::~OpdMakeWorker() {

}

bool OpdMakeWorker::init() {
    data.Reset();
    state = use_current_skp ? 8 : 1;
    return true;
}

bool OpdMakeWorker::ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    CHARA_NUM chara_num = rob_chr->chara_num;
    const RobData* rob_data = get_rob_data(chara_num);
    switch (state) {
    case 1: {
        opd_make_manager->chara_data.PopItems(chara_num, items);

        for (int32_t i = 0; i < ITEM_SUB_MAX; i++) {
            rob_chr->item_cos_data.set_chara_num(chara_num);
            rob_chr->item_cos_data.set_item(items[i]);
        }

        rob_chr->rob_disp->reset_init_data(rob_chara_bone_data_get_node(
            rob_chara_array[chara_id].bone_data, BONE_NODE_N_HARA_CP));
        rob_chr->rob_disp->set_shadow_group(chara_id);
        rob_chr->rob_disp->shadow_flag = 0x05;
        state = 2;
    }
    case 2: {
        if (!task_rob_load_check_load_req_data()) {
            task_rob_load_append_load_req_data_obj(chara_num, rob_chr->item_cos_data.get_cos());
            state = 3;
        }
    } break;
    case 3: {
        if (task_rob_load_check_load_req_data())
            break;

        rob_chr->item_cos_data.reload_items(chara_id, aft_bone_data, aft_data, aft_obj_db);

        for (int32_t i = RPK_ITEM_BEGIN; i <= RPK_ITEM_END; i++) {
            rob_chara_item_equip_object* skin_disp = rob_chr->rob_disp->get_skin_disp((ROB_PARTS_KIND)i);
            if (!objset_info_storage_get_obj_name(skin_disp->obj_uid) && skin_disp->osage_nodes_count)
                continue;

            const object_info* body_obj_uid = &rob_data->body_obj_uid[RPK_BASE_BEGIN];
            object_info obj_uid = skin_disp->obj_uid;
            for (int32_t j = RPK_BASE_BEGIN; *body_obj_uid != obj_uid; j++, body_obj_uid++) {
                if (j <= RPK_BASE_END)
                    continue;

                if (opd_make_manager->use_opdi)
                    state = 4;
                else
                    state = 6;
                return false;
            }
        }
        state = 10;
    } break;
    case 4: {
        data.ReadOpdiFiles(rob_chr, opd_make_manager->motion_ids);
        state = 5;
    } break;
    case 5: {
        if (data.CheckOpdiFilesNotReady())
            break;
        state = 6;
    }
    case 6: {
        std::vector<osage_init_data> osage_init;
        for (const uint32_t& i : opd_make_manager->motion_ids)
            osage_init.push_back(osage_init_data(rob_chr, i));
        skin_param_manager_add_task(chara_id, osage_init);

        state = 7;
    } break;
    case 7: {
        if (skin_param_manager_check_task_ready(chara_id))
            break;
        state = 8;
    }
    case 8: {
        path_create_directory(get_ram_osage_play_data_tmp_dir());
        opd_maker_array[chara_id].InitThread(rob_chr, &opd_make_manager->motion_ids,
            opd_make_manager->use_opdi ? &data : 0);
        state = 9;
    } break;
    case 9: {
        if (opd_maker_array[chara_id].IsWaiting())
            break;

        data.Reset();

        const char* ram_osage_play_data_path = get_ram_osage_play_data_dir();
        const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();

        std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, chara_id);

        std::vector<std::string> files = path_get_files(chara_dir.c_str());
        for (std::string& i : files) {
            if (!i.size())
                continue;

            std::string tmp_path = sprintf_s_string("%s/%s", chara_dir.c_str(), i.c_str());
            std::string ram_path = sprintf_s_string("%s/%s", ram_osage_play_data_path, i.c_str());
            path_fs_copy_file(tmp_path.c_str(), ram_path.c_str());
            path_delete_file(tmp_path.c_str());
        }

        if (use_current_skp)
            return true;
        state = 10;
    }
    case 10: {
        task_rob_load_append_free_req_data_obj(chara_num, rob_chr->item_cos_data.get_cos());

        for (int32_t i = 0; i < ITEM_SUB_MAX; i++) {
            rob_chr->item_cos_data.set_chara_num(chara_num);
            rob_chr->item_cos_data.set_item_zero(items[i]);
        }

        rob_chr->item_cos_data.reload_items(chara_id, aft_bone_data, aft_data, aft_obj_db);
        skin_param_manager_reset(chara_id);
        if (opd_make_manager->chara_data.CheckNoItems(chara_num))
            return true;
        state = 1;
    } break;
    }
    return false;
}

bool OpdMakeWorker::dest() {
    if (opd_maker_array[chara_id].IsWaiting()) {
        opd_maker_array[chara_id].SetEnd();
        return false;
    }
    else {
        opd_maker_array[chara_id].Reset();
        return true;
    }
}

void OpdMakeWorker::disp() {

}

bool OpdMakeWorker::add_task(bool use_current_skp) {
    if (!task_rob_manager_check_chara_loaded(chara_id))
        return false;

    this->use_current_skp = use_current_skp;
    return app::TaskWork::add_task(this, "OPD_MAKE_WORKER");
}

bool OpdMakeWorker::del_task() {
    return del();
}

#if OPD_PLAY_GEN
inline OpdPlayGen::Object::Object() : file_hash(hash_fnv1a64m_empty), generate(true) {

}

inline OpdPlayGen::Object::Object(uint64_t file_hash) : file_hash(file_hash), generate(false) {

}

OpdPlayGen::OpdPlayGen() : state(), thread() {

}

OpdPlayGen::~OpdPlayGen() {

}

bool OpdPlayGen::CheckState() {
    switch (state) {
    default:
        if (thread) {
            thread->join();
            delete thread;
            thread = 0;
        }
        return true;
    case 1:
        if (skin_param_manager_array_check_task_ready())
            return false;

        state = 2;
        return false;
    case 2:
        state = 3;
        thread = new std::thread(OpdPlayGen::ThreadMain, this);
        if (thread)
            SetThreadDescription((HANDLE)thread->native_handle(), L"OPD Play");
        return false;
    case 3:
        return false;
    }
}

void OpdPlayGen::Ctrl() {
    if (state != 3)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    opd_play_gen_run = true;
    std::vector<uint32_t> motion_ids[6];
    for (auto& i : gen_data) {
        const uint32_t motion_id = i.first;

        for (int32_t chara_id = 0; chara_id < ROB_ID_MAX && i.second.size(); chara_id++) {
            rob_chara* rob_chr = rob_chara_array_get(chara_id);
            if (!rob_chr)
                continue;

            std::vector<pv_data_set_motion>& set_motion = this->set_motion[chara_id];

            if (set_motion.size()) {
                bool found_set_motion = false;
                for (pv_data_set_motion& j : set_motion)
                    if (j.motion_id == motion_id) {
                        found_set_motion = true;
                        break;
                    }

                if (!found_set_motion)
                    continue;
            }

            rob_chara_item_equip* rob_disp = rob_chr->rob_disp;

            for (int32_t j = RPK_ITEM_BEGIN; j < RPK_ITEM_END && i.second.size(); j++) {
                rob_chara_item_equip_object* skin_disp = rob_disp->get_skin_disp((ROB_PARTS_KIND)j);

                auto find = [](std::vector<std::pair<object_info, Object>>& vec, const object_info key) {
                    auto k = vec.begin();
                    size_t l = vec.size();
                    size_t temp;
                    while (l > 0) {
                        if (k[temp = l / 2].first >= key)
                            l /= 2;
                        else {
                            k += temp + 1;
                            l -= temp + 1;
                        }
                    }
                    if (k == vec.end() || key < k->first)
                        return vec.end();
                    return k;
                };

                auto elem = find(i.second, skin_disp->obj_uid);
                if (elem != i.second.end()) {
                    if (elem->second.generate) {
                        obj_info[chara_id][j - RPK_ITEM_BEGIN] = skin_disp->obj_uid;
                        motion_ids[chara_id].push_back(motion_id);
                    }

                    i.second.erase(elem);
                }
            }
        }
    }

    for (int32_t chara_id = 0; chara_id < ROB_ID_MAX; chara_id++) {
        std::vector<uint32_t>& _motion_ids = motion_ids[chara_id];
        std::sort(_motion_ids.begin(), _motion_ids.end());
        _motion_ids.erase(std::unique(_motion_ids.begin(), _motion_ids.end()), _motion_ids.end());
    }

    for (int32_t chara_id = 0; chara_id < ROB_ID_MAX; chara_id++) {
        rob_chara* rob_chr = rob_chara_array_get(chara_id);
        if (!rob_chr)
            continue;

        for (const uint32_t motion_id : motion_ids[chara_id]) {
            rob_chr->set_motion_id(motion_id, 0.0f, 0.0f,
                true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            //rob_chr->set_step_motion_step(1.0f);

            float_t frame_count = (float_t)(int32_t)rob_chr->bone_data->get_frame_count() - 1.0f;
            uint32_t iterations = 60;
            if (rob_chr->data.motdata.field_0.iterations > 0)
                iterations = rob_chr->data.motdata.field_0.iterations;

            bool has_opd = false;
            {
                const uint32_t* opd_motion_ids = get_opd_motion_ids();
                while (*opd_motion_ids != -1 && !has_opd) {
                    if (*opd_motion_ids == motion_id)
                        has_opd = true;
                    opd_motion_ids++;
                }

                const uint32_t motion_set_id = aft_mot_db->get_motion_set_id_by_motion_id(motion_id);
                const uint32_t* opd_motion_set_ids = get_opd_motion_set_ids();
                while (*opd_motion_set_ids != -1 && !has_opd) {
                    if (*opd_motion_set_ids == motion_set_id)
                        has_opd = true;
                    opd_motion_set_ids++;
                }
            }

            std::vector<pv_data_set_motion>& set_motion = this->set_motion[chara_id];

            bool has_osage_reset = false; // Added
            rob_osage_mothead osg_mhd(rob_chr, 0, motion_id, 0.0f, false, aft_bone_data, aft_mot_db);
            if (has_opd) {
                rob_osage_mothead_data* osage_reset = new rob_osage_mothead_data(
                    MOTHEAD_DATA_OSAGE_RESET, motion_id, aft_mot_db);
                const mothead_data* osage_reset_data = osage_reset->find_next_data();

                uint64_t frame = 0;
                uint64_t iteration = 0;
                while (true) {
                    osg_mhd.set_frame((float_t)(int64_t)frame);
                    osg_mhd.ctrl();

                    if (osage_reset_data) { // Added
                        if (osage_reset_data->frame == 0) {
                            uint32_t _motion_id = motion_id;
                            float_t _frame = (float_t)(int64_t)frame;

                            rob_chara_item_equip* rob_disp = rob_chr->rob_disp;
                            for (int32_t i = 0; i < 600; i++) // Should calm phys
                                osg_mhd.ctrl();

                            rob_chara_add_motion_reset_data(rob_chr, _motion_id, _frame, 0);
                            set_motion.push_back({ motion_id, { (float_t)(int64_t)frame, 0  } });
                            has_osage_reset = true;
                        }

                        osage_reset_data = 0;
                    }

                    frame++;
                    iteration++;
                    if ((float_t)(int64_t)frame >= frame_count) {
                        if (iteration > iterations)
                            break;

                        frame = 0;
                        osg_mhd.init_data(aft_mot_db);
                    }
                }

                if (osage_reset)
                    delete osage_reset;
                osage_reset_data = 0;
                osage_reset = 0;
            }

            osg_mhd.init_data(aft_mot_db);

            opd_chara_data_array_init_data(chara_id, motion_id);

            bool no_loop = has_osage_reset; // Added

            size_t set_motion_index = 0;
            uint64_t frame_int = 0;
            float_t frame = 0.0f;
            while (frame <= frame_count) {
                osg_mhd.set_frame(frame);

                while (set_motion_index < set_motion.size()) {
                    if (set_motion.data()[set_motion_index].motion_id != motion_id) {
                        set_motion_index++;
                        continue;
                    }

                    float_t _frame = (float_t)(int32_t)prj::roundf(
                        set_motion.data()[set_motion_index].frame_stage_index.first);
                    if (_frame > frame)
                        break;

                    rob_chr->set_motion_skin_param(motion_id, _frame);
                    rob_chr->set_motion_reset_data(motion_id, _frame);
                    set_motion_index++;
                }

                osg_mhd.ctrl();
                opd_chara_data_array_add_frame_data(chara_id);
                frame = (float_t)(int64_t)++frame_int;
            }

            opd_chara_data* opd_chr_data = opd_chara_data_array_get(chara_id);

            for (int32_t i = 0; i < RPK_ITEM_MAX; i++) {
                object_info obj_info = this->obj_info[chara_id][i];
                this->obj_info[chara_id][i] = {};
                if (obj_info.is_null())
                    continue;

                int32_t load_count = 1;
                auto elem = osage_play_data_manager->file_data.find({ obj_info, motion_id });
                if (elem != osage_play_data_manager->file_data.end())
                    continue;

                std::vector<std::vector<opd_vec3_data_vec>>& opd_data = opd_chr_data->opd_data[i];

                size_t nodes_count = 0;
                for (const std::vector<opd_vec3_data_vec>& j : opd_data)
                    nodes_count += j.size();

                int64_t _frame_count = (int64_t)frame_count;
                float_t* buf = (float_t*)malloc(sizeof(float_t) * 3ULL * _frame_count * nodes_count);
                if (!buf)
                    continue;

                opd_file_data data;
                memset(&data.head, 0, sizeof(data.head));
                data.head.obj_info.first = obj_info.id;
                data.head.obj_info.second = obj_info.set_id;
                data.head.motion_id = motion_id;
                data.head.frame_count = (uint32_t)_frame_count;
                data.head.nodes_count = (uint16_t)nodes_count;
                *(uint16_t*)((uint8_t*)&data.head + 0x12) = no_loop ? 0x01 : 0x00;
                data.data = buf;
                data.load_count = load_count;

                for (const std::vector<opd_vec3_data_vec>& j : opd_data)
                    for (const opd_vec3_data_vec& k : j) {
                        memcpy(buf, k.x.data(), _frame_count * sizeof(float_t));
                        buf += _frame_count;
                        memcpy(buf, k.y.data(), _frame_count * sizeof(float_t));
                        buf += _frame_count;
                        memcpy(buf, k.z.data(), _frame_count * sizeof(float_t));
                        buf += _frame_count;
                    }

                osage_play_data_manager->file_data.insert({ { obj_info, motion_id }, data });
            }

            opd_chara_data_array_encode_data(chara_id);
            opd_chr_data->reset();
        }

        motion_ids[chara_id].clear();
        motion_ids[chara_id].shrink_to_fit();
    }
    opd_play_gen_run = false;

    Reset();
}

void OpdPlayGen::Reset() {
    state = 0;

    gen_data.clear();

    for (int32_t i = 0; i < ROB_ID_MAX; i++)
        set_motion[i].clear();
}

void OpdPlayGen::ThreadMain(OpdPlayGen* opd_play) {
    opd_play->Ctrl();
}
#endif

osage_play_data_header::osage_play_data_header() : frame_count(), nodes_count() {
    signature = 0x11033115;
    obj_info = { -1, -1 };
    motion_id = -1;
}

opd_file_data::opd_file_data() : data(), load_count() {

}

void opd_file_data::unload() {
    if (--load_count > 0)
        return;

    head = {};
    prj::MemoryManager::free(prj::MemCTemp, data);
}

opd_vec3_data_vec::opd_vec3_data_vec() {

}

opd_vec3_data_vec::~opd_vec3_data_vec() {

}

opd_chara_data::opd_chara_data() : chara_id(), init(), frame_index(), frame_count(), motion_id(), field_18() {
    chara_id = opd_chara_data_counter++;
    reset();
}

opd_chara_data::~opd_chara_data() {

}

void opd_chara_data::add_frame_data() {
    if (!init || frame_index >= frame_count)
        return;

    rob_chara_item_equip* rob_disp = rob_chara_array_get(chara_id)->rob_disp;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++) {
        rob_chara_item_equip_object* skin_disp
            = rob_disp->get_skin_disp((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        std::vector<std::vector<opd_vec3_data_vec>>& opd_data = this->opd_data[i];

        size_t node_index = 0;
        for (ExOsageBlock*& j : skin_disp->osage_blk) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];

            RobJointNode* k_begin = j->osage_work.joint_node_vec.data() + 1;
            RobJointNode* k_end = j->osage_work.joint_node_vec.data() + j->osage_work.joint_node_vec.size();
            size_t l = 0;
            for (RobJointNode* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.data()[frame_index] = k->reset_data.pos.x;
                opd_node_data.data()[l].y.data()[frame_index] = k->reset_data.pos.y;
                opd_node_data.data()[l].z.data()[frame_index] = k->reset_data.pos.z;
            }
        }

        for (ExClothBlock*& j : skin_disp->cloth) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];

            CLOTH_VERTEX* k_begin = j->cloth_work.vtxarg.data() + j->cloth_work.width;
            CLOTH_VERTEX* k_end = j->cloth_work.vtxarg.data() + j->cloth_work.vtxarg.size();
            size_t l = 0;
            for (CLOTH_VERTEX* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.data()[frame_index] = k->reset_data.pos.x;
                opd_node_data.data()[l].y.data()[frame_index] = k->reset_data.pos.y;
                opd_node_data.data()[l].z.data()[frame_index] = k->reset_data.pos.z;
            }
        }
    }
    frame_index++;
}

void opd_chara_data::encode_data() {
    if (!init || frame_index != frame_count)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    const char* motion_name = aft_mot_db->get_motion_name(this->motion_id);
    if (!motion_name)
        return;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;

    std::string buf = sprintf_s_string("%s/%d", get_ram_osage_play_data_tmp_dir(), chara_id);

    p_farc_write* opd = this->opd;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++, opd++) {
        rob_chara_item_equip_object* skin_disp
            = rob_disp->get_skin_disp((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        object_info obj_info = skin_disp->obj_uid;
        const char* object_name = objset_info_storage_get_obj_name(obj_info);
        if (!object_name)
            continue;

        const std::vector<std::vector<opd_vec3_data_vec>>& opd_data = this->opd_data[i];

        size_t nodes_count = 0;
        for (const std::vector<opd_vec3_data_vec>& j : opd_data)
            nodes_count += j.size();

        size_t max_size = sizeof(osage_play_data_header) + (6ULL * (frame_count + 2ULL) * nodes_count);
        uint8_t* data = prj::MemoryManager::alloc<uint8_t>(prj::MemCTemp, max_size, "OPD_ENCORD_BUF");
        if (!data)
            continue;

        memset(data, 0, max_size);

        osage_play_data_header* opd_head = (osage_play_data_header*)data;
        opd_head->signature = 0;
        opd_head->frame_count = frame_count;
        opd_head->motion_id = motion_id;
        opd_head->obj_info = { (uint16_t)obj_info.id, (uint16_t)obj_info.set_id };
        opd_head->nodes_count = (uint16_t)skin_disp->osage_nodes_count;

        size_t size = sizeof(osage_play_data_header);
        uint8_t* d = data + size;

        size_t osage_node_index = 0;
        bool higher_accuracy = false;
        for (const std::vector<opd_vec3_data_vec>& j : opd_data) {
            if (osage_node_index < skin_disp->osage_blk.size())
                higher_accuracy = skin_disp->osage_blk.data()[osage_node_index]->has_children_node;

            for (const opd_vec3_data_vec& k : j) {
                size_t size_x = 0;
                opd_encode_data(k.x, d, size_x, higher_accuracy);
                d += size_x;
                size += size_x;

                size_t size_y = 0;
                opd_encode_data(k.y, d, size_y, higher_accuracy);
                d += size_y;
                size += size_y;

                size_t size_z = 0;
                opd_encode_data(k.z, d, size_z, higher_accuracy);
                d += size_z;
                size += size_z;
            }
            osage_node_index++;
        }

        std::string buf = string_to_lower(sprintf_s_string("%s_%s.opd", object_name, motion_name));
        opd->add_file(data, size, buf);
        prj::MemoryManager::free(prj::MemCTemp, data);
    }

    reset();
}

void opd_chara_data::encode_init_data(uint32_t motion_id) {
    if (!init || frame_index != frame_count)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    const char* motion_name = aft_mot_db->get_motion_name(this->motion_id);
    if (!motion_name)
        return;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;

    p_farc_write* opdi = this->opdi;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++, opdi++) {
        rob_chara_item_equip_object* skin_disp
            = rob_disp->get_skin_disp((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        object_info obj_info = skin_disp->obj_uid;
        const char* object_name = objset_info_storage_get_obj_name(obj_info);
        if (!object_name)
            continue;

        size_t size = sizeof(osage_play_data_init_header) + sizeof(vec3) * 2 * skin_disp->osage_nodes_count;
        uint8_t* data = prj::MemoryManager::alloc<uint8_t>(prj::MemCTemp, size, "OPDI_WRITE_BUF");

        osage_play_data_init_header* opdi_head = (osage_play_data_init_header*)data;
        opdi_head->motion_id = motion_id;
        opdi_head->obj_info = { (uint16_t)obj_info.id, (uint16_t)obj_info.set_id };
        opdi_head->nodes_count = (uint16_t)skin_disp->osage_nodes_count;

        vec3* d = (vec3*)(data + sizeof(osage_play_data_init_header));

        for (ExOsageBlock*& j : skin_disp->osage_blk) {
            if (!(j->osage_work.joint_node_vec.size()))
                break;

            RobJointNode* k_begin = j->osage_work.joint_node_vec.data() + 1;
            RobJointNode* k_end = j->osage_work.joint_node_vec.data() + j->osage_work.joint_node_vec.size();
            for (RobJointNode* k = k_begin; k != k_end; k++) {
                d[0] = k->pos;
                d[1] = k->vec;
                d += 2;
            }
        }

        for (ExClothBlock*& j : skin_disp->cloth) {
            if (!(j->cloth_work.vtxarg.size()))
                break;

            CLOTH_VERTEX* k_begin = j->cloth_work.vtxarg.data() + j->cloth_work.width;
            CLOTH_VERTEX* k_end = j->cloth_work.vtxarg.data() + j->cloth_work.vtxarg.size();
            size_t l = 0;
            for (CLOTH_VERTEX* k = k_begin; k != k_end; k++, l++) {
                d[0] = k->pos;
                d[1] = k->vec;
                d += 2;
            }
        }

        std::string buf = string_to_lower(sprintf_s_string("%s_%s.opdi", object_name, motion_name));
        opdi->add_file(data, size, buf);
        prj::MemoryManager::free(prj::MemCTemp, data);
    }
}

void opd_chara_data::fs_copy_file() {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;

    std::string tmp_dir = sprintf_s_string("%s/%d", get_dev_ram_opdi_dir(), rob_chr->chara_id);

    for (int32_t i = 0; i < RPK_ITEM_MAX; i++) {
        rob_chara_item_equip_object* skin_disp
            = rob_disp->get_skin_disp((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;
        opdi[i].write_file();

        const char* object_name = objset_info_storage_get_obj_name(skin_disp->obj_uid);
        if (!object_name)
            continue;

        std::string tmp_path = sprintf_s_string("%s/%s.opdi", tmp_dir.c_str(), object_name);
        std::string ram_path = sprintf_s_string("%s/%s.opdi", get_dev_ram_opdi_dir(), object_name);
        path_fs_copy_file(tmp_path.c_str(), ram_path.c_str());
        path_delete_file(tmp_path.c_str());
    }

    path_delete_directory(tmp_dir.c_str());
}

void opd_chara_data::init_data(uint32_t motion_id) {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;
    int32_t frame_count = (int32_t)rob_chr->data.motdata.field_0.frame;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++) {
        rob_chara_item_equip_object* skin_disp
            = rob_disp->get_skin_disp((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        std::vector<std::vector<opd_vec3_data_vec>>& opd_data = this->opd_data[i];
        opd_data.clear();
        opd_data.resize(skin_disp->cloth.size() + skin_disp->osage_blk.size());

        size_t node_index = 0;
        for (ExOsageBlock*& j : skin_disp->osage_blk) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];
            opd_node_data.resize(j->osage_work.joint_node_vec.size() - 1);

            RobJointNode* k_begin = j->osage_work.joint_node_vec.data() + 1;
            RobJointNode* k_end = j->osage_work.joint_node_vec.data() + j->osage_work.joint_node_vec.size();
            size_t l = 0;
            for (RobJointNode* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.resize(frame_count);
                opd_node_data.data()[l].y.resize(frame_count);
                opd_node_data.data()[l].z.resize(frame_count);
            }
        }

        for (ExClothBlock*& j : skin_disp->cloth) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];
            opd_node_data.resize(j->cloth_work.vtxarg.size() - j->cloth_work.width);

            CLOTH_VERTEX* k_begin = j->cloth_work.vtxarg.data() + j->cloth_work.width;
            CLOTH_VERTEX* k_end = j->cloth_work.vtxarg.data() + j->cloth_work.vtxarg.size();
            size_t l = 0;
            for (CLOTH_VERTEX* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.resize(frame_count);
                opd_node_data.data()[l].y.resize(frame_count);
                opd_node_data.data()[l].z.resize(frame_count);
            }
        }
    }

    frame_index = 0;
    this->frame_count = frame_count;
    init = true;
    this->motion_id = motion_id;
}

void opd_chara_data::open_opd_file() {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;

    p_farc_write* opd = this->opd;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++, opd++) {
        rob_chara_item_equip_object* skin_disp
            = rob_disp->get_skin_disp((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();
        path_create_directory(ram_osage_play_data_tmp_path);

        std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, chara_id);
        if (!path_check_directory_exists(chara_dir.c_str()))
            path_create_directory(chara_dir.c_str());

        const char* object_name = objset_info_storage_get_obj_name(skin_disp->obj_uid);
        if (!object_name)
            continue;

        std::string tmp_path = sprintf_s_string("%s/%s.farc", chara_dir.c_str(), object_name);
        opd->open(tmp_path.c_str(), true, 1);
    }
}

void opd_chara_data::open_opdi_file() {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;

    p_farc_write* opdi = this->opdi;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++, opdi++) {
        rob_chara_item_equip_object* skin_disp
            = rob_disp->get_skin_disp((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();
        path_create_directory(ram_osage_play_data_tmp_path);

        std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, rob_chr->chara_id);
        if (!path_check_directory_exists(chara_dir.c_str()))
            path_create_directory(chara_dir.c_str());

        const char* object_name = objset_info_storage_get_obj_name(skin_disp->obj_uid);
        if (!object_name)
            continue;

        std::string tmp_path = sprintf_s_string("%s/%s.opdi", chara_dir.c_str(), object_name);
        opdi->open(tmp_path.c_str(), true, 1);
    }
}

void opd_chara_data::reset() {
    init = false;
    motion_id = -1;
    frame_index = 0;
    frame_count = 0;
    field_18 = 0;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++)
        opd_data[i].clear();
}

void opd_chara_data::write_file() {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++)
        if (rob_disp->get_skin_disp((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i))->osage_nodes_count)
            opd[i].write_file();
}

OsagePlayDataManager::OsagePlayDataManager() : state() {
    Reset();
    state = 0;
}

OsagePlayDataManager::~OsagePlayDataManager() {

}

bool OsagePlayDataManager::init() {
    LoadOpdFileList();
    state = 0;
    return true;
}

bool OsagePlayDataManager::ctrl() {
    if (state)
        return false;

    for (auto i = file_handlers.begin(); i != file_handlers.end();) {
        if (!(*i)->check_not_ready()) {
            LoadOpdFile(*i);
            delete (*i);
            i = file_handlers.erase(i);
        }
        else
            i++;
    }

    bool ret = !file_handlers.size();
#if OPD_PLAY_GEN
    if (ret && opd_play_gen && !opd_play_gen->CheckState())
        return false;
#endif
    return ret;
}

bool OsagePlayDataManager::dest() {
#if OPD_PLAY_GEN
    if (opd_play_gen) {
        delete opd_play_gen;
        opd_play_gen = 0;
    }
#endif

    return true;
}

void OsagePlayDataManager::disp() {

}

bool OsagePlayDataManager::add() {
    return app::TaskWork::add_task(this, "OSAGE_PLAY_DATA_MANAGER");
}

void OsagePlayDataManager::AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids) {
    if (CheckTaskReady() || !rob_chr)
        return;

    rob_chara_item_equip* rob_disp = rob_chr->rob_disp;
    for (int32_t i = RPK_ITEM_BEGIN; i < RPK_ITEM_END; i++) {
        rob_chara_item_equip_object* skin_disp = rob_disp->get_skin_disp((ROB_PARTS_KIND)i);
        if (!skin_disp || skin_disp->obj_uid.is_null()
            || (!skin_disp->osage_blk.size() && !skin_disp->cloth.size()))
            continue;

        for (const uint32_t j : motion_ids)
            req_data.push_back(skin_disp->obj_uid, j);
    }
}

bool OsagePlayDataManager::CheckTaskReady() {
    return app::TaskWork::check_task_ready(this);
}

void OsagePlayDataManager::GetOpdFileData(object_info obj_info,
    uint32_t motion_id, const float_t*& data, uint32_t& count) {
    data = 0;
    count = 0;

    auto elem = file_data.find({ obj_info, motion_id });
    if (elem != file_data.end()) {
        data = elem->second.data;
        count = elem->second.head.frame_count;

#if OPD_PLAY_GEN
        osage_play_data_header* head = &elem->second.head;
        if (*(uint16_t*)((uint8_t*)head + 0x12) & 0x01)
            count |= 0x80000000u;
#endif
    }
}

void OsagePlayDataManager::LoadOpdFile(p_file_handler* pfhndl) {
#if OPD_PLAY_GEN
    if (pfhndl->get_data() && pfhndl->get_size() == 0x14
        && *(uint32_t*)pfhndl->get_data() == 'OPDP') {
        if (!opd_play_gen)
            opd_play_gen = new OpdPlayGen;

        uint64_t file_hash = hash_string_fnv1a64m(pfhndl->ptr->file);
        for (auto& i : opd_play_gen->gen_data)
            for (auto& j : i.second)
                if (j.second.file_hash == file_hash) {
                    j.second.generate = true;
                    opd_play_gen->state = 1;
                    return;
                }
        return;
    }
#endif

    opd_file_data data;
    if (opd_decode_data(pfhndl->get_data(), data.data, data.head))
        file_data.insert({ { object_info(data.head.obj_info.first,
            data.head.obj_info.second), data.head.motion_id }, data });
}

void OsagePlayDataManager::LoadOpdFileList() {
#if OPD_PLAY_GEN
    prj::vector_pair<uint32_t, std::vector<std::pair<object_info, OpdPlayGen::Object>>> req_data_mot;
#endif

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    prj::sort_unique(req_data);

    auto i_begin = req_data.begin();
    auto i_end = req_data.end();
    for (auto i = i_begin; i != i_end; ) {
        auto elem = file_data.find(*i);
        if (elem != file_data.end()) {
            elem->second.load_count++;

            i = req_data.erase(i);
            i_end = req_data.end();
            continue;
        }

        const char* object_name = objset_info_storage_get_obj_name(i->first);
        if (object_name) {
            std::string obj_name_buf = string_to_lower(object_name);

            const char* motion_name = aft_mot_db->get_motion_name(i->second);
            if (motion_name) {
                std::string mot_name_buf = string_to_lower(motion_name);

                std::string file_buf = sprintf_s_string("%s_%s.%s",
                    obj_name_buf.c_str(), mot_name_buf.c_str(), "opd");

#if OPD_PLAY_GEN
                size_t idx = -1;
                if (sv_opd_play_gen) {
                    for (auto& j : req_data_mot)
                        if (j.first == i->second) {
                            idx = &j - req_data_mot.data();
                            break;
                        }

                    if (idx == -1) {
                        idx = req_data_mot.size();
                        req_data_mot.push_back(i->second, {});
                    }

                    req_data_mot.data()[idx].second.push_back({ i->first, hash_string_fnv1a64m(file_buf) });
                }
#endif

                std::string farc_buf = OsagePlayDataManager::GetOpdFarcFilePath(
                    sprintf_s_string("%s.farc", obj_name_buf.c_str()));
                if (aft_data->check_file_exists(farc_buf.c_str())) {
                    file_handlers.push_back(new p_file_handler);
                    file_handlers.back()->read_file(aft_data, farc_buf.c_str(), file_buf.c_str(), prj::MemCTemp, true);
                    i++;
                    continue;
                }

#if OPD_PLAY_GEN
                if (sv_opd_play_gen)
                    req_data_mot.data()[idx].second.back().second.generate = true;
#endif
            }
        }

        i = req_data.erase(i);
        i_end = req_data.end();
    }
    req_data.clear();

#if OPD_PLAY_GEN
    if (req_data_mot.size()) {
        if (!opd_play_gen)
            opd_play_gen = new OpdPlayGen;

        bool generate = false;
        for (auto& i : req_data_mot) {
            auto& object = opd_play_gen->gen_data[i.first];
            object.insert(object.end(), i.second.begin(), i.second.end());

            std::sort(object.begin(), object.end());
            object.erase(std::unique(object.begin(), object.end()), object.end());

            for (auto& j : i.second)
                if (j.second.generate) {
                    generate = true;
                    break;
                }
        }

        if (generate)
            opd_play_gen->state = 1;
    }
#endif
}

void OsagePlayDataManager::Reset() {
#if OPD_PLAY_GEN
    if (opd_play_gen)
        opd_play_gen->Reset();
#endif

    file_handlers.clear();
    req_data.clear();
    for (auto& i : file_data)
        i.second.unload();
    file_data.clear();
}

std::string OsagePlayDataManager::GetOpdFarcFilePath(const std::string& path) {
    data_struct* aft_data = &data_list[DATA_AFT];

    std::string ram_path_buf = std::string(get_ram_osage_play_data_dir()) + "/" + path;
    if (aft_data->check_file_exists(ram_path_buf.c_str()))
        return ram_path_buf;

    return std::string(get_rom_osage_play_data_dir()) + "/" + path;
}

osage_play_data_database_struct::osage_play_data_database_struct() {
    reset();
}

osage_play_data_database_struct::~osage_play_data_database_struct() {
    reset();
}

size_t& osage_play_data_database_struct::find(const std::string& key) {
    auto elem = map.find(key);
    if (elem == map.end() || key.compare(elem->first) < 0)
        elem = map.insert({ key, 0 }).first;
    return elem->second;
}

size_t& osage_play_data_database_struct::get_ver_by_name(size_t& ver, const std::string& name) {
    ver = 0;

    auto elem = map.find(name);
    if (elem == map.end())
        ver = elem->second;
    return ver;
}

void osage_play_data_database_struct::load(const std::string& path) {
    data_struct* aft_data = &data_list[DATA_AFT];
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    key_val kv;
    aft_data->load_file(&kv, path.c_str(), key_val::load_file);

    if (!kv.open_scope("file")) {
        kv.close_scope();
        return;
    }

    uint32_t count = 0;
    if (!kv.read("length", count) || !count)
        return;

    for (uint32_t j = 0; j < count; j++) {
        if (!kv.open_scope_fmt(j))
            continue;

        std::string name;
        if (kv.read("name", name) && aft_obj_db->get_object_info(name.c_str()).not_null()) {
            int32_t ver;
            if (kv.read("ver", ver))
                find(name) = ver;
        }
        kv.close_scope();
    }
    kv.close_scope();
}

void osage_play_data_database_struct::reset() {
    map.clear();
}

inline rob_osage_mothead_data::rob_osage_mothead_data(mothead_data_type type,
    uint32_t motion_id, const motion_database* mot_db) {
    data = 0;
    this->type = type;
    data = mothead_storage_get_mot_by_motion_id(motion_id, mot_db)->data;
    init = true;
}

inline rob_osage_mothead_data::~rob_osage_mothead_data() {

}

inline const mothead_data* rob_osage_mothead_data::find_next_data() {
    if (!data)
        return 0;

    if (init)
        init = false;
    else
        data++;

    const mothead_data* data = this->data;
    if (data && data->type >= MOTHEAD_DATA_TYPE_0) {
        mothead_data_type type = data->type;
        while (type != this->type) {
            data++;
            type = data->type;
            if (type < MOTHEAD_DATA_TYPE_0) {
                data = 0;
                break;
            }
        }
    }
    else
        data = 0;

    this->data = data;
    return data;
}

rob_osage_mothead::rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index, uint32_t motion_id,
    float_t frame, const bone_database* bone_data, const motion_database* mot_db) : rob_chr(),
    motion_id(), frame(), last_frame(),
    rob_parts_adjust(), rob_parts_adjust_data(), rob_adjust_global(), rob_adjust_global_data(),
    sleeve_adjust(), sleeve_adjust_data(), disable_collision(), disable_collision_data(),
    rob_chara_coli_ring(), rob_chara_coli_ring_data(), field_68() {
    rob_parts_adjust = 0;
    rob_adjust_global = 0;
    sleeve_adjust = 0;
    disable_collision = 0;
    rob_chara_coli_ring = 0;
    this->rob_chr = rob_chr;
    if (rob_chr) {
        rob_chr->set_motion_id(motion_id, frame, 0.0f,
            true, false, MOTION_BLEND_CROSS, bone_data, mot_db);
        this->frame = frame;
        this->motion_id = motion_id;
        last_frame = rob_chr->bone_data->get_frame_count() - 1.0f;
        rob_chr->set_stage_data_ring(stage_index);
        rob_chr->reset_osage();
        rob_chr->set_bone_data_frame(frame);
        rob_chr->pos_reset();
        init_data(mot_db);
        set_mothead_frame();
    }
    else
        reset();

}

#if OPD_PLAY_GEN
rob_osage_mothead::rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index,
    uint32_t motion_id, float_t frame, bool set_motion_reset_data,
    const bone_database* bone_data, const motion_database* mot_db) : rob_chr(),
    motion_id(), frame(), last_frame(),
    rob_parts_adjust(), rob_parts_adjust_data(), rob_adjust_global(), rob_adjust_global_data(),
    sleeve_adjust(), sleeve_adjust_data(), disable_collision(), disable_collision_data(),
    rob_chara_coli_ring(), rob_chara_coli_ring_data(), field_68() {
    rob_parts_adjust = 0;
    rob_adjust_global = 0;
    sleeve_adjust = 0;
    disable_collision = 0;
    rob_chara_coli_ring = 0;
    this->rob_chr = rob_chr;
    if (rob_chr) {
        rob_chr->set_motion_id(motion_id, frame, 0.0f,
            true, set_motion_reset_data, MOTION_BLEND_CROSS, bone_data, mot_db);
        this->frame = frame;
        this->motion_id = motion_id;
        last_frame = rob_chr->bone_data->get_frame_count() - 1.0f;
        rob_chr->set_stage_data_ring(stage_index);
        rob_chr->reset_osage();
        rob_chr->set_bone_data_frame(frame);
        rob_chr->pos_reset();
        init_data(mot_db);
        set_mothead_frame();
    }
    else
        reset();

}
#endif

rob_osage_mothead::~rob_osage_mothead() {
    reset();
}

// 0x14053D360
void rob_osage_mothead::ctrl() {
    rob_chara* rob_chr = this->rob_chr;
    if (!rob_chr)
        return;

    rob_chr->set_bone_data_frame(frame);
    set_mothead_frame();
    rob_chr->adjust_ctrl();
    rob_chr->rob_motion_modifier_ctrl();
    rob_disp_rob_chara_ctrl_thread_main(rob_chr);
}

// 0x14053D3E0
void rob_osage_mothead::reset() {
    rob_chr = 0;
    motion_id = -1;
    frame = 0.0f;
    last_frame = 0.0f;
    reset_data();
}

// 0x14053D6C0
void rob_osage_mothead::init_data(const motion_database* mot_db) {
    reset_data();

    rob_parts_adjust = new rob_osage_mothead_data(MOTHEAD_DATA_ROB_PARTS_ADJUST, motion_id, mot_db);
    rob_parts_adjust_data = rob_parts_adjust->find_next_data();

    rob_adjust_global = new rob_osage_mothead_data(MOTHEAD_DATA_ROB_ADJUST_GLOBAL, motion_id, mot_db);
    rob_adjust_global_data = rob_adjust_global->find_next_data();

    sleeve_adjust = new rob_osage_mothead_data(MOTHEAD_DATA_SLEEVE_ADJUST, motion_id, mot_db);
    sleeve_adjust_data = sleeve_adjust->find_next_data();

    disable_collision = new rob_osage_mothead_data(MOTHEAD_DATA_DISABLE_COLLISION, motion_id, mot_db);
    disable_collision_data = disable_collision->find_next_data();

    rob_chara_coli_ring = new rob_osage_mothead_data(MOTHEAD_DATA_ROB_CHARA_COLI_RING, motion_id, mot_db);
    rob_chara_coli_ring_data = rob_chara_coli_ring->find_next_data();
}

// 0x14053D450
void rob_osage_mothead::reset_data() {
    if (rob_parts_adjust)
        delete rob_parts_adjust;
    rob_parts_adjust = 0;
    rob_parts_adjust_data = 0;

    if (rob_adjust_global)
        delete rob_adjust_global;
    rob_adjust_global = 0;
    rob_adjust_global_data = 0;

    if (sleeve_adjust)
        delete sleeve_adjust;
    sleeve_adjust = 0;
    sleeve_adjust_data = 0;

    if (disable_collision)
        delete disable_collision;
    disable_collision = 0;
    disable_collision_data = 0;

    if (rob_chara_coli_ring)
        delete rob_chara_coli_ring;
    rob_chara_coli_ring_data = 0;
    rob_chara_coli_ring = 0;
}

// 0x14053EC90
void rob_osage_mothead::set_coli_ring(const mothead_data* mhd_data) {
    const void* data = mhd_data->data;

    rob_chara_set_coli_ring(rob_chr, ((int8_t*)data)[0]);
}

// 0x14053E7B0
void rob_osage_mothead::set_frame(float_t value) {
    if (last_frame > value)
        frame = value;
}

// 0x14053E7C0
void rob_osage_mothead::set_disable_collision(const mothead_data* mhd_data) {
    const void* data = mhd_data->data;

    rob_chr->set_disable_collision((rob_osage_parts)((uint8_t*)data)[0], !!((uint8_t*)data)[1]);
}

// 0x14053E170
void rob_osage_mothead::set_mothead_frame() {
    while (rob_parts_adjust_data) {
        if ((float_t)rob_parts_adjust_data->frame > frame)
            break;

        set_rob_parts_adjust(rob_parts_adjust_data);

        rob_parts_adjust_data = rob_parts_adjust->find_next_data();
    }

    while (rob_adjust_global_data) {
        if ((float_t)rob_adjust_global_data->frame > frame)
            break;

        set_rob_adjust_global(rob_adjust_global_data);

        rob_adjust_global_data = rob_adjust_global->find_next_data();
    }

    while (sleeve_adjust_data) {
        if ((float_t)sleeve_adjust_data->frame > frame)
            break;

        set_sleeve_adjust(sleeve_adjust_data);

        sleeve_adjust_data = sleeve_adjust->find_next_data();
    }

    while (disable_collision_data) {
        if ((float_t)disable_collision_data->frame > frame)
            break;

        set_disable_collision(disable_collision_data);

        disable_collision_data = disable_collision->find_next_data();
    }

    while (rob_chara_coli_ring_data) {
        if ((float_t)rob_chara_coli_ring_data->frame > frame)
            break;

        set_coli_ring(rob_chara_coli_ring_data);

        rob_chara_coli_ring_data = rob_chara_coli_ring->find_next_data();
    }
}

// 0x14053EA00
void rob_osage_mothead::set_rob_adjust_global(const mothead_data* mhd_data) {
    const void* data = mhd_data->data;

    rob_chara_data_adjust v14;
    v14.reset();

    int8_t type = ((int8_t*)data)[4];
    if (type >= 0 && type < 6) {
        float_t set_frame = (float_t)mhd_data->frame;
        v14.enable = true;
        v14.frame = frame - set_frame;
        v14.transition_frame = frame - set_frame;
        v14.motion_id = rob_chr->data.motion.motion_id;
        v14.set_frame = set_frame;
        v14.transition_duration = (float_t)((int32_t*)data)[0];
        v14.type = type;
        v14.cycle_type = ((int8_t*)data)[5];
        v14.ex_force.x = ((float_t*)data)[2];
        v14.ex_force.y = ((float_t*)data)[3];
        v14.ex_force.z = ((float_t*)data)[4];
        v14.ex_force_cycle_strength.x = ((float_t*)data)[5];
        v14.ex_force_cycle_strength.y = ((float_t*)data)[6];
        v14.ex_force_cycle_strength.z = ((float_t*)data)[7];
        v14.ex_force_cycle.x = ((float_t*)data)[8];
        v14.ex_force_cycle.y = ((float_t*)data)[9];
        v14.ex_force_cycle.z = ((float_t*)data)[10];
        v14.cycle = ((float_t*)data)[11];
        v14.phase = ((float_t*)data)[12];
    }
    rob_chara_set_adjust_global(rob_chr, &v14);
}

// 0x14053E810
void rob_osage_mothead::set_rob_parts_adjust(const mothead_data* mhd_data) {
    const void* data = mhd_data->data;

    rob_chara_data_adjust v16;
    v16.reset();

    int8_t type = ((int8_t*)data)[5];
    if (type >= 0 && type < 6) {
        float_t set_frame = (float_t)mhd_data->frame;
        v16.enable = true;
        v16.frame = frame - set_frame;
        v16.transition_frame = frame - set_frame;
        v16.motion_id = rob_chr->data.motion.motion_id;
        v16.set_frame = set_frame;
        v16.transition_duration = (float_t)((int32_t*)data)[0];
        v16.type = type;
        v16.cycle_type = ((int8_t*)data)[7];
        v16.ignore_gravity = !!((uint8_t*)data)[6];
        v16.ex_force.x = ((float_t*)data)[2];
        v16.ex_force.y = ((float_t*)data)[3];
        v16.ex_force.z = ((float_t*)data)[4];
        v16.ex_force_cycle_strength.x = ((float_t*)data)[5];
        v16.ex_force_cycle_strength.y = ((float_t*)data)[6];
        v16.ex_force_cycle_strength.z = ((float_t*)data)[7];
        v16.ex_force_cycle.x = ((float_t*)data)[8];
        v16.ex_force_cycle.y = ((float_t*)data)[9];
        v16.ex_force_cycle.z = ((float_t*)data)[10];
        v16.cycle = ((float_t*)data)[11];
        v16.force = ((float_t*)data)[13];
        v16.phase = ((float_t*)data)[12];
        v16.strength = ((float_t*)data)[14];
        v16.strength_transition = (float_t)((int32_t*)data)[15];
    }
    else
        rob_chr->rob_disp->reset_nodes_ex_force((rob_osage_parts)((uint8_t*)data)[4]);

    rob_chara_set_parts_adjust_by_index(rob_chr, (rob_osage_parts)((uint8_t*)data)[4], &v16);
}

// 0x14053EBE0
void rob_osage_mothead::set_sleeve_adjust(const mothead_data* mhd_data) {
    const void* data = mhd_data->data;

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

OpdMakeManager::CharaCostume::CharaCostume() {

}

OpdMakeManager::CharaCostume::~CharaCostume() {

}

OpdMakeManager::CharaData::CharaData() : left(), count() {
    Reset();
}

OpdMakeManager::CharaData::~CharaData() {

}

void OpdMakeManager::CharaData::AddCostumes(const std::list<std::pair<CHARA_NUM, int32_t>>& costumes) {
    for (const std::pair<CHARA_NUM, int32_t>& i : costumes) {
        const item_cos_data* cos = item_table_handler_array_get_item_cos_data(i.first, i.second);
        for (int32_t j = 0; j < ITEM_SUB_MAX; j++) {
            int32_t item_no = cos->arr[j];
            if (!item_no)
                continue;

            const item_table_item* item = item_table_handler_array_get_item(i.first, item_no);
            if (!item)
                continue;

            if (!(item->attr & 0x0C))
                chara_costumes[i.first].items[j].push_back(item_no);
            else if (item->org_itm)
                chara_costumes[i.first].items[j].push_back(item->org_itm);
        }
    }
}

void OpdMakeManager::CharaData::AddObjects(const std::vector<std::string>& customize_items) {
    for (const std::string& i : customize_items) {
        CHARA_NUM chara_num = CN_MAX;
        int32_t item_no = 0;
        customize_item_table_handler_data_get_chara_item(i, chara_num, item_no);
        if (chara_num >= CN_MAX || !item_no)
            continue;

        const item_table_item* item = item_table_handler_array_get_item(chara_num, item_no);
        if (!item)
            continue;

        if (!(item->attr & 0x0C))
            chara_costumes[chara_num].items[item->sub_id].push_back(item_no);
        else if (item->org_itm)
            chara_costumes[chara_num].items[item->sub_id].push_back(item->org_itm);
    }
}

bool OpdMakeManager::CharaData::CheckNoItems(CHARA_NUM chara_num) {
    std::vector<uint32_t>* chara_items = chara_costumes[chara_num].items;
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++, chara_items++)
        if (chara_items->size())
            return false;
    return true;
}

void OpdMakeManager::CharaData::PopItems(CHARA_NUM chara_num, int32_t items[ITEM_SUB_MAX]) {
    std::vector<uint32_t>* chara_items = chara_costumes[chara_num].items;
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++) {
        items[i] = 0;
        if (!chara_items[i].size())
            continue;

        items[i] = chara_items[i].back();
        chara_items[i].pop_back();
        left--;
    }
}

void OpdMakeManager::CharaData::Reset() {
    for (int32_t i = 0; i < CN_MAX; i++)
        for (int32_t j = 0; j < ITEM_SUB_MAX; j++)
            chara_costumes[i].items[j].clear();

    left = 0;
    count = 0;
}

void OpdMakeManager::CharaData::SortUnique() {
    for (int32_t i = 0; i < CN_MAX; i++)
        for (int32_t j = 0; j < ITEM_SUB_MAX; j++) {
            prj::sort_unique(chara_costumes[i].items[j]);
            count += chara_costumes[i].items[j].size();
        }

    left = count;
}

OpdMakeManager::OpdMakeManager() : mode(), workers() {
    mode = 0;
    chara = CN_MIKU;

    int32_t chara_id = 0;
    for (OpdMakeWorker*& i : workers)
        i = new OpdMakeWorker(chara_id++);

    motion_ids.clear();
    data.workers.resize(OPD_MAKE_COUNT);
    for (OpdMakeManagerData::Worker& i : data.workers) {
        i.items.resize(ITEM_SUB_MAX);
        for (uint32_t& j : i.items)
            j = 0;
    }
    use_current_skp = false;
    use_opdi = false;
}

OpdMakeManager::~OpdMakeManager() {
    for (OpdMakeWorker*& i : workers) {
        delete i;
        i = 0;
    }
}

bool OpdMakeManager::init() {
    mode = 1;

    rctx_ptr->render_manager->set_pass_sw(rndr::RND_PASSID_3D, false);

    if (!path_check_directory_exists(get_ram_osage_play_data_dir()))
        path_create_directory(get_ram_osage_play_data_dir());

    path_delete_directory(get_ram_osage_play_data_tmp_dir());

    if (use_current_skp || !app::TaskWork::check_task_ready(task_rob_manager)) {
        task_rob_manager_add_task();
        return true;
    }
    else {
        task_rob_manager_del_task();
        return false;
    }
}

bool OpdMakeManager::ctrl() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    bool ret = false;
    switch (mode) {
    case 1:
        mode = 3;
        break;
    case 3:
        motion_set_ids.clear();
        for (const uint32_t& i : motion_ids) {
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

        chara = CN_MIKU;
        mode = 5;
        break;
    } break;
    case 5: {
        bool has_items = false;
        for (int32_t i = 0; i < CN_MAX; i++)
            if (!chara_data.CheckNoItems((CHARA_NUM)i)) {
                chara = (CHARA_NUM)i;
                has_items = true;
                break;
            }

        if (has_items) {
            for (int32_t i = 0; i < OPD_MAKE_COUNT; i++) {
                rob_chara_pv_data pv_data;
                pv_data.type = ROB_CHARA_TYPE_3;
                rob_chara_array_init_chara_num(chara, pv_data, 499, false);
            }
            mode = 6;
        }
        else
            mode = 11;
    } break;
    case 6: {
        bool wait = false;
        for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
            if (!task_rob_manager_check_chara_loaded(i))
                wait = true;

        if (!wait) {
            task_rob_manager->hide();
            mode = 7;
        }
    } break;
    case 7: {
        int32_t j = 0;
        for (OpdMakeWorker*& i : workers)
            if (rob_chara_array_get(j++))
                i->add_task(use_current_skp);
        mode = 8;
    } break;
    case 8: {
        bool wait = false;
        for (OpdMakeWorker*& i : workers)
            if (app::TaskWork::check_task_ready(i))
                wait = true;

        if (!wait)
            mode = use_current_skp ? 12 : 9;
    } break;
    case 9:
        for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
            rob_chara_array_free_chara_id(i);
        task_rob_manager->run();
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
    }
    data.mode = mode;
    data.left = (uint32_t)chara_data.left;
    data.count = (uint32_t)chara_data.count;
    return ret;
}

bool OpdMakeManager::dest() {
    for (OpdMakeWorker*& i : workers)
        if (app::TaskWork::check_task_ready(i))
            return false;

    if (!use_current_skp) {
        for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
            rob_chara_array_free_chara_id(i);

        for (int32_t i = 0; i < ROB_ID_MAX; i++)
            skin_param_manager_reset(i);

        for (uint32_t& i : motion_set_ids) {
            motion_set_unload_motion(i);
            motion_set_unload_mothead(i);
        }

        task_rob_manager_del_task();
    }

    rctx_ptr->render_manager->set_pass_sw(rndr::RND_PASSID_3D, true);

    if (path_check_directory_exists(get_ram_osage_play_data_tmp_dir()))
        path_delete_directory(get_ram_osage_play_data_tmp_dir());
    return true;
}

void OpdMakeManager::disp() {

}

void OpdMakeManager::add_task(const OpdMakeManagerArgs& args) {
    if (app::TaskWork::check_task_ready(this) || !args.motion_ids)
        return;

    chara_data.Reset();

    if (args.modules) {
        const std::vector<module_data>& modules = module_data_handler_data_get_modules();
        std::list<std::pair<CHARA_NUM, int32_t>> costumes;
        for (const uint32_t& i : *args.modules)
            costumes.push_back({ modules[i].chara_num, modules[i].cos });
        chara_data.AddCostumes(costumes);
    }

    if (args.objects)
        chara_data.AddObjects(*args.objects);

    chara_data.SortUnique();

    motion_ids.assign(args.motion_ids->begin(), args.motion_ids->end());
    use_current_skp = args.use_current_skp;
    use_opdi = args.use_opdi;

    app::TaskWork::add_task(this, "OPD_MAKE_MANAGER");
}

bool OpdMakeManager::del_task() {
    for (OpdMakeWorker*& i : workers)
        i->del_task();
    return app::Task::del();
}

OpdMakeManagerData* OpdMakeManager::GetData() {
    return &data;
}

ReqData::ReqData() : chara_num(), count() {

}

ReqData::ReqData(CHARA_NUM chara_num, int32_t count) {
    this->chara_num = chara_num;
    this->count = count;
}

ReqData::~ReqData() {

}

void ReqData::Reset() {
    chara_num = CN_MAX;
    count = 0;
}

ReqDataObj::ReqDataObj() : ReqData(chara_num, count), cos() {

}

ReqDataObj::ReqDataObj(CHARA_NUM chara_num, int32_t count) : ReqData(chara_num, count), cos() {

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
    pos = 0.0f;
    field_14 = 0.0f;
    rot_z = 0.0f;
    field_1C = 0;
    rot_speed = 0.0f;
    gravity = 0.0f;
    scale = 1.0f;
    alpha = 0.0f;
    mat_scale = mat4_null;
    mat = mat4_null;
    prev_parent_mat = mat4_null;
    remaining = -1.0f;
    alive = false;
}

rob_chara_age_age_object::rob_chara_age_age_object() : objset_info(),
obj_index(), field_C(), num_vertex(), num_index(), vertex_data(),
vertex_data_size(), vertex_array_size(), disp_count(), count(), field_C3C() {

}

void rob_chara_age_age_object::calc_vertex(rob_chara_age_age_object_vertex*& vtx_data,
    obj_mesh* mesh, const mat4& mat, float_t alpha) {
    const float_t alpha_1 = 1.0f - alpha;

    obj_vertex_data* vertex_array = mesh->vertex_array;
    int32_t num_vertex = mesh->num_vertex;
    obj_vertex_data* obj_vtx = vertex_array;
    rob_chara_age_age_object_vertex* vtx = vtx_data;
    for (int32_t i = num_vertex; i; i--, obj_vtx++, vtx++) {
        mat4_transform_point(&mat, &obj_vtx->position, &vtx->position);
        mat4_transform_vector(&mat, &obj_vtx->normal, &vtx->normal);
        mat4_transform_vector(&mat, (vec3*)&obj_vtx->tangent, (vec3*)&vtx->tangent);
        vtx->tangent.w = obj_vtx->tangent.w;
        vtx->texcoord.x = obj_vtx->texcoord0.x + alpha_1;
        vtx->texcoord.y = obj_vtx->texcoord0.y;
    }
    vtx_data = vtx;
}

void rob_chara_age_age_object::disp(render_context* rctx, size_t chara_num,
    bool npr, bool reflect, const vec3& a5, bool chara_color) {
    int32_t disp_count = this->disp_count;
    if (!objset_info || !disp_count)
        return;

    disp_count = min_def(disp_count, 10);

    std::pair<float_t, int32_t> v44[10];
    for (int32_t i = 0; i < disp_count; i++) {
        v44[i].first = vec3::dot(pos[i], a5);
        v44[i].second = i;
    }

    if (disp_count >= 2)
        std::sort(v44, v44 + disp_count,
            [](const std::pair<float_t, int32_t>& a, const std::pair<float_t, int32_t>& b) {
                return a.first < b.first;
            });

    obj_vert_buf.cycle_index();

    GL::ArrayBuffer buffer = obj_vert_buf.get_buffer();
    size_t vtx_data = (size_t)buffer.MapMemory(gl_state);
    if (!vtx_data)
        return;

    size_t vertex_array_size = this->vertex_array_size;
    for (int32_t i = 0; i < disp_count; i++)
        memmove((void*)(vtx_data + vertex_array_size * i),
            (void*)((size_t)vertex_data + vertex_array_size * v44[i].second), vertex_array_size);

    buffer.UnmapMemory(gl_state);

    mesh.num_vertex = disp_count * num_vertex;
    sub_mesh.num_index = disp_count * num_index;
    sub_mesh.material_index = npr ? 1 : 0;

    mdl::ObjFlags flags = (mdl::ObjFlags)(mdl::OBJ_SSS | mdl::OBJ_4 | mdl::OBJ_SHADOW);
    if (reflect)
        enum_or(flags, mdl::OBJ_CHARA_REFLECT);
    rctx->disp_manager->set_obj_flags(flags);
    rctx->disp_manager->set_chara_color(chara_color);
    rctx->disp_manager->set_shadow_group(chara_num ? SHADOW_GROUP_STAGE : SHADOW_GROUP_CHARA);
    rctx->disp_manager->entry_obj_by_obj(mat4_identity, &obj,
        &get_obj_set_texture(), &obj_vert_buf, &obj_index_buf, 0, 1.0f);
}

bool rob_chara_age_age_object::get_objset_info_obj_index(object_info obj_info) {
    objset_info = objset_info_storage_get_objset_info(obj_info.set_id);
    if (!objset_info)
        return false;

    auto elem = objset_info->obj_id_data.find(obj_info.id);
    if (elem == objset_info->obj_id_data.end())
        return false;

    obj_index = elem->second;
    return true;
}

::obj* rob_chara_age_age_object::get_obj_set_obj() {
    return objset_info->obj_set->obj_data[obj_index];
}

std::vector<GLuint>& rob_chara_age_age_object::get_obj_set_texture() {
    return objset_info->gentex;
}

void rob_chara_age_age_object::load(object_info obj_info, int32_t count) {
    reset();

    this->count = count;
    num_vertex = 0;
    num_index = 0;
    disp_count = 0;

    if (!get_objset_info_obj_index(obj_info)) {
        obj_index = -1;
        objset_info = 0;
        return;
    }
    else {
        ::obj* o = get_obj_set_obj();
        if (o->num_mesh != 1 || o->mesh_array[0].num_submesh != 1
            || o->mesh_array[0].submesh_array[0].index_format != OBJ_INDEX_U16) {
            obj_index = -1;
            objset_info = 0;
            return;
        }
    }

    if (!objset_info)
        return;

    ::obj* o = get_obj_set_obj();
    obj_mesh* m = &o->mesh_array[0];
    obj_sub_mesh* sm = &m->submesh_array[0];

    int32_t num_vertex = m->num_vertex;
    this->num_vertex = num_vertex;

    int32_t num_index = sm->num_index;
    this->num_index = num_index;

    size_t vertex_array_size = sizeof(rob_chara_age_age_object_vertex) * num_vertex;
    this->vertex_array_size = (uint32_t)vertex_array_size;

    size_t vertex_data_size = vertex_array_size * count;
    this->vertex_data_size = (uint32_t)vertex_data_size;

    rob_chara_age_age_object_vertex* vtx_data = force_malloc<rob_chara_age_age_object_vertex>(vertex_data_size);
    vertex_data = vtx_data;

    obj_vertex_data* vertex_array = m->vertex_array;
    for (int32_t i = count; i > 0; i--)
        for (int32_t j = num_vertex, k = 0; j; j--, k++, vtx_data++) {
            vtx_data->position = vertex_array[k].position;
            vtx_data->normal = vertex_array[k].normal;
            vtx_data->tangent = vertex_array[k].tangent;
            vtx_data->texcoord = vertex_array[k].texcoord0;
        }

    obj_vert_buf.load_data(vertex_data_size, vertex_data, 2, GL::BUFFER_USAGE_STREAM);

    this->num_index = num_index + 1;
    int32_t num_idx_data = (int32_t)(count * (num_index + 1));

    uint16_t* idx_data = force_malloc<uint16_t>(num_idx_data);
    uint32_t* index_array = sm->index_array;
    uint32_t index_offset = 0;
    uint32_t l = 0;
    for (int32_t i = count; i > 0; i--) {
        for (int32_t j = num_index, k = 0; j; j--, k++)
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

    if (mesh.num_vertex && mesh.vertex_array)
        for (int32_t i = 0; i < 2; i++) {
            extern render_context* rctx_ptr;
            rctx_ptr->disp_manager->add_vertex_array(&mesh, &sub_mesh, material,
                obj_vert_buf.get_buffer(), obj_vert_buf.get_offset(), obj_index_buf.buffer, 0, 0);

            obj_vert_buf.cycle_index();
        }
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
    objset_info = 0;
    obj_index = -1;
}

void rob_chara_age_age_object::update(rob_chara_age_age_data* data, int32_t count, float_t alpha) {
    if (!objset_info)
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
            mat4_get_translation(&data->mat_scale, &pos[disp_count]);
            scale = max_def(scale, data->scale);
            disp_count++;
        }
    this->disp_count = disp_count;

    if (disp_count < 1)
        return;

    float_t radius = scale * sm->bounding_sphere.radius;

    obj_bounding_sphere v75;
    v75.center = pos[0];
    v75.radius = radius;

    for (int32_t i = 1; i < disp_count; i++) {
        obj_bounding_sphere v74;
        v74.center = pos[i];
        v74.radius = radius;
        v75 = combine_bounding_spheres(&v75, &v74);
    }

    sub_mesh.bounding_sphere = v75;
    mesh.bounding_sphere = v75;
    obj.bounding_sphere = v75;

    vec3 min = pos[0] - radius;
    vec3 max = pos[0] + radius;
    for (int32_t i = 1; i < disp_count; i++) {
        min = vec3::min(min, pos[i] - radius);
        max = vec3::max(max, pos[i] + radius);
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
    speed = 1.0f;
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

    vec3 pos[2];
    mat4_get_translation(&mat, &pos[0]);
    mat4_get_translation(&this->mat, &pos[1]);
    this->mat = mat;

    if (vec3::distance(pos[0], pos[1]) > 0.2f)
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

            vec3 pos[2];
            mat4_get_translation(&mat, &pos[0]);
            mat4_get_translation(&data->prev_parent_mat, &pos[1]);

            vec3 v15 = (pos[0] - pos[1]) * (move_cancel - 1.0f);
            if (vec3::length(v15) <= 1.0f) {
                data->pos = v15;
                data->field_14 = 0.0f;
                data->rot_z = data->part_id == 1 ? -0.1f : 0.1f;
                data->field_1C = 0;
                data->rot_speed = init_data->rot_speed;
                mat4_mul_translate(&data->mat, &init_data->pos, &data->mat);
                mat4_mul_rotate_x(&data->mat, init_data->rot_x, &data->mat);
                mat4_mul_translate(&data->mat, 0.0f, 0.0f, init_data->offset, &data->mat);
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
            data->rot_z += step * data->rot_speed * speed;
        data->pos.x += (90.0f - data->remaining) * (float_t)(1.0 / 90.0)
            * data->gravity * 2.5f * step * speed;
        data->pos.z -= (90.0f - data->remaining) * 0.000011f * step * speed;

        mat4 m;
        mat4_mul_translate(&mat, &init_data->pos, &m);
        mat4_mul_rotate_x(&m, init_data->rot_x, &m);
        mat4_mul_translate(&m, 0.0f, 0.0f, init_data->offset, &m);
        mat4_mul_rotate_y(&m, init_data->rot_y, &m);
        mat4_mul_translate(&m, &data->pos, &m);
        mat4_mul_rotate_z(&m, data->rot_z, &m);
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
    if (alpha >= 0.1f && visible)
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
    speed = 1.0f;
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

void rob_chara_age_age::set_speed(float_t value) {
    speed = value;
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

void rob_sleeve_handler::get_sleeve_data(CHARA_NUM chara_num,
    int32_t cos, rob_sleeve_data& l, rob_sleeve_data& r) {
    l = {};
    r = {};

    auto elem = sleeves.find({ chara_num, cos });
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

        CHARA_NUM chara_num = get_chara_num_from_char_id(chara);

        rob_sleeve& sleeve = sleeves.insert({ { chara_num, cos - 1 }, {} }).first->second;
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
        std::string file(i);
        file.append("rob_sleeve_data.txt");

        if (aft_data->check_file_exists("rom/", file.c_str())) {
            p_file_handler* pfhndl = new p_file_handler;
            pfhndl->read_file(aft_data, "rom/", file.c_str(), prj::MemCTemp);
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
motion_index(), pv(), thread(), display(), not_reset(), exit(), field_D4() {
    Reset();

    {
        std::unique_lock<std::mutex> u_lock(display_mtx);
        display = false;
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
        std::wstring buf = swprintf_s_string(L"PV Osage Manager #%d", pv_osage_manager_counter++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf.c_str());
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

bool PvOsageManager::init() {
    state = 0;
    return true;
}

bool PvOsageManager::ctrl() {
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

bool PvOsageManager::dest() {
    return true;
}

void PvOsageManager::disp() {

}

void PvOsageManager::AddMotionFrameResetData(int32_t stage_index,
    uint32_t motion_id, float_t frame, int32_t iterations) {
    if (!CheckResetFrameNotFound(motion_id, frame))
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    rob_chara* rob_chr = rob_chara_array_get(chara_id);

    rob_osage_mothead osg_mhd(rob_chr, stage_index, motion_id, frame, aft_bone_data, aft_mot_db);
    rob_chr->adjust_ctrl();
    rob_chr->rob_motion_modifier_ctrl();

    if (iterations < 0) {
        iterations = 60;
        if (rob_chr->data.motdata.field_0.iterations > 0)
            iterations = rob_chr->data.motdata.field_0.iterations;
    }

    rob_chara_add_motion_reset_data(rob_chr, motion_id, frame, iterations);

    reset_frames_list.insert({ frame, true });
    rob_chr->reset_osage();

    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        rob_chr->set_disable_collision((rob_osage_parts)i, false);
}

bool PvOsageManager::CheckResetFrameNotFound(uint32_t motion_id, float_t frame) {
    return reset_frames_list.find(frame) == reset_frames_list.end();
}

bool PvOsageManager::GetDisplay() {
    std::unique_lock<std::mutex> u_lock(display_mtx);
    return display;
}

bool PvOsageManager::GetNotReset() {
    std::unique_lock<std::mutex> u_lock(not_reset_mtx);
    return not_reset;
}

void PvOsageManager::SetDisplay(bool value) {
    std::unique_lock<std::mutex> u_lock(display_mtx);
    display = value;
}

void PvOsageManager::SetNotReset(bool value) {
    std::unique_lock<std::mutex> u_lock(not_reset_mtx);
    not_reset = value;
}

void PvOsageManager::SetPvId(int32_t pv_id, int32_t chara_id, bool reset) {
    if (!pv_set_motion.size())
        return;

    waitable_timer timer;
    while (GetDisplay()) {
        SetNotReset(true);
        timer.sleep(1);
    }

    SetDisplay(true);
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
    prj::sort_unique(pv_set_motion);

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
        rob_chr->set_face_mottbl_motion(0, 6, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, true, aft_mot_db);
        rob_chr->set_hand_l_mottbl_motion(0, 192, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chr->set_hand_r_mottbl_motion(0, 192, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chr->set_mouth_mottbl_motion(0, 131, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chr->set_eyes_mottbl_motion(0, 165, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
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
    if (rob_chr->data.motdata.field_0.iterations > 0)
        iterations = rob_chr->data.motdata.field_0.iterations;

    float_t frame = v34.front() - (float_t)iterations + 1.0f;
    while (frame < 0.0f)
        frame += last_frame;

    while (frame > last_frame)
        frame -= last_frame;

    float_t frame_1 = frame - 1.0f;
    if (frame_1 < 0.0f)
        frame_1 = last_frame - 1.0f;

    rob_osage_mothead osg_mhd(rob_chr, a2->frames.front().second, motion_id, frame_1, aft_bone_data, aft_mot_db);
    float_t* i_begin = v34.data();
    float_t* i_end = v34.data() + v34.size();
    for (float_t* i = i_begin; i != i_end; ) {
        osg_mhd.set_frame(frame);
        osg_mhd.ctrl();

        float_t frame_1 = frame;
        frame = prj::floorf(frame) + 1.0f;

        if (iterations <= 1) {
            if (frame_1 == *i) {
                if (frame_1 == 0.0f && v32)
                    rob_chara_add_motion_reset_data(rob_chr, motion_id, last_frame, 0);
                rob_chara_add_motion_reset_data(rob_chr, motion_id, frame_1, 0);
                i++;
                continue;
            }
            if (frame_1 + 1.0f > *i)
                frame = *i;
        }
        else
            iterations--;

        if (frame >= last_frame) {
            frame = 0.0f;
            osg_mhd.init_data(aft_mot_db);
        }
    }

    rob_chr->reset_osage();
    for (int32_t i = ROB_OSAGE_PARTS_LEFT; i < ROB_OSAGE_PARTS_MAX; i++)
        rob_chr->set_disable_collision((rob_osage_parts)i, false);
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
                if (rob_chr->data.motdata.field_0.field_58 == 1)
                    sub_1404F88A0(i.stage, osage_init_motion_id, (float_t)i.frame);
                else
                    AddMotionFrameResetData(i.stage, osage_init_motion_id, (float_t)i.frame, -1);
            }
        }

    if (rob_chr->data.motdata.field_0.field_58 == 1) {
        sub_1404F83A0(&v4);
    }
    else {
        bool v12 = true;
        const mothead_data* data = mothead_storage_get_mot_by_motion_id(motion_id, aft_mot_db)->data;
        if (data && data->type >= MOTHEAD_DATA_TYPE_0) {
            mothead_data_type type = data->type;
            while (type != MOTHEAD_DATA_OSAGE_RESET) {
                data++;
                type = data->type;
                if (type < MOTHEAD_DATA_TYPE_0)
                    goto LABEL_1;
            }

            while (data->frame) {
                data++;
                mothead_data_type type = data->type;
                if (type < MOTHEAD_DATA_TYPE_0)
                    goto LABEL_1;

                while (type != MOTHEAD_DATA_OSAGE_RESET) {
                    data++;
                    type = data->type;
                    if (type < MOTHEAD_DATA_TYPE_0)
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

        data = mothead_storage_get_mot_by_motion_id(motion_id, aft_mot_db)->data;
        if (data && data->type >= MOTHEAD_DATA_TYPE_0) {
            mothead_data_type type = data->type;
            while (type != MOTHEAD_DATA_MOTION_SKIN_PARAM) {
                data++;
                type = data->type;
                if (type < MOTHEAD_DATA_TYPE_0)
                    return;
            }

            while (true) {
                int32_t iterations = 60;
                if (data->data)
                    iterations = *(int32_t*)data->data;

                AddMotionFrameResetData(0, motion_id, (float_t)data->frame, iterations);

                data++;
                mothead_data_type type = data->type;
                if (type < MOTHEAD_DATA_TYPE_0)
                    break;

                while (type != MOTHEAD_DATA_MOTION_SKIN_PARAM) {
                    data++;
                    type = data->type;
                    if (type < MOTHEAD_DATA_TYPE_0)
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
            std::unique_lock<std::mutex> u_lock(pv_osg_mgr->display_mtx);
            pv_osg_mgr->display = false;
        }
    }

    {
        std::unique_lock<std::mutex> u_lock(pv_osg_mgr->display_mtx);
        pv_osg_mgr->display = false;
    }
}

RobThreadParent::RobThreadParent() : exit(), thread() {
    thread = new std::thread(RobThreadParent::ThreadMain, this);
    if (thread) {
        std::wstring buf = swprintf_s_string(L"Rob Thread Parent #%d", rob_thread_parent_counter++);
        SetThreadDescription((HANDLE)thread->native_handle(), buf.c_str());
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
    for (int32_t i = 0; i < ROB_ID_MAX; i++)
        arr[i] = new RobThreadParent;
}

RobThreadHandler::~RobThreadHandler() {
    for (int32_t i = 0; i < ROB_ID_MAX; i++) {
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

bool RobImplTask::init() {
    return true;
}

bool RobImplTask::ctrl() {
    return false;
}

bool RobImplTask::dest() {
    return true;
}

void RobImplTask::disp() {

}

void RobImplTask::AppendList(rob_chara* rob_chr, std::list<rob_chara*>* list) {
    if (!rob_chr || !list || rob_chr->chara_id >= ROB_ID_MAX || list->size() >= ROB_ID_MAX)
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
    if (!chara_id || !list || *chara_id >= ROB_ID_MAX || !list->size())
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

bool TaskRobBase::init() {
    return true;
}

bool TaskRobBase::ctrl() {
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
        if (i && !i->data.flag.bit.rf_31)
            rob_base_rob_chara_ctrl(i);

    for (rob_chara*& i : free_chara)
        if (i)
            rob_base_rob_chara_free(i);
    free_chara.clear();
    return false;
}

bool TaskRobBase::dest() {
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

bool TaskRobBase::check_type(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobBase::is_frame_dependent() {
    return true;
}

TaskRobCollision::TaskRobCollision() {

}

TaskRobCollision::~TaskRobCollision() {

}

bool TaskRobCollision::init() {
    return true;
}

bool TaskRobCollision::ctrl() {
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
            if (i && !i->data.flag.bit.rf_31)
                sub_14050EA90(v13);*/
    }

    free_chara.clear();
    return false;
}

bool TaskRobCollision::dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobCollision::check_type(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobCollision::is_frame_dependent() {
    return true;
}

TaskRobDisp::TaskRobDisp() {

}

TaskRobDisp::~TaskRobDisp() {

}

bool TaskRobDisp::init() {
    return true;
}

bool TaskRobDisp::ctrl() {
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
        if (i && !i->data.flag.bit.rf_31)
            rob_disp_rob_chara_ctrl(i);

    for (rob_chara*& i : ctrl_chara)
        if (i && !i->data.flag.bit.rf_31)
            rob_thread_handler->AppendRobCharaFunc(i->chara_id, i,
                rob_disp_rob_chara_ctrl_thread_main);

    rob_thread_handler->sub_14054E3F0();

    for (rob_chara*& i : free_chara)
        rob_disp_rob_chara_free(i);
    free_chara.clear();
    return false;
}

bool TaskRobDisp::dest() {
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

void TaskRobDisp::disp() {
    for (rob_chara*& i : ctrl_chara) {
        if (!i || pv_osage_manager_array_get_disp(i->chara_id))
            continue;

        if (i->is_visible() && !i->data.flag.bit.rf_31)
            rob_disp_rob_chara_disp(i);
    }
}

bool TaskRobDisp::check_type(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_3;
}

bool TaskRobDisp::is_frame_dependent() {
    return false;
}

TaskRobInfo::TaskRobInfo() {

}

TaskRobInfo::~TaskRobInfo() {

}

bool TaskRobInfo::init() {
    return true;
}

bool TaskRobInfo::ctrl() {
    for (rob_chara*& i : init_chara)
        AppendCtrlCharaList(i);
    init_chara.clear();

    for (rob_chara*& i : ctrl_chara)
        if (i && !i->data.flag.bit.rf_31)
            i->rob_info_ctrl();

    free_chara.clear();
    return false;
}

bool TaskRobInfo::dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobInfo::check_type(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobInfo::is_frame_dependent() {
    return true;
}

TaskRobLoad::TaskRobLoad() : field_68(), field_6C(), field_F0() {

}

TaskRobLoad::~TaskRobLoad() {

}

bool TaskRobLoad::init() {
    field_F0 = 0;
    return true;
}

bool TaskRobLoad::ctrl() {
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

bool TaskRobLoad::dest() {
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

bool TaskRobLoad::AppendFreeReqData(CHARA_NUM chara_num) {
    if (chara_num < CN_MIKU || chara_num > CN_TETO)
        return false;

    for (auto i = load_req_data.end(); i != load_req_data.begin(); ) {
        i--;
        if (i->chara_num == chara_num) {
            i = load_req_data.erase(i);
            return true;
        }
    }

    ReqData value;
    value.chara_num = chara_num;
    free_req_data.push_back(value);
    return true;
}

bool TaskRobLoad::AppendFreeReqDataObj(CHARA_NUM chara_num, const item_cos_data* cos) {
    if (chara_num < CN_MIKU || chara_num > CN_TETO)
        return false;

    for (auto i = load_req_data_obj.end(); i != load_req_data_obj.begin(); ) {
        i--;
        if (i->chara_num == chara_num
            && !memcmp(&i->cos, cos, sizeof(item_cos_data))) {
            i = load_req_data_obj.erase(i);
            return true;
        }
    }

    ReqDataObj value;
    value.chara_num = chara_num;
    value.cos = *cos;
    free_req_data_obj.push_back(value);
    return true;
}

bool TaskRobLoad::AppendLoadReqData(CHARA_NUM chara_num) {
    if (chara_num < CN_MIKU || chara_num > CN_TETO)
        return false;

    for (auto i = free_req_data.end(); i != free_req_data.begin(); ) {
        i--;
        if (i->chara_num == chara_num) {
            i = free_req_data.erase(i);
            return true;
        }
    }

    ReqData value;
    value.chara_num = chara_num;
    value.count = 1;
    load_req_data.push_back(value);
    return true;
}

bool TaskRobLoad::AppendLoadReqDataObj(CHARA_NUM chara_num, const item_cos_data* cos) {
    if (chara_num < CN_MIKU || chara_num > CN_TETO)
        return false;

    for (auto i = free_req_data_obj.end(); i != free_req_data_obj.begin(); ) {
        i--;
        if (i->chara_num == chara_num
            && !memcmp(&i->cos, cos, sizeof(item_cos_data))) {
            i = free_req_data_obj.erase(i);
            return true;
        }
    }

    ReqDataObj value;
    value.chara_num = chara_num;
    value.cos = *cos;
    value.count = 1;
    load_req_data_obj.push_back(value);
    return true;
}

void TaskRobLoad::AppendLoadedReqData(ReqData* req_data) {
    for (ReqData& i : loaded_req_data)
        if (i.chara_num == req_data->chara_num) {
            if (i.count != 0x7FFFFFFF)
                i.count++;
            return;
        }

    loaded_req_data.push_back(*req_data);
}

void TaskRobLoad::AppendLoadedReqDataObj(ReqDataObj* req_data_obj) {
    for (ReqDataObj& i : loaded_req_data_obj)
        if (i.chara_num == req_data_obj->chara_num
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
            UnloadCharaItemsParent(i.chara_num, &i.cos);
            FreeLoadedReqDataObj(&i);
        }
        free_req_data_obj.clear();
    }

    if (free_req_data.size()) {
        for (ReqData& i : free_req_data) {
            UnloadCharaObjSetMotionSet(i.chara_num);
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
            LoadCharaItemsParent(i.chara_num, &i.cos, aft_data, aft_obj_db);
            load_item_req_data_obj.push_back(i);
        }
        load_req_data_obj.clear();
        ret = 3;
    }

    if (load_req_data.size()) {
        for (ReqData& i : load_req_data) {
            AppendLoadedReqData(&i);
            LoadCharaObjSetMotionSet(i.chara_num, aft_data, aft_obj_db, aft_mot_db);
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
            if (LoadCharaItemsCheckNotReadParent(i.chara_num, &i.cos))
                return -1;
        load_item_req_data_obj.clear();
    }
    if (!load_item_req_data.size())
        return 2;

    if (load_item_req_data.size()) {
        for (ReqData& i : load_item_req_data)
            if (LoadCharaObjSetMotionSetCheck(i.chara_num))
                return -1;
        load_item_req_data.clear();
    }
    if (!load_item_req_data.size())
        return 2;

    return -1;
}

void TaskRobLoad::FreeLoadedReqData(ReqData* req_data) {
    for (auto i = loaded_req_data.begin(); i != loaded_req_data.end(); i++)
        if (i->chara_num == req_data->chara_num) {
            if (i->count > 0)
                i->count--;

            if (!i->count)
                i = loaded_req_data.erase(i);
            return;
        }
}

void TaskRobLoad::FreeLoadedReqDataObj(ReqDataObj* req_data_obj) {
    for (auto i = loaded_req_data_obj.begin(); i != loaded_req_data_obj.end(); i++)
        if (i->chara_num == req_data_obj->chara_num
            && !memcmp(&i->cos, &req_data_obj->cos, sizeof(item_cos_data))) {
            if (i->count > 0)
                i->count--;

            if (!i->count)
                i = loaded_req_data_obj.erase(i);
            return;
        }
}

void TaskRobLoad::LoadCharaItem(CHARA_NUM chara_num,
    int32_t item_no, void* data, const object_database* obj_db) {
    if (!item_no)
        return;

    const std::vector<uint32_t>* item_objset
        = item_table_handler_array_get_item_objset(chara_num, item_no);
    if (!item_objset)
        return;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1)
            objset_info_storage_load_set(data, obj_db, i);
}

bool TaskRobLoad::LoadCharaItemCheckNotRead(CHARA_NUM chara_num, int32_t item_no) {
    if (!item_no)
        return false;

    const std::vector<uint32_t>* item_objset
        = item_table_handler_array_get_item_objset(chara_num, item_no);
    if (!item_objset)
        return true;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1 && objset_info_storage_load_obj_set_check_not_read(i))
            return true;
    return false;
}

void TaskRobLoad::LoadCharaItems(CHARA_NUM chara_num,
    item_cos_data* cos, void* data, const object_database* obj_db) {
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++)
        LoadCharaItem(chara_num, cos->arr[i], data, obj_db);
}

bool TaskRobLoad::LoadCharaItemsCheckNotRead(CHARA_NUM chara_num, item_cos_data* cos) {
    for (int32_t& i : cos->arr)
        if (LoadCharaItemCheckNotRead(chara_num, i))
            return true;
    return false;
}

bool TaskRobLoad::LoadCharaItemsCheckNotReadParent(CHARA_NUM chara_num, item_cos_data* cos) {
    return TaskRobLoad::LoadCharaItemsCheckNotRead(chara_num, cos)
        || cos->data.kami == 649 && objset_info_storage_load_obj_set_check_not_read(3291);
}

void TaskRobLoad::LoadCharaItemsParent(CHARA_NUM chara_num,
    item_cos_data* cos, void* data, const object_database* obj_db) {
    LoadCharaItems(chara_num, cos, data, obj_db);
    if (cos->data.kami == 649)
        objset_info_storage_load_set(data, obj_db, 3291);
}

void TaskRobLoad::LoadCharaObjSetMotionSet(CHARA_NUM chara_num,
    void* data, const object_database* obj_db, const motion_database* mot_db) {
    const RobData* rob_data = get_rob_data(chara_num);
    objset_info_storage_load_set(data, obj_db, rob_data->object_set);
    motion_set_load_motion(cmn_set_id, "", mot_db);
    motion_set_load_mothead(cmn_set_id, "", mot_db);
    motion_set_load_motion(rob_data->motfile, "", mot_db);
}

bool TaskRobLoad::LoadCharaObjSetMotionSetCheck(CHARA_NUM chara_num) {
    const RobData* rob_data = get_rob_data(chara_num);
    if (objset_info_storage_load_obj_set_check_not_read(rob_data->object_set)
        || motion_storage_check_mot_file_not_ready(cmn_set_id)
        || mothead_storage_check_mhd_file_not_ready(cmn_set_id))
        return true;
    return motion_storage_check_mot_file_not_ready(rob_data->motfile);
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

void TaskRobLoad::UnloadCharaItem(CHARA_NUM chara_num, int32_t item_no) {
    if (!item_no)
        return;

    const std::vector<uint32_t>* item_objset
        = item_table_handler_array_get_item_objset(chara_num, item_no);
    if (!item_objset)
        return;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1)
            objset_info_storage_unload_set(i);
}

void TaskRobLoad::UnloadCharaItems(CHARA_NUM chara_num, item_cos_data* cos) {
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++)
        UnloadCharaItem(chara_num, cos->arr[i]);
}

void TaskRobLoad::UnloadCharaItemsParent(CHARA_NUM chara_num, item_cos_data* cos) {
    if (cos->data.kami == 649)
        objset_info_storage_unload_set(3291);
    UnloadCharaItems(chara_num, cos);
}

void TaskRobLoad::UnloadCharaObjSetMotionSet(CHARA_NUM chara_num) {
    const RobData* rob_data = get_rob_data(chara_num);
    objset_info_storage_unload_set(rob_data->object_set);
    motion_set_unload_motion(cmn_set_id);
    motion_set_unload_mothead(cmn_set_id);
    motion_set_unload_motion(rob_data->motfile);
}

void TaskRobLoad::UnloadLoadedChara() {
    for (ReqDataObj& i : loaded_req_data_obj)
        for (int32_t j = i.count; j; j--)
            UnloadCharaItemsParent(i.chara_num, &i.cos);

    loaded_req_data_obj.clear();

    for (ReqData& i : loaded_req_data)
        for (int32_t j = i.count; j; j--)
            UnloadCharaObjSetMotionSet(i.chara_num);

    loaded_req_data.clear();
}

TaskRobManager::TaskRobManager() : ctrl_state(), dest_state() {

}

TaskRobManager::~TaskRobManager() {

}

bool TaskRobManager::init() {
    task_rob_load_add_task();
    rob_manager_rob_impl* rob_impls1 = rob_manager_rob_impls1_get(this);
    for (; rob_impls1->task; rob_impls1++) {
        RobImplTask* task = rob_impls1->task;
        app::TaskWork::add_task(task, rob_impls1->name);
        task->frame_dependent = task->is_frame_dependent();
        task->FreeCharaLists();
    }

    rob_manager_rob_impl* rob_impls2 = rob_manager_rob_impls2_get(this);
    for (; rob_impls2->task; rob_impls2++) {
        RobImplTask* task = rob_impls2->task;
        app::TaskWork::add_task(task, rob_impls2->name);
        task->frame_dependent = task->is_frame_dependent();
        task->FreeCharaLists();
    }

    ctrl_state = 0;
    dest_state = 0;
    return true;
}

bool TaskRobManager::ctrl() {
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
            task_rob_load_append_free_req_data(i->chara_num);
            task_rob_load_append_free_req_data_obj(i->chara_num, i->item_cos_data.get_cos());
            FreeLoadedCharaList(&i->chara_id);
        }

        for (rob_chara*& i : free_chara) {
            rob_chara_type type = i->type;
            for (rob_manager_rob_impl* j = rob_manager_rob_impls1_get(this); j->task; j++) {
                if (!j->task->check_type(type))
                    continue;

                j->task->FreeInitCharaList(&i->chara_id);
                j->task->FreeCtrlCharaList(&i->chara_id);
                j->task->AppendFreeCharaList(i);
            }

            for (rob_manager_rob_impl* j = rob_manager_rob_impls2_get(this); j->task; j++) {
                if (!j->task->check_type(type))
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
            task_rob_load_append_load_req_data(i->chara_num);
            task_rob_load_append_load_req_data_obj(i->chara_num, i->item_cos_data.get_cos());
        }
        init_chara.clear();
        ctrl_state = 0;
    }
    return false;
}

bool TaskRobManager::dest() {
    switch (dest_state) {
    case 0: {
        rob_manager_rob_impl* rob_impls1 = rob_manager_rob_impls1_get(this);
        for (; rob_impls1->task; rob_impls1++)
            rob_impls1->task->del();

        rob_manager_rob_impl* rob_impls2 = rob_manager_rob_impls2_get(this);
        for (; rob_impls2->task; rob_impls2++)
            rob_impls2->task->del();

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

void TaskRobManager::disp() {

}

void TaskRobManager::AppendFreeCharaList(rob_chara* rob_chr) {
    if (!rob_chr || rob_chr->chara_id >= ROB_ID_MAX || free_chara.size() >= ROB_ID_MAX)
        return;

    int32_t chara_id = rob_chr->chara_id;
    for (auto i = init_chara.begin(); i != init_chara.end();)
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
    if (!rob_chr || rob_chr->chara_id >= ROB_ID_MAX || init_chara.size() >= ROB_ID_MAX)
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
    if (!rob_chr || rob_chr->chara_id >= ROB_ID_MAX || loaded_chara.size() >= ROB_ID_MAX)
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
    if (!app::TaskWork::check_task_ready(this)
        || rob_chr->chara_id < 0 || rob_chr->chara_id >= ROB_ID_MAX)
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
    if (!rob_chr || rob_chr->chara_id >= ROB_ID_MAX || !load_chara.size())
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
            if (task->check_type(type))
                task->AppendInitCharaList(i);
        }

        rob_manager_rob_impl* rob_impls2 = rob_manager_rob_impls2_get(this);
        for (; rob_impls2->task; rob_impls2++) {
            RobImplTask* task = rob_impls2->task;
            if (task->check_type(type))
                task->AppendInitCharaList(i);
        }
    }
}

void TaskRobManager::FreeLoadedCharaList(int8_t* chara_id) {
    if (!chara_id || *chara_id >= ROB_ID_MAX || !loaded_chara.size())
        return;

    for (auto i = loaded_chara.begin(); i != loaded_chara.end();)
        if ((*i)->chara_id == *chara_id) {
            i = loaded_chara.erase(i);
            return;
        }
        else
            i++;
}

bool TaskRobManager::GetFreeCharaListEmpty() {
    if (!app::TaskWork::check_task_ready(this))
        return false;

    if (ctrl_state == 1 && !init_chara.size())
        return !!free_chara.size();
    return true;
}

bool TaskRobManager::GetWait(rob_chara* rob_chr) {
    if (!app::TaskWork::check_task_ready(this))
        return false;

    int32_t chara_id = rob_chr->chara_id;
    if (chara_id < 0 || chara_id >= ROB_ID_MAX)
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

bool TaskRobMotionModifier::init() {
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
    rob_chr->rob_disp->set_osage_step(step);

    if (!(rob_chr->data.motion.field_28 & 0x40)) {
        if (rob_chr->data.action.field_B8.field_7C > 0.0f)
            rob_chr->data.motion.step_data.frame = rob_chr->data.action.field_B8.field_80;
        return;
    }

    if (!rob_chr->data.motdata.field_0.field_298)
        return;

    if (!rob_chr->field_20 || rob_chr->data.motion.frame_data.frame >= rob_chr->data.motdata.field_0.field_29C) {
        rob_chr->data.motion.step_data.frame = 1.0f;
        rob_chr->data.motion.field_28 &= ~0x40;
        return;
    }

    float_t v6 = 1.0f;
    /*size_t v7 = rob_chr->field_20;
    if (rob_chr->data.motdata.field_0.field_298 & 0x200
        || (stru_140A2E410[*(int32_t*)(v7 + 6744)][rob_chr->data.motdata.field_0.field_298 & 0xFF]
            && *(int32_t*)(v7 + 6736) & 0x02)) {
        float_t v11 = rob_chr->data.motdata.field_0.field_2A4;
        switch (rob_chr->data.motdata.field_0.field_2A0) {
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
            v6 = (rob_chr->data.motdata.field_0.field_29C - rob_chr->data.motion.frame_data.frame) / v12;
    }*/

    if (v6 < rob_chr->data.motdata.field_0.field_2A8)
        v6 = rob_chr->data.motdata.field_0.field_2A8;

    if (v6 <= rob_chr->data.motdata.field_0.field_2AC)
        rob_chr->data.motion.step_data.frame = v6;
    else
        rob_chr->data.motion.step_data.frame = rob_chr->data.motdata.field_0.field_2AC;
    return;
}

bool TaskRobMotionModifier::ctrl() {
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

bool TaskRobMotionModifier::dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobMotionModifier::check_type(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobMotionModifier::is_frame_dependent() {
    return true;
}

TaskRobPrepareAction::TaskRobPrepareAction() {

}

TaskRobPrepareAction::~TaskRobPrepareAction() {

}

bool TaskRobPrepareAction::init() {
    return true;
}

bool TaskRobPrepareAction::ctrl() {
    return false;
}

bool TaskRobPrepareAction::dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobPrepareAction::check_type(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobPrepareAction::is_frame_dependent() {
    return true;
}

TaskRobPrepareControl::TaskRobPrepareControl() {

}

TaskRobPrepareControl::~TaskRobPrepareControl() {

}

bool TaskRobPrepareControl::init() {
    return true;
}

static void sub_140548660(rob_chara* rob_chr) {
    rob_chr->data.motion.frame_data.prev_frame = rob_chr->data.motion.frame_data.frame;
    rob_chr->bone_data->motion_step();
    if (rob_chr->data.motion.field_28 & 0x80)
        rob_chr->bone_data->set_frame(rob_chr->data.motion.frame_data.last_set_frame);
    rob_chr->data.motion.frame_data.frame = rob_chr->bone_data->get_frame();
    rob_chr->data.motion.field_150.face.Step();
    rob_chr->data.motion.field_150.hand_l.Step();
    rob_chr->data.motion.field_150.hand_r.Step();
    rob_chr->data.motion.field_150.mouth.Step();
    rob_chr->data.motion.field_150.eyes.Step();
    rob_chr->data.motion.field_150.eyelid.Step();
    rob_chr->data.motion.field_3B0.face.Step();
    rob_chr->data.motion.field_3B0.hand_l.Step();
    rob_chr->data.motion.field_3B0.hand_r.Step();
    rob_chr->data.motion.field_3B0.mouth.Step();
    rob_chr->data.motion.field_3B0.eyes.Step();
    rob_chr->data.motion.field_3B0.eyelid.Step();
}

static void sub_140548460(rob_chara* rob_chr) {
    rob_chr->data.flag.bit.old_not_normal = rob_chr->data.flag.bit.not_normal;
    if (rob_chr->data.motdata.field_0.field_20.field_0 & 0x01)
        rob_chr->data.flag.bit.not_normal = 1;
    else
        rob_chr->data.flag.bit.not_normal = 0;
}

bool TaskRobPrepareControl::ctrl() {
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

bool TaskRobPrepareControl::dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobPrepareControl::check_type(rob_chara_type type) {
    return type >= ROB_CHARA_TYPE_0 && type <= ROB_CHARA_TYPE_2;
}

bool TaskRobPrepareControl::is_frame_dependent() {
    return true;
}
