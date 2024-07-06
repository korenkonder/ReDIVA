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
#include "../customize_item_table.hpp"
#include "../module_table.hpp"
#include "../data.hpp"
#include "../effect.hpp"
#include "../hand_item.hpp"
#include "../mdata_manager.hpp"
#include "../pv_db.hpp"
#include "../pv_expression.hpp"
#include "../random.hpp"
#include "../resolution_mode.hpp"
#include "../stage.hpp"
#include "motion.hpp"
#include "skin_param.hpp"

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

struct opd_farc_params {
    size_t align;
    bool compress;
    bool encrypt;

    opd_farc_params();
};

struct opd_farc_file {
    std::string name;
    size_t offset;
    size_t size;

    opd_farc_file();
    opd_farc_file(const std::string& name, size_t offset, size_t size);
    ~opd_farc_file();
};

struct opd_farc {
    std::string path;
    opd_farc_params params;
    std::vector<opd_farc_file> files;
    std::string data_path;
    file_stream* stream;
    size_t farc_size;

    opd_farc();
    ~opd_farc();

    bool add_file(const void* data, size_t size, const std::string& name);
    bool open(const std::string& path, const opd_farc_params& params);
    void reset();
    bool write_file();
};

struct p_opd_farc {
    opd_farc* ptr;

    p_opd_farc();
    ~p_opd_farc();

    bool add_file(const void* data, size_t size, const std::string& file);
    bool open(const std::string& path, const opd_farc_params& params);
    bool open(const std::string& path, bool compress, size_t align);
    bool write_file();
};

struct opd_chara_data {
    int32_t chara_id;
    bool init;
    uint32_t frame_index;
    uint32_t frame_count;
    int32_t motion_id;
    uint64_t field_18;
    std::vector<std::vector<opd_vec3_data_vec>> opd_data[ITEM_OSAGE_COUNT];
    p_opd_farc opd[ITEM_OSAGE_COUNT];
    p_opd_farc opdi[ITEM_OSAGE_COUNT];

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
#if OPD_PLAY
    struct GenData {
        object_info obj_info[ITEM_OSAGE_COUNT];
        uint32_t motion_id;
        int32_t chara_id;

        inline GenData() {
            motion_id = -1;
            chara_id = -1;
        }
    };

    std::vector<GenData> gen_data;
    std::vector<pv_data_set_motion> set_motion;
#else
    prj::vector_pair<object_info, uint32_t> opd_req_data;
    std::list<p_file_handler*> file_handlers;
#endif
    std::map<std::pair<object_info, uint32_t>, opd_file_data> opd_file_data;

    OsagePlayDataManager();
    virtual ~OsagePlayDataManager() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    bool add();
#if OPD_PLAY
    void AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids,
        const std::vector<pv_data_set_motion>& set_motion);
#else
    void AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids);
#endif
    bool CheckTaskReady();
    void GetOpdFileData(object_info obj_info,
        uint32_t motion_id, const float_t*& data, uint32_t& count);
    void LoadOpdFile(p_file_handler* pfhndl);
#if !OPD_PLAY
    void LoadOpdFileList();
#endif
    void Reset();
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
    rob_osage_mothead_data* type_62;
    const mothead_data* type_62_data;
    rob_osage_mothead_data* type_75;
    const mothead_data* type_75_data;
    rob_osage_mothead_data* type_67;
    const mothead_data* type_67_data;
    rob_osage_mothead_data* type_74;
    const mothead_data* type_74_data;
    rob_osage_mothead_data* type_79;
    const mothead_data* type_79_data;
    int32_t field_68;

#if OPD_PLAY
    rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index, uint32_t motion_id, float_t frame,
        const bone_database* bone_data, const motion_database* mot_db, bool set_motion_reset_data = false);
#else
    rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index, uint32_t motion_id,
        float_t frame, const bone_database* bone_data, const motion_database* mot_db);
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
        CharaCostume chara_costumes[CHARA_MAX];

        CharaData();
        ~CharaData();

        void AddCostumes(const std::list<std::pair<chara_index, int32_t>>& costumes);
        void AddObjects(const std::vector<std::string>& customize_items);
        bool CheckNoItems(chara_index chara_index);
        void PopItems(chara_index chara_index, int32_t items[ITEM_SUB_MAX]);
        void Reset();
        void SortUnique();
    };

    int32_t mode;
    CharaData chara_data;
    chara_index chara;
    std::vector<uint32_t> motion_ids;
    std::vector<uint32_t> motion_set_ids;
    OpdMakeWorker* workers[4];
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
};

typedef void(*mothead_func)(mothead_func_data*, const void*,
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
    void disp(render_context* rctx, size_t chara_index,
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

    void AddMotionFrameResetData(int32_t stage_index, uint32_t motion_id, float_t frame, int32_t iterations);
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

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

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
static void bone_data_mult_1_ik_hands(bone_data* a1, const vec3& pos);
static void bone_data_mult_1_ik_hands_2(bone_data* a1, const vec3& pos, float_t angle_scale);
static void bone_data_mult_1_ik_legs(bone_data* a1, const vec3& pos);
static bool bone_data_mult_1_exp_data(bone_data* a1, bone_node_expression_data* a2, bone_data* a3);
static void bone_data_mult_ik(bone_data* a1, int32_t skeleton_select);

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data);
static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    const std::vector<bone_database_bone>* bones, const vec3* common_position, const vec3* position);
static void bone_data_parent_load_rob_chara(bone_data_parent* bone);

static void mot_blend_interpolate(mot_blend* a1, std::vector<bone_data>& bones,
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
static void mothead_func_70(mothead_func_data* func_data,
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
static void rob_chara_bone_data_get_ik_scale(
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data);
static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index);
static bone_node* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index);
static void rob_chara_bone_data_ik_scale_calculate(
    rob_chara_bone_data_ik_scale* ik_scale, std::vector<bone_data>& bones,
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
static void rob_chara_bone_data_set_look_anim_param(rob_chara_bone_data* rob_bone_data,
    const rob_chara_look_anim_eye_param* params, eyes_adjust* eyes_adjust);
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

static void opd_chara_data_array_add_frame_data(int32_t chara_id);
static void opd_chara_data_array_encode_data(int32_t chara_id);
static void opd_chara_data_array_encode_init_data(int32_t chara_id, int32_t motion_id);
static void opd_chara_data_array_fs_copy_file(int32_t chara_id);
static opd_chara_data* opd_chara_data_array_get(int32_t chara_id);
static void opd_chara_data_array_init_data(int32_t chara_id, int32_t motion_id);
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
rob_cmn_mottbl_header* rob_cmn_mottbl_data;
rob_chara_age_age* rob_chara_age_age_array;
rob_sleeve_handler* rob_sleeve_handler_data;
RobThreadHandler* rob_thread_handler;
TaskRobLoad* task_rob_load;
TaskRobManager* task_rob_manager;

static int32_t opd_chara_data_counter = 0;
static int32_t opd_maker_counter = 0;
static int32_t pv_osage_manager_counter = 0;
static int32_t rob_thread_parent_counter = 0;

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
    { mothead_func_70, 0 },
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
        51, 194, 797, (uint32_t)-1, // EDT, EDT2, EDTF1
    };

    return opd_motion_set_ids;
}

const float_t get_osage_gravity_const() {
    return 0.00299444468691945f;
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

static std::vector<int32_t> opd_make_start_get_motion_ids() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    const uint32_t* opd_motion_set_ids = get_opd_motion_set_ids();
    std::vector<int32_t> motion_ids;
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

    motion_ids.push_back(195); // CMN_MRA00_13_01
    prj::sort_unique(motion_ids);
    return motion_ids;
}

void opd_make_start() {
    std::vector<std::string>& objects = opd_checker_get()->GetObjects();
    std::vector<int32_t> motion_ids = opd_make_start_get_motion_ids();

    OpdMakeManagerArgs args;
    args.modules = 0;
    args.use_current_skp = false;
    args.use_opdi = true;
    args.motion_ids = &motion_ids;
    args.objects = &objects;
    opd_make_manager->add_task(args);
}

void opd_make_stop() {
    opd_checker_terminate_thread();
    opd_make_manager_del_task();
}

bool osage_play_data_manager_add_task() {
    return osage_play_data_manager->add();
}

#if OPD_PLAY
void osage_play_data_manager_append_chara_motion_id(rob_chara* rob_chr, uint32_t motion_id,
    const std::vector<pv_data_set_motion>& set_motion) {
    std::vector<uint32_t> motion_ids;
    motion_ids.push_back(motion_id);
    osage_play_data_manager->AppendCharaMotionId(rob_chr, motion_ids, set_motion);
}
#else
void osage_play_data_manager_append_chara_motion_id(rob_chara* rob_chr, uint32_t motion_id) {
    std::vector<uint32_t> motion_ids;
    motion_ids.push_back(motion_id);
    osage_play_data_manager->AppendCharaMotionId(rob_chr, motion_ids);
}
#endif

void osage_play_data_manager_append_chara_motion_ids(
    rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids) {
#if OPD_PLAY
    osage_play_data_manager->AppendCharaMotionId(rob_chr, motion_ids, {});
#else
    osage_play_data_manager->AppendCharaMotionId(rob_chr, motion_ids);
#endif
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
        path.append(i);
        path.append("/");
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
        opd_chara_data_array = new opd_chara_data[ROB_CHARA_COUNT];

    if (!opd_checker)
        opd_checker = new OpdChecker;
    
    if (!opd_make_manager)
        opd_make_manager = new OpdMakeManager;

    if (!opd_maker_array)
        opd_maker_array = new OpdMaker[4];

    if (!osage_play_data_database)
        osage_play_data_database = new osage_play_data_database_struct;
    
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

    if (opd_chara_data_array) {
        delete[] opd_chara_data_array;
        opd_chara_data_array = 0;
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

    switch (adjust->type) {
    case 1: {
        mat4 mat = mat4_identity;
        mat4_mul_rotate_y(&mat, (float_t)((double_t)rob_chr->data.miku_rot.rot_y_int16
            * M_PI * (1.0 / 32768.0)), &mat);
        mat4_transform_vector(&mat, &adjust->curr_external_force, &adjust->curr_external_force);
    }
    case 2: {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, MOT_BONE_KL_MUNE_B_WJ)->ex_data_mat;
        if (mat)
            mat4_transform_vector(mat, &adjust->curr_external_force, &adjust->curr_external_force);
    } break;
    case 3: {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, MOT_BONE_KL_KUBI)->ex_data_mat;
        if (mat)
            mat4_transform_vector(mat, &adjust->curr_external_force, &adjust->curr_external_force);
    } break;
    case 4: {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, MOT_BONE_FACE_ROOT)->ex_data_mat;
        if (mat)
            mat4_transform_vector(mat, &adjust->curr_external_force, &adjust->curr_external_force);
    } break;
    case 5: {
        mat4* mat = rob_chara_bone_data_get_node(rob_chr->bone_data, MOT_BONE_KL_KOSI_ETC_WJ)->ex_data_mat;
        if (mat) {
            adjust->curr_external_force.z = -adjust->curr_external_force.z;
            mat4_transform_vector(mat, &adjust->curr_external_force, &adjust->curr_external_force);
        }
    } break;
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

float_t rob_chara::get_pos_scale(int32_t bone, vec3& pos) {
    if (bone < 0 || bone > 26)
        return 0.0f;
    pos = data.field_1E68.field_DF8[bone].pos;
    return data.field_1E68.field_DF8[bone].scale;
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
        float_t v16 = prj::floorf((v12 - v8) * 0.5f) + v8;
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

static void mothead_apply_mot_inner(struc_380* a1, int32_t type, const mothead_mot_data* a3) {
    struc_652* v19 = &a1->field_10->field_0;
    const void* v35 = a3->data;
    switch (type) {
    case 0x00: {
        if (v19->field_58 != -1)
            break;

        uint32_t v8 = 0;
        if (!a3 || a3->type < 0)
            break;

        prj::vector_pair<const void*, uint32_t> v31; // Was std::list
        while (a3->type != 0)
            if ((a3++)->type < 0)
                return;

        if (!a3->data)
            break;

        do {
            if (!a3->type) {
                const void* v10 = a3->data;
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
            for (std::pair<const void*, uint32_t>& i : v31)
                if (v15 < i.second) {
                    v35 = i.first;
                    break;
                }
        }

        v19->field_58 = ((int16_t*)v35)[0];
        v19->field_5C = ((int32_t*)v35)[1];
        v19->field_60 = ((int32_t*)v35)[2];
        v19->field_64 = ((int32_t*)v35)[3];
        v19->field_68 = ((int32_t*)v35)[4];
        v19->loop_count = ((int16_t*)v35)[5];
        v19->loop_begin = (float_t)((int16_t*)v35)[12];
        v19->loop_end = (float_t)((int16_t*)v35)[13];
    } break;
    case 0x01: {
        v19->field_78 = (float_t)((int16_t*)v35)[0];
        if (((int16_t*)v35)[1] >= 0)
            v19->field_7C = (float_t)((int16_t*)v35)[1];
        if (((int16_t*)v35)[2] >= 0)
            v19->field_80 = (float_t)((int16_t*)v35)[2];
        v19->field_20.field_0 |= 2;
        a1->rob_chara_data->motion.field_24 = 0;
    } break;
    case 0x02: {
        v19->field_1E8 = ((int32_t*)v35)[0];
        v19->field_1EC = (float_t)((int16_t*)v35)[2];
        v19->field_1F0 = (float_t)((int16_t*)v35)[3];
        v19->field_1F4 = (float_t)((int16_t*)v35)[4];
        v19->field_1F8 = (float_t)((int16_t*)v35)[5];
        v19->field_1FC = ((float_t*)v35)[3];
        v19->field_200 += ((float_t*)v35)[4];
        a1->rob_chara_data->field_8.field_B8.field_10.y = -get_osage_gravity_const() * ((float_t*)v35)[4];
        if (v19->field_1F4 < v19->field_1F0)
            v19->field_1F4 = v19->field_1F0;
    } break;
    case 0x28: {
        v19->field_238 = ((int16_t*)v35)[0];
        if (a1->rob_chara_data->motion.field_28 & 0x08)
            v19->field_23C = -((float_t*)v35)[1];
        else
            v19->field_23C = ((float_t*)v35)[1];
        v19->field_240 = ((int32_t*)v35)[2];
        a1->rob_chara_data->field_8.field_B8.field_10.y = -get_osage_gravity_const() * ((float_t*)v35)[3];
    } break;
    case 0x29: {
        if (v19->field_244 <= 0)
            v19->field_248 = a3;
        v19->field_244++;
    } break;
    case 0x2E: {
        if (v19->field_10.field_0 & 0x100) {
            if (a1->rob_chara_data->motion.field_28 & 0x08)
                v19->field_274 = -((int16_t*)v35)[0];
            else
                v19->field_274 = ((int16_t*)v35)[0];
        }
    } break;
    case 0x33: {
        if (a1->rob_chara_data->motion.field_28 & 0x08)
            v19->field_2B8 = -((int16_t*)v35)[0];
        else
            v19->field_2B8 = ((int16_t*)v35)[0];
    } break;
    case 0x37: {
        v19->field_2BC = ((int32_t*)v35)[0];
        v19->field_2C0 = ((float_t*)v35)[1];
        v19->field_2C4 = ((float_t*)v35)[2];
    } break;
    case 0x40: {
        float_t v31 = ((float_t*)v35)[0];
        if (a1->rob_chara_data->motion.field_28 & 0x08)
            v31 = -v31;
        v19->field_318.x = v31;
        v19->field_318.y = ((float_t*)v35)[1];
        v19->field_318.z = ((float_t*)v35)[2];
    } break;
    case 0x42: {
        v19->field_324 = ((float_t*)v35)[0];
        if (v19->field_324 < 0.0f)
            v19->field_324 = -v19->field_324;
    } break;
    case 0x44: {
        if (((int32_t*)v35)[0] > 0)
            v19->iterations = ((int32_t*)v35)[0];
    } break;
    }
}

static void mothead_apply_mot(struc_223* a1, rob_chara* rob_chr, const mothead_mot_data* a3) {
    if (!a3 || !a3->data)
        return;

    struc_380 v5;
    v5.rob_chara = rob_chr;
    v5.rob_chara_data = &rob_chr->data;
    v5.field_10 = &rob_chr->data.field_1588;

    while (a3->data && a3->type >= 0) {
        mothead_apply_mot_inner(&v5, a3->type, a3);
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

    item_equip->set_osage_move_cancel(0, 0.0f);
    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        item_equip->set_disable_collision((rob_osage_parts)i, false);
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

static void rob_chara_head_adjust(rob_chara* rob_chr);
static bool rob_chara_hands_adjust(rob_chara* rob_chr);
static bool sub_14053B580(rob_chara* rob_chr, int32_t a2);
static void sub_14053B260(rob_chara* rob_chr);

static bool sub_14053B530(rob_chara* rob_chr) {
    return sub_14053B580(rob_chr, 2) | sub_14053B580(rob_chr, 3);
}

static void rob_chara_bone_data_set_left_hand_scale(rob_chara_bone_data* rob_bone_data, float_t scale) {
    mat4* kl_te_wj_mat = rob_bone_data->get_mats_mat(ROB_BONE_KL_TE_L_WJ);
    if (!kl_te_wj_mat)
        return;

    vec3 pos;
    mat4_get_translation(kl_te_wj_mat, &pos);
    pos -= pos * scale;

    mat4 mat;
    mat4_scale(scale, scale, scale, &mat);
    mat4_set_translation(&mat, &pos);

    for (int32_t i = ROB_BONE_KL_TE_L_WJ; i <= ROB_BONE_NL_OYA_C_L_WJ; i++) {
        mat4* m = rob_bone_data->get_mats_mat(i);
        if (m)
            mat4_mul(m, &mat, m);
    }

    for (int32_t i = MOT_BONE_KL_TE_L_WJ; i <= MOT_BONE_NL_OYA_C_L_WJ; i++) {
        bone_node* node = rob_chara_bone_data_get_node(rob_bone_data, i);
        if (node)
            node->exp_data.scale = { scale, scale, scale };
    }
}

static void rob_chara_bone_data_set_right_hand_scale(rob_chara_bone_data* rob_bone_data, float_t scale) {
    mat4* kl_te_wj_mat = rob_bone_data->get_mats_mat(ROB_BONE_KL_TE_R_WJ);
    if (!kl_te_wj_mat)
        return;

    vec3 pos;
    mat4_get_translation(kl_te_wj_mat, &pos);
    pos -= pos * scale;

    mat4 mat;
    mat4_scale(scale, scale, scale, &mat);
    mat4_set_translation(&mat, &pos);

    for (int32_t i = ROB_BONE_KL_TE_R_WJ; i <= ROB_BONE_NL_OYA_C_R_WJ; i++) {
        mat4* m = rob_bone_data->get_mats_mat(i);
        if (m)
            mat4_mul(m, &mat, m);
    }

    for (int32_t i = MOT_BONE_KL_TE_R_WJ; i <= MOT_BONE_NL_OYA_C_R_WJ; i++) {
        bone_node* node = rob_chara_bone_data_get_node(rob_bone_data, i);
        if (node)
            node->exp_data.scale = { scale, scale, scale };
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

static const rob_chara_look_anim_eye_param* rob_chara_look_anim_eye_param_array_get(chara_index chara_index) {
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

    return &rob_chara_look_anim_eye_param_array[chara_index];
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
    rob_chara_bone_data_set_look_anim_param(this->bone_data,
        rob_chara_look_anim_eye_param_array_get(chara_index), &pv_data->eyes_adjust);
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
    rob_chara_bone_data_set_hand_l_frame(rob_chr->bone_data, motion->data.frame);
    rob_chara_bone_data_set_hand_l_play_frame_step(rob_chr->bone_data, motion->data.play_frame_step);
    rob_chara_bone_data_set_hand_l_anim_blend_duration(rob_chr->bone_data,
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

static vec3* rob_chara_bone_data_get_global_position(rob_chara_bone_data* rob_bone_data) {
    return &rob_bone_data->motion_loaded.front()->bone_data.global_position;
}

static void rob_chara_data_adjuct_set_pos(rob_chara_adjust_data* rob_chr_adj,
    const vec3& pos, bool pos_adjust, const vec3* global_position) {
    float_t scale = rob_chr_adj->scale;
    float_t item_scale = rob_chr_adj->item_scale; // X
    vec3 _offset = rob_chr_adj->offset;
    if (global_position)
        _offset.y += global_position->y;

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
    mat4* mat = bone_data->get_mats_mat(ROB_BONE_N_HARA_CP);

    vec3 pos;
    mat4_get_translation(mat, &pos);

    vec3* global_position = 0;
    if (rob_chr_adj->get_global_pos)
        global_position = rob_chara_bone_data_get_global_position(bone_data);
    rob_chara_data_adjuct_set_pos(rob_chr_adj, pos, pos_adjust, global_position);

    float_t scale = rob_chr_adj->scale;
    mat4_scale(scale, scale, scale, &rob_chr_adj->mat);
    mat4_set_translation(&rob_chr_adj->mat, &rob_chr_adj->pos);

    float_t item_scale = rob_chr_adj->item_scale; // X
    mat4_scale(item_scale, item_scale, item_scale, &rob_chr_adj->item_mat);
    mat4_set_translation(&rob_chr_adj->item_mat, &rob_chr_adj->item_pos);
}

void rob_chara::set_data_miku_rot_position(const vec3& value) {
    data.miku_rot.position = value;
}

void rob_chara::set_data_miku_rot_rot_y_int16(int16_t value) {
    data.miku_rot.rot_y_int16 = value;
}

void rob_chara::set_disable_collision(rob_osage_parts parts, bool disable) {
    item_equip->set_disable_collision(parts, disable);
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
    rob_chr->set_hand_l_mottbl_motion(1, 192, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    sub_140551870(rob_chr, blend_duration, blend_offset, aft_mot_db);
}

static void sub_140554690(rob_chara* rob_chr, int32_t mottbl_index, float_t value,
    int32_t state, float_t blend_duration, float_t a6, float_t step,
    int32_t a8, float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->data.motion.field_2A |= 0x04;
    rob_chr->set_hand_l_mottbl_motion(1, mottbl_index, value, state,
        blend_duration, a6, step, a8, blend_offset, aft_mot_db);
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
    rob_chr->set_hand_r_mottbl_motion(1, 192, 0.0f, -1, 0.0f, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
    sub_1405519B0(rob_chr, blend_duration, blend_offset, aft_mot_db);
}
static void sub_140554710(rob_chara* rob_chr, int32_t mottbl_index, float_t value,
    int32_t state, float_t blend_duration, float_t a6, float_t step,
    int32_t a8, float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->data.motion.field_2A |= 0x08;
    rob_chr->set_hand_r_mottbl_motion(1, mottbl_index, value, state,
        blend_duration, a6, step, a8, blend_offset, aft_mot_db);
}

void rob_chara::set_hand_item(int32_t uid, float_t blend_duration) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    int32_t mottbl_index = 192;
    object_info obj_left = {};
    object_info obj_right = {};
    float_t left_hand_scale = -1.0f;
    float_t right_hand_scale = -1.0f;

    const hand_item* hand_item = hand_item_handler_data_get_hand_item(uid, chara_index);
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
        sub_140554690(this, mottbl_index, 1.0f, 0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chara_set_hand_l_object(this, obj_left, 1);
    }

    if (obj_right.is_null())
        sub_140550C10(this, blend_duration, 0.0f, aft_mot_db);
    else {
        sub_140554710(this, mottbl_index, 1.0f, 0, blend_duration, 0.0f, 1.0f, -1, 0.0f, aft_mot_db);
        rob_chara_set_hand_r_object(this, obj_right, 1);
    }

    data.adjust_data.left_hand_scale_default = left_hand_scale;
    data.adjust_data.right_hand_scale_default = right_hand_scale;
}

void rob_chara::set_hand_item_l(int32_t uid) {
    const hand_item* hand_item = hand_item_handler_data_get_hand_item(uid, chara_index);
    object_info obj_info = {};
    if (hand_item && hand_item->obj_left.not_null())
        obj_info = hand_item->obj_left;
    rob_chara_set_hand_l_object(this, obj_info, 0);
}

void rob_chara::set_hand_item_r(int32_t uid) {
    const hand_item* hand_item = hand_item_handler_data_get_hand_item(uid, chara_index);
    object_info obj_info = {};
    if (hand_item && hand_item->obj_right.not_null())
        obj_info = hand_item->obj_right;
    rob_chara_set_hand_r_object(this, obj_info, 0);
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

static void sub_14041C950(rob_chara_bone_data* rob_bone_data, bool update_view_point, bool a3,
    float_t head_rot_strength, float_t eyes_rot_strength, float_t duration, float_t eyes_rot_step, float_t a8, bool ft) {
    rob_chara_look_anim* look_anim = &rob_bone_data->look_anim;

    if (!look_anim->head_rotation && a3)
        look_anim->init_head_rotation = true;
    look_anim->update_view_point = update_view_point;
    look_anim->head_rotation = a3;
    if (!look_anim->eyes_rotation && a3)
        look_anim->init_eyes_rotation = true;
    look_anim->eyes_rotation = a3;
    look_anim->head_rot_strength = head_rot_strength >= 0.0f ? head_rot_strength : 1.0f;
    look_anim->eyes_rot_strength = eyes_rot_strength >= 0.0f ? eyes_rot_strength : 1.0f;
    look_anim->eyes_rot_step = eyes_rot_step >= 0.0f ? eyes_rot_step : 1.0f;
    look_anim->duration = (a3 && !look_anim->head_rotation || !a3 && look_anim->head_rotation)
        && duration >= 0.0f ? duration : 0.0f;
    look_anim->eyes_rot_frame = 0.0f;
    look_anim->step = 1.0f;
    look_anim->head_rot_frame = 0.0f;
    look_anim->field_B0 = a8;
    look_anim->ft = ft;
}

void rob_chara::set_look_camera(bool update_view_point, bool enable, float_t head_rot_strength,
    float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8, bool ft) {
    sub_14041C950(bone_data, update_view_point, enable,
        head_rot_strength, eyes_rot_strength, blend_duration, eyes_rot_step, a8, ft);
    if (fabsf(blend_duration) <= 0.000001f)
        data.field_2 |= 0x80;
}

void rob_chara::set_look_camera_new(bool enable, float_t head_rot_strength,
    float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8) {
    set_look_camera(true, enable, head_rot_strength,
        eyes_rot_strength, blend_duration, eyes_rot_step, a8, true);
}

void rob_chara::set_look_camera_old(bool enable, float_t head_rot_strength,
    float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8) {
    set_look_camera(true, enable, head_rot_strength,
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
    item_equip->set_motion_skin_param(chara_id, motion_id, (int32_t)prj::roundf(frame));
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

        blend = clamp_def(blend, 0.0f, 1.0f);
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
        m = bone_data->get_mats_mat(v3->bone_index);
        mat4_mul_translate(m, &v3->bone_offset, &mat);
        *v42 = mat;

        m = bone_data->get_mats_mat(v6->bone_index);
        mat4_mul_translate(m, &v3->bone_offset, &mat);
        *v43 = mat;

        float_t chara_scale = data.adjust_data.scale;

        vec3 v23 = *(vec3*)&v42->row3 * chara_scale + data.adjust_data.pos;
        *(vec3*)&v42->row3 = v23;

        vec3 v29 = *(vec3*)&v43->row3 * chara_scale + data.adjust_data.pos;
        *(vec3*)&v43->row3 = v29;

        float_t v20 = v3->scale * chara_scale;
        v39->scale = v20;
        v39->field_24 = v20;
        v39->prev_pos = v39->pos;
        v39->pos = v23;

        float_t v19 = v6->scale * chara_scale;
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
        v3++;
        v6++;
    }
}

void rob_chara::sub_1405163C0(int32_t index, mat4& mat) {
    if (index >= 0 && index <= 26)
        mat = data.field_1E68.field_78[index];
}

void rob_chara::sub_140551000() {
    struc_223* v1 = &data.field_1588;
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

OpdMakeManagerData::Chara::Chara() : mode(), progress() {
    chara = CHARA_MAX;
}

OpdMakeManagerData::Chara::~Chara() {

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
            uint8_t* data = force_malloc<uint8_t>(length);
            if (fs.read(data, length)) {
                adler_buf adler;
                adler.get_adler(data, length - 8);
                ret = *(uint32_t*)&data[length - 4] == adler.adler;
                free_def(data);
            }
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
            uint32_t footer[2] = {};
            if (!fs.set_position(length - 8, SEEK_SET)
                && fs.read(footer, sizeof(footer)))
                ret = footer[0] == version;
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
    if (a1->check_flags_not_null())
        return;

    if (a1->motion_bone_index == MOTION_BONE_N_HITO_L_EX)
        printf("");

    mat4 mat;
    if (a1->has_parent)
        mat = *a1->parent_mat;
    else
        mat = mat4_identity;

    if (a1->type == BONE_DATABASE_BONE_POSITION) {
        mat4_mul_translate(&mat, &a1->position, &mat);
        a1->rot_mat[0] = mat4_identity;
    }
    else if (a1->type == BONE_DATABASE_BONE_TYPE_1) {
        mat4_inverse_transform_point(&mat, &a1->position, &a1->position);
        mat4_mul_translate(&mat, &a1->position, &mat);
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
            mat4_mul_rotate_zyx(&a1->rot_mat[0], &a1->rotation, &rot_mat);
        else {
            a1->position = a1->base_position[skeleton_select];
            mat4_rotate_zyx(&a1->rotation, &rot_mat);
        }

        mat4_mul_translate(&mat, &a1->position, &mat);
        mat4_mul(&rot_mat, &mat, &mat);
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
            a1->position = a1->base_position[1];

        mat4_mul_translate(&mat, &a1->position, &mat);
        if (solve_ik) {
            a1->node[0].exp_data.rotation = 0.0f;
            if (!a1->check_flags_not_null())
                mat4_get_rotation(&a1->rot_mat[0], &a1->node[0].exp_data.rotation);
            else if (bone_data_mult_1_exp_data(a1, &a1->node[0].exp_data, a3))
                mat4_rotate_zyx(&a1->node[0].exp_data.rotation, &a1->rot_mat[0]);
            else {
                *a1->node[0].mat = mat;

                if (bone_data_mult_1_ik(a1, a3)) {
                    mat4 rot_mat;
                    mat4_invert_rotation_fast(&mat, &rot_mat);
                    mat4_mul(a1->node[0].mat, &rot_mat, &rot_mat);
                    mat4_clear_trans(&rot_mat, &rot_mat);
                    mat4_get_rotation(&rot_mat, &a1->node[0].exp_data.rotation);
                    a1->rot_mat[0] = rot_mat;
                }
                else
                    a1->rot_mat[0] = mat4_identity;
            }
        }

        mat4_mul(&a1->rot_mat[0], &mat, &mat);
    }
    else {
        mat4_mul_translate(&mat, &a1->position, &mat);
        if (solve_ik)
            a1->node[0].exp_data.rotation = 0.0f;
    }

    *a1->node[0].mat = mat;
    if (solve_ik) {
        a1->node[0].exp_data.position = a1->position;
        a1->node[0].exp_data.reset_scale();
    }

    if (a1->type < BONE_DATABASE_BONE_HEAD_IK_ROTATION)
        return;

    mat4_mul(&a1->rot_mat[1], &mat, &mat);
    *a1->node[1].mat = mat;

    if (a1->type == BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        mat4_mul_translate(&mat, a1->ik_segment_length[1], 0.0f, 0.0f, &mat);
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
        mat4_mul_translate(&mat, a1->ik_segment_length[1], 0.0f, 0.0f, &mat);
        mat4_mul(&a1->rot_mat[2], &mat, &mat);
        *a1->node[2].mat = mat;

        mat4_mul_translate(&mat, a1->ik_2nd_segment_length[1], 0.0f, 0.0f, &mat);
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
    vec3 pos;

    switch (a1->motion_bone_index) {
    case MOTION_BONE_N_SKATA_L_WJ_CD_EX:
        mat4_get_translation(a2[MOTION_BONE_C_KATA_L].node[2].mat, &pos);
        bone_data_mult_1_ik_hands(a1, pos);
        break;
    case MOTION_BONE_N_SKATA_R_WJ_CD_EX:
        mat4_get_translation(a2[MOTION_BONE_C_KATA_R].node[2].mat, &pos);
        bone_data_mult_1_ik_hands(a1, pos);
        break;
    case MOTION_BONE_N_SKATA_B_L_WJ_CD_CU_EX:
        mat4_get_translation(a2[MOTION_BONE_N_UP_KATA_L_EX].node[0].mat, &pos);
        bone_data_mult_1_ik_hands_2(a1, pos, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_B_R_WJ_CD_CU_EX:
        mat4_get_translation(a2[MOTION_BONE_N_UP_KATA_R_EX].node[0].mat, &pos);
        bone_data_mult_1_ik_hands_2(a1, pos, 0.333f);
        break;
    case MOTION_BONE_N_SKATA_C_L_WJ_CD_CU_EX:
        mat4_get_translation(a2[MOTION_BONE_N_UP_KATA_L_EX].node[0].mat, &pos);
        bone_data_mult_1_ik_hands_2(a1, pos, 0.5f);
        break;
    case MOTION_BONE_N_SKATA_C_R_WJ_CD_CU_EX:
        mat4_get_translation(a2[MOTION_BONE_N_UP_KATA_R_EX].node[0].mat, &pos);
        bone_data_mult_1_ik_hands_2(a1, pos, 0.5f);
        break;
    case MOTION_BONE_N_MOMO_A_L_WJ_CD_EX:
        mat4_get_translation(a2[MOTION_BONE_CL_MOMO_L].node[2].mat, &pos);
        mat4_inverse_transform_point(a1->node[0].mat, &pos, &pos);
        bone_data_mult_1_ik_legs(a1, -pos);
        break;
    case MOTION_BONE_N_MOMO_A_R_WJ_CD_EX:
        mat4_get_translation(a2[MOTION_BONE_CL_MOMO_R].node[2].mat, &pos);
        mat4_inverse_transform_point(a1->node[0].mat, &pos, &pos);
        bone_data_mult_1_ik_legs(a1, -pos);
        break;
    case MOTION_BONE_N_HARA_CD_EX:
        mat4_get_translation(a2[MOTION_BONE_KL_MUNE_B_WJ].node[0].mat, &pos);
        mat4_inverse_transform_point(a1->node[0].mat, &pos, &pos);
        bone_data_mult_1_ik_legs(a1, pos);
        break;
    default:
        return false;
    }
    return true;
}

static void bone_data_mult_1_ik_hands(bone_data* a1, const vec3& pos) {
    vec3 v15;
    mat4_inverse_transform_point(a1->node[0].mat, &pos, &v15);

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

    mat4_mul(&rot_mat, a1->node[0].mat, a1->node[0].mat);
}

static void bone_data_mult_1_ik_hands_2(bone_data* a1, const vec3& pos, float_t angle_scale) {
    vec3 v8;
    mat4_inverse_transform_point(a1->node[0].mat, &pos, &v8);
    v8.x = 0.0f;

    float_t len = vec3::length(v8);
    if (len <= 0.000001f)
        return;

    v8 *= 1.0f / len;
    float_t angle = atan2f(v8.z, v8.y);
    mat4_mul_rotate_x(a1->node[0].mat, angle * angle_scale, a1->node[0].mat);
}

static void bone_data_mult_1_ik_legs(bone_data* a1, const vec3& pos) {
    vec3 v9 = pos;

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

    mat4_mul(&rot_mat, a1->node[0].mat, a1->node[0].mat);
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
        mat4_mul(&mat, &dst, &dst);
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
    mat4_inverse_transform_point(&mat, &a1->ik_target, &v30);
    float_t v6 = vec2::length_squared(*(vec2*)&v30);
    float_t v8 = vec3::length_squared(v30);
    float_t v9 = sqrtf(v6);
    float_t v10 = sqrtf(v8);
    mat4 rot_mat;
    if (v9 > 0.000001f && v8 > 0.000001f) {
        mat4_rotate_z(v30.y / v9, v30.x / v9, &rot_mat);
        mat4_mul_rotate_y(&rot_mat, -v30.z / v10, v9 / v10, &rot_mat);
        mat4_mul(&rot_mat, &mat, &mat);
    }
    else
        rot_mat = mat4_identity;

    if (a1->pole_target_mat) {
        vec3 pole_target;
        mat4_get_translation(a1->pole_target_mat, &pole_target);
        mat4_inverse_transform_point(&mat, &pole_target, &pole_target);
        float_t pole_target_length = vec2::length(*(vec2*)&pole_target.y);
        if (pole_target_length > 0.000001f) {
            pole_target *= 1.0f / pole_target_length;
            float_t rot_cos = pole_target.y;
            float_t rot_sin = pole_target.z;
            mat4_mul_rotate_x(&mat, rot_sin, rot_cos, &mat);
            mat4_mul_rotate_x(&rot_mat, rot_sin, rot_cos, &rot_mat);
        }
    }

    if (a1->type == BONE_DATABASE_BONE_HEAD_IK_ROTATION) {
        a1->rot_mat[1] = rot_mat;
        *a1->node[1].mat = mat;
        mat4_mul_translate(&mat, a1->ik_segment_length[skeleton_select], 0.0f, 0.0f, &mat);
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

    mat4_mul_rotate_z(&mat, rot_sin, rot_cos, &mat);
    *a1->node[1].mat = mat;
    mat4_mul_rotate_z(&rot_mat, rot_sin, rot_cos, &rot_mat);
    a1->rot_mat[1] = rot_mat;
    mat4_mul_translate(&mat, ik_segment_length, 0.0f, 0.0f, &mat);
    mat4_mul_rotate_z(&mat, rot_2nd_sin, rot_2nd_cos, &mat);
    *a1->node[2].mat = mat;
    mat4_rotate_z(rot_2nd_sin, rot_2nd_cos, &rot_mat);
    a1->rot_mat[2] = rot_mat;
    mat4_mul_translate(&mat, ik_2nd_segment_length, 0.0f, 0.0f, &mat);
    *a1->node[3].mat = mat;
}

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data) {
    const char* base_name = bone_database_skeleton_type_to_string(rob_bone_data->base_skeleton_type);
    const char* name = bone_database_skeleton_type_to_string(rob_bone_data->skeleton_type);
    const std::vector<bone_database_bone>* common_bones = bone_data->get_skeleton_bones(base_name);
    const std::vector<vec3>* common_position = bone_data->get_skeleton_positions(base_name);
    const std::vector<vec3>* position = bone_data->get_skeleton_positions(name);
    if (!common_bones || !common_position || !position)
        return;

    bone->rob_bone_data = rob_bone_data;
    bone_data_parent_load_rob_chara(bone);
    bone_data_parent_load_bone_database(bone, common_bones, common_position->data(), position->data());
}

static void bone_data_parent_load_bone_database(bone_data_parent* bone,
    const std::vector<bone_database_bone>* bones, const vec3* common_position, const vec3* position) {
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
        bone_node->flags = i.flags;
        if (i.type >= BONE_DATABASE_BONE_POSITION_ROTATION)
            bone_node->key_set_count = 6;
        else
            bone_node->key_set_count = 3;
        bone_node->base_position[0] = *common_position++;
        bone_node->base_position[1] = *position++;
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
            bone_node->ik_segment_length[0] = (common_position++)->x;
            bone_node->ik_segment_length[1] = (position++)->x;

            chain_pos += 2;
            ik_bone_count++;
            total_bone_count += 3;
            break;
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            bone_node->ik_segment_length[0] = (common_position++)->x;
            bone_node->ik_segment_length[1] = (position++)->x;
            bone_node->ik_2nd_segment_length[0] = (common_position++)->x;
            bone_node->ik_2nd_segment_length[1] = (position++)->x;

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

static void mot_blend_interpolate(mot_blend* a1, std::vector<bone_data>& bones,
    std::vector<uint16_t>* bone_indices, bone_database_skeleton_type skeleton_type) {
    if (!a1->mot_key_data.key_sets_ready || !a1->mot_key_data.mot_data || a1->field_30)
        return;

    float_t frame = a1->mot_play_data.frame_data.frame;
    vec3* keyframe_data = (vec3*)a1->mot_key_data.key_set_data.data();
    for (uint16_t& i : *bone_indices) {
        bone_data* data = &bones[i];
        bool get_data = sub_140410250(&a1->field_0.field_8, data->motion_bone_index);
        if (get_data) {
            mot_key_data_interpolate(&a1->mot_key_data, frame, data->key_set_offset, data->key_set_count);
            data->frame = frame;
        }
        keyframe_data = data->set_key_data(keyframe_data, skeleton_type, get_data, 0);
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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_1(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_2(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr_data->field_1588.field_0.field_20.field_0 |= 0x100;
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    if (rob_chr_data->motion.field_28 & 0x08)
        rob_chr_data->field_1588.field_0.field_274 -= ((int16_t*)data)[0];
    else
        rob_chr_data->field_1588.field_0.field_274 += ((int16_t*)data)[0];
}

static void mothead_func_3(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    int32_t v4 = ((int32_t*)data)[0];
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    (&rob_chr_data->field_1588.field_0.field_20.field_0)[v4 >> 5] |= 1 << (v4 & 0x1F);
}

static void mothead_func_4(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    int32_t v4 = ((int32_t*)data)[0];
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    (&rob_chr_data->field_1588.field_0.field_20.field_0)[v4 >> 5] &= ~(1 << (v4 & 0x1F));
}

static void mothead_func_5(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr_data->motion.field_28 ^= 0x04;
}

static void mothead_func_6(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr_data->miku_rot.field_30.x = 0.0f;
    func_data->rob_chr_data->miku_rot.field_30.z = 0.0f;
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
    rob_chara_data* rob_chr_data = func_data->rob_chr_data;
    struc_651& v2 = rob_chr_data->field_1588.field_330;

    float_t v8 = (float_t)((int16_t*)data)[0];
    int32_t v5 = ((int32_t*)data)[1];
    float_t v9 = ((float_t*)data)[2];
    int32_t v10 = ((int32_t*)data)[3];

    if (v8 == (int16_t)0xFA0C && v10 == 0xD62721C5) { // X
        printf("");
        float_t value = v9;
        float_t duration = (float_t)v5;
        v2.arm_adjust_next_value = value;
        v2.arm_adjust_prev_value = rob_chr_data->arm_adjust_scale;
        v2.arm_adjust_start_frame = mhd_data->frame;
        v2.arm_adjust_duration = max_def(duration, 0.0f);
        return;
    }

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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    float_t frame, int32_t state, float_t blend_duration, float_t a6,
    float_t step, int32_t a8, float_t blend_offset, const motion_database* mot_db);

static void mothead_func_50_set_face_motion_id(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    rob_chara_set_face_motion_id(func_data->rob_chr, ((int32_t*)data)[0],
        ((float_t*)data)[1], 0, 0.0f, 0.0f, 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_51(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_52(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {

}

static void mothead_func_53_set_face_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
     func_data->rob_chr->set_face_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
         ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, false, mot_db);
}

static void mothead_func_54_set_hand_r_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_hand_r_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 12.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_55_set_hand_l_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_hand_l_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 12.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_56_set_mouth_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_mouth_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_57_set_eyes_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_eyes_mottbl_motion(0, ((int32_t*)data)[0], ((float_t*)data)[1], ((int32_t*)data)[3],
        ((float_t*)data)[2] * 6.0f, ((float_t*)data)[4], 1.0f, -1, 0.0f, mot_db);
}

static void mothead_func_58_set_eyelid_mottbl_motion(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
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
    motion_blend_mot* v2 = func_data->rob_chr->bone_data->motion_loaded.front();
    v2->mot_key_data.frame = -1.0f;
    v2->mot_key_data.field_68.field_0 = ((int32_t*)data)[0];
    v2->mot_key_data.field_68.field_4 = ((float_t*)data)[1];
}

static void mothead_func_69_motion_max_frame(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    float_t max_frame = (float_t)*(int32_t*)data;
    func_data->rob_chr->bone_data->set_motion_max_frame(max_frame);
    /*pv_game* v6 = pv_game_get();
    if (v6)
        pv_game::set_data_itmpv_max_frame(v6, func_data->rob_chr->chara_id, max_frame);*/
}

static void mothead_func_70(mothead_func_data* func_data,
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

static void mothead_func_74_disable_collision(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr->set_disable_collision((rob_osage_parts)((uint8_t*)data)[0], !!((uint8_t*)data)[1]);
}

void rob_chara_set_adjust_global(rob_chara* rob_chr, rob_chara_data_adjust* a2) {
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
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    //rob_chara_set_coli_ring(func_data->rob_chr, ((int8_t*)data)[0]);
}

static void mothead_func_80_adjust_get_global_pos(mothead_func_data* func_data,
    const void* data, const mothead_data* mhd_data, int32_t frame, const motion_database* mot_db) {
    func_data->rob_chr_data->adjust_data.get_global_pos = ((uint8_t*)data)[0];
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

        keyframe_data = data->set_key_data(keyframe_data, skeleton_type, get_data, reverse);
    }

    uint32_t bone_key_set_count = a1->bone_data.bone_key_set_count;
    if (frame != a1->mot_key_data.frame) {
        mot_key_data_interpolate(&a1->mot_key_data, frame,
            bone_key_set_count, a1->bone_data.global_key_set_count);
        a1->mot_key_data.frame = frame;
    }

    keyframe_data = (vec3*)&a1->mot_key_data.key_set_data.data()[bone_key_set_count];
    vec3 global_position = reverse ? -keyframe_data[0] : keyframe_data[0];
    vec3 global_rotation = keyframe_data[1];
    a1->bone_data.global_position = global_position;
    a1->bone_data.global_rotation = global_rotation;

    float_t rot_y = a1->bone_data.rot_y;
    mat4 mat;
    mat4_rotate_y(rot_y, &mat);
    mat4_mul_translate(&mat, &global_position, &mat);
    mat4_mul_rotate_zyx(&mat, &global_rotation, &mat);
    for (bone_data& i : a1->bone_data.bones)
        switch (i.type) {
        case BONE_DATABASE_BONE_TYPE_1:
            mat4_transform_point(&mat, &i.position, &i.position);
            break;
        case BONE_DATABASE_BONE_POSITION_ROTATION: {
            mat4 rot_mat;
            mat4_clear_trans(&mat, &rot_mat);
            i.rot_mat[0] = rot_mat;
            mat4_transform_point(&mat, &i.position, &i.position);
        } break;
        case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        case BONE_DATABASE_BONE_ARM_IK_ROTATION:
        case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
            mat4_transform_point(&mat, &i.ik_target, &i.ik_target);
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

static void sub_140412E10(motion_blend_mot* a1, int32_t skeleton_select) {
    for (bone_data& i : a1->bone_data.bones)
        if ((1 << (i.motion_bone_index & 0x1F))
            & a1->field_0.field_8.bitfield.data()[i.motion_bone_index >> 5]) {
            i.store_curr_rot_trans(skeleton_select);
            if (i.type == BONE_DATABASE_BONE_POSITION_ROTATION && (a1->field_4F8.field_0 & 0x02))
                i.position_prev[skeleton_select] += a1->field_4F8.field_90;
        }
}

static void sub_140412F20(mot_blend* a1, std::vector<bone_data>* a2) {
    for (bone_data& i : *a2)
        if (((1 << (i.motion_bone_index & 0x1F))
            & a1->field_0.field_8.bitfield.data()[i.motion_bone_index >> 5]) != 0)
            i.store_curr_rot_trans(0);
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
        a1->blend->field_8 = true;
        a1->blend->enable = true;
        break;
    }
}

static void motion_blend_mot_set_duration(motion_blend_mot* mot,
    float_t duration, float_t step, float_t offset) {
    if (mot->blend)
        mot->blend->SetDuration(duration, step, offset);
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
    float_t* buf = force_malloc<float_t>(3ULL * file_head->frame_count * file_head->nodes_count);
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

struct opd_value_range {
    float_t value;
    float_t delta;
    uint16_t field_8;
    int16_t count;
    int32_t field_C;
    uint8_t field_10;
    uint8_t field_11;

    inline opd_value_range() : value(), delta(),
        field_8(), count(), field_C(), field_10(), field_11() {
    }

    inline opd_value_range(float_t value, int16_t count) : delta(),
        field_8(), field_C(), field_10(), field_11() {
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

            float_t diff = 0.0f;
            float_t start_value = vec.data()[start].value;
            float_t delta_value = vec.data()[end].value - start_value;
            int32_t start_count = vec.data()[start].count;
            float_t d = (float_t)vec.data()[end].count - vec.data()[start].count;

            bool v16 = false;
            size_t count = end - start;
            size_t _count = end - start;
            opd_value_range* val = &vec.data()[start + 1];
            for (size_t i = 1; i < _count; i++, val++) {
                float_t _diff = fabsf((float_t)(val->count - start_count)
                    * delta_value / d + start_value - val->value);
                if (_diff > epsilon) {
                    bool v24 = !!(val->field_10 & 0x30);
                    if ((diff < _diff && (!v16 || v24)) || (!v16 && v24)) {
                        diff = _diff;
                        count = i;
                        v16 = v24;
                    }
                }
            }

            if (count == _count)
                break;

            size_t _start = start + count;
            if (count + 1 >= 4)
                find_range(start, start + count, epsilon);
            else if (start <= _start) {
                opd_value_range* val = &vec.data()[start];
                for (size_t i = count + 1; i; i--, val++)
                    val->field_11 |= 0x01;
            }

            if (end - _start + 1 < 4) {
                opd_value_range* val = &vec.data()[_start];
                for (size_t i = _start; i <= end; i++, val++)
                    val->field_11 |= 0x01;
                return;
            }
            start = _start;
        }

        opd_value_range* val = &vec.data()[start];
        for (size_t i = start; i <= end; i++, val++)
            if (i == start || i == end)
                val->field_11 |= 0x01;
            else
                val->field_11 &= ~0x01;
    }

    size_t get_next_index(size_t index) {
        size_t count = vec.size();
        if (index >= count)
            return count;

        opd_value_range* val = &vec.data()[index];
        while (!(val->field_11 & 0x01)) {
            index++;
            val++;
            if (count <= index)
                return count;
        }
        return index;
    }
};

static uint32_t opd_calculate_max_value_shift(float_t value) {
    value = fabsf(value);
    float_t max_value = 16384.0f;
    uint32_t shift = 0;
    for (; shift < 24; shift++, max_value *= 0.5f)
        if (value >= max_value)
            break;
    return shift;
}

static uint32_t opd_encode_calculate_max_shift(const float_t* data, size_t size) {
    uint32_t max_shift = 24;
    for (size_t i = size, j = 0; i; i--, j++)
        max_shift = min_def(max_shift, opd_calculate_max_value_shift(data[j]));
    return (int16_t)max_shift;
}

static uint32_t opd_encode(const float_t* src_data, size_t src_size, const float_t epsilon, int16_t* dst_data, size_t& dst_size) {
    const uint32_t shift = opd_encode_calculate_max_shift(src_data, src_size);
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
            val_range.field_10 = 0x01;
            val_range.field_11 |= 0x01;
            enc.vec.push_back(val_range);
            continue;
        }

        opd_value_range& prev_val = enc.vec.back();
        float_t delta = value - prev_val.value;
        val_range.delta = delta;
        if (j == src_size - 1) {
            val_range.field_10 = 0x02;
            val_range.field_11 |= 0x01;
        }

        float_t prev_end_val = prev_val.delta;
        if (fabs(delta - prev_end_val) > 0.000001f) {
            if (delta > prev_end_val) {
                if (j != 1 && delta >= 0.0f && prev_end_val <= 0.0f)
                    prev_val.field_10 |= 0x20;
                else if (prev_val.field_10 & 0x04)
                    prev_val.field_10 |= 0x80;
            }
            if (delta < prev_end_val) {
                if (j != 1 && delta <= 0.0f && prev_end_val >= 0.0f)
                    prev_val.field_10 |= 0x10;
                else if (prev_val.field_10 & 0x04)
                    prev_val.field_10 |= 0x40;
            }
        }
        else {
            val_range.field_10 |= 0x04;
            if (!(prev_val.field_10 & 0x04))
                prev_val.field_10 |= 0x08;
        }
        enc.vec.push_back(val_range);
    }

    enc.find_range(0, src_size - 1, epsilon);

    size_t data_offset = 0;
    opd_value_range* v33 = enc.vec.data();
    for (size_t i = 0; i < src_size; ) {
        size_t next_index = enc.get_next_index(i);
        if (next_index == src_size)
            break;

        if (next_index <= i + 1)
            dst_data[data_offset++] = opd_data_encode_shift(v33[next_index].value, shift);
        else {
            dst_data[data_offset++] = (int16_t)((v33[next_index].count * 2) | 0x0001);
            dst_data[data_offset++] = opd_data_encode_shift(v33[next_index].value, shift);
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
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        chara_id = 0;
    return &pv_osage_manager_array[chara_id];
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
        mat4_mul(i.mat, mat, i.mat);

    sub_140414900(&rob_bone_data->motion_loaded.front()->field_4F8, mat);
}

static void sub_140507F60(rob_chara* rob_chr) {
    sub_14041DA50(rob_chr->bone_data, sub_140504E80(rob_chr));
    rob_chr->data.miku_rot.field_48 = rob_chr->data.miku_rot.field_24;

    mat4* v3 = rob_chr->bone_data->get_mats_mat(ROB_BONE_N_HARA);
    mat4_get_translation(v3, &rob_chr->data.miku_rot.field_24);
    rob_chr->data.miku_rot.field_54 = rob_chr->data.miku_rot.field_24
        - rob_chr->data.miku_rot.field_48;

    mat4* v8 = rob_chr->bone_data->get_mats_mat(ROB_BONE_N_HARA_CP);
    vec3 v10 = { 0.0f, 0.0f, 1.0f };
    mat4_transform_vector(v8, &v10, &v10);
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

static const void* sub_140551F60(rob_chara* rob_chr, uint32_t* a2) {
    int16_t v2 = rob_chr->data.field_1588.field_0.field_244;
    const mothead_mot_data* v3 = rob_chr->data.field_1588.field_0.field_248;
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

static void sub_140412DA0(motion_blend_mot* a1, vec3* position) {
    if (a1->bone_data.bones.size())
        *position = a1->bone_data.bones.front().position;
}

static void sub_140419800(rob_chara_bone_data* rob_bone_data, vec3* position) {
    sub_140412DA0(rob_bone_data->motion_loaded.front(), position);
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
    mat4_transform_vector(&mat, &v9, &v9);

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
    rob_chr->bone_data->look_anim.disable = v11;

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

    effect_fog_ring_data_reset();
    effect_splash_data_reset();
}

static void rob_disp_rob_chara_ctrl_thread_main(rob_chara* rob_chr) {
    if (rob_chr->bone_data->get_frame() < 0.0f)
        rob_chr->bone_data->set_frame(0.0f);

    float_t frame_count = rob_chr->bone_data->get_frame_count();
    if (rob_chr->bone_data->get_frame() > frame_count)
        rob_chr->bone_data->set_frame(frame_count);

    rob_chr->item_equip->set_opd_blend_data(&rob_chr->bone_data->motion_loaded);

    vec3 pos = 0.0f;
    rob_chr->get_pos_scale(0, pos);
    rob_chr->item_equip->position = pos;
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
    rob_chara_age_age_array_reset(rob_chr->chara_id);
    rob_chr->item_equip->reset();
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

static void sub_140407280(rob_chara_look_anim* look_anim, std::vector<bone_data>& bones, mat4* mat, float_t step) {
    vec3 v69;
    mat4_get_translation(bones[MOTION_BONE_CL_KAO].node[2].mat, &v69);
    if (!look_anim->field_190 && !look_anim->field_191)
        return;

    float_t v14 = step * 0.25f;
    float_t v15 = look_anim->field_15C.field_8;
    float_t v16 = look_anim->field_15C.field_C;
    mat4* c_kata_r_mat = bones[MOTION_BONE_C_KATA_R].node->mat;
    mat4* c_kata_l_mat = bones[MOTION_BONE_C_KATA_L].node->mat;
    mat4* n_kao_mat = bones[MOTION_BONE_N_KAO].node->mat;

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

    mat4 cl_kao_mat = *bones[MOTION_BONE_CL_KAO].node[1].mat;
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

    mat4 cl_kao_mat_backup = bones[MOTION_BONE_CL_KAO].rot_mat[1];

    mat4 v74;
    mat4_invert(bones[MOTION_BONE_CL_KAO].node->mat, &v74);
    mat4_clear_trans(&v74, &v74);
    mat4_clear_trans(&cl_kao_mat, &cl_kao_mat);
    mat4_mul(&cl_kao_mat, &v74, &bones[MOTION_BONE_CL_KAO].rot_mat[1]);

    for (int32_t i = MOTION_BONE_CL_KAO; i <= MOTION_BONE_N_KUBI_WJ_EX; i++)
        bone_data_mult_1(&bones[i], mat, &bones[MOTION_BONE_N_HARA_CP], true);

    bones[MOTION_BONE_CL_KAO].rot_mat[1] = cl_kao_mat_backup;
}

static void sub_1404189A0(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    sub_140407280(&rob_bone_data->look_anim, v2->bone_data.bones,
        &v2->field_4F8.mat, v2->mot_play_data.frame_data.step);
}

static void sub_140406FC0(rob_chara_look_anim* look_anim, bone_data* bone, mat4* eye_mat, vec3 pos,
    const vec3 rot_neg, const vec3 rot_pos, bool eyes_rot_anim, float_t eyes_rot_blend, float_t eyes_rot_step) {
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
        mat4_lerp_rotation(&bone->rot_mat[0], &mat, &mat, eyes_rot_blend);
    else
        mat4_lerp_rotation(eye_mat, &mat, &mat, eyes_rot_step);
    *eye_mat = mat;
    bone->rot_mat[0] = mat;
}

static void sub_140409170(rob_chara_look_anim* look_anim, mat4* adjust_mat,
    std::vector<bone_data>& bones, mat4* mat, float_t step) {
    if (look_anim->disable)
        return;

    look_anim->bones = &bones;
    look_anim->mat = *mat;

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
    mat4_invert(adjust_mat, &v61);
    mat4_transform_point(&v61, &look_anim->view_point, &look_anim->view_point);

    mat4* kl_eye_l_parent_mat = bones[MOTION_BONE_KL_EYE_L].parent_mat;
    mat4* kl_eye_r_parent_mat = bones[MOTION_BONE_KL_EYE_R].parent_mat;

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
        *(vec3*)&v61.row3 = *(vec3*)&kl_eye_l_parent_mat->row3 + *(vec3*)&kl_eye_r_parent_mat->row3;

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
    sub_140406FC0(look_anim, &bones[MOTION_BONE_KL_EYE_L],
        &look_anim->left_eye_mat, pos_left - look_anim->param.pos,
        rot_neg_left, rot_pos_left, eyes_rot_anim, eyes_rot_blend, eyes_rot_step);

    const vec3 _xor = vec3(-0.0f, 0.0f, 0.0f);
    vec3 rot_neg_right = vec3(xrot_neg, yrot_neg_right, 0.0f);
    vec3 rot_pos_right = vec3(xrot_pos, yrot_pos_right, 0.0f);
    vec3 pos_right;
    mat4_inverse_transform_point(kl_eye_r_parent_mat, &look_anim->view_point, &pos_right);
    sub_140406FC0(look_anim, &bones[MOTION_BONE_KL_EYE_R],
        &look_anim->right_eye_mat, pos_right - (look_anim->param.pos ^ _xor),
        rot_neg_right, rot_pos_right, eyes_rot_anim, eyes_rot_blend, eyes_rot_step);

    for (int32_t i = MOTION_BONE_KL_EYE_L; i <= MOTION_BONE_KL_HIGHLIGHT_L_WJ; i++)
        bone_data_mult_1(&bones[i], mat, &bones[MOTION_BONE_N_HARA_CP], true);

    for (int32_t i = MOTION_BONE_KL_EYE_R; i <= MOTION_BONE_KL_HIGHLIGHT_R_WJ; i++)
        bone_data_mult_1(&bones[i], mat, &bones[MOTION_BONE_N_HARA_CP], true);
}

static void sub_14041A160(rob_chara_bone_data* rob_bone_data, mat4* adjust_mat) {
    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    sub_140409170(&rob_bone_data->look_anim, adjust_mat, v2->bone_data.bones,
        &v2->field_4F8.mat, v2->mot_play_data.frame_data.step);
}

static void rob_chara_head_adjust(rob_chara* rob_chr) {
    if (rob_chr->type != ROB_CHARA_TYPE_2)
        return;

    bool v4 = !(rob_chr->data.field_2 & 0x80);
    rob_chr->data.field_2 &= ~0x80;

    rob_chara_bone_data* rob_bone_data = rob_chr->bone_data;
    vec3* target_view_point = 0;
    if (rob_bone_data->get_look_anim_head_rotation()
        || rob_bone_data->get_look_anim_ext_head_rotation()) {
        if (rob_bone_data->get_look_anim_update_view_point()) {
            vec3 view_point;
            rctx_ptr->camera->get_view_point(view_point);
            rob_bone_data->set_look_anim_target_view_point(&view_point);
        }

        if (rob_bone_data->check_look_anim_head_rotation()
            || rob_bone_data->check_look_anim_ext_head_rotation())
            target_view_point = rob_bone_data->get_look_anim_target_view_point();
    }

    sub_14041C620(rob_bone_data, target_view_point, &rob_chr->data.adjust_data.mat, v4);
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
    mat4_inverse_transform_point(mat, &a2, &v8);
    float_t v5 = vec2::length(*(vec2*)&v8);
    float_t v6 = vec3::length(v8);
    if (fabsf(v5) > 0.000001f && fabsf(v6) > 0.000001f) {
        float_t v7 = v5;
        v5 = 1.0f / v5;
        v6 = 1.0f / v6;
        mat4_mul_rotate_z(mat, v5 * v8.y, v5 * v8.x, mat);
        mat4_mul_rotate_y(mat, -v6 * v8.z, v6 * v7, mat);
    }
}

static void sub_140406A70(vec3* a1, std::vector<bone_data>& bones, mat4* a3, vec3* a4,
    const motion_bone_index* a5, float_t rotation_blend, float_t arm_length, bool solve_ik) {
    bone_data* v14 = &bones[a5[0]];

    if (v14->pole_target_mat) {
        vec3 v36;
        mat4_get_translation(v14->pole_target_mat, &v36);

        mat4 v38 = *v14->node->mat;
        sub_14040AE10(&v38, v14->ik_target);
        mat4_inverse_transform_point(&v38, &v36, &v36);

        v38 = *v14->node->mat;
        sub_14040AE10(&v38, *a4);

        vec3 v37;
        mat4_transform_point(&v38, &v36, &v37);
        mat4_set_translation(v14->pole_target_mat, &v37);
    }

    v14->ik_target = *a4;
    v14->rot_mat[1] = mat4_identity;
    v14->rot_mat[2] = mat4_identity;
    bone_data_mult_ik(v14, 1);
    if (a5[1] == MOTION_BONE_NONE)
        return;

    bone_data* v25 = &bones[a5[1]];
    if (rotation_blend > 0.0f) {
        mat4 v38;
        mat4 v39;
        mat4_clear_trans(v25->parent_mat, &v38);
        mat4_clear_trans(v25->node->mat, &v39);
        mat4_transpose(&v38, &v38);
        mat4_mul(&v39, &v38, &v38);
        if (rotation_blend < 1.0f)
            mat4_lerp_rotation(&v25->rot_mat[0], &v38, &v38, rotation_blend);
        v25->rot_mat[0] = v38;
    }

    if (solve_ik) {
        a5++;
        while (*a5 != MOTION_BONE_NONE)
            bone_data_mult_1(&bones[*a5++], a3, &bones[MOTION_BONE_N_HARA_CP], true);
    }
    else
        bone_data_mult_1(v25, a3, &bones[MOTION_BONE_N_HARA_CP], false);
}

static void sub_140418A00(rob_chara_bone_data* rob_bone_data, vec3* a2,
    const motion_bone_index* a3, float_t rotation_blend, float_t arm_length, bool solve_ik) {
    motion_blend_mot* v7 = rob_bone_data->motion_loaded.front();
    mat4 m = v7->field_4F8.mat;
    sub_140406A70(rob_bone_data->field_76C, v7->bone_data.bones,
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

    vec3 v39 = v38 + v27;

    vec3 v18 = 0.0f;
    if (v15 > 0) {
        mat4_transform_point(&v42, &rob_chr->data.motion.hand_adjust[hand].offset, &v38);
        v18 = v38 + v27;
    }

    sub_140418A00(rob_chr->bone_data, &v39, off_140C9E020[hand], rotation_blend,
        rob_chr->data.motion.hand_adjust[hand].arm_length, solve_ik);

    while (v15 > 0) {
        mat4* v40 = rob_chr->bone_data->get_mats_mat(rob_kl_te_bones[hand]);
        mat4_set_translation(v40, &v18);

        vec3 v37 = -rob_chr->data.motion.hand_adjust[hand].offset;
        mat4_transform_point(v40, &v37, &v39);
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
            mat4* v26 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))
                ->get_mats_mat(dword_140A2DDD0[v0.field_C]);
            mat4_get_translation(v26, &v45);
            v45 += v0.field_10;
        }
        else {
            mat4* v23 = ((rob_chara_bone_data*)((size_t)rob_chr->field_20 + 0x28))
                ->get_mats_mat(dword_140A2DDD0[v0.field_C]);

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
    sub_140418A00(rob_chr->bone_data, &v48, off_140C9E000[a2], 0.0f, 0.0f, true);
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

static void sub_1403F9B20(rob_chara_sleeve_adjust* a1, motion_bone_index motion_bone_index) {
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

    mat4* v42 = v15[motion_bone_index].node[2].mat;
    sub_14040AE10(v42, v39 + v41);
}

static void sub_1403FAF30(rob_chara_sleeve_adjust* a1, std::vector<bone_data>& bones, float_t step) {
    a1->step = step;
    a1->bones = &bones;
    if (a1->enable1)
        sub_1403FA770(a1);

    if (a1->enable2) {
        sub_1403F9B20(a1, MOTION_BONE_C_KATA_L);
        sub_1403F9B20(a1, MOTION_BONE_C_KATA_R);
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

static float_t sub_140406E90(bone_data* a1, bone_data* a2, float_t a3, vec3* a4) {
    vec3 v18 = 0.0f;
    mat4_transform_point(a1->node[3].mat, &v18, &v18);
    *a4 = v18;

    vec3 v17;
    v17.x = 0.01f;
    v17.y = -0.05f;
    v17.z = 0.0f;
    mat4_transform_point(a2->node[0].mat, &v18, &v17);

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
    rob_chara_bone_data_ik_scale* ik_scale, std::vector<bone_data>& bones,
    bone_database_skeleton_type base_skeleton_type,
    bone_database_skeleton_type skeleton_type, const bone_database* bone_data) {
    const char* base_name = bone_database_skeleton_type_to_string(base_skeleton_type);
    const char* name = bone_database_skeleton_type_to_string(skeleton_type);
    const float_t* base_heel_height = bone_data->get_skeleton_heel_height(base_name);
    const float_t* heel_height = bone_data->get_skeleton_heel_height(name);
    if (!base_heel_height || !heel_height)
        return;

    ::bone_data* b_cl_mune = &bones[MOTION_BONE_CL_MUNE];
    ::bone_data* b_kl_kubi = &bones[MOTION_BONE_KL_KUBI];
    ::bone_data* b_c_kata_l = &bones[MOTION_BONE_C_KATA_L];
    ::bone_data* b_cl_momo_l = &bones[MOTION_BONE_CL_MOMO_L];

    float_t base_height = fabsf(b_cl_momo_l->base_position[0].y) + b_cl_momo_l->ik_segment_length[0]
        + b_cl_momo_l->ik_2nd_segment_length[0] + *base_heel_height;
    float_t height = fabsf(b_cl_momo_l->base_position[1].y) + b_cl_momo_l->ik_segment_length[1]
        + b_cl_momo_l->ik_2nd_segment_length[1] + *heel_height;
    ik_scale->ratio0 = height / base_height;
    ik_scale->ratio1 = (fabsf(b_kl_kubi->base_position[1].y) + b_cl_mune->ik_segment_length[1])
        / (fabsf(b_kl_kubi->base_position[0].y) + b_cl_mune->ik_segment_length[0]);
    ik_scale->ratio2 = (b_c_kata_l->ik_segment_length[1] + b_c_kata_l->ik_2nd_segment_length[1])
        / (b_c_kata_l->ik_segment_length[0] + b_c_kata_l->ik_2nd_segment_length[0]);
    ik_scale->ratio3 = (fabsf(b_kl_kubi->base_position[1].y) + b_cl_mune->ik_segment_length[1] + height)
        / (fabsf(b_kl_kubi->base_position[0].y) + b_cl_mune->ik_segment_length[0] + base_height);
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
    return !a1->blend || !a1->blend->enable;
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

static void sub_140412BB0(motion_blend_mot* a1, std::vector<bone_data>& bones) {
    for (bone_data& i : a1->bone_data.bones)
        i.copy_rot_trans(&bones[i.motion_bone_index]);
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
            rob_bone_data->motion_loaded.front()->bone_data.bones);
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
        rob_bone_data->motion_loaded.front()->bone_data.bones);
    motion_blend_mot_load_file(rob_bone_data->motion_loaded.front(),
        motion_id, MOTION_BLEND_CROSS, 1.0f, bone_data, mot_db);
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

static void rob_chara_bone_data_set_look_anim_param(rob_chara_bone_data* rob_bone_data,
    const rob_chara_look_anim_eye_param* param, eyes_adjust* eyes_adjust) {
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
        bone_data* data = i->bone_data.bones.data();
        data[MOTION_BONE_KL_EYE_L].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        data[MOTION_BONE_KL_EYE_L].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
        data[MOTION_BONE_KL_EYE_R].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        data[MOTION_BONE_KL_EYE_R].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
    }

    rob_bone_data->look_anim.set_eyes_xrot_adjust(eyes_xrot_adjust_neg, eyes_xrot_adjust_pos);
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
    mat4_mul(&mat, &rot_mat, &rot_mat);
    mat = b_kl_hara_xz->rot_mat[0];
    mat4_mul(&mat, &rot_mat, &rot_mat);
    mat = b_kl_hara_etc->rot_mat[0];
    mat4_mul(&mat, &rot_mat, &rot_mat);
    b_n_hara_cp->rot_mat[0] = rot_mat;
    b_kg_hara_y->rot_mat[0] = mat4_identity;
    b_kl_hara_xz->rot_mat[0] = mat4_identity;
    b_kl_hara_etc->rot_mat[0] = mat4_identity;

    float_t v8 = a1->field_4F8.field_C0;
    float_t v9 = a1->field_4F8.field_C4;
    vec3 v10 = a1->field_4F8.field_C8;
    a1->field_4F8.field_A8 = b_n_hara_cp->position;
    if (!a1->mot_key_data.skeleton_select) {
        if (a2 != v9) {
            b_n_hara_cp->position.x = (b_n_hara_cp->position.x - v10.x) * v9 + v10.x;
            b_n_hara_cp->position.z = (b_n_hara_cp->position.z - v10.z) * v9 + v10.z;
        }
        b_n_hara_cp->position.y = ((b_n_hara_cp->position.y - v10.y) * v8) + v10.y;
    }
    else {
        if (a2 != v9) {
            v9 /= a2;
            b_n_hara_cp->position.x = (b_n_hara_cp->position.x - v10.x) * v9 + v10.x;
            b_n_hara_cp->position.z = (b_n_hara_cp->position.z - v10.z) * v9 + v10.z;
        }

        if (a2 != v8) {
            v8 /= a2;
            b_n_hara_cp->position.y = (b_n_hara_cp->position.y - v10.y) * v8 + v10.y;
        }
    }
}

static void sub_140410A40(motion_blend_mot* a1, std::vector<bone_data>* a2, std::vector<bone_data>* a3) {
    MotionBlend* v4 = a1->blend;
    if (!v4 || !v4->enable)
        return;

    v4->Field_20(a2, a3);

    for (bone_data& i : *a2) {
        if (!sub_140410250(&a1->field_0.field_8, i.motion_bone_index))
            continue;

        bone_data* v8 = 0;
        if (a3)
            v8 = &a3->data()[i.motion_bone_index];
        v4->Blend(&a2->data()[i.motion_bone_index], v8);
    }
}

static void sub_140410B70(motion_blend_mot* a1, std::vector<bone_data>* a2) {
    MotionBlend* v3 = a1->blend;
    if (!v3 || !v3->enable)
        return;

    v3->Field_20(&a1->bone_data.bones, a2);

    for (bone_data& i : a1->bone_data.bones) {
        if (!sub_140410250(&a1->field_0.field_8, i.motion_bone_index))
            continue;

        bone_data* v6 = 0;
        if (a2)
            v6 = &a2->data()[i.motion_bone_index];
        v3->Blend(&a1->bone_data.bones.data()[i.motion_bone_index], v6);
    }
}

static void sub_140410CB0(mot_blend* a1, std::vector<bone_data>* a2) {
    if (!a1->blend.enable)
        return;

    for (bone_data& i : *a2)
        if (sub_140410250(&a1->field_0.field_8, i.motion_bone_index))
            a1->blend.Blend(&a2->data()[i.motion_bone_index], 0);
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

        std::vector<bone_data>* bones = 0;
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

    bone_data* v7 = &v3->bone_data.bones.data()[0];
    v3->field_4F8.field_9C = v7->position;
    if (sub_140413790(&v3->field_4F8)) { // WTF???
        v3->field_4F8.field_90 = v7->position;
        v7->position -= v7->position;
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
    mat4* mats = rob_itm_equip->matrices;
    float_t y = 1.0f;
    y = min_def(y, mats[ROB_BONE_N_HARA_CP     ].row3.y);
    y = min_def(y, mats[ROB_BONE_KL_ASI_L_WJ_CO].row3.y);
    y = min_def(y, mats[ROB_BONE_KL_ASI_R_WJ_CO].row3.y);
    y = min_def(y, mats[ROB_BONE_KL_TE_L_WJ    ].row3.y);
    y = min_def(y, mats[ROB_BONE_KL_TE_R_WJ    ].row3.y);
    return y;
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

static void opd_chara_data_array_add_frame_data(int32_t chara_id) {
    opd_chara_data_array_get(chara_id)->add_frame_data();
}

static void opd_chara_data_array_encode_data(int32_t chara_id) {
    opd_chara_data_array_get(chara_id)->encode_data();
}

static void opd_chara_data_array_encode_init_data(int32_t chara_id, int32_t motion_id) {
    opd_chara_data_array_get(chara_id)->encode_init_data(motion_id);
}

static void opd_chara_data_array_fs_copy_file(int32_t chara_id) {
    opd_chara_data_array_get(chara_id)->fs_copy_file();
}

static opd_chara_data* opd_chara_data_array_get(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        chara_id = 0;
    return &opd_chara_data_array[chara_id];
}

static void opd_chara_data_array_init_data(int32_t chara_id, int32_t motion_id) {
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
    if (!app::TaskWork::check_task_ready(task_rob_manager)
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

void rob_chara_array_reset_pv_data(int32_t chara_id) {
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
    return app::TaskWork::add_task(task_rob_load, 0, "ROB LOAD", 0);
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
    task_rob_load->del();
    return true;
}

bool task_rob_manager_add_task() {
    return app::TaskWork::add_task(task_rob_manager, "ROB_MANAGER TASK");
}

bool task_rob_manager_check_chara_loaded(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return false;
    return task_rob_manager->CheckCharaLoaded(&rob_chara_array[chara_id]);
}

bool task_rob_manager_check_task_ready() {
    return app::TaskWork::check_task_ready(task_rob_manager);
}

bool task_rob_manager_hide_task() {
    return task_rob_manager->hide();
}

bool task_rob_manager_get_wait(int32_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return false;

    return task_rob_manager->GetWait(&rob_chara_array[chara_id]);
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

bone_node_expression_data::bone_node_expression_data() {
    position = 0.0f;
    rotation = 0.0f;
    scale = 1.0f;
    parent_scale = 1.0f;
}

void bone_node_expression_data::mat_set(vec3& parent_scale, mat4& ex_data_mat, mat4& mat) {
    vec3 position = this->position * parent_scale;
    mat4_mul_translate(&ex_data_mat, &position, &ex_data_mat);
    mat4_mul_rotate_zyx(&ex_data_mat, &rotation, &ex_data_mat);
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

    parent = 0;
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

void mot_key_data::reset() {
    key_sets_ready = 0;
    skeleton_type = BONE_DATABASE_SKELETON_NONE;
    frame = -1.0f;
    key_set_count = 0;
    mot_data = 0;
    skeleton_select = 0;
    field_68 = {};

    key_set.clear();
    key_set_data.clear();
    motion_id = -1;
}

eyes_adjust::eyes_adjust() : xrot_adjust(), base_adjust() {
    neg = -1.0f;
    pos = -1.0f;
}

bone_data::bone_data() : type(), has_parent(), motion_bone_index(), mirror(), parent(),
flags(), key_set_offset(), key_set_count(), frame(), ik_target(), pole_target_mat(),
parent_mat(), node(), ik_segment_length(), ik_2nd_segment_length(), arm_length() {
    eyes_xrot_adjust_neg = 1.0f;
    eyes_xrot_adjust_pos = 1.0f;
}

void bone_data::copy_rot_trans(bone_data* data) {
    if (check_flags_not_null())
        return;

    switch (type) {
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        position = data->position;
        position_prev[0] = data->position_prev[0];
        break;
    }

    switch (type) {
    case BONE_DATABASE_BONE_ROTATION:
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        rot_mat[0] = data->rot_mat[0];
        rot_mat_prev[0][0] = data->rot_mat_prev[0][0];
        break;
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        rot_mat[0] = data->rot_mat[0];
        rot_mat[1] = data->rot_mat[1];
        rot_mat_prev[0][0] = data->rot_mat_prev[0][0];
        rot_mat_prev[1][0] = data->rot_mat_prev[1][0];
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        rot_mat[0] = data->rot_mat[0];
        rot_mat[1] = data->rot_mat[1];
        rot_mat[2] = data->rot_mat[2];
        rot_mat_prev[0][0] = data->rot_mat_prev[0][0];
        rot_mat_prev[1][0] = data->rot_mat_prev[1][0];
        rot_mat_prev[2][0] = data->rot_mat_prev[2][0];
        break;
    }
}

bool bone_data::check_flags_not_null() {
    return !!flags;
}

vec3* bone_data::set_key_data(vec3* keyframe_data,
    bone_database_skeleton_type skeleton_type, bool get_data, bool reverse_x) {
    if (type == BONE_DATABASE_BONE_POSITION_ROTATION) {
        if (get_data) {
            position = *keyframe_data;
            if (reverse_x)
                position.x = -position.x;
        }
        keyframe_data++;
    }
    else if (type >= BONE_DATABASE_BONE_HEAD_IK_ROTATION
        && type <= BONE_DATABASE_BONE_LEGS_IK_ROTATION) {
        if (get_data) {
            ik_target = *keyframe_data;
            if (reverse_x)
                ik_target.x = -ik_target.x;
        }
        keyframe_data++;
    }

    if (get_data) {
        if (type == BONE_DATABASE_BONE_TYPE_1 || type == BONE_DATABASE_BONE_POSITION) {
            position = *keyframe_data;
            if (reverse_x)
                position.x = -position.x;
        }
        else if (!flags) {
            rotation = *keyframe_data;
            if (reverse_x) {
                const skeleton_rotation_offset* rot_off = skeleton_rotation_offset_array;
                size_t index = motion_bone_index;
                if (rot_off[index].x)
                    rotation.x = rot_off[index].rotation.x - rotation.x;
                if (rot_off[index].y)
                    rotation.y = rot_off[index].rotation.y - rotation.y;
                if (rot_off[index].z)
                    rotation.z = rot_off[index].rotation.z - rotation.z;
            }
        }
    }
    keyframe_data++;
    return keyframe_data;
}

void bone_data::store_curr_rot_trans(int32_t skeleton_select) {
    if (check_flags_not_null())
        return;

    switch (type) {
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        position_prev[skeleton_select] = position;
        break;
    }

    switch (type) {
    case BONE_DATABASE_BONE_ROTATION:
        rot_mat_prev[0][skeleton_select] = rot_mat[0];
        break;
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        rot_mat_prev[0][skeleton_select] = rot_mat[0];
        break;
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        rot_mat_prev[0][skeleton_select] = rot_mat[0];
        rot_mat_prev[1][skeleton_select] = rot_mat[1];
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        rot_mat_prev[0][skeleton_select] = rot_mat[0];
        rot_mat_prev[1][skeleton_select] = rot_mat[1];
        rot_mat_prev[2][skeleton_select] = rot_mat[2];
        break;
    }
}

bone_data_parent::bone_data_parent() : rob_bone_data(),
motion_bone_count(), ik_bone_count(), chain_pos(), global_position(),
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

void MotionBlendCross::Field_20(std::vector<bone_data>* bones_curr, std::vector<bone_data>* bones_prev) {
    if (!bones_curr || !bones_prev)
        return;

    bone_data& n_hara_cp_curr = (*bones_curr)[MOTION_BONE_N_HARA_CP];
    bone_data& n_hara_curr = (*bones_curr)[MOTION_BONE_N_HARA];
    bone_data& n_hara_cp_prev = (*bones_prev)[MOTION_BONE_N_HARA_CP];
    bone_data& n_hara_prev = (*bones_prev)[MOTION_BONE_N_HARA];
    mat4_mul(&n_hara_curr.rot_mat[0], &n_hara_cp_curr.rot_mat[0], &field_64);
    mat4_mul(&n_hara_cp_prev.rot_mat[0], &rot_y_mat, &field_A4);
    mat4_blend_rotation(&field_A4, &n_hara_cp_curr.rot_mat[0], &n_hara_cp_curr.rot_mat[0], blend);
    mat4_mul(&n_hara_prev.rot_mat[0], &field_A4, &field_A4);
    mat4_blend_rotation(&n_hara_prev.rot_mat[0], &n_hara_curr.rot_mat[0], &n_hara_curr.rot_mat[0], blend);
    mat4_mul(&n_hara_curr.rot_mat[0], &n_hara_cp_curr.rot_mat[0], &field_E4);
    mat4_transpose(&field_E4, &field_E4);
}

void MotionBlendCross::Blend(bone_data* curr, bone_data* prev) {
    if (!curr || !prev || curr->check_flags_not_null())
        return;

    mat4 prev_mat;
    mat4 mat;
    mat4 ik_mat;

    switch (curr->type) {
    case BONE_DATABASE_BONE_ROTATION:
    default:
        if (curr->motion_bone_index == MOTION_BONE_KL_KOSI_Y) {
            mat4_mul(&prev->rot_mat[0], &field_A4, &prev_mat);
            mat4_mul(&curr->rot_mat[0], &field_64, &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
            mat4_mul(&mat, &field_E4, &curr->rot_mat[0]);
        }
        else if (curr->motion_bone_index < 0 || curr->motion_bone_index > MOTION_BONE_N_HARA)
            mat4_blend_rotation(&prev->rot_mat[0], &curr->rot_mat[0], &curr->rot_mat[0], blend);
        break;
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
        curr->position = vec3::lerp(prev->position, curr->position, blend);
        break;
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        if (trans_xz) {
            curr->position.x = lerp_def(prev->position.x, curr->position.x, blend);
            curr->position.z = lerp_def(prev->position.z, curr->position.z, blend);
        }

        if (trans_y)
            curr->position.y = lerp_def(prev->position.y, curr->position.y, blend);
        break;
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        if (curr->motion_bone_index == MOTION_BONE_CL_MUNE) {
            mat4_mul(&prev->rot_mat[0], &field_A4, &prev_mat);
            mat4_mul(&prev->rot_mat[1], &prev_mat, &prev_mat);
            mat4_mul(&curr->rot_mat[0], &field_64, &mat);
            mat4_mul(&curr->rot_mat[1], &mat, &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
            mat4_mul(&mat, &field_E4, &mat);
        }
        else {
            mat4_mul(&prev->rot_mat[1], &prev->rot_mat[0], &prev_mat);
            mat4_mul(&curr->rot_mat[1], &curr->rot_mat[0], &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
        }
        mat4_transpose(&curr->rot_mat[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->rot_mat[1]);
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        mat4_blend_rotation(&prev->rot_mat[2], &curr->rot_mat[2], &curr->rot_mat[2], blend);
        mat4_mul(&prev->rot_mat[1], &prev->rot_mat[0], &prev_mat);
        mat4_mul(&curr->rot_mat[1], &curr->rot_mat[0], &mat);
        mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
        mat4_transpose(&curr->rot_mat[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->rot_mat[1]);
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

void MotionBlendFreeze::Field_20(std::vector<bone_data>* bones_curr, std::vector<bone_data>* bones_prev) {
    if (!bones_curr)
        return;

    bone_data& n_hara_cp = (*bones_curr)[MOTION_BONE_N_HARA_CP];
    bone_data& n_hara = (*bones_curr)[MOTION_BONE_N_HARA];
    mat4_mul(&n_hara.rot_mat[0], &n_hara_cp.rot_mat[0], &field_74);
    mat4_mul(&n_hara_cp.rot_mat_prev[0][field_24], &rot_y_mat, &field_B4);
    mat4_blend_rotation(&field_B4, &n_hara_cp.rot_mat[0], &n_hara_cp.rot_mat[0], blend);
    mat4_mul(&n_hara.rot_mat_prev[0][field_24], &field_B4, &field_B4);
    mat4_blend_rotation(&n_hara.rot_mat_prev[0][field_24], &n_hara.rot_mat[0], &n_hara.rot_mat[0], blend);
    mat4_mul(&n_hara.rot_mat[0], &n_hara_cp.rot_mat[0], &field_F4);
    mat4_transpose(&field_F4, &field_F4);
}

void MotionBlendFreeze::Blend(bone_data* curr, bone_data* prev) {
    if (!curr || curr->check_flags_not_null())
        return;

    mat4 prev_mat;
    mat4 mat;
    mat4 ik_mat;

    switch (curr->type) {
    case BONE_DATABASE_BONE_ROTATION:
    default:
        if (curr->motion_bone_index == MOTION_BONE_KL_KOSI_Y) {
            mat4_mul(&curr->rot_mat_prev[0][field_24], &field_B4, &prev_mat);
            mat4_mul(&curr->rot_mat[0], &field_74, &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
            mat4_mul(&mat, &field_F4, &curr->rot_mat[0]);
        }
        else if (curr->motion_bone_index < 0 || curr->motion_bone_index > MOTION_BONE_N_HARA)
            mat4_blend_rotation(&curr->rot_mat_prev[0][field_24], &curr->rot_mat[0], &curr->rot_mat[0], blend);
        break;
    case BONE_DATABASE_BONE_TYPE_1:
    case BONE_DATABASE_BONE_POSITION:
        curr->position = vec3::lerp(curr->position_prev[field_24], curr->position, blend);
        break;
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        if (trans_xz) {
            curr->position.x = lerp_def(curr->position_prev[field_24].x, curr->position.x, blend);
            curr->position.z = lerp_def(curr->position_prev[field_24].z, curr->position.z, blend);
        }

        if (trans_y)
            curr->position.y = lerp_def(curr->position_prev[field_24].y, curr->position.y, blend);
        break;
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION:
        if (curr->motion_bone_index == MOTION_BONE_CL_MUNE) {
            mat4_mul(&curr->rot_mat_prev[0][field_24], &field_B4, &prev_mat);
            mat4_mul(&curr->rot_mat_prev[1][field_24], &prev_mat, &prev_mat);
            mat4_mul(&curr->rot_mat[0], &field_74, &mat);
            mat4_mul(&curr->rot_mat[1], &mat, &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
            mat4_mul(&mat, &field_F4, &mat);
        }
        else {
            mat4_mul(&curr->rot_mat_prev[1][field_24], &curr->rot_mat_prev[0][field_24], &prev_mat);
            mat4_mul( &curr->rot_mat[1], &curr->rot_mat[0], &mat);
            mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
        }
        mat4_transpose(&curr->rot_mat[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->rot_mat[1]);
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        mat4_blend_rotation(&curr->rot_mat_prev[2][field_24], &curr->rot_mat[2], &curr->rot_mat[2], blend);
        mat4_mul(&curr->rot_mat_prev[1][field_24], &curr->rot_mat_prev[0][field_24], &prev_mat);
        mat4_mul(&curr->rot_mat[1], &curr->rot_mat[0], &mat);
        mat4_blend_rotation(&prev_mat, &mat, &mat, blend);
        mat4_transpose(&curr->rot_mat[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->rot_mat[1]);
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

void PartialMotionBlendFreeze::Field_20(std::vector<bone_data>* bones_curr, std::vector<bone_data>* bones_prev) {

}

void PartialMotionBlendFreeze::Blend(bone_data* curr, bone_data* prev) {
    if (!curr || curr->check_flags_not_null())
        return;

    mat4 prev_mat;
    mat4 mat;
    mat4 ik_mat;

    switch (curr->type) {
    case BONE_DATABASE_BONE_ROTATION:
    default:
        mat4_lerp_rotation(&curr->rot_mat_prev[0][0], &curr->rot_mat[0], &curr->rot_mat[0], blend);
        break;
    case BONE_DATABASE_BONE_POSITION:
    case BONE_DATABASE_BONE_TYPE_1:
        curr->position = vec3::lerp(curr->position_prev[0], curr->position, blend);
        break;
    case BONE_DATABASE_BONE_POSITION_ROTATION:
        mat4_lerp_rotation(&curr->rot_mat_prev[0][0], &curr->rot_mat[0], &curr->rot_mat[0], blend);
        curr->position = vec3::lerp(curr->position_prev[0], curr->position, blend);
        break;
    case BONE_DATABASE_BONE_ARM_IK_ROTATION:
    case BONE_DATABASE_BONE_LEGS_IK_ROTATION:
        mat4_lerp_rotation(&curr->rot_mat_prev[2][0], &curr->rot_mat[2], &curr->rot_mat[2], blend);
    case BONE_DATABASE_BONE_HEAD_IK_ROTATION: {
        mat4_mul(&curr->rot_mat_prev[1][0], &curr->rot_mat_prev[0][0], &prev_mat);
        mat4_mul(&curr->rot_mat[1], &curr->rot_mat[0], &mat);
        mat4_lerp_rotation(&prev_mat, &mat, &mat, blend);
        mat4_transpose(&curr->rot_mat[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->rot_mat[1]);
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
    mot_key_data.reset();
    mot_play_data.reset();
    field_4F8.field_C0 = 1.0f;
    field_4F8.field_C4 = 1.0f;
}

motion_blend_mot::~motion_blend_mot() {

}

bool motion_blend_mot::get_blend_enable() {
    if (blend && !blend->Field_30())
        return blend->enable;
    return false;
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
    mot_key_data.reset();
    mot_play_data.reset();
    blend.Reset();
    field_0.bone_check_func = 0;
    sub_1404119A0(&field_0.field_8);
    field_0.motion_bone_count = 0;
    field_30 = 0;
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

void rob_chara_look_anim::set_eyes_xrot_adjust(float_t neg, float_t pos) {
    eyes_xrot_adjust_neg = neg;
    eyes_xrot_adjust_pos = pos;
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

rob_chara_bone_data::rob_chara_bone_data() : field_0(), field_1(), object_bone_count(), total_bone_count(),
motion_bone_count(), ik_bone_count(), chain_pos(), disable_eye_motion(), field_76C(), look_anim() {
    base_skeleton_type = BONE_DATABASE_SKELETON_NONE;
    skeleton_type = BONE_DATABASE_SKELETON_NONE;
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
        motion_blend_mot_interpolate(i);

    bone_database_skeleton_type skeleton_type = base_skeleton_type;
    motion_blend_mot* v5 = motion_loaded.front();
    std::vector<bone_data>& bones = v5->bone_data.bones;
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
    look_anim.reset();
    sleeve_adjust.reset();
}

void rob_chara_bone_data::set_disable_eye_motion(bool value) {
    disable_eye_motion = value;
}

void rob_chara_bone_data::set_frame(float_t frame) {
    motion_loaded.front()->mot_play_data.frame_data.set_frame(frame);
}

void rob_chara_bone_data::set_look_anim_target_view_point(vec3* value) {
    look_anim.target_view_point = *value;
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

    mdl::ObjFlags flags = rctx->disp_manager->get_obj_flags();
    mdl::ObjFlags chara_flags = flags;
    if (fabsf(alpha - 1.0f) > 0.000001f)
        enum_or(chara_flags, obj_flags);
    else
        enum_and(chara_flags, ~(mdl::OBJ_ALPHA_ORDER_3 | mdl::OBJ_ALPHA_ORDER_2 | mdl::OBJ_ALPHA_ORDER_1));
    rctx->disp_manager->set_obj_flags(chara_flags);
    if (can_disp) {
        rctx->disp_manager->entry_obj_by_object_info_object_skin(obj_info,
            &texture_pattern, &texture_data, alpha, mats, ex_data_bone_mats.data(), 0, mat);

        for (ExNodeBlock*& i : node_blocks)
            i->Disp(mat, rctx);
    }
    rctx->disp_manager->set_obj_flags(flags);
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

RobOsageNode* rob_chara_item_equip_object::get_normal_ref_osage_node(const std::string& str, size_t* index) {
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

    RobOsageNode* node = 0;
    for (ExOsageBlock*& i : osage_blocks)
        if (!name.compare(i->name)) {
            if (node_idx + 1 < i->rob.nodes.size())
                node = i->rob.GetNode(node_idx + 1);
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

    int32_t num_bone_name = ex_data->num_bone_name;
    ex_data_bone_nodes.resize(num_bone_name);
    ex_data_bone_mats.resize(num_bone_name);
    ex_data_mats.resize(num_bone_name);

    bone_node* bone_nodes = ex_data_bone_nodes.data();
    mat4* bone_mats = ex_data_bone_mats.data();
    mat4* mats = ex_data_mats.data();
    for (int32_t i = 0; i < num_bone_name; i++)
        bone_nodes[i].mat = &bone_mats[i];

    if (ex_data->bone_name_array) {
        ex_bones.clear();
        const char** bone_name_array = ex_data->bone_name_array;
        for (int32_t i = 0; i < num_bone_name; i++) {
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
    int32_t num_block = ex_data->num_block;
    for (int32_t i = 0; i < num_block; i++, block++) {
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
                bone_name_array[block->osage->name_index & 0x7FFF],
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
        for (int32_t i = 0; i < ex_data->num_osage_sibling_info; i++, sibling_info++) {
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

    obj_skin* skin = objset_info_storage_get_obj_skin(this->obj_info);
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
    for (const skin_param_osage_root_boc& i : skp_root.boc)
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

void rob_chara_item_equip_object::set_disable_collision(rob_osage_parts_bit parts_bits, bool disable) {
    for (ExOsageBlock*& i : osage_blocks)
        if (i->rob.CheckPartsBits(parts_bits))
            i->SetDisableCollision(disable);
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
    if (!skp_file_data)
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
        field_1B8 |= root.coli_type != SkinParam::RootCollisionTypeEnd;
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
    skp_file_data->field_88 |= skp_file_data->skin_param.coli_type > SkinParam::RootCollisionTypeEnd;

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
        for (ExOsageBlock*& j : osage_blocks) {
            if (i.ed_root.compare(j->name)
                || i.ed_node >= node_data->size()
                || i.st_node >= j->rob.nodes.size())
                continue;

            RobOsageNodeData& ed_node = node_data->data()[i.ed_node];
            RobOsageNode* st_node = j->rob.GetNode(i.st_node + 1ULL);
            ed_node.boc.push_back(st_node);
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

    std::string buf = string_to_lower(sprintf_s_string(
        "ext_skp_%s.txt", obj_db->get_object_name(obj_info)));

    key_val kv;
    ((data_struct*)data)->load_file(&kv, "rom/skin_param/", buf.c_str(), key_val::load_file);
    skp_load(&kv, bone_data);
}

bool rob_chara_item_equip_object::skp_load_normal_ref(
    const skin_param_osage_root& skp_root, std::vector<RobOsageNodeData>* node_data) {
    if (!skp_root.normal_ref.size())
        return false;

    for (const skin_param_osage_root_normal_ref& i : skp_root.normal_ref) {
        size_t index = 0;
        RobOsageNode* n = get_normal_ref_osage_node(i.n, &index);
        if (!n)
            continue;

        RobOsageNodeData* data = node_data ? &(*node_data)[index] : n->data_ptr;
        data->normal_ref.n = n;
        data->normal_ref.u = get_normal_ref_osage_node(i.u, 0);
        data->normal_ref.d = get_normal_ref_osage_node(i.d, 0);
        data->normal_ref.l = get_normal_ref_osage_node(i.l, 0);
        data->normal_ref.r = get_normal_ref_osage_node(i.r, 0);
        data->normal_ref.GetMat();
    }
    return true;
}

rob_chara_item_equip::rob_chara_item_equip() : bone_nodes(), matrices(), item_equip_object(), field_18(),
item_equip_range(), first_item_equip_object(), max_item_equip_object(), field_A0(), shadow_type(), position(),
field_D4(), disable_update(), field_DC(), texture_color_coefficients(), wet(), wind_strength(), chara_color(),
npr_flag(), mat(), field_13C(), field_8BC(), field_8C0(), field_8C4(), field_8C8(), field_8CC(),
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
        mat4_mul_rotate_zyx(&mat, (float_t)(-90.9 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    if (rob_itm_equip->field_DC == 2) {
        name = "kl_te_l_wj";
        mat4_translate(0.0f, 0.0015f, -0.0812f, &mat);
        mat4_mul_rotate_zyx(&mat, (float_t)(-34.5 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    else
        name = "j_1_hyoutan_000wj";

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;

    bone_node* node = rob_itm_equip->item_equip_object[rob_itm_equip->field_D4].get_bone_node(name, aft_bone_data);
    if (!node || !node->mat)
        return;

    mat4_mul(node->mat, &mat, &mat);
    int32_t tex_pat_count = (int32_t)rob_itm_equip->texture_pattern.size();
    if (tex_pat_count)
        rctx->disp_manager->set_texture_pattern(tex_pat_count, rob_itm_equip->texture_pattern.data());
    rctx->disp_manager->entry_obj_by_object_info(&mat, rob_itm_equip->field_D0);
    if (tex_pat_count)
        rctx->disp_manager->set_texture_pattern();
}

void rob_chara_item_equip::disp(int32_t chara_id, render_context* rctx) {
    mdl::ObjFlags flags = (mdl::ObjFlags)0;
    if (rctx->chara_reflect)
        enum_or(flags, mdl::OBJ_CHARA_REFLECT);
    if (rctx->chara_refract)
        enum_or(flags, mdl::OBJ_REFRACT);

    mdl::DispManager& disp_manager = *rctx->disp_manager;
    if (shadow_type != -1) {
        if (field_A0 & 0x04) {
            vec3 pos = position;
            pos.y -= 0.2f;
            shadow_ptr_get()->field_1D0[shadow_type].push_back(pos);

            float_t v9;
            if (sub_140512F60(this) <= -0.2f)
                v9 = -0.5f;
            else
                v9 = 0.05f;
            shadow_ptr_get()->field_1C0[shadow_type] = v9;
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
    rctx->render_manager->field_31C |= npr_flag;
    sub_140512C20(this, rctx);
    rob_chara_age_age_array_disp(rctx, chara_id, rctx->chara_reflect, chara_color);

    if (item_equip_range)
        for (int32_t i = first_item_equip_object; i < max_item_equip_object; i++)
            item_equip_object[i].disp(&mat, rctx);
    else {
        for (int32_t i = ITEM_ATAMA; i < ITEM_MAX; i++) {
            mdl::ObjFlags chara_flags = (mdl::ObjFlags)0;
            if (!field_18[i]) {
                if (rctx->chara_reflect)
                    enum_or(chara_flags, mdl::OBJ_CHARA_REFLECT);
                if (rctx->chara_refract)
                    enum_or(chara_flags, mdl::OBJ_REFRACT);
            }

            mdl::ObjFlags flags = (mdl::ObjFlags)(mdl::OBJ_4 | mdl::OBJ_SHADOW);
            if (i == ITEM_HARA)
                flags = (mdl::ObjFlags)0;

            if (!(field_A0 & 0x04))
                enum_and(flags, ~mdl::OBJ_SHADOW);

            disp_manager.set_obj_flags((mdl::ObjFlags)(chara_flags | flags | mdl::OBJ_SSS));
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

void rob_chara_item_equip::set_disable_collision(rob_osage_parts parts, bool disable) {
    item_equip_object[parts == ROB_OSAGE_PARTS_MUFFLER ? ITEM_OUTER : ITEM_KAMI]
        .set_disable_collision((rob_osage_parts_bit)(1 << parts), disable);
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
        v11.use_blend = false;
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

static void sub_140522A30(rob_chara_item_cos_data* item_cos_data, rob_chara_item_equip* rob_itm_equip,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    const chara_init_data* chr_init_data = chara_init_data_get(item_cos_data->chara_index);
    for (int32_t i = ITEM_ATAMA; i < ITEM_KAMI; i++) {
        rob_itm_equip->load_outfit_object_info((item_id)i,
            chr_init_data->field_7E4[i - 1], false, bone_data, data, obj_db);
        auto elem = item_cos_data->item_change.find(i);
        if (elem != item_cos_data->item_change.end())
            item_cos_data->set_texture_pattern(rob_itm_equip, elem->second, (item_id)i, false);
        else
            item_cos_data->set_texture_pattern(rob_itm_equip, (item_id)i, false);
    }

    for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++) {
        rob_itm_equip->load_body_parts_object_info((item_id)i, {}, bone_data, data, obj_db);
        item_cos_data->set_texture_pattern(rob_itm_equip, (item_id)i, false);
    }
    item_cos_data->set_texture_pattern(rob_itm_equip, ITEM_NONE, true);

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
    for (int32_t i = 0; i < 9; i++) {
        std::string buf = sprintf_s_string("%sITM%03d_ATAM_HEAD_%02d_SP__DIVSKN", chara_name, item_no, i);
        object_info obj_info = obj_db->get_object_info(buf.c_str());
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
    item_id id, bool tex_pat_for_all) {
    if (tex_pat_for_all)
        rob_itm_equip->set_texture_pattern(0, 0);
    else
        rob_itm_equip->set_object_texture_pattern(id, 0, 0);
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

void rob_chara_item_cos_data::set_texture_pattern(rob_chara_item_equip* rob_itm_equip,
    const std::vector<uint32_t>& item_nos, item_id id, bool tex_pat_for_all) {
    if (id < ITEM_BODY || id >= ITEM_MAX)
        return;

    for (const uint32_t& i : item_nos) {
        auto elem = texture_change.find(i);
        if (elem == texture_change.end())
            continue;

        for (item_cos_texture_change_tex& j : elem->second)
            if (j.org && j.chg)
                texture_pattern[id].push_back({ j.org->id, j.chg->id });
    }

    if (tex_pat_for_all)
        rob_itm_equip->set_texture_pattern(
            texture_pattern[id].data(), texture_pattern[id].size());
    else
        rob_itm_equip->set_object_texture_pattern(id,
            texture_pattern[id].data(), texture_pattern[id].size());
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
        resolution_struct* res_wind_int = res_window_internal_get();
        float_t v14 = (float_t)(v12 * 0.5f) * (float_t)res_wind_int->width;
        float_t v15 = (float_t)((1.0f - v11) * 0.5f) * (float_t)res_wind_int->height;
        if (apply_offset) {
            resolution_struct* res_wind = res_window_get();
            v14 += (float_t)res_wind_int->x_offset;
            v15 += (float_t)(res_wind->height - res_wind_int->y_offset - res_wind_int->height);
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

void OpdMaker::Data::ReadOpdiFiles(rob_chara* rob_chr, std::vector<uint32_t>& motions) {
    if (!empty || !rob_chr)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++) {
        rob_chara_item_equip_object* itm_eq_obj = rob_itm_equip->get_item_equip_object((item_id)i);
        if (itm_eq_obj->obj_info.is_null() || !itm_eq_obj->osage_nodes_count)
            continue;

        object_info obj_info = itm_eq_obj->obj_info;
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
            file_handler->read_file(aft_data, farc_path.c_str(), file.c_str(), true);
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
        if (rob_chr->data.field_1588.field_0.iterations > 0)
            iterations = rob_chr->data.field_1588.field_0.iterations;

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
            int32_t frame = 0;
            int32_t iteration = 0;
            while (true) {
                osg_mhd.set_frame((float_t)frame);
                osg_mhd.ctrl();

                frame++;
                iteration++;
                if ((float_t)frame >= frame_count) {
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

        int32_t frame_int = 0;
        float_t frame = 0.0f;
        while (frame < frame_count) {
            osg_mhd.set_frame(frame);
            osg_mhd.ctrl();
            opd_chara_data_array_add_frame_data(chara_id);
            frame = (float_t)++frame_int;
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

        void* file_data = force_malloc(file_length);
        if (!file_data)
            continue;

        fs.open(tmp_path.c_str(), "rb");
        size_t read_length = fs.read(file_data, file_length);
        fs.close();

        if (!read_length) {
            free_def(file_data);
            continue;
        }

        adler_buf adler;
        adler.get_adler(file_data, file_length);
        footer[1] = adler.adler;

        fs.open(tmp_path.c_str(), "ab");
        if (fs.check_not_null())
            fs.write(footer, sizeof(footer));
        fs.close();
        free_def(file_data);
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
    chara_index chara_index = rob_chr->chara_index;
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    switch (state) {
    case 1: {
        opd_make_manager->chara_data.PopItems(chara_index, items);

        for (int32_t i = 0; i < ITEM_SUB_MAX; i++) {
            rob_chr->item_cos_data.set_chara_index(chara_index);
            rob_chr->item_cos_data.set_item(items[i]);
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
    } break;
    case 3: {
        if (task_rob_load_check_load_req_data())
            break;

        rob_chr->item_cos_data.reload_items(chara_id, aft_bone_data, aft_data, aft_obj_db);

        for (int32_t i = ITEM_KAMI; i < ITEM_MAX; i++) {
            rob_chara_item_equip_object* itm_eq_obj = rob_chr->item_equip->get_item_equip_object((item_id)i);
            if (!objset_info_storage_get_obj_name(itm_eq_obj->obj_info) && itm_eq_obj->osage_nodes_count)
                continue;

            int32_t j = ITEM_ATAMA;
            const object_info* k = chr_init_data->field_7E4;
            object_info v14 = itm_eq_obj->obj_info;
            for (; *k != v14; j++, k++) {
                if (j < ITEM_KAMI)
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
            std::string fs_copy_file_tmp_path = sprintf_s_string(
                "%s/%s.fs_copy_file.tmp", ram_osage_play_data_tmp_path, i.c_str());
            std::string ram_path = sprintf_s_string("%s/%s", ram_osage_play_data_path, i.c_str());

            if (!path_check_file_exists(tmp_path.c_str()))
                continue;

            if (!path_check_directory_exists(ram_osage_play_data_tmp_path)
                || !path_rename_file(tmp_path.c_str(), fs_copy_file_tmp_path.c_str())) {
                path_delete_file(tmp_path.c_str());
                continue;
            }

            path_delete_file(tmp_path.c_str());

            path_rename_file(fs_copy_file_tmp_path.c_str(), ram_path.c_str());
            path_delete_file(fs_copy_file_tmp_path.c_str());
        }

        if (use_current_skp)
            return true;
        state = 10;
    }
    case 10: {
        task_rob_load_append_free_req_data_obj(chara_index, rob_chr->item_cos_data.get_cos());

        for (int32_t i = 0; i < ITEM_SUB_MAX; i++) {
            rob_chr->item_cos_data.set_chara_index(chara_index);
            rob_chr->item_cos_data.set_item_zero(items[i]);
        }

        rob_chr->item_cos_data.reload_items(chara_id, aft_bone_data, aft_data, aft_obj_db);
        skin_param_manager_reset(chara_id);
        if (opd_make_manager->chara_data.CheckNoItems(chara_index))
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
    free_def(data);
}

opd_vec3_data_vec::opd_vec3_data_vec() {

}

opd_vec3_data_vec::~opd_vec3_data_vec() {

}

opd_farc_params::opd_farc_params() : compress(), encrypt() {
    align = 1;
}

opd_farc_file::opd_farc_file() : offset(), size() {

}

opd_farc_file::opd_farc_file(const std::string& name, size_t offset, size_t size) {
    this->name.assign(name);
    this->offset = offset;
    this->size = size;
}

opd_farc_file::~opd_farc_file() {

}

opd_farc::opd_farc() : stream(), farc_size() {

}

opd_farc::~opd_farc() {

}

bool opd_farc::add_file(const void* data, size_t size, const std::string& name) {
    if (!data || !size || !name.size() || !stream)
        return false;

    size_t offset = stream->get_position();
    stream->write(data, size);
    files.push_back({ name, offset, size });
    return true;
}

bool opd_farc::open(const std::string& path, const opd_farc_params& params) {
    reset();
    if (!path.size() || !params.align || ((params.align - 1) & params.align))
        return 0;

    this->params = params;
    this->path.assign(path);
    data_path.assign(path);
    data_path.append(".data");
    stream = new file_stream;
    stream->open(data_path.c_str(), "wb");
    return stream->check_not_null();
}

void opd_farc::reset() {
    path.clear();
    params = {};
    files.clear();
    data_path.clear();
    if (stream) {
        delete stream;
        stream = 0;
    }
    farc_size = 0;
}

bool opd_farc::write_file() {
    std::string path_tmp(path);
    path_tmp.append(".tmp");

    file_stream fs;
    bool ret = false;
    if (!stream)
        goto End;

    stream->close();

    delete stream;
    stream = 0;

    if (!path_delete_file(path.c_str()))
        goto End;

    fs.open(data_path.c_str(), "rb");
    if (fs.check_not_null() && fs.get_length()) {
        bool compress = params.compress;
        bool encrypt = params.encrypt;
        farc f;

        for (opd_farc_file& i : files) {
            fs.set_position(i.offset, SEEK_SET);
            uint8_t* data = force_malloc<uint8_t>(i.size);
            size_t len = fs.read(data, i.size);
            if (len) {
                farc_file* ff = f.add_file(i.name.c_str());
                ff->data = data;
                ff->size = i.size;
                ff->compressed = compress;
                ff->encrypted = encrypt;
                ff->data_changed = true;
            }
        }

        farc_signature signature;
        farc_flags flags;
        if (encrypt) {
            signature = FARC_FARC;
            flags = FARC_AES;

            if (compress)
                enum_or(flags, FARC_GZIP);
        }
        else if (compress) {
            signature = FARC_FArC;
            flags = FARC_NONE;
        }
        else {
            signature = FARC_FArc;
            flags = FARC_NONE;
        }
        f.alignment = (uint32_t)params.align;
        f.write(path_tmp.c_str(), signature, flags, false, false);
        ret = true;
    }
    fs.close();

    if (ret) {
        path_delete_file(path.c_str());
        if (!path_rename_file(path_tmp.c_str(), path.c_str()))
            ret = false;
    }

End:
    path_delete_file(data_path.c_str());
    path_delete_file(path_tmp.c_str());
    reset();
    return ret;
}

p_opd_farc::p_opd_farc() : ptr() {
    ptr = new opd_farc;
}

p_opd_farc::~p_opd_farc() {
    if (ptr) {
        delete ptr;
        ptr = 0;
    }
}

bool p_opd_farc::add_file(const void* data, size_t size, const std::string& file) {
    return ptr->add_file(data, size, file);
}

bool p_opd_farc::open(const std::string& path, const opd_farc_params& params) {
    return ptr->open(path, params);
}

bool p_opd_farc::open(const std::string& path, bool compress, size_t align) {
    opd_farc_params farc_data;
    farc_data.compress = compress;
    farc_data.align = align;
    return open(path, farc_data);
}

bool p_opd_farc::write_file() {
    return ptr->write_file();
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

    rob_chara_item_equip* rob_itm_equip = rob_chara_array_get(chara_id)->item_equip;
    for (int32_t i = 0; i < ITEM_OSAGE_COUNT; i++) {
        rob_chara_item_equip_object* itm_eq_obj
            = rob_itm_equip->get_item_equip_object((item_id)(ITEM_OSAGE_FIRST + i));
        if (!itm_eq_obj->osage_nodes_count)
            continue;

        std::vector<std::vector<opd_vec3_data_vec>>& opd_data = this->opd_data[i];

        size_t node_index = 0;
        for (ExOsageBlock*& j : itm_eq_obj->osage_blocks) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];

            RobOsageNode* k_begin = j->rob.nodes.data() + 1;
            RobOsageNode* k_end = j->rob.nodes.data() + j->rob.nodes.size();
            size_t l = 0;
            for (RobOsageNode* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.data()[frame_index] = k->reset_data.pos.x;
                opd_node_data.data()[l].y.data()[frame_index] = k->reset_data.pos.y;
                opd_node_data.data()[l].z.data()[frame_index] = k->reset_data.pos.z;
            }
        }

        for (ExClothBlock*& j : itm_eq_obj->cloth_blocks) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];

            CLOTHNode* k_begin = j->rob.nodes.data() + j->rob.root_count;
            CLOTHNode* k_end = j->rob.nodes.data() + j->rob.nodes.size();
            size_t l = 0;
            for (CLOTHNode* k = k_begin; k != k_end; k++, l++) {
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

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;

    std::string buf = sprintf_s_string("%s/%d", get_ram_osage_play_data_tmp_dir(), chara_id);

    p_opd_farc* opd = this->opd;
    for (int32_t i = 0; i < ITEM_OSAGE_COUNT; i++, opd++) {
        rob_chara_item_equip_object* itm_eq_obj
            = rob_itm_equip->get_item_equip_object((item_id)(ITEM_OSAGE_FIRST + i));
        if (!itm_eq_obj->osage_nodes_count)
            continue;

        object_info obj_info = itm_eq_obj->obj_info;
        const char* object_name = objset_info_storage_get_obj_name(obj_info);
        if (!object_name)
            continue;

        const std::vector<std::vector<opd_vec3_data_vec>>& opd_data = this->opd_data[i];

        size_t nodes_count = 0;
        for (const std::vector<opd_vec3_data_vec>& j : opd_data)
            nodes_count += j.size();

        size_t max_size = sizeof(osage_play_data_header) + (6ULL * (frame_count + 2ULL) * nodes_count);
        uint8_t* data = (uint8_t*)malloc(max_size);
        if (!data)
            continue;

        osage_play_data_header* opd_head = (osage_play_data_header*)data;
        opd_head->signature = 0;
        opd_head->frame_count = frame_count;
        opd_head->motion_id = motion_id;
        opd_head->obj_info = { (uint16_t)obj_info.id, (uint16_t)obj_info.set_id };
        opd_head->nodes_count = (uint16_t)itm_eq_obj->osage_nodes_count;

        size_t size = sizeof(osage_play_data_header);
        uint8_t* d = data + size;

        size_t osage_node_index = 0;
        bool higher_accuracy = false;
        for (const std::vector<opd_vec3_data_vec>& j : opd_data) {
            if (osage_node_index < itm_eq_obj->osage_blocks.size())
                higher_accuracy = itm_eq_obj->osage_blocks.data()[osage_node_index]->has_children_node;

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
        free_def(data);
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

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;

    p_opd_farc* opdi = this->opdi;
    for (int32_t i = 0; i < ITEM_OSAGE_COUNT; i++, opdi++) {
        rob_chara_item_equip_object* itm_eq_obj
            = rob_itm_equip->get_item_equip_object((item_id)(ITEM_OSAGE_FIRST + i));
        if (!itm_eq_obj->osage_nodes_count)
            continue;

        object_info obj_info = itm_eq_obj->obj_info;
        const char* object_name = objset_info_storage_get_obj_name(obj_info);
        if (!object_name)
            continue;

        size_t size = sizeof(osage_play_data_init_header) + sizeof(vec3) * 2 * itm_eq_obj->osage_nodes_count;
        uint8_t* data = force_malloc<uint8_t>(size);

        osage_play_data_init_header* opdi_head = (osage_play_data_init_header*)data;
        opdi_head->motion_id = motion_id;
        opdi_head->obj_info = { (uint16_t)obj_info.id, (uint16_t)obj_info.set_id };
        opdi_head->nodes_count = (uint16_t)itm_eq_obj->osage_nodes_count;

        vec3* d = (vec3*)(data + sizeof(osage_play_data_init_header));

        for (ExOsageBlock*& j : itm_eq_obj->osage_blocks) {
            if (!(j->rob.nodes.size()))
                break;

            RobOsageNode* k_begin = j->rob.nodes.data() + 1;
            RobOsageNode* k_end = j->rob.nodes.data() + j->rob.nodes.size();
            for (RobOsageNode* k = k_begin; k != k_end; k++) {
                d[0] = k->pos;
                d[1] = k->delta_pos;
                d += 2;
            }
        }

        for (ExClothBlock*& j : itm_eq_obj->cloth_blocks) {
            if (!(j->rob.nodes.size()))
                break;

            CLOTHNode* k_begin = j->rob.nodes.data() + j->rob.root_count;
            CLOTHNode* k_end = j->rob.nodes.data() + j->rob.nodes.size();
            size_t l = 0;
            for (CLOTHNode* k = k_begin; k != k_end; k++, l++) {
                d[0] = k->pos;
                d[1] = k->delta_pos;
                d += 2;
            }
        }

        std::string buf = string_to_lower(sprintf_s_string("%s_%s.opdi", object_name, motion_name));
        opdi->add_file(data, size, buf);
        free_def(data);
    }
}

void opd_chara_data::fs_copy_file() {

}

void opd_chara_data::init_data(uint32_t motion_id) {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    int32_t frame_count = (int32_t)rob_chr->data.field_1588.field_0.frame;
    for (int32_t i = 0; i < ITEM_OSAGE_COUNT; i++) {
        rob_chara_item_equip_object* itm_eq_obj
            = rob_itm_equip->get_item_equip_object((item_id)(ITEM_OSAGE_FIRST + i));
        if (!itm_eq_obj->osage_nodes_count)
            continue;

        std::vector<std::vector<opd_vec3_data_vec>>& opd_data = this->opd_data[i];
        opd_data.clear();
        opd_data.resize(itm_eq_obj->cloth_blocks.size() + itm_eq_obj->osage_blocks.size());

        size_t node_index = 0;
        for (ExOsageBlock*& j : itm_eq_obj->osage_blocks) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];
            opd_node_data.resize(j->rob.nodes.size() - 1);

            RobOsageNode* k_begin = j->rob.nodes.data() + 1;
            RobOsageNode* k_end = j->rob.nodes.data() + j->rob.nodes.size();
            size_t l = 0;
            for (RobOsageNode* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.resize(frame_count);
                opd_node_data.data()[l].y.resize(frame_count);
                opd_node_data.data()[l].z.resize(frame_count);
            }
        }

        for (ExClothBlock*& j : itm_eq_obj->cloth_blocks) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];
            opd_node_data.resize(j->rob.nodes.size() - j->rob.root_count);

            CLOTHNode* k_begin = j->rob.nodes.data() + j->rob.root_count;
            CLOTHNode* k_end = j->rob.nodes.data() + j->rob.nodes.size();
            size_t l = 0;
            for (CLOTHNode* k = k_begin; k != k_end; k++, l++) {
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

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;

    p_opd_farc* opd = this->opd;
    for (int32_t i = 0; i < ITEM_OSAGE_COUNT; i++, opd++) {
        rob_chara_item_equip_object* itm_eq_obj
            = rob_itm_equip->get_item_equip_object((item_id)(ITEM_OSAGE_FIRST + i));
        if (!itm_eq_obj->osage_nodes_count)
            continue;

        const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();
        path_create_directory(ram_osage_play_data_tmp_path);

        std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, chara_id);
        if (!path_check_directory_exists(chara_dir.c_str()))
            path_create_directory(chara_dir.c_str());

        const char* object_name = objset_info_storage_get_obj_name(itm_eq_obj->obj_info);
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

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;

    p_opd_farc* opdi = this->opdi;
    for (int32_t i = 0; i < ITEM_OSAGE_COUNT; i++, opdi++) {
        rob_chara_item_equip_object* itm_eq_obj
            = rob_itm_equip->get_item_equip_object((item_id)(ITEM_OSAGE_FIRST + i));
        if (!itm_eq_obj->osage_nodes_count)
            continue;

        const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();
        path_create_directory(ram_osage_play_data_tmp_path);

        std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, rob_chr->chara_id);
        if (!path_check_directory_exists(chara_dir.c_str()))
            path_create_directory(chara_dir.c_str());

        const char* object_name = objset_info_storage_get_obj_name(itm_eq_obj->obj_info);
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
    for (int32_t i = 0; i < ITEM_OSAGE_COUNT; i++)
        opd_data[i].clear();
}

void opd_chara_data::write_file() {
    rob_chara* rob_chr = rob_chara_array_get(chara_id);
    if (!rob_chr)
        return;

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    for (int32_t i = 0; i < ITEM_OSAGE_COUNT; i++)
        if (rob_itm_equip->get_item_equip_object((item_id)(ITEM_OSAGE_FIRST + i))->osage_nodes_count)
            opd[i].write_file();
}

OsagePlayDataManager::OsagePlayDataManager() : state() {
    Reset();
    state = 0;
}

OsagePlayDataManager::~OsagePlayDataManager() {

}

bool OsagePlayDataManager::init() {
#if !OPD_PLAY
    LoadOpdFileList();
#endif
    state = 0;
    return true;
}

bool OsagePlayDataManager::ctrl() {
    if (state)
        return false;

#if OPD_PLAY
    for (OsagePlayDataManager::GenData& i : gen_data) {
        if (i.chara_id < 0 || i.chara_id >= ROB_CHARA_COUNT)
            continue;

        const uint32_t motion_id = i.motion_id;
        const uint32_t chara_id = i.chara_id;

        data_struct* aft_data = &data_list[DATA_AFT];
        bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        rob_chara* rob_chr = rob_chara_array_get(chara_id);

        rob_osage_mothead osg_mhd(rob_chr, 0, motion_id, 0.0f, aft_bone_data, aft_mot_db, true);

        float_t frame_count = (float_t)(int32_t)rob_chr->bone_data->get_frame_count() - 1.0f;

        osg_mhd.init_data(aft_mot_db);
        
        opd_chara_data_array_init_data(chara_id, motion_id);

        int32_t set_motion_index = 0;
        int32_t frame_int = 0;
        float_t frame = 0.0f;
        while (frame <= frame_count) {
            osg_mhd.set_frame(frame);

            while (set_motion_index < set_motion.size()) {
                if (set_motion.data()[set_motion_index].motion_id != motion_id) {
                    set_motion_index++;
                    continue;
                }

                float_t _frame = (float_t)(int32_t)prj::roundf(set_motion.data()[set_motion_index].frame_stage_index.first);
                if (_frame > frame)
                    break;

                rob_chr->set_motion_reset_data(motion_id, _frame);
                rob_chr->set_motion_skin_param(motion_id, _frame);
                set_motion_index++;
            }

            osg_mhd.ctrl();
            opd_chara_data_array_add_frame_data(chara_id);
            frame = (float_t)++frame_int;
        }

        opd_chara_data* opd_chr_data = opd_chara_data_array_get(chara_id);

        rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
        for (int32_t j = 0; j < ITEM_OSAGE_COUNT; j++) {
            object_info obj_info = i.obj_info[j];
            if (obj_info.is_null())
                continue;

            int32_t load_count = 1;
            auto elem = opd_file_data.find({ obj_info, motion_id });
            if (elem != opd_file_data.end()) {
                load_count = elem->second.load_count;
                while (elem->second.load_count > 0)
                    elem->second.unload();
                opd_file_data.erase(elem);
            }

            std::vector<std::vector<opd_vec3_data_vec>>& opd_data = opd_chr_data->opd_data[j];

            size_t nodes_count = 0;
            for (const std::vector<opd_vec3_data_vec>& j : opd_data)
                nodes_count += j.size();

            int64_t _frame_count = (int64_t)frame_count;
            float_t* buf = force_malloc<float_t>(3ULL * _frame_count * nodes_count);
            if (!buf)
                continue;

            ::opd_file_data data;
            data.head.obj_info.first = obj_info.id;
            data.head.obj_info.second = obj_info.set_id;
            data.head.motion_id = motion_id;
            data.head.frame_count = (uint32_t)_frame_count;
            data.head.nodes_count = (uint16_t)nodes_count;
            data.data = buf;
            data.load_count = load_count;

            for (std::vector<opd_vec3_data_vec>& k : opd_data)
                for (opd_vec3_data_vec& l : k) {
                    memcpy(buf, l.x.data(), _frame_count * sizeof(float_t));
                    buf += _frame_count;
                    memcpy(buf, l.y.data(), _frame_count * sizeof(float_t));
                    buf += _frame_count;
                    memcpy(buf, l.z.data(), _frame_count * sizeof(float_t));
                    buf += _frame_count;
                }

            opd_file_data.insert({ { obj_info, motion_id }, data });
        }

        opd_chr_data->reset();
    }
    gen_data.clear();
    set_motion.clear();
    return true;
#else
    for (auto i = file_handlers.begin(); i != file_handlers.end();) {
        if (!(*i)->check_not_ready()) {
            LoadOpdFile(*i);
            delete (*i);
            i = file_handlers.erase(i);
        }
        else
            i++;
    }

    return !file_handlers.size();
#endif
}

bool OsagePlayDataManager::dest() {
    return true;
}

void OsagePlayDataManager::disp() {

}

bool OsagePlayDataManager::add() {
    return app::TaskWork::add_task(this, "OSAGE_PLAY_DATA_MANAGER");
}

#if OPD_PLAY
void OsagePlayDataManager::AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids,
    const std::vector<pv_data_set_motion>& set_motion) {
    if (CheckTaskReady() || !rob_chr)
        return;

    OsagePlayDataManager::GenData gen_data;
    gen_data.chara_id = rob_chr->chara_id;

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    for (int32_t i = ITEM_KAMI; i < ITEM_ITEM16; i++) {
        rob_chara_item_equip_object* itm_eq_obj = rob_itm_equip->get_item_equip_object((item_id)i);
        if (!itm_eq_obj || itm_eq_obj->obj_info.is_null()
            || (!itm_eq_obj->osage_blocks.size() && !itm_eq_obj->cloth_blocks.size()))
            continue;

        gen_data.obj_info[i - ITEM_OSAGE_FIRST] = itm_eq_obj->obj_info;
    }

    for (const uint32_t i : motion_ids) {
        gen_data.motion_id = i;
        this->gen_data.push_back(gen_data);
    }

    this->set_motion.assign(set_motion.begin(), set_motion.end());
}
#else
void OsagePlayDataManager::AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids) {
    if (CheckTaskReady() || !rob_chr)
        return;

    rob_chara_item_equip* rob_itm_equip = rob_chr->item_equip;
    for (int32_t i = ITEM_KAMI; i < ITEM_ITEM16; i++) {
        rob_chara_item_equip_object* itm_eq_obj = rob_itm_equip->get_item_equip_object((item_id)i);
        if (!itm_eq_obj || itm_eq_obj->obj_info.is_null()
            || (!itm_eq_obj->osage_blocks.size() && !itm_eq_obj->cloth_blocks.size()))
            continue;

        for (const uint32_t j : motion_ids)
            opd_req_data.push_back(itm_eq_obj->obj_info, j);
    }
}
#endif

bool OsagePlayDataManager::CheckTaskReady() {
    return app::TaskWork::check_task_ready(this);
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
    ::opd_file_data data;
    if (opd_decode_data(pfhndl->get_data(), data.data, data.head))
        opd_file_data.insert({ { object_info(data.head.obj_info.first,
            data.head.obj_info.second), data.head.motion_id }, data });
}

#if !OPD_PLAY
void OsagePlayDataManager::LoadOpdFileList() {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    prj::sort_unique(opd_req_data);
    for (const std::pair<object_info, uint32_t>& i : opd_req_data) {
        auto elem = opd_file_data.find(i);
        if (elem != opd_file_data.end())
            continue;

        const char* object_name = objset_info_storage_get_obj_name(i.first);
        if (!object_name)
            continue;

        const char* motion_name = aft_mot_db->get_motion_name(i.second);
        if (!motion_name)
            continue;

        std::string obj_name_buf = string_to_lower(object_name);
        std::string mot_name_buf = string_to_lower(motion_name);

        std::string file_buf = sprintf_s_string("%s_%s.%s",
            obj_name_buf.c_str(), mot_name_buf.c_str(), "opd");

        std::string rom_farc_buf = sprintf_s_string("%s/%s.farc",
            get_rom_osage_play_data_dir(), obj_name_buf.c_str());
        if (aft_data->check_file_exists(rom_farc_buf.c_str())) {
            file_handlers.push_back(new p_file_handler);
            file_handlers.back()->read_file(aft_data, rom_farc_buf.c_str(), file_buf.c_str(), true);
            continue;
        }

        std::string ram_farc_buf = sprintf_s_string("%s/%s.farc",
            get_ram_osage_play_data_dir(), obj_name_buf.c_str());
        if (path_check_file_exists(ram_farc_buf.c_str())) {
            file_handlers.push_back(new p_file_handler);
            file_handlers.back()->read_file(0, ram_farc_buf.c_str(), file_buf.c_str(), true);
            continue;
        }
    }
    opd_req_data.clear();
}
#endif

void OsagePlayDataManager::Reset() {
#if OPD_PLAY
    gen_data.clear();
    set_motion.clear();
#else
    file_handlers.clear();
    opd_req_data.clear();
#endif
    for (auto& i : opd_file_data)
        i.second.unload();
    opd_file_data.clear();
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

#if OPD_PLAY
rob_osage_mothead::rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index, uint32_t motion_id, float_t frame,
    const bone_database* bone_data, const motion_database* mot_db, bool set_motion_reset_data) : rob_chr(),
#else
rob_osage_mothead::rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index, uint32_t motion_id,
    float_t frame, const bone_database* bone_data, const motion_database* mot_db) : rob_chr(),
#endif
    motion_id(), frame(), last_frame(), type_62(), type_62_data(), type_75(), type_75_data(),
    type_67(), type_67_data(), type_74(), type_74_data(), type_79(), type_79_data(), field_68() {
    type_62 = 0;
    type_75 = 0;
    type_67 = 0;
    type_74 = 0;
    type_79 = 0;
    this->rob_chr = rob_chr;
    if (rob_chr) {
#if OPD_PLAY
        rob_chr->set_motion_id(motion_id, frame, 0.0f,
            true, set_motion_reset_data, MOTION_BLEND_CROSS, bone_data, mot_db);
#else
        rob_chr->set_motion_id(motion_id, frame, 0.0f,
            true, false, MOTION_BLEND_CROSS, bone_data, mot_db);
#endif
        this->frame = frame;
        this->motion_id = motion_id;
        last_frame = rob_chr->bone_data->get_frame_count() - 1.0f;
        rob_chr->reset_osage();
        rob_chr->set_bone_data_frame(frame);
        rob_chara_item_equip_ctrl_iterate_nodes(rob_chr->item_equip);
        init_data(mot_db);
        set_mothead_frame();
    }
    else
        reset();

}

rob_osage_mothead::~rob_osage_mothead() {
    reset();
}

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

void rob_osage_mothead::reset() {
    rob_chr = 0;
    motion_id = -1;
    frame = 0.0f;
    last_frame = 0.0f;
    reset_data();
}

void rob_osage_mothead::init_data(const motion_database* mot_db) {
    reset_data();

    type_62 = new rob_osage_mothead_data(MOTHEAD_DATA_ROB_PARTS_ADJUST, motion_id, mot_db);
    type_62_data = type_62->find_next_data();

    type_75 = new rob_osage_mothead_data(MOTHEAD_DATA_ROB_ADJUST_GLOBAL, motion_id, mot_db);
    type_75_data = type_75->find_next_data();

    type_67 = new rob_osage_mothead_data(MOTHEAD_DATA_SLEEVE_ADJUST, motion_id, mot_db);
    type_67_data = type_67->find_next_data();

    type_74 = new rob_osage_mothead_data(MOTHEAD_DATA_DISABLE_COLLISION, motion_id, mot_db);
    type_74_data = type_74->find_next_data();

    type_79 = new rob_osage_mothead_data(MOTHEAD_DATA_ROB_CHARA_COLI_RING, motion_id, mot_db);
    type_79_data = type_79->find_next_data();
}

void rob_osage_mothead::reset_data() {
    if (type_62)
        delete type_62;
    type_62 = 0;
    type_62_data = 0;

    if (type_75)
        delete type_75;
    type_75 = 0;
    type_75_data = 0;

    if (type_67)
        delete type_67;
    type_67 = 0;
    type_67_data = 0;

    if (type_74)
        delete type_74;
    type_74 = 0;
    type_74_data = 0;

    if (type_79)
        delete type_79;
    type_79_data = 0;
    type_79 = 0;
}

void rob_osage_mothead::set_coli_ring(const mothead_data* mhd_data) {
    const void* data = mhd_data->data;

    //rob_chara_set_coli_ring(rob_chr, ((int8_t*)data)[0]);
}

void rob_osage_mothead::set_frame(float_t value) {
    if (last_frame > value)
        frame = value;
}

void rob_osage_mothead::set_disable_collision(const mothead_data* mhd_data) {
    const void* data = mhd_data->data;

    rob_chr->set_disable_collision((rob_osage_parts)((uint8_t*)data)[0], !!((uint8_t*)data)[1]);
}

void rob_osage_mothead::set_mothead_frame() {
    while (type_62_data) {
        if ((float_t)type_62_data->frame > frame)
            break;

        set_rob_parts_adjust(type_62_data);

        type_62_data = type_62->find_next_data();
    }

    while (type_75_data) {
        if ((float_t)type_75_data->frame > frame)
            break;

        set_rob_adjust_global(type_75_data);

        type_75_data = type_75->find_next_data();
    }

    while (type_67_data) {
        if ((float_t)type_67_data->frame > frame)
            break;

        set_sleeve_adjust(type_67_data);

        type_67_data = type_67->find_next_data();
    }

    while (type_74_data) {
        if ((float_t)type_74_data->frame > frame)
            break;

        set_disable_collision(type_74_data);

        type_74_data = type_74->find_next_data();
    }

    while (type_79_data) {
        if ((float_t)type_79_data->frame > frame)
            break;

        set_coli_ring(type_79_data);

        type_79_data = type_79->find_next_data();
    }
}

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

void OpdMakeManager::CharaData::AddCostumes(const std::list<std::pair<chara_index, int32_t>>& costumes) {
    for (const std::pair<chara_index, int32_t>& i : costumes) {
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
        chara_index chara_index = CHARA_MAX;
        int32_t item_no = 0;
        customize_item_table_handler_data_get_chara_item(i, chara_index, item_no);
        if (chara_index < CHARA_MIKU || chara_index >= CHARA_MAX || !item_no)
            continue;

        const item_table_item* item = item_table_handler_array_get_item(chara_index, item_no);
        if (!item)
            continue;

        if (!(item->attr & 0x0C))
            chara_costumes[chara_index].items[item->sub_id].push_back(item_no);
        else if (item->org_itm)
            chara_costumes[chara_index].items[item->sub_id].push_back(item->org_itm);
    }
}

bool OpdMakeManager::CharaData::CheckNoItems(chara_index chara_index) {
    std::vector<uint32_t>* chara_items = chara_costumes[chara_index].items;
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++, chara_items++)
        if (chara_items->size())
            return false;
    return true;
}

void OpdMakeManager::CharaData::PopItems(chara_index chara_index, int32_t items[ITEM_SUB_MAX]) {
    std::vector<uint32_t>* chara_items = chara_costumes[chara_index].items;
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++, chara_items++) {
        items[i] = 0;
        if (!chara_items->size())
            continue;

        items[i] = chara_items->back();
        chara_items->pop_back();
        left--;
    }
}

void OpdMakeManager::CharaData::Reset() {
    for (CharaCostume& i : chara_costumes)
        for (std::vector<uint32_t>& j : i.items)
            j.clear();

    left = 0;
    count = 0;
}

void OpdMakeManager::CharaData::SortUnique() {
    for (CharaCostume& i : chara_costumes)
        for (std::vector<uint32_t>& j : i.items) {
            prj::sort_unique(j);
            count += (uint32_t)j.size();
        }

    left = count;
}

OpdMakeManager::OpdMakeManager() : mode(), workers(), use_current_skp(), use_opdi() {
    mode = 0;
    chara = CHARA_MIKU;

    int32_t chara_id = 0;
    for (OpdMakeWorker*& i : workers)
        i = new OpdMakeWorker(chara_id++);
}

OpdMakeManager::~OpdMakeManager() {

}

bool OpdMakeManager::init() {
    mode = 1;

    rctx_ptr->render_manager->set_pass_sw(rndr::RND_PASSID_3D, false);

    if (path_check_directory_exists(get_ram_osage_play_data_dir()))
        path_delete_directory(get_ram_osage_play_data_dir());

    path_create_directory(get_ram_osage_play_data_dir());

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

        chara = CHARA_MIKU;
        mode = 5;
        break;
    } break;
    case 5: {
        bool has_items = false;
        for (int32_t i = 0; i < CHARA_MAX; i++)
            if (!chara_data.CheckNoItems((chara_index)i)) {
                chara = (chara_index)i;
                has_items = true;
                break;
            }

        if (has_items) {
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
        bool wait = false;
        for (int32_t i = 0; i < 4; i++)
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
        for (int32_t i = 0; i < 4; i++)
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
        std::list<std::pair<chara_index, int32_t>> costumes;
        for (const uint32_t& i : *args.modules)
            costumes.push_back({ modules[i].chara_index, modules[i].cos });
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
    prev_parent_mat;
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

void rob_chara_age_age_object::disp(render_context* rctx, size_t chara_index,
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
    size_t vtx_data = (size_t)buffer.MapMemory();
    if (!vtx_data)
        return;

    size_t vertex_array_size = this->vertex_array_size;
    for (int32_t i = 0; i < disp_count; i++)
        memmove((void*)(vtx_data + vertex_array_size * i),
            (void*)((size_t)vertex_data + vertex_array_size * v44[i].second), vertex_array_size);

    buffer.UnmapMemory();

    mesh.num_vertex = disp_count * num_vertex;
    sub_mesh.num_index = disp_count * num_index;
    sub_mesh.material_index = npr ? 1 : 0;

    mdl::ObjFlags flags = (mdl::ObjFlags)(mdl::OBJ_SSS | mdl::OBJ_4 | mdl::OBJ_SHADOW);
    if (reflect)
        enum_or(flags, mdl::OBJ_CHARA_REFLECT);
    rctx->disp_manager->set_obj_flags(flags);
    rctx->disp_manager->set_chara_color(chara_color);
    rctx->disp_manager->set_shadow_type(chara_index ? SHADOW_STAGE : SHADOW_CHARA);
    rctx->disp_manager->entry_obj_by_obj(&mat4_identity, &obj,
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

    obj_vert_buf.load_data(vertex_data_size, vertex_data, 2, true);

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
            data->rot_z += step * data->rot_speed * rot_speed;
        data->pos.x += (90.0f - data->remaining) * (float_t)(1.0 / 90.0)
            * data->gravity * 2.5f * step * rot_speed;
        data->pos.z -= (90.0f - data->remaining) * 0.000011f * step * rot_speed;

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
        std::string file(i);
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

void PvOsageManager::AddMotionFrameResetData(int32_t stage_index, uint32_t motion_id, float_t frame, int32_t iterations) {
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
        if (rob_chr->data.field_1588.field_0.iterations > 0)
            iterations = rob_chr->data.field_1588.field_0.iterations;
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

    rob_osage_mothead osg_mhd(rob_chr, a2->frames.front().second, motion_id, frame_1, aft_bone_data, aft_mot_db);
    for (float_t& i : v34) {
        osg_mhd.set_frame(frame);
        osg_mhd.ctrl();

        float_t frame_1 = frame;
        frame = prj::floorf(frame) + 1.0f;

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
        const mothead_data* data = mothead_storage_get_mot_by_motion_id(motion_id, aft_mot_db)->data;
        if (data && data->type >= MOTHEAD_DATA_TYPE_0) {
            mothead_data_type type = data->type;
            while (type != MOTHEAD_DATA_WIND_RESET) {
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

                while (type != MOTHEAD_DATA_WIND_RESET) {
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
            while (type != MOTHEAD_DATA_OSAGE_RESET) {
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

                while (type != MOTHEAD_DATA_OSAGE_RESET) {
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
        if (i && !(i->data.field_3 & 0x80))
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
            if (i && !(i->data.field_3 & 0x80))
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

        if (i->is_visible() && !(i->data.field_3 & 0x80))
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
        if (i && !(i->data.field_3 & 0x80))
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
            objset_info_storage_load_set(data, obj_db, i);
}

bool TaskRobLoad::LoadCharaItemCheckNotRead(chara_index chara_index, int32_t item_no) {
    if (!item_no)
        return false;

    const std::vector<uint32_t>* item_objset
        = item_table_handler_array_get_item_objset(chara_index, item_no);
    if (!item_objset)
        return true;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1 && objset_info_storage_load_obj_set_check_not_read(i))
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
        || cos->data.kami == 649 && objset_info_storage_load_obj_set_check_not_read(3291);
}

void TaskRobLoad::LoadCharaItemsParent(chara_index chara_index,
    item_cos_data* cos, void* data, const object_database* obj_db) {
    LoadCharaItems(chara_index, cos, data, obj_db);
    if (cos->data.kami == 649)
        objset_info_storage_load_set(data, obj_db, 3291);
}

void TaskRobLoad::LoadCharaObjSetMotionSet(chara_index chara_index,
    void* data, const object_database* obj_db, const motion_database* mot_db) {
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    objset_info_storage_load_set(data, obj_db, chr_init_data->object_set);
    motion_set_load_motion(cmn_set_id, "", mot_db);
    motion_set_load_mothead(cmn_set_id, "", mot_db);
    motion_set_load_motion(chr_init_data->motion_set, "", mot_db);
}

bool TaskRobLoad::LoadCharaObjSetMotionSetCheck(chara_index chara_index) {
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    if (objset_info_storage_load_obj_set_check_not_read(chr_init_data->object_set)
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
            objset_info_storage_unload_set(i);
}

void TaskRobLoad::UnloadCharaItems(chara_index chara_index, item_cos_data* cos) {
    for (int32_t i = 0; i < ITEM_SUB_MAX; i++)
        UnloadCharaItem(chara_index, cos->arr[i]);
}

void TaskRobLoad::UnloadCharaItemsParent(chara_index chara_index, item_cos_data* cos) {
    if (cos->data.kami == 649)
        objset_info_storage_unload_set(3291);
    UnloadCharaItems(chara_index, cos);
}

void TaskRobLoad::UnloadCharaObjSetMotionSet(chara_index chara_index) {
    const chara_init_data* chr_init_data = chara_init_data_get(chara_index);
    objset_info_storage_unload_set(chr_init_data->object_set);
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
            task_rob_load_append_free_req_data(i->chara_index);
            task_rob_load_append_free_req_data_obj(i->chara_index, i->item_cos_data.get_cos());
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
            task_rob_load_append_load_req_data(i->chara_index);
            task_rob_load_append_load_req_data_obj(i->chara_index, i->item_cos_data.get_cos());
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
    if (!rob_chr || rob_chr->chara_id >= ROB_CHARA_COUNT || free_chara.size() >= ROB_CHARA_COUNT)
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
    if (!app::TaskWork::check_task_ready(this)
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
    if (!chara_id || *chara_id >= ROB_CHARA_COUNT || !loaded_chara.size())
        return;

    for (auto i = loaded_chara.begin(); i != loaded_chara.end();)
        if ((*i)->chara_id == *chara_id) {
            i = loaded_chara.erase(i);
            return;
        }
        else
            i++;
}

bool TaskRobManager::GetWait(rob_chara* rob_chr) {
    if (!app::TaskWork::check_task_ready(this))
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
