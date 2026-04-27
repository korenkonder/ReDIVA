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
        const void* GetOpdiFileData(object_info obj_info, uint32_t motnum);
        bool IsValidOpdiFile(rob_chara* rob_chr, uint32_t motnum);
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
    bool SetOsagePlayInitData(uint32_t motnum);
    void SetEnd();
    void SetWaiting(bool value);

    static void ThreadMain(OpdMaker* opd_maker);
};

struct OpdMakeWorker : public app::Task {
    int32_t state;
    ROB_ID rob_id;
    int32_t items[25];
    bool use_current_skp;
    OpdMaker::Data data;

    OpdMakeWorker(ROB_ID rob_id);
    virtual ~OpdMakeWorker() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    bool open(bool use_current_skp);
    bool close();
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
    uint32_t motnum;
    uint16_t nodes_count;
};

struct osage_play_data_header {
    uint32_t signature;
    std::pair<uint16_t, uint16_t> obj_info;
    uint32_t motnum;
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
    ROB_ID rob_id;
    bool init;
    uint32_t frame;
    uint32_t frame_max;
    uint32_t motnum;
    uint64_t field_18;
    std::vector<std::vector<opd_vec3_data_vec>> opd_data[RPK_ITEM_MAX];
    p_farc_write opd[RPK_ITEM_MAX];
    p_farc_write opdi[RPK_ITEM_MAX];

    opd_chara_data();
    ~opd_chara_data();

    void add_frame_data();
    void encode_data();
    void encode_init_data(uint32_t motnum);
    void fs_copy_file();
    void init_data(uint32_t motnum);
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

    bool open();
    void AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids);
    bool CheckTaskReady();
    void GetOpdFileData(object_info obj_info,
        uint32_t motnum, const float_t*& data, uint32_t& count);
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
    int32_t type;
    MhpList* pp_list;

    rob_osage_mothead_data(int32_t type, uint32_t motnum, const motion_database* mot_db);
    ~rob_osage_mothead_data();

    MhpList* find_next_data();
};

struct rob_osage_mothead {
    rob_chara* rob_chr;
    uint32_t motnum;
    float_t frame;
    float_t last_frame;
    rob_osage_mothead_data* rob_parts_adjust;
    MhpList* rob_parts_adjust_data;
    rob_osage_mothead_data* rob_adjust_global;
    MhpList* rob_adjust_global_data;
    rob_osage_mothead_data* sleeve_adjust;
    MhpList* sleeve_adjust_data;
    rob_osage_mothead_data* disable_collision;
    MhpList* disable_collision_data;
    rob_osage_mothead_data* rob_chara_coli_ring;
    MhpList* rob_chara_coli_ring_data;
    int32_t field_68;

    rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index, uint32_t motnum,
        float_t frame, const bone_database* bone_data, const motion_database* mot_db);
#if OPD_PLAY_GEN
    rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index,
        uint32_t motnum, float_t frame, bool set_motion_reset_data,
        const bone_database* bone_data, const motion_database* mot_db);
#endif
    ~rob_osage_mothead();

    void ctrl();
    void init_data(const motion_database* mot_db);
    void reset();
    void reset_data();
    void set_coli_ring(MhpList* list);
    void set_disable_collision(MhpList* list);
    void set_frame(float_t value);
    void set_mothead_frame();
    void set_rob_parts_adjust(MhpList* list);
    void set_rob_adjust_global(MhpList* list);
    void set_sleeve_adjust(MhpList* list);
};

struct OpdMakeManager : app::Task {
    struct CharaCostume {
        std::vector<uint32_t> items[ROB_ITEM_EQUIP_SUB_ID_MAX];

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
        void PopItems(CHARA_NUM chara_num, int32_t items[ROB_ITEM_EQUIP_SUB_ID_MAX]);
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

    void open(const OpdMakeManagerArgs& args);
    bool close();
    OpdMakeManagerData* GetData();
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
    RobItemEquip item_set;

    ReqDataObj();
    ReqDataObj(CHARA_NUM cn, int32_t count);
    virtual ~ReqDataObj() override;

    virtual void Reset() override;
};

struct RobMhSmd;

typedef void (RobMhSmd::* SmdFunc)(const void*, MhdList*);

struct RobMhSmd {
    rob_chara* impl;
    RobBase* base;
    RobMotData* motd;

    static const SmdFunc functbl[];

    void exec_func(int32_t type, MhdList* list);

    void smd_next(const void* mhadr, MhdList* list);
    void func_1(const void* mhadr, MhdList* list);
    void func_2(const void* mhadr, MhdList* list);
    void func_3(const void* mhadr, MhdList* list);
    void func_4(const void* mhadr, MhdList* list);
    void func_5(const void* mhadr, MhdList* list);
    void func_6(const void* mhadr, MhdList* list);
    void func_7(const void* mhadr, MhdList* list);
    void func_8(const void* mhadr, MhdList* list);
    void func_9(const void* mhadr, MhdList* list);
    void func_10(const void* mhadr, MhdList* list);
    void func_11(const void* mhadr, MhdList* list);
    void func_12(const void* mhadr, MhdList* list);
    void func_13(const void* mhadr, MhdList* list);
    void func_14(const void* mhadr, MhdList* list);
    void func_15(const void* mhadr, MhdList* list);
    void func_16(const void* mhadr, MhdList* list);
    void func_17(const void* mhadr, MhdList* list);
    void func_18(const void* mhadr, MhdList* list);
    void func_19(const void* mhadr, MhdList* list);
    void func_20(const void* mhadr, MhdList* list);
    void func_21(const void* mhadr, MhdList* list);
    void func_22(const void* mhadr, MhdList* list);
    void func_23(const void* mhadr, MhdList* list);
    void func_24(const void* mhadr, MhdList* list);
    void func_25(const void* mhadr, MhdList* list);
    void func_26(const void* mhadr, MhdList* list);
    void func_27(const void* mhadr, MhdList* list);
    void func_28(const void* mhadr, MhdList* list);
    void func_29(const void* mhadr, MhdList* list);
    void func_30(const void* mhadr, MhdList* list);
    void func_31(const void* mhadr, MhdList* list);
    void func_32(const void* mhadr, MhdList* list);
    void func_33(const void* mhadr, MhdList* list);
    void func_34(const void* mhadr, MhdList* list);
    void func_35(const void* mhadr, MhdList* list);
    void func_36(const void* mhadr, MhdList* list);
    void func_37(const void* mhadr, MhdList* list);
    void func_38(const void* mhadr, MhdList* list);
    void func_39(const void* mhadr, MhdList* list);
    void func_40(const void* mhadr, MhdList* list);
    void smd_shift(const void* mhadr, MhdList* list);
    void func_42(const void* mhadr, MhdList* list);
    void func_43(const void* mhadr, MhdList* list);
    void func_44(const void* mhadr, MhdList* list);
    void func_45(const void* mhadr, MhdList* list);
    void func_46(const void* mhadr, MhdList* list);
    void func_47(const void* mhadr, MhdList* list);
    void func_48(const void* mhadr, MhdList* list);
    void func_49(const void* mhadr, MhdList* list);
    void func_50(const void* mhadr, MhdList* list);
    void func_51(const void* mhadr, MhdList* list);
    void func_52(const void* mhadr, MhdList* list);
    void func_53(const void* mhadr, MhdList* list);
    void func_54(const void* mhadr, MhdList* list);
    void func_55(const void* mhadr, MhdList* list);
    void func_56(const void* mhadr, MhdList* list);
    void func_57(const void* mhadr, MhdList* list);
    void func_58(const void* mhadr, MhdList* list);
    void func_59(const void* mhadr, MhdList* list);
    void func_60(const void* mhadr, MhdList* list);
    void func_61(const void* mhadr, MhdList* list);
    void func_62(const void* mhadr, MhdList* list);
    void func_63(const void* mhadr, MhdList* list);
    void func_64(const void* mhadr, MhdList* list);
    void func_65(const void* mhadr, MhdList* list);
    void func_66(const void* mhadr, MhdList* list);
    void func_67(const void* mhadr, MhdList* list);
    void func_68(const void* mhadr, MhdList* list);
};

enum PpFuncExecFlag {
    PPFEF_NONE = 0x00,
    PPFEF_BF_SKIP = 0x01,
};

struct RobMhPp;

struct PpFuncDesc {
    void (RobMhPp::* func)(const void*, MhpList*, int32_t, const motion_database*);
    PpFuncExecFlag exec_flag;
};

struct RobMhPp {
    rob_chara* impl;
    RobBase* base;
    RobBase* base_emy;
    RobMhSmd smd_class;

    static const PpFuncDesc functbl[];

    void exec_func(int32_t type, MhpList*& play_prog, int64_t frame, const motion_database* mot_db);

    void func_0(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_1(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_2(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_3(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_4(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_5(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_6(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_7(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_8(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_9(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_10(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_11(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_12(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_13(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_14(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_15(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_16(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_17(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_18(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_19(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_20(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_21(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_22(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_23(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_24(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_25(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_26(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_27(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_28(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_29(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_30(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_31(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_32(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_33(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_34(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_35(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_36(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_37(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_38(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_39(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_40(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_41(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_42(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_43(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_44(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_45(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_46(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_47(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_48(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_49(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_50_set_face_motion_id(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_51(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_52(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_53_set_face_mottbl_motion(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_54_set_hand_r_mottbl_motion(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_55_set_hand_l_mottbl_motion(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_56_set_mouth_mottbl_motion(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_57_set_eyes_mottbl_motion(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_58_set_eyelid_mottbl_motion(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_59_set_rob_chara_head_object(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_60_set_look_camera(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_61_set_eyelid_motion_from_face(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_62_rob_parts_adjust(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_63(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_64_osage_reset(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_65_motion_skin_param(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_66_osage_step(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_67_sleeve_adjust(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_68(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_69_motion_max_frame(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_70_camera_max_frame(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_71_osage_move_cancel(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_72(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_73_rob_hand_adjust(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_74_disable_collision(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_75_rob_adjust_global(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_76_rob_arm_adjust(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_77_disable_eye_motion(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_78(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_79_rob_chara_coli_ring(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);
    void func_80_adjust_get_global_pos(
        const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db);

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
    int32_t part;
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
    void disp(render_context* rctx, size_t id,
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
    void disp(render_context* rctx, size_t id,
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
    uint32_t motnum;
    prj::vector_pair<float_t, int32_t> frames;

    osage_set_motion();
    ~osage_set_motion();

    void init_frame(uint32_t motnum, float_t frame, int32_t stage_index);
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
    ROB_ID rob_id;
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
        uint32_t motnum, float_t frame, int32_t init_cnt);
    bool CheckResetFrameNotFound(uint32_t motnum, float_t frame);
    bool GetDisplay();
    bool GetNotReset();
    void Reset();
    void SetDisplay(bool value);
    void SetNotReset(bool value);
    void SetPvId(int32_t pv_id, ROB_ID rob_id, bool reset);
    void SetPvSetMotion(const std::vector<pv_data_set_motion>& set_motion);

    void sub_1404F77E0();
    bool sub_1404F7AF0();
    void sub_1404F7BD0(bool not_reset);
    void sub_1404F82F0();
    void sub_1404F83A0(::osage_set_motion* a2);
    void sub_1404F88A0(uint32_t stage_index, uint32_t motnum, float_t frame);
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

    void AppendRobCharaFunc(rob_chara* rob_chr, void(*func)(rob_chara*));
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

    void AppendRobCharaFunc(ROB_ID rob_id, rob_chara* rob_chr, void(*func)(rob_chara*));
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

    bool AppendFreeReqData(CHARA_NUM cn);
    bool AppendFreeReqDataObj(CHARA_NUM cn, const RobItemEquip* item_set);
    bool AppendLoadReqData(CHARA_NUM cn);
    bool AppendLoadReqDataObj(CHARA_NUM cn, const RobItemEquip* item_set);
    void AppendLoadedReqData(ReqData* req_data);
    void AppendLoadedReqDataObj(ReqDataObj* req_data_obj);
    int32_t CtrlFunc1();
    int32_t CtrlFunc2();
    int32_t CtrlFunc3();
    int32_t CtrlFunc4();
    void FreeLoadedReqData(ReqData* req_data);
    void FreeLoadedReqDataObj(ReqDataObj* req_data_obj);
    bool LoadCharaItemsCheckNotRead(CHARA_NUM cn, RobItemEquip* item_set);
    bool LoadCharaItemsCheckNotReadParent(CHARA_NUM cn, RobItemEquip* item_set);
    void LoadCharaItems(CHARA_NUM cn,
        RobItemEquip* item_set, void* data, const object_database* obj_db);
    void LoadCharaObjSetMotionSet(CHARA_NUM cn,
        void* data, const object_database* obj_db, const motion_database* mot_db);
    bool LoadCharaObjSetMotionSetCheck(CHARA_NUM cn);
    void ResetReqData();
    void ResetReqDataObj();
    void UnloadCharaItems(CHARA_NUM cn, RobItemEquip* item_set);
    void UnloadCharaObjSetMotionSet(CHARA_NUM cn);
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

    virtual bool check_type(RobType type) = 0;
    virtual bool sync() = 0;

    void AppendList(rob_chara* rob_chr, std::list<rob_chara*>* list);
    void AppendCtrlCharaList(rob_chara* rob_chr);
    void AppendFreeCharaList(rob_chara* rob_chr);
    void AppendInitCharaList(rob_chara* rob_chr);
    void FreeList(int8_t* id, std::list<rob_chara*>* list);
    void FreeCharaLists();
    void FreeCtrlCharaList(int8_t* id);
    void FreeFreeCharaList(int8_t* id);
    void FreeInitCharaList(int8_t* id);
};

class TaskRobPrepareControl : public RobImplTask {
public:
    TaskRobPrepareControl();
    virtual ~TaskRobPrepareControl() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(RobType type) override;
    virtual bool sync() override;
};

class TaskRobPrepareAction : public RobImplTask {
public:
    TaskRobPrepareAction();
    virtual ~TaskRobPrepareAction() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(RobType type) override;
    virtual bool sync() override;
};

class TaskRobBase : public RobImplTask {
public:
    TaskRobBase();
    virtual ~TaskRobBase() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(RobType type) override;
    virtual bool sync() override;
};

class TaskRobCollision : public RobImplTask {
public:
    TaskRobCollision();
    virtual ~TaskRobCollision() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(RobType type) override;
    virtual bool sync() override;
};

class TaskRobInfo : public RobImplTask {
public:
    TaskRobInfo();
    virtual ~TaskRobInfo() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(RobType type) override;
    virtual bool sync() override;
};

class TaskRobMotionModifier : public RobImplTask {
public:
    TaskRobMotionModifier();
    virtual ~TaskRobMotionModifier() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;

    virtual bool check_type(RobType type) override;
    virtual bool sync() override;
};

class TaskRobDisp : public RobImplTask {
public:
    TaskRobDisp();
    virtual ~TaskRobDisp() override;

    virtual bool init() override;
    virtual bool ctrl() override;
    virtual bool dest() override;
    virtual void disp() override;

    virtual bool check_type(RobType type) override;
    virtual bool sync() override;
};

class TaskRobManager : public app::Task {
public:
    int32_t ctrl_state;
    int32_t dest_state;
    TaskRobPrepareControl task_rob_prepare_ctrl;
    TaskRobPrepareAction task_rob_prepare_act;
    TaskRobBase task_rob_base;
    TaskRobCollision task_rob_collision;
    TaskRobInfo task_rob_info;
    TaskRobMotionModifier task_rob_motion_modifier;
    TaskRobDisp task_rob_disp;
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
    void FreeLoadedCharaList(int8_t* id);
    bool GetFreeCharaListEmpty();
    bool GetWait(rob_chara* rob_chr);

    void reset_list();
};

struct RobTaskList {
    RobImplTask* task;
    const char* name;

    inline RobTaskList(RobImplTask* task = 0, const char* name = "") : task(task), name(name) {}
};

static void bone_data_parent_data_init(bone_data_parent* bone,
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data);
static void bone_data_parent_load_rob_chara(bone_data_parent* bone);

static uint32_t get_common_rob_mot(CHARA_NUM cn, uint32_t kamae_type, MOTTABLE_TYPE mottbl_type);

static void mot_key_data_get_key_set_count_by_bone_database_bones(mot_key_data* a1,
    const std::vector<BODYTYPE>* body_type_table);
static void mot_key_data_get_key_set_count(mot_key_data* a1, size_t block_max, size_t leaf_pos);
static void mot_key_data_init_key_sets(mot_key_data* a1, BONE_KIND kind, size_t block_max, size_t leaf_pos);
static const mot_data* mot_key_data_load_file(mot_key_data* a1, uint32_t motnum, const motion_database* mot_db);
static void mot_key_data_reserve_key_sets(mot_key_data* a1);

static PvOsageManager* pv_osage_manager_array_get(ROB_ID rob_id);

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
    int32_t rob_id, int32_t part, mat4& mat);
static void rob_chara_age_age_disp(rob_chara_age_age* arr,
    render_context* rctx, int32_t rob_id, bool reflect, bool chara_color);
static void rob_chara_age_age_load(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part);
static void rob_chara_age_age_reset(rob_chara_age_age* arr, int32_t rob_id);
static void rob_chara_age_age_set_alpha(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, float_t value);
static void rob_chara_age_age_set_disp(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, bool value);
static void rob_chara_age_age_set_move_cancel(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, float_t value);
static void rob_chara_age_age_set_npr(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, bool value);
static void rob_chara_age_age_set_speed(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, float_t value);
static void rob_chara_age_age_set_skip(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part);
static void rob_chara_age_age_set_step(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, float_t value);
static void rob_chara_age_age_set_step_full(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part);

static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    const std::vector<BODYTYPE>* body_type_table);
static void rob_chara_bone_data_get_adjust_scale(
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data);
static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index);
static RobNode* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index);
static void rob_chara_bone_data_adjust_scale_calculate(
    rob_chara_bone_data_adjust_scale* adjust_scale, prj::sys_vector<RobBlock>& block_vec,
    BONE_KIND kind, BONE_KIND disp_kind, const bone_database* bone_data);
static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    BONE_KIND kind, BONE_KIND disp_kind, const bone_database* bone_data);
static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
    BONE_KIND kind, BONE_KIND disp_kind, const bone_database* bone_data);
static void rob_chara_bone_data_motion_blend_mot_free(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_blend_mot_list_free(rob_chara_bone_data* rob_bone_data,
    size_t last_index);
static void rob_chara_bone_data_motion_blend_mot_list_init(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data,
    uint32_t motnum, MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db);
static void rob_chara_bone_data_reserve(rob_chara_bone_data* rob_bone_data);
static void rob_chara_bone_data_set_look_anim_param(rob_chara_bone_data* rob_bone_data,
    const rob_chara_look_anim_eye_param* params, const eyes_adjust* eyes_adjust);
static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
    const std::vector<BODYTYPE>* body_type_table, const std::string* bone_node_name_table);
static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    const uint16_t* node_parent_table);
static void rob_chara_bone_data_set_yrot(rob_chara_bone_data* rob_bone_data, float_t value);
static void rob_chara_bone_data_set_step(rob_chara_bone_data* rob_bone_data, float_t value);

static void rob_chara_age_age_ctrl(rob_chara* rob_chr, int32_t part, const char* name);
static object_info rob_chara_get_head_object(rob_chara* rob_chr, int32_t head_object_id);
static object_info rob_chara_get_object_info(rob_chara* rob_chr, ROB_PARTS_KIND rpk);
static void rob_chara_load_default_motion(rob_chara* rob_chr,
    const bone_database* bone_data, const motion_database* mot_db);
static void rob_chara_load_default_motion_sub(rob_chara* rob_chr, int32_t motion_body_type,
    uint32_t motnum, const bone_database* bone_data, const motion_database* mot_db);

static void rob_chara_set_adjust(rob_chara* rob_chr, rob_chara_data_adjust* adjust_new,
    rob_chara_data_adjust* adjust, rob_chara_data_adjust* adjust_prev);

static void rob_cmn_mottbl_read(void* a1, const void* data, size_t size);

static void opd_chara_data_array_add_frame_data(ROB_ID rob_id);
static void opd_chara_data_array_encode_data(ROB_ID rob_id);
static void opd_chara_data_array_encode_init_data(ROB_ID rob_id, uint32_t motnum);
static void opd_chara_data_array_fs_copy_file(ROB_ID rob_id);
static opd_chara_data* opd_chara_data_array_get(ROB_ID rob_id);
static void opd_chara_data_array_init_data(ROB_ID rob_id, uint32_t motnum);
static void opd_chara_data_array_open_opd_file(ROB_ID rob_id);
static void opd_chara_data_array_open_opdi_file(ROB_ID rob_id);
static void opd_chara_data_array_write_file(ROB_ID rob_id);

static const RobTaskList* get_rob_manager_list_before(TaskRobManager* task_rob_mgr);
static const RobTaskList* get_rob_manager_list_after(TaskRobManager* task_rob_mgr);

int32_t RobItem::s_have_dbg_ref;
RobItemHave RobItem::s_have_dbg[CN_MAX];
int32_t RobItem::s_have_sub_dbg_ref;
RobItemHaveSub RobItem::s_have_sub_dbg[CN_MAX];

bool RobManagement::colli_check_on = true;
CHARA_NUM RobManagement::chara_num[ROB_ID_MAX];
int32_t RobManagement::instance_count;
rob_chara RobManagement::rob_impl[ROB_ID_MAX];
RobInit RobManagement::rob_init[ROB_ID_MAX];

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
static int32_t rob_skin_disp_disable_node_blocks = 0;

const PpFuncDesc RobMhPp::functbl[] = {
    { &RobMhPp::func_0 , PPFEF_BF_SKIP },
    { &RobMhPp::func_1 , PPFEF_BF_SKIP },
    { &RobMhPp::func_2 , PPFEF_NONE },
    { &RobMhPp::func_3 , PPFEF_NONE },
    { &RobMhPp::func_4 , PPFEF_NONE },
    { &RobMhPp::func_5 , PPFEF_NONE },
    { &RobMhPp::func_6 , PPFEF_NONE },
    { &RobMhPp::func_7 , PPFEF_NONE },
    { &RobMhPp::func_8 , PPFEF_BF_SKIP },
    { &RobMhPp::func_9 , PPFEF_NONE },
    { &RobMhPp::func_10, PPFEF_NONE },
    { &RobMhPp::func_11, PPFEF_NONE },
    { &RobMhPp::func_12, PPFEF_NONE },
    { &RobMhPp::func_13, PPFEF_NONE },
    { &RobMhPp::func_14, PPFEF_NONE },
    { &RobMhPp::func_15, PPFEF_NONE },
    { &RobMhPp::func_16, PPFEF_NONE },
    { &RobMhPp::func_17, PPFEF_NONE },
    { &RobMhPp::func_18, PPFEF_NONE },
    { &RobMhPp::func_19, PPFEF_NONE },
    { &RobMhPp::func_20, PPFEF_NONE },
    { &RobMhPp::func_21, PPFEF_NONE },
    { &RobMhPp::func_22, PPFEF_NONE },
    { &RobMhPp::func_23, PPFEF_NONE },
    { &RobMhPp::func_24, PPFEF_NONE },
    { &RobMhPp::func_25, PPFEF_BF_SKIP },
    { &RobMhPp::func_26, PPFEF_NONE },
    { &RobMhPp::func_27, PPFEF_NONE },
    { &RobMhPp::func_28, PPFEF_NONE },
    { &RobMhPp::func_29, PPFEF_NONE },
    { &RobMhPp::func_30, PPFEF_NONE },
    { &RobMhPp::func_31, PPFEF_BF_SKIP },
    { &RobMhPp::func_32, PPFEF_NONE },
    { &RobMhPp::func_33, PPFEF_NONE },
    { &RobMhPp::func_34, PPFEF_NONE },
    { &RobMhPp::func_35, PPFEF_NONE },
    { &RobMhPp::func_36, PPFEF_NONE },
    { &RobMhPp::func_37, PPFEF_NONE },
    { &RobMhPp::func_38, PPFEF_NONE },
    { &RobMhPp::func_39, PPFEF_NONE },
    { &RobMhPp::func_40, PPFEF_NONE },
    { &RobMhPp::func_41, PPFEF_NONE },
    { &RobMhPp::func_42, PPFEF_NONE },
    { &RobMhPp::func_43, PPFEF_NONE },
    { &RobMhPp::func_44, PPFEF_NONE },
    { &RobMhPp::func_45, PPFEF_NONE },
    { &RobMhPp::func_46, PPFEF_NONE },
    { &RobMhPp::func_47, PPFEF_NONE },
    { &RobMhPp::func_48, PPFEF_NONE },
    { &RobMhPp::func_49, PPFEF_NONE },
    { &RobMhPp::func_50_set_face_motion_id, PPFEF_NONE },
    { &RobMhPp::func_51, PPFEF_NONE },
    { &RobMhPp::func_52, PPFEF_NONE },
    { &RobMhPp::func_53_set_face_mottbl_motion, PPFEF_NONE },
    { &RobMhPp::func_54_set_hand_r_mottbl_motion, PPFEF_NONE },
    { &RobMhPp::func_55_set_hand_l_mottbl_motion, PPFEF_NONE },
    { &RobMhPp::func_56_set_mouth_mottbl_motion, PPFEF_NONE },
    { &RobMhPp::func_57_set_eyes_mottbl_motion, PPFEF_NONE },
    { &RobMhPp::func_58_set_eyelid_mottbl_motion, PPFEF_NONE },
    { &RobMhPp::func_59_set_rob_chara_head_object, PPFEF_NONE },
    { &RobMhPp::func_60_set_look_camera, PPFEF_NONE },
    { &RobMhPp::func_61_set_eyelid_motion_from_face, PPFEF_NONE },
    { &RobMhPp::func_62_rob_parts_adjust, PPFEF_NONE },
    { &RobMhPp::func_63, PPFEF_NONE },
    { &RobMhPp::func_64_osage_reset, PPFEF_NONE },
    { &RobMhPp::func_65_motion_skin_param, PPFEF_NONE },
    { &RobMhPp::func_66_osage_step, PPFEF_NONE },
    { &RobMhPp::func_67_sleeve_adjust, PPFEF_NONE },
    { &RobMhPp::func_68, PPFEF_NONE },
    { &RobMhPp::func_69_motion_max_frame, PPFEF_NONE },
    { &RobMhPp::func_70_camera_max_frame, PPFEF_NONE },
    { &RobMhPp::func_71_osage_move_cancel, PPFEF_NONE },
    { &RobMhPp::func_72, PPFEF_NONE },
    { &RobMhPp::func_73_rob_hand_adjust, PPFEF_NONE },
    { &RobMhPp::func_74_disable_collision, PPFEF_NONE },
    { &RobMhPp::func_75_rob_adjust_global, PPFEF_NONE },
    { &RobMhPp::func_76_rob_arm_adjust, PPFEF_NONE },
    { &RobMhPp::func_77_disable_eye_motion, PPFEF_NONE },
    { &RobMhPp::func_78, PPFEF_NONE },
    { &RobMhPp::func_79_rob_chara_coli_ring, PPFEF_NONE },
    { &RobMhPp::func_80_adjust_get_global_pos, PPFEF_NONE },
};

const SmdFunc RobMhSmd::functbl[] = {
    &RobMhSmd::smd_next,
    &RobMhSmd::func_1,
    &RobMhSmd::func_2,
    &RobMhSmd::func_3,
    &RobMhSmd::func_4,
    &RobMhSmd::func_5,
    &RobMhSmd::func_6,
    &RobMhSmd::func_7,
    &RobMhSmd::func_8,
    &RobMhSmd::func_9,
    &RobMhSmd::func_10,
    &RobMhSmd::func_11,
    &RobMhSmd::func_12,
    &RobMhSmd::func_13,
    &RobMhSmd::func_14,
    &RobMhSmd::func_15,
    &RobMhSmd::func_16,
    &RobMhSmd::func_17,
    &RobMhSmd::func_18,
    &RobMhSmd::func_19,
    &RobMhSmd::func_20,
    &RobMhSmd::func_21,
    &RobMhSmd::func_22,
    &RobMhSmd::func_23,
    &RobMhSmd::func_24,
    &RobMhSmd::func_25,
    &RobMhSmd::func_26,
    &RobMhSmd::func_27,
    &RobMhSmd::func_28,
    &RobMhSmd::func_29,
    &RobMhSmd::func_30,
    &RobMhSmd::func_31,
    &RobMhSmd::func_32,
    &RobMhSmd::func_33,
    &RobMhSmd::func_34,
    &RobMhSmd::func_35,
    &RobMhSmd::func_36,
    &RobMhSmd::func_37,
    &RobMhSmd::func_38,
    &RobMhSmd::func_39,
    &RobMhSmd::func_40,
    &RobMhSmd::smd_shift,
    &RobMhSmd::func_42,
    &RobMhSmd::func_43,
    &RobMhSmd::func_44,
    &RobMhSmd::func_45,
    &RobMhSmd::func_46,
    &RobMhSmd::func_47,
    &RobMhSmd::func_48,
    &RobMhSmd::func_49,
    &RobMhSmd::func_40,
    &RobMhSmd::func_51,
    &RobMhSmd::func_52,
    &RobMhSmd::func_53,
    &RobMhSmd::func_54,
    &RobMhSmd::func_55,
    &RobMhSmd::func_56,
    &RobMhSmd::func_57,
    &RobMhSmd::func_58,
    &RobMhSmd::func_59,
    &RobMhSmd::func_60,
    &RobMhSmd::func_61,
    &RobMhSmd::func_62,
    &RobMhSmd::func_63,
    &RobMhSmd::func_64,
    &RobMhSmd::func_65,
    &RobMhSmd::func_66,
    &RobMhSmd::func_67,
    &RobMhSmd::func_68,
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

static const BONE_BLK lc_part_block_tbl[] = {
    BLK_C_KATA_R, BLK_C_KATA_L,
    BLK_CL_MOMO_R, BLK_CL_MOMO_L,
};

static const BONE_BLK c_kata_r_ik_blk_tbl[] = {
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

static const BONE_BLK c_kata_l_ik_blk_tbl[] = {
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

static const BONE_BLK c_momo_r_ik_blk_tbl[] = {
    BLK_CL_MOMO_R, BLK_KL_ASI_R_WJ_CO, BLK_KL_TOE_R_WJ,
    BLK_N_HIZA_R_WJ_EX, BLK_N_MOMO_A_R_WJ_CD_EX,
    BLK_N_MOMO_B_R_WJ_EX, BLK_N_MOMO_C_R_WJ_EX, BLK_DUMMY
};

static const BONE_BLK c_momo_l_ik_blk_tbl[] = {
    BLK_CL_MOMO_L, BLK_KL_ASI_L_WJ_CO, BLK_KL_TOE_L_WJ,
    BLK_N_HIZA_L_WJ_EX, BLK_N_MOMO_A_L_WJ_CD_EX,
    BLK_N_MOMO_B_L_WJ_EX, BLK_N_MOMO_C_L_WJ_EX, BLK_DUMMY
};

static const BONE_ID lc_target_bone_tbl[] = {
    BONE_ID_DUMMY, BONE_ID_KL_TE_R_WJ, BONE_ID_KL_TE_L_WJ, BONE_ID_KL_ASI_R_WJ_CO,
    BONE_ID_KL_ASI_L_WJ_CO, BONE_ID_J_SUNE_R_WJ, BONE_ID_J_SUNE_L_WJ,
    BONE_ID_UDE_R_WJ, BONE_ID_UDE_L_WJ, BONE_ID_CL_KAO, BONE_ID_DUMMY,
    BONE_ID_DUMMY, BONE_ID_DUMMY, BONE_ID_DUMMY, BONE_ID_DUMMY, BONE_ID_DUMMY,
    BONE_ID_DUMMY, BONE_ID_KL_TE_R_WJ, BONE_ID_KL_TE_L_WJ, BONE_ID_KL_ASI_R_WJ_CO,
    BONE_ID_KL_ASI_L_WJ_CO, BONE_ID_J_SUNE_R_WJ, BONE_ID_J_SUNE_L_WJ,
    BONE_ID_UDE_R_WJ, BONE_ID_UDE_L_WJ, BONE_ID_CL_KAO, BONE_ID_DUMMY,
};

static const BONE_BLK lc_arm_block_tbl[] = {
    BLK_C_KATA_L,
    BLK_C_KATA_R,
};

static const BONE_ID lc_target_arm_tbl[] = {
    BONE_ID_KL_TE_L_WJ,
    BONE_ID_KL_TE_R_WJ
};

static const BONE_BLK* recalc_ik_block_tbl[] = {
    c_kata_r_ik_blk_tbl,
    c_kata_l_ik_blk_tbl,
    c_momo_r_ik_blk_tbl,
    c_momo_l_ik_blk_tbl,
};

static const BONE_BLK* recalc_flip_ik_block_tbl[] = {
    c_kata_l_ik_blk_tbl,
    c_kata_r_ik_blk_tbl,
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

const int16_t leaf_ctrl_part_mirror_tbl[] = {
    LCPART_TE_L,
    LCPART_TE_R,
    LCPART_ASI_L,
    LCPART_ASI_R,
    LCPART_MAX,
};

const int16_t leaf_ctrl_target_mirror_tbl[] = {
    LCTAG_ABS,
    LCTAG_TE_L,
    LCTAG_TE_R,
    LCTAG_ASI_L,
    LCTAG_ASI_R,
    LCTAG_SUNE_L,
    LCTAG_SUNE_R,
    LCTAG_UDE_L,
    LCTAG_UDE_R,
    LCTAG_KAO,
    LCTAG_ADJ_EMY,
    LCTAG_ADJ_EMY_BODY,
    LCTAG_ADJ_EMY_HEIGHT,
    LCTAG_ADJ_REV,
    LCTAG_ADJ_BODY_REV,
    LCTAG_ADJ_HEIGHT_REV,
    LCTAG_OFS,
    LCTAG_OFS_TE_L,
    LCTAG_OFS_TE_R,
    LCTAG_OFS_ASI_L,
    LCTAG_OFS_ASI_R,
    LCTAG_OFS_SUNE_L,
    LCTAG_OFS_SUNE_R,
    LCTAG_OFS_UDE_L,
    LCTAG_OFS_UDE_R,
    LCTAG_OFS_KAO,
    LCTAG_OFS_BODY,
    LCTAG_MAX,
};

const bool leaf_ctrl_tagofs_mirror_flip_tbl[][3] = {
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 1, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 1, 0, 0 },
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 0, 1 },
    { 0, 1, 0 },
    { 1, 0, 0 },
    { 0, 0, 0 },
};

const ROB_ITEM_EQUIP_SUB_ID equip_sub_id_phase2[] = {
    ROB_ITEM_EQUIP_SUB_ID_ZUJO, ROB_ITEM_EQUIP_SUB_ID_KAMI, ROB_ITEM_EQUIP_SUB_ID_HITAI,
    ROB_ITEM_EQUIP_SUB_ID_ME, ROB_ITEM_EQUIP_SUB_ID_MEGANE, ROB_ITEM_EQUIP_SUB_ID_MIMI,
    ROB_ITEM_EQUIP_SUB_ID_KUCHI, ROB_ITEM_EQUIP_SUB_ID_MAKI, ROB_ITEM_EQUIP_SUB_ID_KUBI,
    ROB_ITEM_EQUIP_SUB_ID_INNER, ROB_ITEM_EQUIP_SUB_ID_KATA, ROB_ITEM_EQUIP_SUB_ID_U_UDE,
    ROB_ITEM_EQUIP_SUB_ID_L_UDE, ROB_ITEM_EQUIP_SUB_ID_JOHA_MAE, ROB_ITEM_EQUIP_SUB_ID_JOHA_USHIRO,
    ROB_ITEM_EQUIP_SUB_ID_BELT, ROB_ITEM_EQUIP_SUB_ID_KOSI, ROB_ITEM_EQUIP_SUB_ID_SUNE,
    ROB_ITEM_EQUIP_SUB_ID_KUTSU, ROB_ITEM_EQUIP_SUB_ID_HEAD, ROB_ITEM_EQUIP_SUB_ID_MAX,
};

const ROB_ITEM_EQUIP_SUB_ID equip_sub_id_phase3[] = {
    ROB_ITEM_EQUIP_SUB_ID_TE, ROB_ITEM_EQUIP_SUB_ID_ASI, ROB_ITEM_EQUIP_SUB_ID_MAX,
};

const ROB_ITEM_EQUIP_SUB_ID equip_sub_id_phase4[] = {
    ROB_ITEM_EQUIP_SUB_ID_PANTS, ROB_ITEM_EQUIP_SUB_ID_OUTER, ROB_ITEM_EQUIP_SUB_ID_MAX,
};

const ROB_PARTS_KIND rpk_item[] = {
    RPK_ZUJO, RPK_KAMI, RPK_NONE, RPK_NONE,
    RPK_MEGANE, RPK_NONE, RPK_KUCHI, RPK_NONE,
    RPK_KUBI, RPK_NONE, RPK_OUTER, RPK_NONE,
    RPK_NONE, RPK_NONE, RPK_NONE, RPK_NONE,
    RPK_JOHA_USHIRO, RPK_NONE, RPK_NONE, RPK_PANTS,
    RPK_NONE, RPK_NONE, RPK_NONE, RPK_NONE,
    RPK_NONE,
};

static const ROB_ITEM_EQUIP_ID equip_id_conv_table[] = {
    ROB_ITEM_EQUIP_ID_ATAM, ROB_ITEM_EQUIP_ID_ATAM, ROB_ITEM_EQUIP_ID_ATAM,
    ROB_ITEM_EQUIP_ID_KAO, ROB_ITEM_EQUIP_ID_KAO, ROB_ITEM_EQUIP_ID_KAO,
    ROB_ITEM_EQUIP_ID_KAO, ROB_ITEM_EQUIP_ID_KAO, ROB_ITEM_EQUIP_ID_JOHA,
    ROB_ITEM_EQUIP_ID_JOHA, ROB_ITEM_EQUIP_ID_JOHA, ROB_ITEM_EQUIP_ID_UDE,
    ROB_ITEM_EQUIP_ID_UDE, ROB_ITEM_EQUIP_ID_UDE, ROB_ITEM_EQUIP_ID_UDE,
    ROB_ITEM_EQUIP_ID_JOHA, ROB_ITEM_EQUIP_ID_JOHA, ROB_ITEM_EQUIP_ID_KAHA,
    ROB_ITEM_EQUIP_ID_KAHA, ROB_ITEM_EQUIP_ID_KAHA, ROB_ITEM_EQUIP_ID_KAHA,
    ROB_ITEM_EQUIP_ID_KAHA, ROB_ITEM_EQUIP_ID_KAHA, ROB_ITEM_EQUIP_ID_JOHA,
    ROB_ITEM_EQUIP_ID_KAO,
};

extern render_context* rctx_ptr;

extern uint32_t cmn_set_id;

static RobManagement s_rob_management;

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

void RobTransform::init() {
    pos = 0.0f;
    rot = 0.0f;
    scale = 1.0f;
    hsc = 1.0f;
}

void RobTransform::init(const vec3& p, const vec3& r) {
    pos = p;
    rot = r;
    scale = 1.0f;
    hsc = 1.0f;
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

void RobNode::init(const char* in_name, mat4* in_mat, mat4* in_no_scale_mat) {
    name = in_name;

    mat_ptr = in_mat;
    if (in_mat)
        *in_mat = mat4_identity;

    parent = 0;
    transform.init();

    no_scale_mat = in_no_scale_mat;
    if (in_no_scale_mat)
        *in_no_scale_mat = mat4_identity;
}

bool Fcurve::fcurve_init(const void* data) {
    frame_max = 0;
    if (!data)
        return 0;

    const mot_data* d = (const mot_data*)data;
    frame_max = d->frame_max;

    int32_t fc_max = d->key_set_count;
    bool motion_body_type = d->motion_body_type != 0;
    this->fc_max = (uint16_t)fc_max;
    key_num_type = d->info & 0x8000;

    const mot_key_set_data* key_set = d->key_set_array;
    FcurveKey* fck = fck_ptr;

    for (int32_t i = 0; i < fc_max; i++, fck++, key_set++)
        fck->kind = (FcurveKeyKind)key_set->type;

    fcurve_init_u16(d->key_set_array);
    return motion_body_type;
}

static uint32_t count_leading_zeros(uint16_t value) {
    uint32_t v3 = 16;
    if (value >> 8) {
        v3 = 8;
        value >>= 8;
    }

    if (value >> 4) {
        v3 -= 4;
        value >>= 4;
    }

    if (value >> 2) {
        v3 -= 2;
        value >>= 2;
    }

    if (value >> 1)
        value = 2;
    return v3 - value;
}

void Fcurve::fcurve_init_u16(const void* data) {
    const mot_key_set_data* key_set = (const mot_key_set_data*)data;
    FcurveKey* fck = fck_ptr;
    for (int32_t i = 0; i < fc_max; i++, key_set++, fck++) {
        if (fck->kind == FCURVE_KEY_KIND_STATIC_DATA)
            fck->val = key_set->values;
        else if (fck->kind != FCURVE_KEY_KIND_STATIC_0) {
            uint16_t sum = key_set->keys_count;
            fck->num = key_set->frames;
            fck->val = key_set->values;

            fck->sum = sum;
            fck->cache_idx = 0;

            uint16_t last_idx = frame_max;
            if (sum > 1) {
                if (sum > last_idx)
                    sum = frame_max;
                last_idx = (16 - count_leading_zeros(sum)) * frame_max / sum;
            }
            fck->last_idx = last_idx;
        }
    }
}

void Fcurve::interpolate(float_t frame, float_t* value,
    FcurveKey* fck, uint32_t in_fc_max, const struc_369* a6) {
    if (a6->field_0 == 4)
        frame = prj::floorf(frame);

    if (!in_fc_max)
        return;

    for (uint32_t i = in_fc_max; i; i--, fck++, value++) {
        FcurveKeyKind kind = fck->kind;
        const float_t* val = fck->val;
        if (kind == FCURVE_KEY_KIND_STATIC_0) {
            *value = 0.0f;
            continue;
        }
        else if (kind == FCURVE_KEY_KIND_STATIC_DATA) {
            *value = val[0];
            continue;
        }
        else if (kind != FCURVE_KEY_KIND_HERMITE && kind != FCURVE_KEY_KIND_HERMITE_TANGENT) {
            *value = 0.0f;
            continue;
        }

        int32_t cache_idx = fck->cache_idx;
        int32_t sum = fck->sum;
        const uint16_t* num = fck->num;
        int32_t frame_int = (int32_t)frame;
        size_t key_index;
        if (cache_idx > sum
            || (frame_int > fck->last_idx + num[cache_idx])
            || cache_idx > 0 && frame_int < num[cache_idx - 1]) {
            const uint16_t* key = num;
            size_t length = sum;
            size_t temp;
            while (length > 0)
                if (key[temp = length / 2] > frame_int)
                    length /= 2;
                else {
                    key += temp + 1;
                    length -= temp + 1;
                }
            key_index = key - num;
        }
        else {
            const uint16_t* key = &num[cache_idx];
            for (const uint16_t* key_end = &num[sum]; key != key_end; key++)
                if (frame_int < *key)
                    break;
            key_index = key - num;
        }

        bool found = false;
        if (key_index < sum)
            for (; key_index < sum; key_index++)
                if ((float_t)num[key_index] >= frame) {
                    found = true;
                    break;
                }

        if (found) {
            float_t next_frame = (float_t)num[key_index];
            if (fabsf(next_frame - frame) > 0.000001f && key_index > 0) {
                float_t curr_frame = (float_t)num[key_index - 1];
                float_t df = frame - curr_frame;
                float_t t = df / (next_frame - curr_frame);
                if (kind == FCURVE_KEY_KIND_HERMITE) {
                    val += key_index - 1;
                    float_t p1 = val[0];
                    float_t p2 = val[1];
                    *value = (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
                }
                else {
                    val += key_index * 2 - 2;
                    float_t p1 = val[0];
                    float_t p2 = val[2];
                    float_t t1 = val[1];
                    float_t t2 = val[3];
                    float_t t_1 = t - 1.0f;
                    *value = (t_1 * t1 + t * t2) * t_1 * df
                        + (t * 2.0f - 3.0f) * (t * t) * (p1 - p2) + p1;
                }
                fck->cache_idx = (int32_t)key_index;
                continue;
            }
        }

        if (key_index > 0)
            key_index--;
        if (kind == FCURVE_KEY_KIND_HERMITE)
            *value = val[key_index];
        else
            *value = val[2 * key_index];
        fck->cache_idx = (int32_t)key_index;
    }
}

void Fcurve::set_fck_ptr(FcurveKey* ptr) {
    fck_ptr = ptr;
}

mot_key_data::mot_key_data() : key_sets_ready(), key_set_count(), key_set(),
mot(), fc_value(), mot_data(), kind(), motion_body_type(), field_68() {
    motnum = -1;
    frame = -1.0f;
}

mot_key_data::~mot_key_data() {

}

void mot_key_data::interpolate(float_t frame, uint32_t key_set_offset, uint32_t key_set_count) {
    mot.interpolate(frame, &fc_value.data()[key_set_offset],
        &key_set.data()[key_set_offset], key_set_count, &field_68);
}

void mot_key_data::reset() {
    key_sets_ready = 0;
    kind = BONE_KIND_NONE;
    frame = -1.0f;
    key_set_count = 0;
    mot_data = 0;
    motion_body_type = 0;
    field_68 = {};

    key_set.clear();
    fc_value.clear();
    motnum = -1;
}

RobBlock::RobBlock() : ik_type(), inherit_type(), block_id(), flip_block_id(),
inherit_mat_id(), expression_id(), key_set_offset(), key_set_count(), frame(),
up_vector_mat_ptr(), inherit_mat_ptr(), node(), len(), arm_length() {
    eyes_xrot_adjust_neg = 1.0f;
    eyes_xrot_adjust_pos = 1.0f;
}

RobBlock::~RobBlock() {

}

// 0x1401E9D60
bool RobBlock::calc_constraint(const RobBlock* block_top) {
    vec3 target;

    switch (block_id) {
    case BLK_N_SKATA_L_WJ_CD_EX:
        mat4_get_translation(block_top[BLK_C_KATA_L].node[2].mat_ptr, &target);
        exp_set_dir(target);
        break;
    case BLK_N_SKATA_R_WJ_CD_EX:
        mat4_get_translation(block_top[BLK_C_KATA_R].node[2].mat_ptr, &target);
        exp_set_dir(target);
        break;
    case BLK_N_SKATA_B_L_WJ_CD_CU_EX:
        mat4_get_translation(block_top[BLK_N_UP_KATA_L_EX].node[0].mat_ptr, &target);
        exp_set_rot(target, 0.333f);
        break;
    case BLK_N_SKATA_B_R_WJ_CD_CU_EX:
        mat4_get_translation(block_top[BLK_N_UP_KATA_R_EX].node[0].mat_ptr, &target);
        exp_set_rot(target, 0.333f);
        break;
    case BLK_N_SKATA_C_L_WJ_CD_CU_EX:
        mat4_get_translation(block_top[BLK_N_UP_KATA_L_EX].node[0].mat_ptr, &target);
        exp_set_rot(target, 0.5f);
        break;
    case BLK_N_SKATA_C_R_WJ_CD_CU_EX:
        mat4_get_translation(block_top[BLK_N_UP_KATA_R_EX].node[0].mat_ptr, &target);
        exp_set_rot(target, 0.5f);
        break;
    case BLK_N_MOMO_A_L_WJ_CD_EX:
        mat4_get_translation(block_top[BLK_CL_MOMO_L].node[2].mat_ptr, &target);
        mat4_inverse_transform_point(node[0].mat_ptr, &target, &target);
        exp_set_dir_zx(-target);
        break;
    case BLK_N_MOMO_A_R_WJ_CD_EX:
        mat4_get_translation(block_top[BLK_CL_MOMO_R].node[2].mat_ptr, &target);
        mat4_inverse_transform_point(node[0].mat_ptr, &target, &target);
        exp_set_dir_zx(-target);
        break;
    case BLK_N_HARA_CD_EX:
        mat4_get_translation(block_top[BLK_KL_MUNE_B_WJ].node[0].mat_ptr, &target);
        mat4_inverse_transform_point(node[0].mat_ptr, &target, &target);
        exp_set_dir_zx(target);
        break;
    default:
        return false;
    }
    return true;
}

void RobBlock::copy_rot_trans(const RobBlock& other) {
    if (check_expression_id_not_null())
        return;

    switch (ik_type) {
    case IKT_0N:
    case IKT_0T:
    case IKT_ROOT:
        leaf_pos[1] = other.leaf_pos[1];
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

void RobBlock::exp_set_dir(const vec3& target) {
    vec3 vx;
    mat4_inverse_transform_point(node[0].mat_ptr, &target, &vx);

    float_t len;
    len = vec3::length_squared(vx);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vx *= 1.0f / len;

    // Gram-Schmidt process, but hacky
    vec3 vz;
    vz.x = vx.y * 0.0f - vx.x * vx.z - vx.z;
    vz.y = vx.z * -vx.y - 0.0f * vx.x;
    vz.z = vx.x * vx.x - -vx.y * vx.y + vx.x;

    // Written other way
    //vec3 vz = vec3::cross(vx, vec3(-vx.y, vx.x, 0.0f)) + vec3(-vx.z, 0.0f, vx.x);

    // Expanded
    //vec3 vz = vec3::cross(vx, vec3::cross(vec3(0.0f, 0.0f, 1.0f), vx))
    //    + vec3::cross(vx, vec3(0.0f, 1.0f, 0.0f));

    len = vec3::length_squared(vz);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vz *= 1.0f / len;

    vec3 vy = vec3::cross(vz, vx);

    mat4 chain_rot;
    mat4_set_row(&chain_rot, 0, vx.x, vx.y, vx.z, 0.0f);
    mat4_set_row(&chain_rot, 1, vy.x, vy.y, vy.z, 0.0f);
    mat4_set_row(&chain_rot, 2, vz.x, vz.y, vz.z, 0.0f);
    mat4_set_row(&chain_rot, 3, 0.0f, 0.0f, 0.0f, 1.0f);

    mat4_mul(&chain_rot, node[0].mat_ptr, node[0].mat_ptr);
}

void RobBlock::exp_set_dir_zx(vec3 vy) {
    float_t len;
    len = vec3::length_squared(vy);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vy *= 1.0f / len;

    // Gram-Schmidt process, but hacky
    vec3 vz;
    vz.x = -vy.x * vy.z - vy.y * 0.0f;
    vz.y = 0.0f * vy.x - vy.z * vy.y - vy.z;
    vz.z = vy.y * vy.y - vy.x * -vy.x + vy.y;

    // Written other way
    //vec3 vz = vec3::cross(vec3(vy.y, -vy.x, 0.0f), vy) + vec3(0.0f, -vy.z, vy.y);

    // Expanded
    //vec3 vz = vec3::cross(vec3::cross(vy, vec3(0.0f, 0.0f, 1.0f)), vy)
    //    + vec3::cross(vec3(1.0f, 0.0f, 0.0f), vy);

    len = vec3::length_squared(vz);
    if (len <= 0.000001f)
        return;

    len = sqrtf(len);
    if (len != 0.0f)
        vz *= 1.0f / len;

    vec3 vx = vec3::cross(vy, vz);

    mat4 chain_rot;
    mat4_set_row(&chain_rot, 0, vx.x, vx.y, vx.z, 0.0f);
    mat4_set_row(&chain_rot, 1, vy.x, vy.y, vy.z, 0.0f);
    mat4_set_row(&chain_rot, 2, vz.x, vz.y, vz.z, 0.0f);
    mat4_set_row(&chain_rot, 3, 0.0f, 0.0f, 0.0f, 1.0f);

    mat4_mul(&chain_rot, node[0].mat_ptr, node[0].mat_ptr);
}

void RobBlock::exp_set_rot(const vec3& glo, float_t keisuu) {
    vec3 local_target;
    mat4_inverse_transform_point(node[0].mat_ptr, &glo, &local_target);
    local_target.x = 0.0f;

    float_t len = vec2::length(*(vec2*)&local_target.y);
    if (fabsf(len) > 0.000001f) {
        float_t angle = atan2f((1.0f / len) * local_target.z, (1.0f / len) * local_target.y);
        mat4_mul_rotate_x(node[0].mat_ptr, angle * keisuu, node[0].mat_ptr);
    }
}

bool RobBlock::get_ex_rotation(RobTransform& transform, const RobBlock* block_top) {
    RobNode* node;

    switch (block_id) {
    case BLK_N_EYE_L_WJ_EX:
        transform.rot.x = -block_top[BLK_KL_EYE_L].node[0].transform.rot.x;
        transform.rot.y = block_top[BLK_KL_EYE_L].node[0].transform.rot.y * (float_t)(-1.0 / 2.0);
        break;
    case BLK_N_EYE_R_WJ_EX:
        transform.rot.x = -block_top[BLK_KL_EYE_R].node[0].transform.rot.x;
        transform.rot.y = block_top[BLK_KL_EYE_R].node[0].transform.rot.y * (float_t)(-1.0 / 2.0);
        break;
    case BLK_N_KUBI_WJ_EX: {
        mat4 mat = block_top[BLK_CL_KAO].chain_rot[0];
        mat4_mul(&block_top[BLK_CL_KAO].chain_rot[1], &mat, &mat);

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
        transform.rot.x = block_top[BLK_KL_TE_L_WJ].node[0].transform.rot.x;
        break;
    case BLK_N_SUDE_L_WJ_EX:
    case BLK_N_SUDE_B_L_WJ_EX:
        node = &block_top[BLK_KL_TE_L_WJ].node[0];
        transform.rot.x = RobBlock::limit_angle(node->transform.rot.x) * (float_t)(1.0 / 3.0);
        break;
    case BLK_N_SUDE_R_WJ_EX:
    case BLK_N_SUDE_B_R_WJ_EX:
        node = &block_top[BLK_KL_TE_R_WJ].node[0];
        transform.rot.x = RobBlock::limit_angle(node->transform.rot.x) * (float_t)(1.0 / 3.0);
        break;
    case BLK_N_HIJI_L_WJ_EX:
        transform.rot.z = block_top[BLK_C_KATA_L].node[2].transform.rot.z * (float_t)(1.0 / 2.0);
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
        transform.rot.x = block_top[BLK_KL_TE_R_WJ].node[0].transform.rot.x;
        break;
    case BLK_N_HIJI_R_WJ_EX:
        transform.rot.z = block_top[BLK_C_KATA_R].node[2].transform.rot.z * (float_t)(1.0 / 2.0);
        break;
    case BLK_N_HIZA_L_WJ_EX:
        transform.rot.z = block_top[BLK_CL_MOMO_L].node[2].transform.rot.z * (float_t)(1.0 / 2.0);
        break;
    case BLK_N_HIZA_R_WJ_EX:
        transform.rot.z = block_top[BLK_CL_MOMO_R].node[2].transform.rot.z * (float_t)(1.0 / 2.0);
        break;
    case BLK_N_MOMO_B_L_WJ_EX:
    case BLK_N_MOMO_C_L_WJ_EX:
        node = &block_top[BLK_CL_MOMO_L].node[0];
        transform.rot.y = RobBlock::limit_angle(node->transform.rot.y
            + node->transform.rot.x) * (float_t)(1.0 / 3.0);
        break;
    case BLK_N_MOMO_B_R_WJ_EX:
    case BLK_N_MOMO_C_R_WJ_EX:
        node = &block_top[BLK_CL_MOMO_R].node[0];
        transform.rot.y = RobBlock::limit_angle(node->transform.rot.y
            + node->transform.rot.x) * (float_t)(1.0 / 3.0);
        break;
    case BLK_N_HARA_B_WJ_EX:
        transform.rot.y = block_top[BLK_CL_MUNE].node[0].transform.rot.y * (float_t)(1.0 / 3.0)
            + block_top[BLK_KL_KOSI_Y].node[0].transform.rot.y * (float_t)(2.0 / 3.0);
        break;
    case BLK_N_HARA_C_WJ_EX:
        transform.rot.y = block_top[BLK_CL_MUNE].node[0].transform.rot.y * (float_t)(2.0 / 3.0)
            + block_top[BLK_KL_KOSI_Y].node[0].transform.rot.y * (float_t)(1.0 / 3.0);
        break;
    default:
        return false;
    }
    return true;
}

void RobBlock::get_mat(int32_t target) {
    if (check_expression_id_not_null())
        return;

    mat4 mat = inherit_type ? *inherit_mat_ptr : mat4_identity;

    if (ik_type == IKT_0T) {
        mat4_mul_translate(&mat, &leaf_pos[1], &mat);
        chain_rot[0] = mat4_identity;
    }
    else if (ik_type == IKT_0N) {
        mat4_inverse_transform_point(&mat, &leaf_pos[1], &leaf_pos[1]);
        mat4_mul_translate(&mat, &leaf_pos[1], &mat);
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
            leaf_pos[1] = chain_pos[target];
            mat4_rotate_zyx(&chain_ang, &chain_rot);
        }

        mat4_mul_translate(&mat, &leaf_pos[1], &mat);
        mat4_mul(&chain_rot, &mat, &mat);
        this->chain_rot[0] = chain_rot;
    }

    *node[0].mat_ptr = mat;

    solve_ik(target);
}

void RobBlock::solve_ik(int32_t target) {
    if (ik_type < IKT_1)
        return;

    mat4 mat = *node[0].mat_ptr;

    vec3 local_target;
    mat4_inverse_transform_point(&mat, &leaf_pos[0], &local_target);

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
        mat4_mul_translate(&mat, len[0][target], 0.0f, 0.0f, &mat);

        *node[2].mat_ptr = mat;
        return;
    }

    float_t len0 = len[0][target];
    float_t len1 = len[1][target];
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

// 0x1401EB6D0
void RobBlock::recalc_fk_block(const mat4& cur_mat, const RobBlock* block_top, bool rot) {
    mat4 mat = inherit_type ? *inherit_mat_ptr : cur_mat;

    if (ik_type != IKT_0N && ik_type != IKT_0T) {
        if (ik_type != IKT_ROOT)
            leaf_pos[1] = chain_pos[1];

        mat4_mul_translate(&mat, &leaf_pos[1], &mat);
        if (rot) {
            node[0].transform.rot = 0.0f;
            if (!check_expression_id_not_null())
                mat4_get_rotation_zyx(&chain_rot[0], &node[0].transform.rot);
            else if (get_ex_rotation(node[0].transform, block_top))
                mat4_rotate_zyx(&node[0].transform.rot, &chain_rot[0]);
            else {
                *node[0].mat_ptr = mat;

                if (calc_constraint(block_top)) {
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
        mat4_mul_translate(&mat, &leaf_pos[1], &mat);
        if (rot)
            node[0].transform.rot = 0.0f;
    }

    *node[0].mat_ptr = mat;

    if (rot) {
        node[0].transform.pos = leaf_pos[1];
        node[0].transform.reset_scale();
    }

    if (ik_type < IKT_1)
        return;

    mat4_mul(&chain_rot[1], &mat, &mat);
    *node[1].mat_ptr = mat;

    if (ik_type == IKT_1) {
        mat4_mul_translate(&mat, len[0][1], 0.0f, 0.0f, &mat);
        *node[2].mat_ptr = mat;

        if (rot) {
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

        if (rot) {
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

const vec3* RobBlock::set_global_leaf_sub(const vec3* val, BONE_KIND kind, bool get_data, bool flip_x) {
    if (ik_type == IKT_ROOT) {
        if (get_data) {
            leaf_pos[1] = *val;
            if (flip_x)
                leaf_pos[1].x = -leaf_pos[1].x;
        }
        val++;
    }
    else if (ik_type == IKT_1 || ik_type == IKT_2 || ik_type == IKT_2R) {
        if (get_data) {
            leaf_pos[0] = *val;
            if (flip_x)
                leaf_pos[0].x = -leaf_pos[0].x;
        }
        val++;
    }

    if (get_data) {
        if (ik_type == IKT_0N || ik_type == IKT_0T) {
            leaf_pos[1] = *val;
            if (flip_x)
                leaf_pos[1].x = -leaf_pos[1].x;
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

void RobBlock::get_smooth_target(int32_t target) {
    if (check_expression_id_not_null())
        return;

    switch (ik_type) {
    case IKT_0N:
    case IKT_0T:
    case IKT_ROOT:
        smooth_pos[target] = leaf_pos[1];
        break;
    }

    switch (ik_type) {
    case IKT_0:
        smooth_rot[0][target] = chain_rot[0];
        break;
    case IKT_0N:
    case IKT_0T:
    case IKT_ROOT:
        smooth_rot[0][target] = chain_rot[0];
        break;
    case IKT_1:
        smooth_rot[0][target] = chain_rot[0];
        smooth_rot[1][target] = chain_rot[1];
        break;
    case IKT_2:
    case IKT_2R:
        smooth_rot[0][target] = chain_rot[0];
        smooth_rot[1][target] = chain_rot[1];
        smooth_rot[2][target] = chain_rot[2];
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

static void RobSkinDisp_ctrl_step(RobSkinDisp* skin_disp, bool disable_ex_force) {
    for (int32_t i = 0; i < 6; i++)
        for (ExNodeBlock*& j : skin_disp->ex_node_block)
            j->CtrlStep(i, disable_ex_force);
}

static void RobSkinDisp_load_opd_data(RobSkinDisp* skin_disp) {
    if (!skin_disp->osage_blk.size() && !skin_disp->cloth.size())
        return;

#if OPD_PLAY_GEN
    RobDisp* rob_disp = (RobDisp*)skin_disp->rob_disp;
    size_t index = 0;
    for (opd_blend_data& i : rob_disp->opd_blend_data) {
#else
    const RobDisp* rob_disp = skin_disp->rob_disp;
    size_t index = 0;
    for (const opd_blend_data& i : rob_disp->opd_blend_data) {
#endif
        const float_t* opd_data = 0;
        uint32_t opd_count = 0;
        osage_play_data_manager_get_opd_file_data(skin_disp->obj_uid, i.motnum, opd_data, opd_count);
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

static void RobSkinDisp_ctrl(RobSkinDisp* skin_disp) {
    if (rob_skin_disp_disable_node_blocks)
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
        RobSkinDisp_load_opd_data(skin_disp);
        for (ExNodeBlock*& i : skin_disp->ex_node_block)
            i->CtrlOsagePlayData();
    }
    else {
        if (skin_disp->osage_depends_on_others)
            RobSkinDisp_ctrl_step(skin_disp, false);

        for (ExNodeBlock*& i : skin_disp->ex_node_block)
            i->ctrl();
    }

    for (ExNodeBlock*& i : skin_disp->ex_node_block)
        i->CtrlEnd();
}

static void RobDisp_ctrl(RobDisp* disp) {
    if (!osage_test_no_pause && !disp->disable_update)
        for (int32_t i = disp->disp_begin; i < disp->disp_max; i++)
            RobSkinDisp_ctrl(&disp->skin_disp[i]);
}

static void rob_chara_data_adjust_ctrl(rob_chara* rob_chr,
    rob_chara_data_adjust* adjust, rob_chara_data_adjust* adjust_prev) {
    float_t cycle = adjust->cycle;
    adjust->frame += rob_chr->rob_base.robmot.step.f;
    if (rob_chr->rob_base.motdata.next_type == MH_NEXT_REPEAT)
        cycle *= (60.0f / rob_chr->rob_base.motdata.frame);
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
        mat4_mul_rotate_y(&mat, rob_chr->rob_base.position.yang.get_rad(), &mat);
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

    adjust->transition_frame += rob_chr->rob_base.robmot.step.f;
}

static void RobSkinDisp_set_parts_ex_force(
    RobSkinDisp* skin_disp, const rob_osage_parts_bit& parts_bits,
    const vec3* ex_force, const float_t& force, const float_t& gain) {
    for (ExOsageBlock*& i : skin_disp->osage_blk)
        if (i->osage_work.CheckPartsBits(parts_bits)) {
            i->osage_work.SetNodesExternalForce(ex_force, gain);
            i->osage_work.SetNodesForce(force);
        }
}

static void RobDisp_set_parts_ex_force(RobDisp* rob_disp,
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

    RobSkinDisp_set_parts_ex_force(&rob_disp->skin_disp[rpk], parts_bits, ex_force, force, strength);
}

static void RobSkinDisp_set_ex_force(RobSkinDisp* skin_disp, const vec3& f) {
    for (ExOsageBlock*& i : skin_disp->osage_blk)
        i->set_ex_force(f);

    for (ExClothBlock*& i : skin_disp->cloth)
        i->set_ex_force(f);
}

static void RobDisp_set_ex_force(RobDisp* rob_disp, const vec3& f) {
    for (int32_t i = rob_disp->disp_begin; i < rob_disp->disp_max; i++)
        RobSkinDisp_set_ex_force(&rob_disp->skin_disp[i], f);
}

void rob_chara::adjust_ctrl() {
    RobDisp* disp = this->disp;

    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++) {
        rob_chara_data_adjust* parts_adjust = &rob_base.robmot.parts_adjust[i];
        if (parts_adjust->enable) {
            rob_chara_data_adjust_ctrl(this, parts_adjust, &rob_base.robmot.parts_adjust_prev[i]);
            rob_osage_parts parts = (rob_osage_parts)i;
            RobDisp_set_parts_ex_force(disp, parts,
                &parts_adjust->curr_ex_force, parts_adjust->curr_force, parts_adjust->curr_strength);
        }
    }

    rob_chara_data_adjust* adjust_global = &rob_base.robmot.adjust_global;
    if (adjust_global->enable) {
        rob_chara_data_adjust_ctrl(this, adjust_global, &rob_base.robmot.adjust_global_prev);
        RobDisp_set_ex_force(disp, adjust_global->curr_ex_force);
    }

    disp->parts_short = false;
    disp->parts_append = false;
    disp->parts_white_one_l = false;
}

static BONE_BLK get_lc_arm_block(int32_t index) {
    if (index >= 0 && index < 2)
        return lc_arm_block_tbl[index];
    return BLK_DUMMY;
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
    arm_adjust->frame += rob_chr->rob_base.robmot.step.f;

    rob_chara_bone_data_set_motion_arm_length(rob_chr->bone_data,
        get_lc_arm_block(index), arm_adjust->value);
}

void rob_chara::arm_adjust_ctrl() {
    for (int32_t i = 0; i < 2; i++)
        rob_chara_data_arm_adjust_ctrl(this, i, &rob_base.robmot.arm_adjust[i]);
}

void rob_chara::autoblink_disable() {
    rob_base.robmot.flag.u32 |= 0x020000;
}

void rob_chara::autoblink_enable() {
    rob_base.robmot.flag.u32 &= ~0x020000;
}

mat4* rob_chara::get_bone_data_mat(size_t index) {
    return rob_chara_bone_data_get_mat(bone_data, index);
}

uint32_t rob_chara::get_common_mot(MOTTABLE_TYPE mottbl_type) const {
    if (mottbl_type >= MTP_FACE_MOT_SLOT_1 && mottbl_type <= MTP_FACE_MOT_SLOT_10)
        return rob_init.face_mot_slot[mottbl_type - MTP_FACE_MOT_SLOT_1];
    else if (mottbl_type == MTP_EYES_BASE_MOT)
        return rob_base.robmot.num;
    else if (mottbl_type >= MTP_226 && mottbl_type <= MTP_235)
        return rob_init.face_mot_slot[mottbl_type - MTP_226];
    return get_common_rob_mot(chara_num, rob_base.action.kamae_type, mottbl_type);
}

const vec3* rob_chara::get_gpos() const  {
    return &rob_base.position.gpos;
}

float_t rob_chara::get_frame() const {
    return bone_data->get_frame();
}

float_t rob_chara::get_frame_max() const {
    return bone_data->get_frame_max();
}

object_info rob_chara::get_rob_data_face_object(int32_t index) {
    if (index >= 0 && index < 15)
        return rob_data->face_objects[index];
    return {};
}

float_t rob_chara::get_face_depth() const {
    return item.get_face_depth();
}

float_t rob_chara::get_pos_scale(ROB_COLLI_ID colli_id, vec3& center) {
    if (colli_id < 0 || colli_id >= ROB_COLLI_ID_MAX)
        return 0.0f;

    center = rob_base.collision.cb_hit[colli_id].ball.c;
    return rob_base.collision.cb_hit[colli_id].ball.r;
}

const RobInit* rob_chara::get_rob_init() const {
    return &rob_init;
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
    sleeve_adjust.block_vec = 0;
    sleeve_adjust.step = 1.0f;
}

static void sub_14041D6F0(rob_chara_bone_data* rob_bone_data, bool a2) {
    MotionSmooth*  v3 = &rob_bone_data->motion_loaded.front()->smooth;
    v3->root_xzpos = v3->root_ypos;
    v3->root_ypos = a2;
}

static void sub_14041D720(rob_chara_bone_data* a1, bool a2) {
    a1->motion_loaded.front()->smooth.root_xzpos = a2;
}

static float_t sub_14054FDE0(rob_chara* rob_chr) {
    if (rob_chr->rob_base.motdata.smooth_f_length >= 0.0f)
        return rob_chr->rob_base.motdata.smooth_f_length;
    else if (rob_chr->rob_base.flag.bit.ringout)
        return 10.0;
    else if ((rob_chr->rob_base.motdata.motkind_fix[MK_ATTACK])
        || (rob_chr->rob_base.motdata.motkind[MK_SYAGAMI_DASH]))
        return 5.0f;
    else if (rob_chr->rob_base.robmot.num == rob_chr->rob_base.robmot.old_num)
        return 4.0f;
    else if (rob_chr->rob_base.motdata.motkind[MK_YARARE]) {
        if (rob_chr->rob_base.flag.bit.old_fix_hara || rob_chr->rob_base.flag.bit.land)
            return 1.0f;
    }
    else {
        if (!rob_chr->rob_base.motdata.motkind[MK_GUARD]
            || !rob_chr->rob_base.motdata.motkind[MK_CHANGE])
            return 7.0f;
    }
    return 3.0f;
}

static float_t sub_14054FEE0(rob_chara* a1) {
    if (a1->rob_base.motdata.smooth_r_length >= 0.0f)
        return a1->rob_base.motdata.smooth_r_length;
    else if (a1->rob_base.motdata.motkind[MK_YARARE])
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
    MotionSmooth* v2 = &rob_bone_data->motion_loaded.front()->smooth;
    v2->field_8 = (uint8_t)(v2->field_0 & 1);
    if (a2)
        v2->field_0 |= 0x01;
    else
        v2->field_0 &= ~0x01;
}

static void sub_14041C680(rob_chara_bone_data* rob_bone_data, bool a2) {
    MotionSmooth* v2 = &rob_bone_data->motion_loaded.front()->smooth;
    if (a2)
        v2->field_0 |= 0x02;
    else
        v2->field_0 &= ~0x02;
}

static void sub_14041D2D0(rob_chara_bone_data* rob_bone_data, bool a2) {
    MotionSmooth* v2 = &rob_bone_data->motion_loaded.front()->smooth;
    if (a2)
        v2->field_0 |= 0x04;
    else
        v2->field_0 &= ~0x04;
}

static void sub_14041BC40(rob_chara_bone_data* rob_bone_data, bool a2) {
    MotionSmooth* v2 = &rob_bone_data->motion_loaded.front()->smooth;
    if (a2)
        v2->field_0 |= 0x08;
    else
        v2->field_0 &= ~0x08;
}

static void sub_14041D6C0(rob_chara_bone_data* rob_bone_data, bool a2) {
    MotionSmooth* v2 = &rob_bone_data->motion_loaded.front()->smooth;
    if (a2)
        v2->field_0 |= 0x10;
    else
        v2->field_0 &= ~0x10;
}

static void sub_14041D340(rob_chara_bone_data* rob_bone_data, bool a2) {
    MotionSmooth* v2 = &rob_bone_data->motion_loaded.front()->smooth;
    if (a2)
        v2->field_0 |= 0x20;
    else
        v2->field_0 &= ~0x20;
}

static void sub_140414F00(MotionSmooth* a1, float_t value) {
    a1->move_yang_bef = a1->move_yang;
    a1->move_yang = value;
}

static void sub_14041D270(rob_chara_bone_data* rob_bone_data, float_t value) {
    sub_140414F00(&rob_bone_data->motion_loaded.front()->smooth, value);
}

static void sub_14041D2A0(rob_chara_bone_data* rob_bone_data, float_t value) {
    rob_bone_data->motion_loaded.front()->smooth.move_yang_bef = value;
}

static void sub_140555B00(rob_chara* rob_chr, bool a2) {
    bool v3;
    if (!a2)
        v3 = 0;
    else if (rob_chr->rob_base.robmot.old_num == -1)
        v3 = 0;
    else {
        v3 = !rob_chr->rob_base.motdata.motkind[MK_YARARE]
            || !rob_chr->rob_base.motdata.motkind_fix[MK_AIR]
            || rob_chr->rob_base.motdata.motkind[MK_NAGERARE];
    }

    bool v6;
    int32_t v7;
    if (rob_chr->rob_base.motdata.end_style) {
        v6 = 0;
        v7 = 2;
    }
    else if (rob_chr->rob_base.motdata.motkind[MK_GUARD]) {
        v6 = 0;
        v7 = 2;
    }
    else {
        v6 = 1;
        v7 = 1;
    }

    if (rob_chr->rob_base.motdata.end_style != 3 && rob_chr->rob_base.action.kamae_type) {
        v6 = 0;
        v7 = 2;
    }

    float_t v8 = rob_chr->rob_base.robmot.frame.f;
    float_t v9 = 0.0f;
    float_t v10 = 0.0f;
    if (v3)
        v10 = sub_14054FDE0(rob_chr);

    float_t v11 = v8 + v10;
    if (v6)
        v9 = sub_14054FEE0(rob_chr);

    float_t v12 = rob_chr->rob_base.motdata.frame;
    float_t v13 = v12 - v9;
    if (v3 && v6 && (v13 < v11 || v13 <= v8)) {
        float_t v14 = rob_chr->rob_base.motdata.frame;
        float_t v16 = prj::floorf((v12 - v8) * 0.5f) + v8;
        if (v11 > v16) {
            float_t v17;
            if (v13 < v16) {
                v13 = v16;
                v17 = v16;
                v9 = v12 - v16;
            }
            else
                v17 = rob_chr->rob_base.motdata.frame - v9;
            v10 = v17 - v8;
        }
        else {
            v13 = v8 + v10;
            v9 = v12 - v11;
        }
    }

    rob_chr->bone_data->set_motion_blend_duration(v9, 1.0f, 1.0f);

    sub_14041D310(rob_chr->bone_data, v9, v13, v7);
    sub_14041D340(rob_chr->bone_data, rob_chr->rob_base.motdata.end_style_flag & 0x40);
    sub_14041D6C0(rob_chr->bone_data, rob_chr->rob_base.motdata.old_motkind[MK_NO_MOVE]
        && rob_chr->rob_base.motdata.motkind[MK_NO_TRANS]);
    sub_14041D6F0(rob_chr->bone_data, rob_chr->rob_base.motdata.motkind_fix[MK_Y_TRANS]
        || rob_chr->rob_base.motdata.motkind_fix[MK_Y_MOVE]);
    sub_14041D720(rob_chr->bone_data, rob_chr->rob_base.motdata.old_motkind_fix[MK_Y_TRANS]
        || rob_chr->rob_base.motdata.old_motkind_fix[MK_Y_MOVE]);
    sub_14041D2A0(rob_chr->bone_data, rob_chr->rob_base.motdata.old_mov_yang.get_rad());
    sub_14041D270(rob_chr->bone_data, rob_chr->rob_base.motdata.mov_yang.get_rad());
}

static void sub_14053A9C0(RobMotData* a1, rob_chara* rob_chr,
    uint32_t motnum, float_t frame_max, const motion_database* mot_db) {
    a1->mot = motnum;
    a1->main_mot_frame = 0.0f;
    a1->frame_max = frame_max;
    a1->follow1_frame = frame_max - 1.0f;
    a1->follow2_frame = frame_max - 1.0f;
    a1->stop_frame = frame_max - 1.0f;
    a1->land_frame = frame_max - 1.0f;

    const MhdData* mot = mothead_storage_get_mot_by_motion_id(motnum, mot_db);
    if (!mot)
        return;

    a1->play_prog = mot->pp_list;
    a1->play_prog_org = mot->pp_list;
    a1->mh_command = mot->cm_list;
    a1->mh_did = mot_db->get_motion_set_id_by_motion_id(motnum);

    a1->motkind_fix.init((uint32_t*)mot->mot_kind.mhk_kind);
    a1->motkind.init((uint32_t*)mot->mot_kind.mhk_kind);

    a1->motkind.reset(MK_ATTACK);
    if (a1->motkind_fix[MK_TURN])
        a1->mov_yang = (int16_t)0x8000;
    if (a1->motkind_fix[MK_DTURN])
        a1->dturn_yang = (int16_t)0x8000;

    a1->start_style = mot->start_style;
    a1->end_style = mot->end_style;

    if (mot->end_style & 0x40) {
        a1->end_style = mot->end_style & ~0x40;
        a1->end_style_flag |= 0x40;
    }

    a1->mhd_smd_exec(rob_chr, mot->mh_list);

    if (a1->next_type == MH_NEXT_REPEAT)
        a1->frame = frame_max - 1.0f;
    else
        a1->frame = a1->frame_max;

    if (a1->motkind[MK_DTURN])
        a1->motkind.set(MK_TURN);
    if (a1->motkind[MK_REVERSE_ATK_FRONT] && !rob_chr->rob_base.robinfo.en_flag.bit.en_at_front)
        a1->motkind.set(MK_PL_SYAGAMI);

    a1->ex_damage_all = 0;
    a1->mov_yang += a1->dturn_yang;
    a1->efc_yang += a1->dturn_yang;

    MhpList* data = mot->pp_list;
    if (!data)
        return;

    int32_t type = data->type;
    if (data->type < 0)
        return;

    while (type != MHP_NUM_11)
        if ((type = (data++)->type) < 0)
            return;

    const void* d = data->data;
    while (d) {
        a1->ex_damage_all += *(int16_t*)d;

        int32_t type = (data++)->type;
        if (type < 0)
            break;

        while (type != MHP_NUM_11)
            if ((type = (data++)->type) < 0)
                return;
        d = data->data;
    }
}

void rob_chara::set_item(ROB_PARTS_KIND rpk, object_info obj_uid,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    disp->set_item(rpk, obj_uid, bone_data, data, obj_db);
}

static void rob_chara_head_adjust(rob_chara* rob_chr);
static bool rob_chara_hands_adjust(rob_chara* rob_chr);
static bool sub_14053B580(rob_chara* rob_chr, MotLeafCtrlPart part);
static void sub_14053B260(rob_chara* rob_chr);

static bool sub_14053B530(rob_chara* rob_chr) {
    return sub_14053B580(rob_chr, LCPART_ASI_R) | sub_14053B580(rob_chr, LCPART_ASI_L);
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
    disp->pos_reset(0);
}

void rob_chara::rob_info_ctrl() {
    rob_base.robinfo.old_en_flag = rob_base.robinfo.en_flag;
    if (rob_base.collision.fld_on > 0.01f)
        rob_base.flag.bit.wall_moved_mot = 1;

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

    float_t left_hand_scale_default = rob_base.adjust.left_hand_scale_default;
    float_t right_hand_scale_default = rob_base.adjust.right_hand_scale_default;
    float_t left_hand_scale = rob_base.adjust.left_hand_scale;
    float_t right_hand_scale = rob_base.adjust.right_hand_scale;

    if (left_hand_scale_default > 0.0f)
        if (left_hand_scale <= 0.0f)
            left_hand_scale = rob_base.adjust.left_hand_scale_default;
        else
            left_hand_scale *= left_hand_scale_default;

    if (right_hand_scale_default > 0.0f)
        if (right_hand_scale <= 0.0f)
            right_hand_scale = rob_base.adjust.right_hand_scale_default;
        else
            right_hand_scale *= right_hand_scale_default;

    if (left_hand_scale > 0.0f)
        rob_chara_bone_data_set_left_hand_scale(bone_data, left_hand_scale);

    if (right_hand_scale > 0.0f)
        rob_chara_bone_data_set_right_hand_scale(bone_data, right_hand_scale);
}

bool rob_chara::replace_rob_motion(uint32_t motnum, float_t frame, 
    float_t blend_duration, bool blend, bool set_motion_reset_data, MotionBlendType blend_type,
    const bone_database* bone_data, const motion_database* mot_db) {
    if (!blend && rob_base.robmot.num == motnum)
        return false;

    if (blend_duration <= 0.0f)
        blend_type = MOTION_BLEND;
    set_rob_motion(motnum, false, frame, blend_type, bone_data, mot_db);
    this->bone_data->set_motion_blend_duration(blend_duration, 1.0f, 0.0f);
    rob_base.robmot.flag.bit.ext_frame_req = true;
    rob_base.robmot.frame.req_f = frame;
    set_motion_skin_param(motnum, (float_t)(int32_t)frame);
    if (blend_duration == 0.0f) {
        if (set_motion_reset_data)
            rob_chara::set_motion_reset_data(motnum, frame);

        disp->skin_disp[RPK_TE_L].init_cnt = 60;
        disp->skin_disp[RPK_TE_R].init_cnt = 60;

        if (check_for_ageageagain_module()) {
            rob_chara_age_age_array_set_skip(idnm, 1);
            rob_chara_age_age_array_set_skip(idnm, 2);
        }
    }
    return true;
}

// 0x140516790
void rob_chara::req_frame_change(float_t frame) {
    rob_base.robmot.flag.bit.ext_frame_req = true;
    rob_base.robmot.frame.req_f = frame;
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

void rob_chara::reset_rob(const RobInit& robinit,
    const bone_database* bone_data, const motion_database* mot_db) {
    int32_t drank_count = rob_base.action.drank_count;
    this->bone_data->reset();
    rob_chara_bone_data_init_data(this->bone_data, BONE_KIND_CMN, rob_data->bone, bone_data);
    rob_base.init();
    rob_base_old.init();
    disp->set_motion_node(rob_chara_bone_data_get_node(
        this->bone_data, BONE_NODE_N_HARA_CP), bone_data);
    type = robinit.rob_type;
    set_disp_flag(robinit.disp);
    energy = robinit.energy;
    frame_speed = 1.0f;

    RobBase* base = &rob_base;

    vec3 pos = robinit.pos;
    if (robinit.rob_type != ROB_TYPE_AUTH) {
        vec3 check_pnt = robinit.pos;
        check_pnt.y += 1.0f;
        //pos.y = field_get_hit_y(check_pnt, 0.0f);
    }

    base->position.pos = pos;
    base->position.gpos = pos;
    base->position.old_gpos = pos;
    base->position.yang = robinit.yang;
    base->position.hara_yang = robinit.yang;
    float_t scale = chara_size_table_get_value(rob_init.chara_size_index);
    base->adjust.scale = scale;
    base->adjust.item_scale = scale; // X
    base->adjust.height_adjust = rob_init.height_adjust;
    base->adjust.pos_adjust_y = chara_pos_adjust_y_table_get_value(rob_init.chara_size_index);
    base->action.kamae_data = rob_data->kamae_tbl[base->action.kamae_type];
    sub_14041C5C0(this->bone_data, sub_140510550(chara_num));
    rob_chara_bone_data_set_look_anim_param(this->bone_data,
        rob_chara_look_anim_eye_param_array_get(chara_num), &robinit.eyes_adjust);
    this->bone_data->sleeve_adjust.sleeve_l = robinit.sleeve_l;
    this->bone_data->sleeve_adjust.sleeve_r = robinit.sleeve_r;
    this->bone_data->sleeve_adjust.enable1 = true;
    this->bone_data->sleeve_adjust.enable2 = false;
    rob_chara_bone_data_get_adjust_scale(this->bone_data, bone_data);
    rob_chara_load_default_motion(this, bone_data, mot_db);
    base->action.action.name = ROB_ACT_MOTION;
    base->action.action.motnum = get_common_mot(MTP_NONE);
    base->action.step = 0;
    base->action.action.mirror = robinit.mirror;
    set_rob_motion(base->action.action.motnum, robinit.mirror, 0.0f, MOTION_BLEND, bone_data, mot_db);
    if (!robinit.drank_reset)
        base->action.drank_count = drank_count;
    reset_osage_pos();
    disp->hyoutan_status = HYOUTAN_STAT_NORMAL;
    disp->set_merge(RPK_ATAMA, false);
    disp->set_merge(RPK_TE_R, false);
    disp->set_merge(RPK_TE_L, false);
    object_info head_obj = rob_chara_get_head_object(this, 0);
    base->robmot.field_150.head_object = head_obj;
    base->robmot.field_3B0.head_object = head_obj;
    set_parts_disp(RPK_MAX, true);
}

void rob_chara::reset_osage() {
    for (rob_chara_data_adjust& i : rob_base.robmot.parts_adjust)
        i.reset();

    for (rob_chara_data_adjust& i : rob_base.robmot.parts_adjust_prev)
        i.reset();

    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        disp->reset_nodes_ex_force((rob_osage_parts)i);

    rob_base.robmot.adjust_global.reset();
    rob_base.robmot.adjust_global_prev.reset();

    disp->reset_ex_force();
    disp->parts_short = false;
    disp->parts_append = false;
    disp->parts_white_one_l = false;

    set_osage_step(-1.0f);
}

void rob_chara::reset_osage_pos() {
    disp_pos_reset = true;
}

void rob_chara::set_base(ROB_PARTS_KIND rpk, object_info obj_info, bool osage_reset,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    disp->set_base(rpk, obj_info, osage_reset, bone_data, data, obj_db);
}

void rob_chara::set_bone_data_frame(float_t frame) {
    bone_data->set_frame(frame);
    bone_data->interpolate();
    bone_data->update(0);
}

// 0x140506D20
void rob_chara::set_chara(ROB_ID id, CHARA_NUM cn, int32_t cos_id, const RobInit& ri) {
    idnm = (int8_t)id;
    chara_num = cn;
    this->cos_id = cos_id;
    rob_init = ri;
    rob_data = get_rob_data(cn);
    if (!check_cos_id_is_501(cos_id)) {
        const RobItemEquip* item_set = get_default_costume_data(cn, cos_id);
        if (item_set)
            RobItem::s_regist_item_all(cn, item_set, &item);
    }
    item.regist_item_all(&ri.item);
}

void rob_chara::set_chara_color(bool value) {
    disp->chara_color = value;
}

void rob_chara::set_chara_height_adjust(bool value) {
    rob_base.adjust.height_adjust = value;
}

void rob_chara::set_chara_pos_adjust(const vec3& value) {
    rob_base.adjust.pos_adjust = value;
}

void rob_chara::set_chara_pos_adjust_y(float_t value) {
    rob_base.adjust.pos_adjust_y = value;
}

void rob_chara::set_chara_size(float_t value) {
    rob_base.adjust.scale = value;
    rob_base.adjust.item_scale = value; // X
}

static void rob_chara_set_eyelid_motion(rob_chara* rob_chr,
    RobEyelidMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 1 || type == 2) {
        rob_chr->rob_base.robmot.field_3B0.eyelid.data = motion->data;
        if (!(rob_chr->rob_base.robmot.flag.u32 & 0x8000))
            return;
    }
    else {
        rob_chr->rob_base.robmot.field_150.eyelid.data = motion->data;
        if (rob_chr->rob_base.robmot.flag.u32 & 0x8000)
            return;
    }

    rob_chr->bone_data->load_eyelid_motion(motion->data.motnum, mot_db);
    rob_chr->bone_data->set_eyelid_frame(motion->data.frame);
    rob_chr->bone_data->set_eyelid_step(motion->data.step);
    rob_chr->bone_data->set_eyelid_blend_duration(
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_eyes_motion(rob_chara* rob_chr,
    RobEyesMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 1 || type == 2) {
        rob_chr->rob_base.robmot.field_3B0.eyes.data = motion->data;
        if (!(rob_chr->rob_base.robmot.flag.u32 & 0x4000))
            return;
    }
    else {
        rob_chr->rob_base.robmot.field_150.eyes.data = motion->data;
        if (rob_chr->rob_base.robmot.flag.u32 & 0x4000)
            return;
    }

    rob_chr->bone_data->load_eyes_motion(motion->data.motnum, mot_db);
    rob_chr->bone_data->set_eyes_frame(motion->data.frame);
    rob_chr->bone_data->set_eyes_step(motion->data.step);
    rob_chr->bone_data->set_eyes_blend_duration(
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_face_motion(rob_chara* rob_chr,
    RobFaceMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 2 || type == 1) {
        rob_chr->rob_base.robmot.field_3B0.face.data = motion->data;
        if (!(rob_chr->rob_base.robmot.flag.u32 & 0x0400))
            return;
    }
    else {
        rob_chr->rob_base.robmot.field_150.face.data = motion->data;
        if (rob_chr->rob_base.robmot.flag.u32 & 0x0400)
            return;
    }

    rob_chr->bone_data->load_face_motion(motion->data.motnum, mot_db);
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
        rob_chr->rob_base.robmot.field_150.hand_l.data = motion->data;
        if ((rob_chr->rob_base.robmot.flag.u32 & 0x0800) || (rob_chr->rob_base.robmot.flag.u32 & 0x040000))
            return;
        break;
    case 1:
        rob_chr->rob_base.robmot.hand_l.data = motion->data;
        if ((rob_chr->rob_base.robmot.flag.u32 & 0x0800) || !(rob_chr->rob_base.robmot.flag.u32 & 0x040000))
            return;
        break;
    case 2:
        rob_chr->rob_base.robmot.field_3B0.hand_l.data = motion->data;
        if (!(rob_chr->rob_base.robmot.flag.u32 & 0x0800) || (rob_chr->rob_base.robmot.flag.u32 & 0x040000))
            return;
        break;
    }

    rob_chr->bone_data->load_hand_l_motion(motion->data.motnum, mot_db);
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
        rob_chr->rob_base.robmot.field_150.hand_r.data = motion->data;
        if ((rob_chr->rob_base.robmot.flag.u32 & 0x1000) || (rob_chr->rob_base.robmot.flag.u32 & 0x080000))
            return;
        break;
    case 1:
        rob_chr->rob_base.robmot.hand_r.data = motion->data;
        if ((rob_chr->rob_base.robmot.flag.u32 & 0x1000) || !(rob_chr->rob_base.robmot.flag.u32 & 0x080000))
            return;
        break;
    case 2:
        rob_chr->rob_base.robmot.field_3B0.hand_r.data = motion->data;
        if (!(rob_chr->rob_base.robmot.flag.u32 & 0x1000) || (rob_chr->rob_base.robmot.flag.u32 & 0x080000))
            return;
        break;
    }

    rob_chr->bone_data->load_hand_r_motion(motion->data.motnum, mot_db);
    rob_chr->bone_data->set_hand_r_frame(motion->data.frame);
    rob_chr->bone_data->set_hand_r_step(motion->data.step);
    rob_chr->bone_data->set_hand_r_blend_duration(
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void rob_chara_set_mouth_motion(rob_chara* rob_chr,
    RobMouthMotion* motion, int32_t type, const motion_database* mot_db) {
    if (type == 1 || type == 2) {
        rob_chr->rob_base.robmot.field_3B0.mouth.data = motion->data;
        if (!(rob_chr->rob_base.robmot.flag.u32 & 0x2000))
            return;
    }
    else {
        rob_chr->rob_base.robmot.field_150.mouth.data = motion->data;
        if (rob_chr->rob_base.robmot.flag.u32 & 0x2000)
            return;
    }

    rob_chr->bone_data->load_mouth_motion(motion->data.motnum, mot_db);
    rob_chr->bone_data->set_mouth_frame(motion->data.frame);
    rob_chr->bone_data->set_mouth_step(motion->data.step);
    rob_chr->bone_data->set_mouth_blend_duration(
        motion->data.blend_duration, motion->data.blend_step, motion->data.blend_offset);
}

static void sub_1405500F0(rob_chara* rob_chr) {
    rob_chr->rob_base.robmot.flag.u32 &= ~0x0100;
    int32_t v1 = rob_chr->rob_base.robmot.field_150.field_1C0;
    if (v1 == 2 || v1 == 3)
        return;

    switch (rob_chr->rob_base.robmot.field_150.face.data.mottbl_type) {
    case MTP_FACE_RESET:
    case MTP_FACE_RESET_OLD:
    case MTP_FACE_SAD:
    case MTP_FACE_SAD_OLD:
    case MTP_FACE_SURPRISE:
    case MTP_FACE_SURPRISE_OLD:
    case MTP_FACE_ADMIRATION:
    case MTP_FACE_ADMIRATION_OLD:
    case MTP_FACE_SMILE:
    case MTP_FACE_SMILE_OLD:
    case MTP_FACE_SETTLED:
    case MTP_FACE_SETTLED_OLD:
    case MTP_FACE_DAZZLING:
    case MTP_FACE_DAZZLING_OLD:
    case MTP_FACE_LASCIVIOUS:
    case MTP_FACE_LASCIVIOUS_OLD:
    case MTP_FACE_STRONG:
    case MTP_FACE_STRONG_OLD:
    case MTP_FACE_CLARIFYING:
    case MTP_FACE_CLARIFYING_OLD:
    case MTP_FACE_GENTLE:
    case MTP_FACE_GENTLE_OLD:
    case MTP_FACE_NAGASI:
    case MTP_FACE_NAGASI_OLD:
    case MTP_FACE_KIRI:
    case MTP_FACE_KIRI_OLD:
    case MTP_FACE_UTURO:
    case MTP_FACE_UTURO_OLD:
    case MTP_FACE_OMOU:
    case MTP_FACE_OMOU_OLD:
    case MTP_FACE_SETUNA:
    case MTP_FACE_SETUNA_OLD:
    case MTP_FACE_GENKI:
    case MTP_FACE_GENKI_OLD:
    case MTP_FACE_YARU:
    case MTP_FACE_YARU_OLD:
    case MTP_FACE_COOL:
    case MTP_FACE_KOMARIWARAI:
    case MTP_FACE_KUMON:
    case MTP_FACE_KUTSUU:
    case MTP_FACE_NAKI:
    case MTP_FACE_NAYAMI:
    case MTP_FACE_SUPSERIOUS:
    case MTP_FACE_TSUYOKIWARAI:
        rob_chr->rob_base.robmot.flag.bit.suspend = 1;
        break;
    }
}

static vec3* rob_chara_bone_data_get_gblctr_pos(rob_chara_bone_data* rob_bone_data) {
    return &rob_bone_data->motion_loaded.front()->bone_data.gblctr_pos;
}

static void rob_chara_data_adjuct_set_pos(RobAdjust* rob_chr_adj,
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

void rob_chara::set_data_adjust_mat(RobAdjust* rob_chr_adj, bool pos_adjust) {
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

void rob_chara::set_disable_collision(rob_osage_parts parts, bool disable) {
    disp->set_disable_collision(parts, disable);
}

void rob_chara::set_disp_flag(bool flag) {
    if (flag) {
        rob_base.flag.bit.disp = 1;
        rob_base.flag.bit.yoketa = 1;
    }
    else {
        rob_base.flag.bit.disp = 0;
        rob_base.flag.bit.yoketa = 0;
    }

    if (check_for_ageageagain_module()) {
        rob_chara_age_age_array_set_disp(idnm, 1, flag);
        rob_chara_age_age_array_set_disp(idnm, 2, flag);
    }
}

void rob_chara::set_eyelid_mottbl_motion(int32_t type, MOTTABLE_TYPE mottbl_type,
    float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobEyelidMotion motion;
    motion.data.motnum = get_common_mot(mottbl_type);
    motion.data.mottbl_type = mottbl_type;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_ptr = &rob_base.robmot.frame;
        motion.data.step_ptr = &rob_base.robmot.step;
    }
    motion.data.loop_state = loop_state;
    if (motion.data.motnum != -1) {
        motion.data.frame_max = get_mot_frame_max(motion.data.motnum, mot_db);
        motion.data.frame = (motion.data.frame_max - 1.0f) * value;
    }

    if (!type) {
        rob_base.robmot.flag.u32 &= ~0x0200;
        rob_base.robmot.field_150.time = 0.0f;
    }
    rob_chara_set_eyelid_motion(this, &motion, type, mot_db);
}

void rob_chara::set_eyelid_mottbl_motion_from_face(int32_t a2,
    float_t blend_duration, float_t value, float_t blend_offset, const motion_database* mot_db) {
    if (!a2)
        a2 = rob_base.robmot.field_150.field_1C0;

    RobEyelidMotion motion;
    switch (a2) {
    case 0:
    case 2:
        motion.data = rob_base.robmot.field_150.face.data;
        break;
    case 1:
        value = 1.0f;
    case 3: {
        uint32_t mottbl_type = rob_base.robmot.field_150.face.data.mottbl_type;
        switch (mottbl_type) {
        case MTP_FACE_RESET:
        case MTP_FACE_RESET_OLD:
        case MTP_FACE_SAD:
        case MTP_FACE_SAD_OLD:
        case MTP_FACE_LAUGH:
        case MTP_FACE_LAUGH_OLD:
        case MTP_FACE_SURPRISE:
        case MTP_FACE_SURPRISE_OLD:
        case MTP_FACE_WINK_OLD:
        case MTP_FACE_ADMIRATION:
        case MTP_FACE_ADMIRATION_OLD:
        case MTP_FACE_SMILE:
        case MTP_FACE_SMILE_OLD:
        case MTP_FACE_SETTLED:
        case MTP_FACE_SETTLED_OLD:
        case MTP_FACE_DAZZLING:
        case MTP_FACE_DAZZLING_OLD:
        case MTP_FACE_LASCIVIOUS:
        case MTP_FACE_LASCIVIOUS_OLD:
        case MTP_FACE_STRONG:
        case MTP_FACE_STRONG_OLD:
        case MTP_FACE_CLARIFYING:
        case MTP_FACE_CLARIFYING_OLD:
        case MTP_FACE_GENTLE:
        case MTP_FACE_GENTLE_OLD:
        case MTP_FACE_CLOSE:
        case MTP_FACE_CLOSE_OLD:
        case MTP_FACE_NAGASI:
        case MTP_FACE_NAGASI_OLD:
        case MTP_FACE_KIRI:
        case MTP_FACE_KIRI_OLD:
        case MTP_FACE_UTURO:
        case MTP_FACE_UTURO_OLD:
        case MTP_FACE_OMOU:
        case MTP_FACE_OMOU_OLD:
        case MTP_FACE_SETUNA:
        case MTP_FACE_SETUNA_OLD:
        case MTP_FACE_GENKI:
        case MTP_FACE_GENKI_OLD:
        case MTP_FACE_YARU:
        case MTP_FACE_YARU_OLD:
        case MTP_FACE_COOL:
        case MTP_FACE_KOMARIWARAI:
        case MTP_FACE_KUMON:
        case MTP_FACE_KUTSUU:
        case MTP_FACE_NAKI:
        case MTP_FACE_NAYAMI:
        case MTP_FACE_SUPSERIOUS:
        case MTP_FACE_TSUYOKIWARAI:
        case MTP_FACE_WINK_L:
        case MTP_FACE_WINK_R:
        case MTP_FACE_WINKG_L:
        case MTP_FACE_WINKG_R:
        case MTP_FACE_WINK_FT_OLD:
        case MTP_FACE_NEW_IKARI_OLD:
        case MTP_FACE_EYEBROW_UP_RIGHT:
        case MTP_FACE_EYEBROW_UP_LEFT:
        case MTP_FACE_KOMARIEGAO:
        case MTP_FACE_KONWAKU:
            motion.data.mottbl_type = (MOTTABLE_TYPE)++mottbl_type;
            break;
        }

        uint32_t motnum = get_common_mot((MOTTABLE_TYPE)mottbl_type);
        motion.data.motnum = motnum;
        if (motnum == -1) {
            blend_duration = 0.0f;
            a2 = 0;
            motion.data = rob_base.robmot.field_150.face.data;
        }
        else if (value <= 0.0f) {
            a2 = 0;
            motion.data = rob_base.robmot.field_150.face.data;
        }
        else {
            motion.data.playback_state = ROB_PARTIAL_MOTION_PLAYBACK_STOP;
            motion.data.frame_max = get_mot_frame_max(motnum, mot_db);
            motion.data.frame = (motion.data.frame_max - 1.0f) * value;
        }
    } break;
    }
    motion.data.blend_offset = blend_offset;
    motion.data.blend_duration = blend_duration;

    rob_base.robmot.field_150.field_1C0 = a2;
    sub_1405500F0(this);
    rob_base.robmot.flag.u32 &= ~0x0200;
    rob_base.robmot.field_150.time = 0.0f;
    rob_chara_set_eyelid_motion(this, &motion, 0, mot_db);
}

void rob_chara::set_eyes_mottbl_motion(int32_t type, MOTTABLE_TYPE mottbl_type,
    float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobEyesMotion motion;
    motion.data.motnum = get_common_mot(mottbl_type);
    motion.data.mottbl_type = mottbl_type;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_ptr = &rob_base.robmot.frame;
        motion.data.step_ptr = &rob_base.robmot.step;
    }
    motion.data.loop_state = loop_state;
    if (motion.data.motnum != -1) {
        motion.data.frame_max = get_mot_frame_max(motion.data.motnum, mot_db);
        motion.data.frame = (motion.data.frame_max - 1.0f) * value;
    }
    rob_chara_set_eyes_motion(this, &motion, type, mot_db);
}

static void sub_140553970(rob_chara* rob_chr, object_info a2, int32_t type) {
    if (type != 1 && type == 2)
        rob_chr->rob_base.robmot.field_3B0.head_object = a2;
    else
        rob_chr->rob_base.robmot.field_150.head_object = a2;
}

void rob_chara::set_face_mottbl_motion(int32_t type, MOTTABLE_TYPE mottbl_type,
    float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, bool a11, const motion_database* mot_db) {
    RobFaceMotion motion;
    motion.data.motnum = get_common_mot(mottbl_type);
    motion.data.mottbl_type = mottbl_type;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_ptr = &rob_base.robmot.frame;
        motion.data.step_ptr = &rob_base.robmot.step;
    }
    motion.data.loop_state = loop_state;
    if (motion.data.motnum != -1) {
        motion.data.frame_max = get_mot_frame_max(motion.data.motnum, mot_db);
        motion.data.frame = (motion.data.frame_max - 1.0f) * value;
    }
    rob_chara_set_face_motion(this, &motion, type, mot_db);

    int32_t head_object_id = 0;
    switch (mottbl_type) {
    case MTP_FACE_CRY: // Crying (><)
        head_object_id = 1;
        break;
    case MTP_FACE_CRY_OLD:
        head_object_id = 7;
        break;
    case MTP_FACE_RESET1: // Crying (><) + Shock
        head_object_id = 2;
        break;
    case MTP_FACE_RESET2: // White-Eyed
        head_object_id = 3;
        break;
    case MTP_FACE_RESET3: // White-Eyed + Shock
        head_object_id = 4;
        break;
    case MTP_FACE_RESET4: // -.-
        head_object_id = 5;
        break;
    case MTP_FACE_RESET5: // -o-
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
    else if (rob_base.robmot.field_150.head_object != v17) {
        sub_140553970(this, v17, type);
        rob_base.robmot.field_150.eyes.data.blend_duration = 0.0f;
        rob_chara_set_eyes_motion(this, &rob_base.robmot.field_150.eyes, 0, mot_db);
        blend_duration = 0.0f;
    }

    if (type == 1 || type != 2) {
        if (a11)
            set_eyelid_mottbl_motion_from_face(2, blend_duration, -1.0f, blend_offset, mot_db);
        else if (rob_base.robmot.flag.u32 & 0x0200) {
            set_eyelid_mottbl_motion_from_face(
                rob_base.robmot.field_150.field_1C0, 3.0f, -1.0f, 0.0f, mot_db);
            rob_base.robmot.flag.u32 |= 0x0200;
        }
        else {
            float_t value = -1.0f;
            if (rob_base.robmot.field_150.field_1C0 == 3) {
                float_t frame_max = rob_base.robmot.field_150.eyelid.data.frame_max - 1.0f;
                if (frame_max > 0.0f)
                    value = rob_base.robmot.field_150.eyelid.data.frame / frame_max;

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
        rob_base.robmot.flag.u32 |= 0x8000;
        rob_chara_set_eyelid_motion(this, &_motion, 2, mot_db);
    }
}

void rob_chara::set_face_object(object_info obj_info, int32_t a3) {
    if (a3 == 2)
        rob_base.robmot.field_3B0.face_object = obj_info;
    else
        rob_base.robmot.field_150.face_object = obj_info;
}

void rob_chara::set_face_object_index(int32_t index) {
    set_face_object(get_rob_data_face_object(index), 0);
}

void rob_chara::set_frame(float_t frame) {
    bone_data->set_frame(frame);
    rob_base.robmot.frame.f = frame;
}

static void sub_140551870(rob_chara* rob_chr, float_t blend_duration,
    float_t blend_offset, const motion_database* aft_mot_db) {
    if (!(rob_chr->rob_base.robmot.flag.u32 & 0x040000))
        return;

    rob_chara_set_hand_l_object(rob_chr, {}, 1);
    rob_chr->rob_base.robmot.flag.u32 &= ~0x040000;

    RobHandMotion motion;
    motion.data = rob_chr->rob_base.robmot.field_150.hand_l.data;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    rob_chara_set_hand_l_motion(rob_chr, &motion, 0, aft_mot_db);
}

static void sub_140550B90(rob_chara* rob_chr, float_t blend_duration,
    float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->set_hand_l_mottbl_motion(1, MTP_HAND_NULL, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
    sub_140551870(rob_chr, blend_duration, blend_offset, aft_mot_db);
}

static void sub_140554690(rob_chara* rob_chr, MOTTABLE_TYPE mottbl_type,
    float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->rob_base.robmot.flag.u32 |= 0x040000;
    rob_chr->set_hand_l_mottbl_motion(1, mottbl_type, value, playback_state,
        blend_duration, play_duration, step, loop_state, blend_offset, aft_mot_db);
}

static void sub_1405519B0(rob_chara* rob_chr, float_t blend_duration,
    float_t blend_offset, const motion_database* aft_mot_db) {
    if (!(rob_chr->rob_base.robmot.flag.u32 & 0x080000))
        return;

    rob_chara_set_hand_r_object(rob_chr, {}, 1);
    rob_chr->rob_base.robmot.flag.u32 &= ~0x080000;

    RobHandMotion motion;
    motion.data = rob_chr->rob_base.robmot.field_150.hand_r.data;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    rob_chara_set_hand_r_motion(rob_chr, &motion, 0, aft_mot_db);
}

static void sub_140550C10(rob_chara* rob_chr, float_t blend_duration,
    float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->set_hand_r_mottbl_motion(1, MTP_HAND_NULL, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
    sub_1405519B0(rob_chr, blend_duration, blend_offset, aft_mot_db);
}
static void sub_140554710(rob_chara* rob_chr, MOTTABLE_TYPE mottbl_type, float_t value,
    rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration, float_t step,
    rob_partial_motion_loop_state loop_state, float_t blend_offset, const motion_database* aft_mot_db) {
    rob_chr->rob_base.robmot.flag.u32 |= 0x080000;
    rob_chr->set_hand_r_mottbl_motion(1, mottbl_type, value, playback_state,
        blend_duration, play_duration, step, loop_state, blend_offset, aft_mot_db);
}

void rob_chara::set_hand_item(int32_t uid, float_t blend_duration) {
    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    MOTTABLE_TYPE mottbl_type = MTP_HAND_NULL;
    object_info obj_left = {};
    object_info obj_right = {};
    float_t left_hand_scale = -1.0f;
    float_t right_hand_scale = -1.0f;

    const hand_item* hand_item = hand_item_handler_data_get_hand_item(uid, chara_num);
    if (hand_item) {
        if (hand_item->obj_left.not_null()) {
            left_hand_scale = hand_item->scale;
            obj_left = hand_item->obj_left;
        }

        if (hand_item->obj_right.not_null()) {
            right_hand_scale = hand_item->scale;
            obj_right = hand_item->obj_right;
        }

        mottbl_type = (MOTTABLE_TYPE)hand_item->mottbl_type;
    }

    if (obj_left.is_null())
        sub_140550B90(this, blend_duration, 0.0f, aft_mot_db);
    else {
        sub_140554690(this, mottbl_type, 1.0f, ROB_PARTIAL_MOTION_PLAYBACK_STOP, blend_duration,
            0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chara_set_hand_l_object(this, obj_left, 1);
    }

    if (obj_right.is_null())
        sub_140550C10(this, blend_duration, 0.0f, aft_mot_db);
    else {
        sub_140554710(this, mottbl_type, 1.0f, ROB_PARTIAL_MOTION_PLAYBACK_STOP, blend_duration,
            0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chara_set_hand_r_object(this, obj_right, 1);
    }

    rob_base.adjust.left_hand_scale_default = left_hand_scale;
    rob_base.adjust.right_hand_scale_default = right_hand_scale;
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
    MOTTABLE_TYPE mottbl_type, float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.motnum = get_common_mot(mottbl_type);
    motion.data.mottbl_type = mottbl_type;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_ptr = &rob_base.robmot.frame;
        motion.data.step_ptr = &rob_base.robmot.step;
    }
    motion.data.loop_state = loop_state;
    if (motion.data.motnum != -1) {
        motion.data.frame_max = get_mot_frame_max(motion.data.motnum, mot_db);
        motion.data.frame = (motion.data.frame_max - 1.0f) * value;
    }
    rob_chara_set_hand_l_motion(this, &motion, type, mot_db);
}

void rob_chara::set_hand_r_mottbl_motion(int32_t type,
    MOTTABLE_TYPE mottbl_type, float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.motnum = get_common_mot(mottbl_type);
    motion.data.mottbl_type = mottbl_type;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_ptr = &rob_base.robmot.frame;
        motion.data.step_ptr = &rob_base.robmot.step;
    }
    motion.data.loop_state = loop_state;
    if (motion.data.motnum != -1) {
        motion.data.frame_max = get_mot_frame_max(motion.data.motnum, mot_db);
        motion.data.frame = (motion.data.frame_max - 1.0f) * value;
    }
    rob_chara_set_hand_r_motion(this, &motion, type, mot_db);
}

void rob_chara::set_left_hand_scale(float_t value) {
    rob_base.adjust.left_hand_scale = value;
}

void rob_chara::set_look_camera(bool update_view_point, bool rotation_enable, float_t head_rot_strength,
    float_t eyes_rot_strength, float_t blend_duration, float_t eyes_rot_step, float_t a8, bool ft) {
    bone_data->set_look_anim(update_view_point, rotation_enable, head_rot_strength,
        eyes_rot_strength, blend_duration, eyes_rot_step, a8, ft);
    if (fabsf(blend_duration) <= 0.000001f)
        rob_base.flag.bit.dmy_yokerare = 1;
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

void rob_chara::set_motion_loop(float_t loop_begin, float_t loop_end, int32_t loop_count) {
    bone_data->set_motion_loop_state(MOT_PLAY_FRAME_DATA_LOOP_CONTINUOUS);
    bone_data->set_motion_loop(loop_begin, loop_count, loop_end);
    if (rob_base.motdata.next_type != MH_NEXT_REPEAT) {
        rob_base.motdata.next_type = MH_NEXT_REPEAT;
        rob_base.motdata.frame = rob_base.motdata.frame_max - 1.0f;
    }
}

void rob_chara::set_motion_reset_data(const uint32_t& motnum, const float_t& frame) {
    disp->set_motion_reset_data(motnum, frame);
}

void rob_chara::set_motion_skin_param(const uint32_t& motnum, const float_t& frame) {
    disp->set_motion_skin_param(idnm, motnum, (int32_t)prj::roundf(frame));
}

void rob_chara::set_motion_step(float_t value) {
    rob_base.robmot.step.req_f = value;
}

void rob_chara::set_mouth_mottbl_motion(int32_t type,
    MOTTABLE_TYPE mottbl_type, float_t value, rob_partial_motion_playback_state playback_state, float_t blend_duration,
    float_t play_duration, float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobMouthMotion motion;
    motion.data.motnum = get_common_mot(mottbl_type);
    motion.data.mottbl_type = mottbl_type;
    motion.data.playback_state = playback_state;
    motion.data.step = step;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    motion.data.play_duration = play_duration;
    if (motion.CheckPlaybackStateCharaMotion()) {
        motion.data.frame_ptr = &rob_base.robmot.frame;
        motion.data.step_ptr = &rob_base.robmot.step;
    }
    motion.data.loop_state = loop_state;
    if (motion.data.motnum != -1) {
        motion.data.frame_max = get_mot_frame_max(motion.data.motnum, mot_db);
        motion.data.frame = (motion.data.frame_max - 1.0f) * value;
    }
    rob_chara_set_mouth_motion(this, &motion, type, mot_db);
}

void rob_chara::set_osage_move_cancel(uint8_t id, float_t mv_ccl) {
    disp->set_osage_move_cancel(id, mv_ccl);
    if (id < 2) {
        rob_chara_age_age_array_set_move_cancel(idnm, 1, mv_ccl);
        rob_chara_age_age_array_set_move_cancel(idnm, 2, mv_ccl);
    }
}

void rob_chara::set_osage_reset() {
    disp->set_osage_reset();
}

void rob_chara::set_osage_step(float_t value) {
    rob_base.robmot.osage_step = value;
    if (value < 0.0f)
        value = rob_base.robmot.step.f;
    disp->set_osage_step(value);
}

void rob_chara::set_parts_disp(ROB_PARTS_KIND rpk, bool flag) {
    if (rpk < 0)
        return;
    else if (rpk < RPK_MAX)
        disp->set_disp_flag(rpk, flag);
    else if (rpk == RPK_MAX)
        for (int32_t i = RPK_DISP_BEGIN; i <= RPK_DISP_END; i++) {
            disp->set_disp_flag((ROB_PARTS_KIND)i, flag);
            if (i == RPK_ATAMA && check_for_ageageagain_module()) {
                rob_chara_age_age_array_set_disp(idnm, 1, flag);
                rob_chara_age_age_array_set_disp(idnm, 2, flag);
                rob_chara_age_age_array_set_skip(idnm, 1);
                rob_chara_age_age_array_set_skip(idnm, 2);
            }
        }
}

void rob_chara::set_right_hand_scale(float_t value) {
    rob_base.adjust.right_hand_scale = value;
}

void rob_chara::set_rob_motion(uint32_t motnum, bool mirror, float_t frame,
    MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db) {
    rob_base.position.yang = calc_mot_yang(false);
    if (!rob_base.motdata.motkind[MK_NO_MOVE]) {
        rob_base.position.pos.x = rob_base.position.gpos.x;
        rob_base.position.pos.z = rob_base.position.gpos.z;
    }

    if (rob_base.motdata.motkind[MK_Y_MOVE])
        rob_base.position.pos.y = rob_base.position.gpos.y;

    if (mirror)
        rob_base.robmot.flag.bit.next_mirror ^= true;

    if (rob_base.robmot.flag.bit.next_mirror)
        rob_base.robmot.flag.bit.mirror = true;
    else
        rob_base.robmot.flag.bit.mirror = false;

    rob_base.robmot.flag.bit.change = true;
    rob_base.robmot.flag.bit.frame_ctrl = false;
    rob_base.robmot.flag.bit.ext_frame_req = false;

    rob_base.robmot.repeat_count = 0;
    rob_base.robmot.shift_req.reset();

    rob_chara_data_adjust* parts_adjust = rob_base.robmot.parts_adjust;
    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++, parts_adjust++)
        parts_adjust->reset();

    rob_base.robmot.adjust_global.reset();

    rob_base.adjust.offset = 0.0f;
    rob_base.adjust.offset_x = true;
    rob_base.adjust.offset_y = false;
    rob_base.adjust.offset_z = true;
    rob_base.adjust.get_global_pos = false;

    rob_chara_data_arm_adjust* arm_adjust = rob_base.robmot.arm_adjust;
    rob_chara_data_hand_adjust* hand_adjust = rob_base.robmot.hand_adjust;
    rob_chara_data_hand_adjust* hand_adjust_prev = rob_base.robmot.hand_adjust_prev;
    for (int32_t i = 0; i < 2; i++, arm_adjust++, hand_adjust++, hand_adjust_prev++) {
        hand_adjust->reset();
        hand_adjust_prev->reset();
        arm_adjust->reset();
        rob_chara_bone_data_set_motion_arm_length(this->bone_data, get_lc_arm_block(i), 0.0f);
    }
    this->bone_data->set_disable_eye_motion(false);

    disp->set_osage_move_cancel(0, 0.0f);
    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        disp->set_disable_collision((rob_osage_parts)i, false);

    rob_base.motdata.old_motkind_fix = rob_base.motdata.motkind_fix;
    rob_base.motdata.old_motkind = rob_base.motdata.motkind;
    rob_base.motdata.old_mov_yang = rob_base.motdata.mov_yang;
    rob_base.motdata.clear();

    sub_14053A9C0(&rob_base.motdata, this, motnum,
        get_mot_frame_max(motnum, mot_db), mot_db);

    if (rob_base.motdata.old_motkind[MK_NEXT_TRANS])
        rob_base.motdata.motkind.set(MK_NO_TRANS);
    if (rob_base.motdata.old_motkind[MK_NEXT_Y_TRANS])
        rob_base.motdata.motkind.set(MK_Y_TRANS);

    if (!rob_base.motdata.motkind[MK_NO_MOVE] && frame > 0.0f)
        rob_base.motdata.motkind.set(MK_NO_TRANS);

    rob_base.robmot.old_num = rob_base.robmot.num;
    rob_base.robmot.frame.f = frame;
    rob_base.robmot.basic_step = BSTEP_BEGIN;
    rob_base.robmot.num = motnum;
    if (rob_base.robmot.step.req_f < 0.0f)
        rob_base.robmot.step.f = 1.0f;
    else
        rob_base.robmot.step.f = rob_base.robmot.step.req_f;

    float_t osage_step = rob_base.robmot.osage_step;
    if (osage_step < 0.0f)
        osage_step = rob_base.robmot.step.f;
    disp->set_osage_step(osage_step);

    rob_chara_bone_data_motion_load(this->bone_data, motnum, blend_type, bone_data, mot_db);
    this->bone_data->set_motion_frame(rob_base.robmot.step.f,
        rob_base.robmot.step.f, rob_base.motdata.frame_max);
    this->bone_data->set_motion_playback_state(MOT_PLAY_FRAME_DATA_PLAYBACK_FORWARD);
    if (rob_base.motdata.next_type == MH_NEXT_REPEAT) {
        this->bone_data->set_motion_loop_state(MOT_PLAY_FRAME_DATA_LOOP_CONTINUOUS);
        this->bone_data->set_motion_loop(rob_base.motdata.loop_begin,
            rob_base.motdata.next_limit, -1.0f);
    }
    rob_chara_bone_data_set_yrot(this->bone_data, rob_base.motdata.dturn_yang.get_rad());
    sub_14041BC40(this->bone_data, rob_base.motdata.motkind[MK_DMIRROR]);
    sub_14041B9D0(this->bone_data);
    if (type == ROB_TYPE_AUTH) {
        sub_14041D340(this->bone_data, !!(rob_base.motdata.end_style_flag & 0x40));
        sub_14041D6C0(this->bone_data, rob_base.motdata.old_motkind[MK_NO_MOVE]
            && rob_base.motdata.motkind[MK_NO_TRANS]);
        sub_14041D6F0(this->bone_data, rob_base.motdata.motkind_fix[MK_Y_TRANS]
            || rob_base.motdata.motkind_fix[MK_Y_MOVE]);
        sub_14041D720(this->bone_data, rob_base.motdata.motkind_fix[MK_Y_TRANS]
            || rob_base.motdata.motkind_fix[MK_Y_MOVE]);
        sub_14041D2A0(this->bone_data, rob_base.motdata.old_mov_yang.get_rad());
        sub_14041D270(this->bone_data, rob_base.motdata.mov_yang.get_rad());
    }
    else
        sub_140555B00(this, blend_type >= MOTION_BLEND_FREEZE && blend_type <= MOTION_BLEND_CROSS);
    rob_base.sound.blow_flag = false;
}

// 0x140516890
void rob_chara::set_rob_motion_external(uint32_t motnum) {
    if (rob_base.flag.bit.disp)
        set_rob_sys_command(ROB_ACT_MOTION, motnum, 0, false);
}

// 0x1405168C0
void rob_chara::set_rob_pos(const vec3& pos) {
    rob_base.position.pos = pos;
}

// 0x140555960
void rob_chara::set_rob_sys_command(ACT_NAME name, int32_t motion_id, const ActParam* param, bool mirror) {
    RobActType& sys_command = rob_base.action.sys_command;
    sys_command.name = name;
    if (param) {
        sys_command.act_param = *param;
        sys_command.motnum = motion_id;
        sys_command.mirror = mirror;
    }
    else {
        sys_command.act_param.init();
        sys_command.motnum = motion_id;
        sys_command.mirror = mirror;
    }
}

// 0x1405168E0
void rob_chara::set_rob_yang(const RobAngle& ang) {
    rob_base.position.yang = ang;
}

void rob_chara::set_shadow_cast(bool value) {
    if (value)
        disp->shadow_flag |= 0x04;
    else
        disp->shadow_flag &= ~0x04;
}

static void RobSkinDisp_set_ring(RobSkinDisp* skin_disp, const osage_ring_data& ring) {
    for (ExOsageBlock*& i : skin_disp->osage_blk)
        i->SetRing(ring);

    for (ExClothBlock*& i : skin_disp->cloth)
        i->SetRing(ring);
}

static void RobDisp_set_ring(RobDisp* disp, const osage_ring_data& ring) {
    for (int32_t i = disp->disp_begin; i < disp->disp_max; i++)
        RobSkinDisp_set_ring(&disp->skin_disp[i], ring);
}

void rob_chara::set_stage_data_ring(const int32_t& stage_index) {
    osage_ring_data ring;
    if (stage_index != -1)
        stage_param_data_coli_data_get_stage_index_data(stage_index, &ring);
    RobDisp_set_ring(disp, ring);
}

void rob_chara::set_step_motion_step(float_t value) {
    rob_base.robmot.step.req_f = value;
    if (value < 0.0f)
        rob_base.robmot.step.f = 1.0f;
    else
        rob_base.robmot.step.f = value;

    rob_chara_bone_data_set_step(bone_data, rob_base.robmot.step.f);

    float_t osage_step = rob_base.robmot.osage_step;
    if (osage_step < 0.0f)
        osage_step = rob_base.robmot.step.f;
    disp->set_osage_step(osage_step);
}

void rob_chara::set_use_opd(bool value) {
    disp->use_opd = value;
}

void rob_chara::set_wind_strength(float_t value) {
    disp->wind_strength = value;
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
    switch (rob_chr->rob_base.motdata.next_type) {
    case MH_NEXT_NONE: {
        rob_chr->rob_base.motdata.motkind.reset(MK_CHANGE);
        rob_chr->rob_base.motdata.motkind.reset(MK_JUMP);
        rob_chr->rob_base.robmot.frame.f = rob_chr->rob_base.motdata.frame;
    } break;
    case MH_NEXT_MOTION: {
        uint32_t mot_id = rob_chr->rob_base.motdata.next.mot_id;
        if (mot_id == -1) {
            rob_chr->rob_base.motdata.motkind.reset(MK_CHANGE);
            rob_chr->rob_base.motdata.motkind.reset(MK_JUMP);
        }
        else
            rob_chr->set_rob_motion(mot_id, !!(rob_chr->rob_base.motdata.next.mot_flag & 0x01),
                0.0f, MOTION_BLEND, bone_data, mot_db);
    } break;
    case MH_NEXT_REPEAT: {
        RobMotData& motdata = rob_chr->rob_base.motdata;
        int32_t next_limit = rob_chr->rob_base.motdata.next_limit;
        if (next_limit > 0) {
            uint32_t mot_id = rob_chr->rob_base.motdata.next_end.mot_id;
            if (mot_id != -1) {
                if (rob_chr->rob_base.robmot.repeat_count >= next_limit) {
                    prj::BitArray<127> v6(mothead_storage_get_mot_by_motion_id(mot_id, mot_db)->mot_kind.mhk_kind);
                    if (!v6[MK_NAGE] && !v6[MK_NAGERARE])
                        rob_chr->set_rob_motion(mot_id, !!(rob_chr->rob_base.motdata.next_end.mot_flag & 0x01),
                            0.0f, MOTION_BLEND, bone_data, mot_db);
                }
                else if (motdata.motkind[MK_NAGE]) {
                    rob_chara* enemy = rob_chr->rob_base.action.nage.enemy;
                    rob_chr->rob_base.action.nage.kaeshi_damage = motdata.nage_damage;
                    if (enemy)
                        enemy->rob_base.action.nage.kaeshi_damage = motdata.nage_damage;
                }
                else if (rob_chr->rob_base.motdata.motkind[MK_NAGERARE])
                    rob_chr->rob_base.action.step = 1;
            }
        }

        rob_chr->rob_base.robmot.frame.f = rob_chr->bone_data->get_frame();
        motdata.play_prog = motdata.play_prog_org;
        rob_chr->rob_base.robmot.repeat_count++;
        return;
    } break;
    case MH_NEXT_KAMAE:
    case MH_NEXT_MAKE:
    case MH_NEXT_DOWN_POSE: {
        rob_chr->set_rob_motion(rob_chr->get_common_mot(MTP_NONE),
            false, 0.0f, MOTION_BLEND, bone_data, mot_db);
    } break;
    }
}

static void sub_140505980(rob_chara* rob_chr) {
    if (rob_chr->rob_base.robmot.basic_step) {
        if (rob_chr->rob_base.robmot.basic_step != BSTEP_MAIN) {
            if (rob_chr->rob_base.robmot.basic_step != BSTEP_FOLLOW1)
                return;
            goto LABEL_23;
        }
    }
    else {
        if (rob_chr->rob_base.motdata.main_mot_frame > rob_chr->rob_base.robmot.frame.f)
            return;

        if (rob_chr->rob_base.motdata.motkind_fix[MK_ATTACK] ) {
            rob_chr->rob_base.motdata.motkind.set(MK_ATTACK);
            if (rob_chr->rob_base.motdata.motkind_fix[MK_RISE])
                rob_chr->rob_base.motdata.motkind.reset(MK_RISE);
        }
        rob_chr->rob_base.motdata.motkind.reset(MK_BEGIN);
        rob_chr->rob_base.robmot.basic_step = BSTEP_MAIN;
    }

    if (rob_chr->rob_base.motdata.follow1_frame > rob_chr->rob_base.robmot.frame.f)
        return;

    rob_chr->rob_base.motdata.motkind.set(MK_FOLLOW);
    rob_chr->rob_base.motdata.motkind.reset(MK_ATTACK_FOLLOW);
    if (rob_chr->rob_base.motdata.motkind[MK_ATTACK]) {
        rob_chr->rob_base.motdata.motkind.reset(MK_ATTACK);
        rob_chr->rob_base.motdata.attack_kind.reset(ATK_HIT_OK);
        if (rob_chr->rob_base.motdata.attack_kind[ATK_GUARD])
            rob_chr->rob_base.motdata.motkind.reset(MK_GUARD);
    }
    else if (rob_chr->rob_base.motdata.motkind[MK_YOKE]
        && rob_chr->rob_base.motdata.motkind[MK_CHANGE]
        && !rob_chr->rob_base.motdata.motkind[MK_YOKE_R]
        && !rob_chr->rob_base.motdata.motkind[MK_YOKE_L])
        rob_chr->rob_base.motdata.motkind.reset(MK_CHANGE);

    if (rob_chr->rob_base.motdata.motkind_fix[MK_UKEMI]
        && rob_chr->rob_base.motdata.motkind_fix[MK_RISE])
        rob_chr->rob_base.motdata.motkind.reset(MK_RISE);
    rob_chr->rob_base.robmot.basic_step = BSTEP_FOLLOW1;

LABEL_23:
    float_t follow2_frame = rob_chr->rob_base.motdata.follow2_frame;
    if (follow2_frame >= 0.0f && follow2_frame <= rob_chr->rob_base.robmot.frame.f) {
        if (follow2_frame < (rob_chr->rob_base.motdata.frame - 1.0f)
            || rob_chr->rob_base.motdata.next_type == MH_NEXT_NONE
            || rob_chr->rob_base.motdata.next_type == MH_NEXT_KAMAE) {
            rob_chr->rob_base.motdata.motkind.reset(MK_CHANGE);
            rob_chr->rob_base.motdata.motkind.reset(MK_JUMP);
            rob_chr->rob_base.motdata.motkind.reset(MK_RISE);
            rob_chr->rob_base.motdata.motkind.reset(MK_YOKE);
            rob_chr->rob_base.flag.bit.mk_change_off = true;
        }
        rob_chr->rob_base.robmot.basic_step = BSTEP_FOLLOW2;
    }
}

// 0x1405044B0
void rob_chara::calc_mot_adjust_scale() {
    if (rob_base.motdata.arm_adjust_duration > 0) { // X
        float_t blend = 1.0f;
        if (fabsf(rob_base.motdata.arm_adjust_duration) > 0.000001f)
            blend = (rob_base.robmot.frame.f
                - (float_t)rob_base.motdata.arm_adjust_start_frame)
                / rob_base.motdata.arm_adjust_duration;

        blend = clamp_def(blend, 0.0f, 1.0f);
        if (fabsf(blend - 1.0f) <= 0.000001f)
            rob_base.motdata.arm_adjust_duration = -1.0f;
        rob_base.arm_adjust_scale = lerp_def(rob_base.motdata.arm_adjust_prev_value,
            rob_base.motdata.arm_adjust_next_value, blend);

        float_t default_scale = chara_size_table_get_value(1);
        rob_base.adjust.item_scale = default_scale
            + (rob_base.adjust.scale - default_scale) * rob_base.arm_adjust_scale;
    }

    float_t v2 = bone_data->adjust_scale.base;
    float_t v4 = v2;
    if (rob_base.motdata.mot_adjust_type == MA_TYPE_NONE) {
        rob_base.robmot.mot_adjust_scale = v2;
        rob_base.robmot.mot_xz_adjust_scale = v2;
        rob_base.robmot.mot_adjust_base_pos = 0.0f;
        return;
    }

    float_t v5 = rob_base.motdata.pre_adjust_scale;
    float_t v6 = v2;
    float_t v7 = rob_base.motdata.pre_adjust_base_ypos;
    float_t v8 = 0.0f;
    if (rob_base.motdata.mot_adjust_flag == 1) {
        switch (rob_base.motdata.mot_adjust_type) {
        case MA_TYPE_COMMON:
            v6 = 1.0f;
            break;
        case MA_TYPE_SCALE:
            v6 = bone_data->adjust_scale.base;
        case MA_TYPE_BODY:
            v6 = bone_data->adjust_scale.body;
        case MA_TYPE_ARM:
            v6 = bone_data->adjust_scale.arm;
        case MA_TYPE_HEIGHT:
            v6 = bone_data->adjust_scale.height;
        case MA_TYPE_EMY_SCALE:
            if (!enemy)
                break;
            v6 = enemy->bone_data->adjust_scale.base;
            if (bone_data->motion_loaded.front()->mot_key_data.motion_body_type == 1)
                v6 *= v4;
        case MA_TYPE_EMY_BODY:
            if (!enemy)
                break;
            v6 = enemy->bone_data->adjust_scale.body;
            if (bone_data->motion_loaded.front()->mot_key_data.motion_body_type == 1)
                v6 *= v4;
        case MA_TYPE_EMY_ARM:
            if (!enemy)
                break;
            v6 = enemy->bone_data->adjust_scale.arm;
            if (bone_data->motion_loaded.front()->mot_key_data.motion_body_type == 1)
                v6 *= v4;
        case MA_TYPE_EMY_HEIGHT:
            if (!enemy)
                break;
            v6 = enemy->bone_data->adjust_scale.height;
            if (bone_data->motion_loaded.front()->mot_key_data.motion_body_type == 1)
                v6 *= v4;
            break;
        case MA_TYPE_DIRECT:
            if (rob_base.motdata.mot_adjust_scale > 0.0f)
                v6 = rob_base.motdata.mot_adjust_scale;
            break;
        }
        v8 = rob_base.motdata.mot_adjust_base_ypos;
    }

    float_t v15 = rob_base.robmot.frame.f;
    float_t v16 = rob_base.motdata.mot_adjust_end;
    if (v15 < v16) {
        float_t v17 = rob_base.motdata.mot_adjust_start;
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

    rob_base.robmot.mot_adjust_scale = v6;
    rob_base.robmot.mot_xz_adjust_scale = v4;
    rob_base.robmot.mot_adjust_base_pos = { 0.0f, v8, 0.0f };
}

// 0x14054FE90
RobAngle rob_chara::calc_mot_yang(bool compel_flag) const  {
    RobAngle ang = rob_base.position.yang;
    if (!compel_flag || !rob_base.flag.bit.compel) {
        RobAngle yarare_yang = rob_base.action.yarare.yarare_yang;
        if (yarare_yang.value)
            return ang + yarare_yang;
    }

    if (rob_base.motdata.motkind[MK_TURN])
        return ang + rob_base.motdata.mov_yang;
    return ang;
}

bool rob_chara::check_disp_left() const  {
    return rob_base.robinfo.flag.bit.disp_left;
}

void rob_chara::sub_1405070E0(const bone_database* bone_data, const motion_database* mot_db) {
    if (sub_140419E90(this->bone_data) || this->bone_data->get_motion_has_looped())
        sub_140505310(this, bone_data, mot_db);
    sub_140505980(this);
    rob_base.motdata.mhd_pp_exec(this, rob_base.robmot.frame.f, mot_db);
    calc_mot_adjust_scale();
}

void rob_chara::sub_140509D30() {
    prj::BallCollision* cb_hit = rob_base.collision.cb_hit;
    prj::BallCollision* cb_rob = rob_base.collision.cb_rob;
    prj::BallCollision* cb_stg = rob_base.collision.cb_stg;
    mat4* mat = rob_base.collision.mat;
    mat4* push_mat = rob_base.collision.push_mat;

    const RobCollisionData* colli = rob_data->colli_data;
    const RobCollisionData* push_colli = rob_data->push_colli_data;
    mat4 m;
    for (int32_t i = 0; i < ROB_COLLI_ID_MAX; i++) {
        mat4_mul_translate(bone_data->get_mats_mat(colli->bone), &colli->trans, &m);
        *mat = m;

        mat4_mul_translate(bone_data->get_mats_mat(push_colli->bone), &push_colli->trans, &m);
        *push_mat = m;

        float_t scale = rob_base.adjust.scale;

        vec3 v23;
        mat4_get_translation(mat, &v23);
        v23 = v23 * scale + rob_base.adjust.pos;
        mat4_set_translation(mat, &v23);

        vec3 v29;
        mat4_get_translation(push_mat, &v29);
        v29 = v29 * scale + rob_base.adjust.pos;
        mat4_set_translation(push_mat, &v29);

        float_t v20 = colli->scale * scale;
        cb_hit->ball.r = v20;
        cb_hit->org_rad = v20;
        cb_hit->old_c = cb_hit->ball.c;
        cb_hit->ball.c = v23;

        float_t v19 = push_colli->scale * scale;
        cb_rob->ball.r = v19;
        cb_rob->org_rad = v19;
        cb_rob->old_c = cb_rob->ball.c;
        cb_rob->ball.c = v29;

        cb_stg->ball.r = v19;
        cb_stg->org_rad = v19;
        cb_stg->old_c = cb_stg->ball.c;
        cb_stg->ball.c = v29;

        cb_hit++;
        cb_rob++;
        cb_stg++;
        mat++;
        push_mat++;
        colli++;
        push_colli++;
    }
}

void rob_chara::sub_1405163C0(int32_t index, mat4& mat) {
    if (index >= 0 && index <= 26)
        mat = rob_base.collision.mat[index];
}

void rob_chara::sub_140551000() {
    RobMotData* v1 = &rob_base.motdata;
    v1->play_prog = v1->play_prog_org;
}

RobManagement::RobManagement() {
    if (instance_count <= 0) {
        instance_count++;
        init();
    }
}

RobManagement::~RobManagement() {
    instance_count--;
    init();
}

bool RobManagement::check_alive_rob(ROB_ID id) {
    return !rob_impl[id].rob_base.robinfo.flag.bit.death && !rob_impl[id].rob_base.flag.bit.ringout;
}

bool RobManagement::check_disp_left(ROB_ID id) {
    return rob_impl[id].check_disp_left();
}

bool RobManagement::check_ringout_only_rob(ROB_ID id) {
    return rob_impl[id].rob_base.flag.bit.ringout;
}

bool RobManagement::check_ringout_transit_rob(ROB_ID id) {
    RobBase& rob_base = rob_impl[id].rob_base;
    return rob_base.ringout.stand
        || rob_base.ringout.fall
        || rob_base.ringout.stop
        || rob_base.ringout.air
        || rob_base.ringout.down
        || rob_base.ringout.replay_ringout;
}

// 0x140531670
ROB_ID RobManagement::create_rob(CHARA_NUM cn,
    const RobInit& ri, int32_t cos_id, bool can_set_default) {
    if (!task_rob_manager->check_alive()
        || ri.rob_type < 0 || ri.rob_type >= ROB_TYPE_MAX)
        return ROB_ID_NULL;

    int32_t free_id = 0;
    while (rob_init[free_id].rob_type != ROB_TYPE_NULL) {
        free_id++;
        if (free_id >= ROB_ID_MAX)
            return ROB_ID_NULL;
    }

    ROB_ID id = (ROB_ID)free_id;
    if (can_set_default && (cos_id < 0 || cos_id > 498))
        cos_id = 0;
    rob_init[id] = ri;
    chara_num[id] = cn;
    rob_impl[id].set_chara(id, cn, cos_id, ri);
    task_rob_manager->AppendInitCharaList(&rob_impl[id]);
    return id;
}

// 0x140531C50
void RobManagement::dest_all() {
    for (int32_t i = 0; i < ROB_ID_MAX; i++)
        if (rob_impl[i].type != ROB_TYPE_NULL) {
            task_rob_manager->AppendFreeCharaList(&rob_impl[i]);
            rob_impl[i].type = ROB_TYPE_NULL;
        }
}

// 0x140531CC0
void RobManagement::dest_rob(ROB_ID id) {
    if (id < 0 || id >= ROB_TYPE_MAX)
        return;

    task_rob_manager->AppendFreeCharaList(&rob_impl[id]);
    rob_init[id].rob_type = ROB_TYPE_NULL;
}

float_t RobManagement::get_adjust_scale(ROB_ID id) const {
    if (is_init(id))
        return rob_impl[id].rob_base.adjust.scale;
    return 1.0f;
}

// 0x140531F10
CHARA_NUM RobManagement::get_chara(ROB_ID id) const {
    return rob_impl[id].chara_num;
}

bool RobManagement::get_colli_check_on() const {
    return colli_check_on;
}

// 0x140531F50 
bool RobManagement::get_disp_on(ROB_ID id) const  {
    if (is_init(id))
        return rob_impl[id].rob_base.flag.bit.disp;
    return false;
}

// 0x140532010
const vec3* RobManagement::get_gpos(ROB_ID id) const  {
    return rob_impl[id].get_gpos();
}

// 0x140532030
rob_chara* RobManagement::get_rob(ROB_ID id) {
    if (is_init(id))
        return &rob_impl[id];
    return 0;
}

// 0x1405320A0
const RobData* RobManagement::get_rob_data(ROB_ID id) const {
    return rob_impl[id].rob_data;
}

// 0x1405320C0
/*RobDetail* RobManagement::get_rob_detail_if(ROB_ID id) {
    return rob_impl[id].rob_detail;
}*/

// 0x1405320F0
rob_chara_bone_data* RobManagement::get_rob_motion_work(ROB_ID id) {
    return rob_impl[id].bone_data;
}

// 0x140532110
size_t RobManagement::get_rob_num() const {
    return ROB_ID_MAX;
}

// 0x140532120
RobDisp* RobManagement::get_rob_robdisp_work(ROB_ID id) {
    return rob_impl[id].disp;
}

// 0x140532140
RobItem* RobManagement::get_rob_robitem_work(ROB_ID id) {
    return &rob_impl[id].item;
}

// 0x140532170
const RobInit* RobManagement::get_rob_init(ROB_ID id) const {
    if (is_init(id))
        return rob_impl[id].get_rob_init();
    return 0;
}

// 0x1405327B0
bool RobManagement::is_init(ROB_ID id) const  {
    return rob_init[id].rob_type != ROB_TYPE_NULL;
}

// 0x140532D40
void RobManagement::reset_osage_pos(ROB_ID id) {
    rob_impl[id].reset_osage_pos();
}

// 0x140532D60
void RobManagement::reset_rob(const RobInit& rob0, const RobInit& rob1,
    const bone_database* bone_data, const motion_database* mot_db) {
    if (!get_disp_on(ROB_ID_1P) || !get_disp_on(ROB_ID_2P))
        set_colli_check_on(false);
    else
        set_colli_check_on(true);
    rob_impl[ROB_ID_1P].reset_rob(rob0, bone_data, mot_db);
    rob_impl[ROB_ID_2P].reset_rob(rob1, bone_data, mot_db);
}

// 0x140532DB0
void RobManagement::reset_rob(ROB_ID id, const RobInit& ri,
    const bone_database* bone_data, const motion_database* mot_db) {
    rob_impl[id].reset_rob(ri, bone_data, mot_db);
    if (!get_disp_on(ROB_ID_1P) || !get_disp_on(ROB_ID_2P))
        set_colli_check_on(false);
    else
        set_colli_check_on(true);
}

void RobManagement::set_colli_check_on(bool value) {
    colli_check_on = value;
}

// 0x140532F80
void RobManagement::set_disp_on(ROB_ID id, bool value) {
    rob_impl[id].set_disp_flag(value);
}

// 0x140533130
void RobManagement::set_mot_frame(ROB_ID id, float_t frame) {
    rob_impl[id]. req_frame_change(frame);
}

// 0x140533150
void RobManagement::set_motion(ROB_ID id, int32_t motnum) {
    rob_impl[id].set_rob_motion_external(motnum);
}

// 0x1405331F0
void RobManagement::set_no_ctrl(ROB_ID id) {
    if (id >= 0 && id < ROB_ID_MAX)
        rob_impl[id].rob_base.flag.bit.no_ctrl = true;
}

// 0x140533170
void RobManagement::set_rob_pos(ROB_ID id, const vec3& pos) {
    rob_impl[id].set_rob_pos(pos);
}

// 0x1405331C0
void RobManagement::set_rob_yang(ROB_ID id, const RobAngle& ang) {
    rob_impl[id].set_rob_yang(ang);
}

// 0x140531430
void RobManagement::init() {
    for (int32_t i = 0; i < ROB_ID_MAX; i++)
        rob_init[i].rob_type = ROB_TYPE_NULL;
}

pv_data_set_motion::pv_data_set_motion() : frame_stage_index() {
    motnum = -1;
}

pv_data_set_motion::pv_data_set_motion(uint32_t motnum) : frame_stage_index() {
    this->motnum = motnum;
}

pv_data_set_motion::pv_data_set_motion(uint32_t motnum, std::pair<float_t, int32_t> frame_stage_index) {
    this->motnum = motnum;
    this->frame_stage_index = frame_stage_index;
}

constexpr bool operator==(const pv_data_set_motion& left, const pv_data_set_motion& right) {
    return left.motnum == right.motnum && left.frame_stage_index == right.frame_stage_index;
}

constexpr bool operator!=(const pv_data_set_motion& left, const pv_data_set_motion& right) {
    return !(left == right);
}

constexpr bool operator<(const pv_data_set_motion& left, const pv_data_set_motion& right) {
    return left.motnum < right.motnum || (!(right.motnum < left.motnum)
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
    motnum = -1;
    frame = -1;
}

osage_init_data::osage_init_data(rob_chara* rob_chr, uint32_t motnum) {
    this->rob_chr = rob_chr;
    pv_id = -1;
    this->motnum = motnum;
    frame = -1;
}

osage_init_data::osage_init_data(rob_chara* rob_chr, int32_t pv_id,
    uint32_t motnum, const std::string& dir, int32_t frame) {
    this->rob_chr = rob_chr;
    this->pv_id = pv_id;
    this->motnum = motnum;
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
    const std::vector<BODYTYPE>* body_type_table
        = bone_data->get_body_type_table(rob_bone_data->kind);
    const std::vector<CHAINPOSRADIUS>* joint_table
        = bone_data->get_joint_table(rob_bone_data->kind);
    const std::vector<CHAINPOSRADIUS>* disp_joint_table
        = bone_data->get_joint_table(rob_bone_data->disp_kind);
    if (!body_type_table || !joint_table || !disp_joint_table)
        return;

    bone->rob_bone_data = rob_bone_data;
    bone_data_parent_load_rob_chara(bone);
    bone->ik_init(body_type_table, joint_table->data(), disp_joint_table->data());
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

    BONE_KIND kind = a1->rob_bone_data->kind;
    const std::string* bone_name = mot_db->bone_name.data();

    const mot_bone_info* bone_info = a2->bone_info_array;
    for (size_t key_set_offset = 0, i = 0; key_set_offset < key_set_count; i++) {
        BONE_BLK blk = (BONE_BLK)bone_data->get_block_index(
            kind, bone_name[bone_info[i].index].c_str());
        if (blk == -1) {
            i++;
            blk = (BONE_BLK)bone_data->get_block_index(
                kind, bone_name[bone_info[i].index].c_str());
            if (blk == -1)
                break;
        }
        bone_indices.push_back((uint16_t)blk);

        RobBlock* block = &a1->block_vec[blk];
        block->key_set_offset = (int32_t)key_set_offset;
        block->frame = -1.0f;
        key_set_offset += block->ik_type >= IKT_ROOT ? 6 : 3;
    }
}

static void bone_data_parent_load_rob_chara(bone_data_parent* bone) {
    rob_chara_bone_data* rob_bone_data = bone->rob_bone_data;
    if (!rob_bone_data)
        return;

    bone->block_max = rob_bone_data->block_max;
    bone->leaf_pos_max = rob_bone_data->leaf_pos_max;
    bone->chain_pos_max = rob_bone_data->chain_pos_max;
    bone->block_vec.clear();
    bone->block_vec.resize(rob_bone_data->block_max);
    bone->global_key_set_count = 6;
    bone->bone_key_set_count = (uint32_t)((bone->block_max + bone->leaf_pos_max) * 3);
}

// 0x1405376C0
static uint32_t get_common_rob_mot(CHARA_NUM cn, uint32_t kamae_type, MOTTABLE_TYPE mottbl_type) {
    if (common_mot_tbl_header
        && cn >= 0 && (uint32_t)cn < common_mot_tbl_header->cn_num
        && mottbl_type >= 0 && (uint32_t)mottbl_type < common_mot_tbl_header->mtp_num) {
        CmnMotTblBase* base = (CmnMotTblBase*)((uint8_t*)common_mot_tbl_header
            + common_mot_tbl_header->data_tbl);
        if (kamae_type < (uint32_t)base[cn].kamae_max)
            return ((uint32_t*)((uint8_t*)common_mot_tbl_header
                + *(uint32_t*)((uint8_t*)common_mot_tbl_header
                    + kamae_type * sizeof(uint32_t) + base[cn].tbl)))[mottbl_type];
    }
    return -1;
}

static void mot_key_data_get_key_set_count_by_bone_database_bones(
    mot_key_data* a1, const std::vector<BODYTYPE>* body_type_table) {
    size_t mat_max;
    size_t block_max;
    size_t node_max;
    size_t leaf_pos_max;
    size_t chain_pos_max;
    bone_database_bones_calculate_count(body_type_table, mat_max,
        block_max, node_max, leaf_pos_max, chain_pos_max);
    mot_key_data_get_key_set_count(a1, block_max, leaf_pos_max);
}

static void mot_key_data_get_key_set_count(mot_key_data* a1, size_t block_max, size_t leaf_pos) {
    a1->key_set_count = (block_max + leaf_pos) * 3 + 16;
}

static void mot_key_data_init(mot_key_data* a1, BONE_KIND kind, const bone_database* bone_data) {
    const std::vector<BODYTYPE>* body_type_table = bone_data->get_body_type_table(kind);
    if (!body_type_table)
        return;

    mot_key_data_get_key_set_count_by_bone_database_bones(a1, body_type_table);
    mot_key_data_reserve_key_sets(a1);
    a1->kind = kind;
}

static void mot_key_data_init_key_sets(mot_key_data* a1, BONE_KIND kind, size_t block_max, size_t leaf_pos) {
    mot_key_data_get_key_set_count(a1, block_max, leaf_pos);
    mot_key_data_reserve_key_sets(a1);
    a1->kind = kind;
}

static const mot_data* mot_key_data_load_file(mot_key_data* a1, uint32_t motnum, const motion_database* mot_db) {
    if (a1->motnum != motnum) {
        const mot_data* mot_data = get_motiondata_address(motnum, mot_db);
        a1->mot_data = mot_data;
        if (mot_data) {
            a1->motion_body_type = a1->mot.fcurve_init(mot_data);
            a1->motnum = motnum;
        }
        else {
            a1->motion_body_type = 0;
            a1->motnum = -1;
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

    a1->mot.set_fck_ptr(a1->key_set.data());
    a1->key_sets_ready = true;
}

void RobMhPp::func_0(const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_1(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_2(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    base->motdata.motkind.set(MK_TURN);
    if (base->robmot.flag.bit.mirror)
        base->motdata.mov_yang -= ((int16_t*)ppadr)[0];
    else
        base->motdata.mov_yang += ((int16_t*)ppadr)[0];
}

void RobMhPp::func_3(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    int32_t v4 = ((int32_t*)ppadr)[0];
    base->motdata.motkind.set(v4);
}

void RobMhPp::func_4(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    int32_t v4 = ((int32_t*)ppadr)[0];
    base->motdata.motkind.reset(v4);
}

void RobMhPp::func_5(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    base->robmot.flag.u32 ^= 0x04;
}

void RobMhPp::func_6(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    base->action.down_counter_ex = 0;
    base->motdata.motkind.set(MK_DOWN);
    base->motdata.motkind.set(MK_DOWN_POSE);
}

static void sub_140551AF0(rob_chara* rob_chr) {
    if (!rob_chr->rob_base.flag.bit.fix_hara) {
        rob_chr->rob_base.position.pos.y = rob_chr->rob_base.position.gpos.y;
        rob_chr->rob_base.flag.bit.fix_hara = 1;
        if (!rob_chr->rob_base.motdata.air_kind[AK_CENTER_MOVE]) {
            rob_chr->rob_base.position.pos.x = rob_chr->rob_base.position.gpos.x;
            rob_chr->rob_base.position.pos.z = rob_chr->rob_base.position.gpos.z;
        }
    }
    rob_chr->rob_base.motdata.motkind.set(MK_AIR);
}

void RobMhPp::func_7(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    int16_t v4 = ((int16_t*)ppadr)[0];
    if (base->robmot.flag.bit.mirror)
        v4 = -v4;
    float_t v8 = (v4 + base->motdata.dturn_yang + base->position.yang).get_rad();
    float_t v9 = sinf(v8) * ((float_t*)ppadr)[1];
    float_t v10 = get_gravity() * ((float_t*)ppadr)[2];
    float_t v11 = cosf(v8) * ((float_t*)ppadr)[1];
    base->position.spd.x = v9;
    base->position.spd.y = v10;
    base->position.spd.z = v11;
    sub_140551AF0(impl);
}

void RobMhPp::func_8(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    int16_t v4 = *((int16_t*)ppadr + 2);
    //PostEvent(EVENT_TYPE_ROB_PP_QUAKE, &v4);
}

void RobMhPp::func_9(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_10(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_11(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_12(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_13(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    /*rob_chara* rob_chr; // r8
    RobBase* v5; // r9
    int32_t v6; // ecx
    int32_t v7; // eax
    char* v8; // rax
    char v9; // al
    int64_t v10; // [rsp+20h] [rbp-28h] BYREF
    int32_t v11; // [rsp+28h] [rbp-20h]
    __int16 v12; // [rsp+2Ch] [rbp-1Ch]
    bool v13; // [rsp+2Eh] [rbp-1Ah]
    int32_t v14; // [rsp+30h] [rbp-18h]

    rob_chr = impl;
    v5 = base;
    v6 = ((int32_t*)ppadr)[1];
    v10 = -1;
    v11 = -1;
    v12 = 0;
    v13 = 0;
    v14 = 0;
    LODWORD(v10) = rob_chr->idnm;
    v11 = ((int32_t*)ppadr)[0];
    v12 = (v5->motion.flag.u32 & 8) != 0;
    if (v6 & 0x01) {
        v8 = (char*)rob_chr->enemy;
        if (!v8)
            return;
        v7 = *v8;
    }
    else
        v7 = rob_chr->idnm;
    HIDWORD(v10) = v7;
    v9 = v5->field_8.field_150;
    HIBYTE(v12) = v9;
    if ((v6 & 2) != 0)
        HIBYTE(v12) = !v9;
    v14 = ((int32_t*)ppadr)[2];
    v13 = (v6 & 4) != 0;
    sub_1401FCE40((int64_t)&v10);*/
}

void RobMhPp::func_14(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_15(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_16(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_17(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    /*__int16 v4; // r14
    RobBase* v5; // rsi
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
    RobBase* v23; // rax
    float_t v24; // xmm1_4
    float_t v25; // xmm3_4
    RobBase* v26; // rax
    float_t v27; // xmm3_4
    float_t v28; // xmm0_4
    __m128 v29; // xmm2
    float_t v30; // xmm0_4
    float_t v31; // xmm1_4
    RobBase* v32; // rax
    float_t v33; // xmm2_4
    float_t v34; // xmm1_4
    float_t v35; // xmm0_4
    RobBase* v36; // r14
    float_t v37; // xmm0_4
    vec3 a3; // [rsp+20h] [rbp-30h] BYREF
    vec3 v39; // [rsp+30h] [rbp-20h] BYREF
    __int16 a1; // [rsp+70h] [rbp+20h] BYREF

    v4 = ((int16_t*)ppadr)[1];
    v5 = base;
    if (v4 & 0x02) {
        v8 = v5->collision.field_1EB4[0].field_0;
        a1 = v5->position.field_2;
        if (v8 > 4.0)
            return;
        v9 = v8 - ((float_t*)ppadr)[5];
        v10 = ((float_t*)ppadr)[1];
        if (v9 < v10 && v10 >= 0.0f || (v10 = ((float_t*)ppadr)[2], v9 > v10) && v10 >= 0.0f)
            v9 = v10;
    LABEL_8:
        v11 = &v5->motdata.target.target_pos.x;
        sub_1405357B0(&a1, &v5->position.gpos, &v5->motdata.target.target_pos, v9);
        v5->motdata.target.target_flag = ROB_TARGET_WALL;
        goto LABEL_27;
    }
    if (v4 & 0x04) {
        v12 = v5->collision.field_1EB4[2].field_0;
        a1 = v5->position.field_2 + 0x8000;
        if (v12 > 4.0)
            return;
        v9 = v12 - ((float_t*)ppadr)[5];
        v13 = ((float_t*)ppadr)[1];
        if (v9 < v13 && v13 >= 0.0f || (v13 = ((float_t*)ppadr)[2], v9 > v13) && v13 >= 0.0f)
            v9 = v13;
        goto LABEL_8;
    }
    if (!(v4 & 0x01))
        return;
    v14 = ((float_t*)ppadr)[2];
    v15 = ((float_t*)ppadr)[3];
    v16 = ((float_t*)ppadr)[5];
    v17 = ((float_t*)ppadr)[4];
    v5->motdata.target.target_flag = ROB_TARGET_ENEMY;
    v18 = -v16;
    if (v18 >= *((float_t*)ppadr + 1)) {
        if (v14 < 0.0f) {
            if ((v4 & 0x20) != 0)
                v14 = ((float_t*)ppadr)[1];
            v18 = v18 - (float_t)-v14;
        }
    }
    else
        v18 = ((float_t*)ppadr)[1];
    v5->motdata.target.target_pos.z = v17;
    v5->motdata.target.target_pos.y = 0.0f;
    v11 = &v5->motdata.target.target_pos.x;
    *(int64_t*)&v39.x = 0;
    v39.z = 0.0f;
    v5->motdata.target.target_pos.x = v15;
    v19 = ((int16_t*)ppadr)[0];
    *(int64_t*)&a3.x = 0;
    a3.z = 0.0f;
    sub_140533530(this, (float_t)v19, &a3);
    sub_140533530(this, base->robmot.frame.f, &v39);
    v20 = &base->position.field_2;
    a3.x = a3.x - v39.x;
    a3.z = a3.z - v39.z;
    sub_140535710(v20, &a3.x, &a3.x);
    a1 = motd->position.field_2;
    sub_140535710(&a1, &v5->motdata.target.target_pos.x, &v5->motdata.target.target_pos.x);
    v21 = v5->motdata.target.target_pos.x - a3.x;
    v22 = v5->motdata.target.target_pos.z - a3.z;
    v5->motdata.target.target_pos.x = v21;
    v5->motdata.target.target_pos.z = v22;
    v23 = motd;
    v24 = v23->position.gpos.y + v5->motdata.target.target_pos.y;
    v25 = v23->position.gpos.z + v5->motdata.target.target_pos.z;
    v5->motdata.target.target_pos.x = v21 + v23->position.gpos.x;
    v26 = base;
    v5->motdata.target.target_pos.y = v24;
    v5->motdata.target.target_pos.z = v25;
    v27 = v25 - v26->position.gpos.z;
    v28 = v5->motdata.target.target_pos.x - v26->position.gpos.x;
    v5->motdata.target.target_pos.y = v24 - v26->position.gpos.y;
    v5->motdata.target.target_pos.z = v27;
    v5->motdata.target.target_pos.x = v28;
    v5->motdata.target.target_pos.y = 0.0f;
    v29 = (__m128)LODWORD(v5->motdata.target.target_pos.x);
    v29.m128_f32[0] = (float_t)((float_t)(v29.m128_f32[0] * v29.m128_f32[0])
        + (float_t)(v5->motdata.target.target_pos.y * v5->motdata.target.target_pos.y))
        + (float_t)(v5->motdata.target.target_pos.z * v5->motdata.target.target_pos.z);
    if (_mm_sqrt_ps(v29).m128_f32[0] > v18) {
        vec3::normalize(&v5->motdata.target.target_pos);
        v30 = v18 * *v11;
        v31 = v18 * v5->motdata.target.target_pos.y;
        v5->motdata.target.target_pos.z = v18 * v5->motdata.target.target_pos.z;
        *v11 = v30;
        v5->motdata.target.target_pos.y = v31;
    }
    v32 = base;
    v33 = v32->position.gpos.z + v5->motdata.target.target_pos.z;
    v34 = v32->position.gpos.y + v5->motdata.target.target_pos.y;
    *v11 = v32->position.gpos.x + *v11;
    v5->motdata.target.target_pos.y = v34;
    v5->motdata.target.target_pos.z = v33;
LABEL_27:
    v35 = (float_t)((int16_t*)ppadr)[0];
    v5->motdata.target.target_end = v35;
    if (v4 & 0x08) {
        v36 = base;
        if (v35 < v36->motdata.land_frame)
            v35 = v36->motdata.land_frame;
        v37 = v35 - v36->motion.frame.f;
        if (v37 < 1.0)
            v37 = 1.0;
        v36->position.spd.x = (float_t)(*v11 - v36->position.gpos.x) * (float_t)(1.0 / v37);
        v36->position.spd.z = (float_t)(v5->motdata.target.target_pos.z - v36->position.gpos.z)
            * (float_t)(1.0 / v37);
        v36->position.spd.y = get_gravity() * ((float_t*)ppadr)[6];
        base->field_8.field_B8.field_10.y = 0.0f;
        sub_140551AF0(impl);
    }*/
}

void RobMhPp::func_18(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_19(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_20(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_21(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_22(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_23(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_24(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_25(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_26(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_27(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_28(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_29(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    prj::BitArrayExt<10> flag(&((uint32_t*)ppadr)[13]);
    uint16_t v8 = ((uint16_t*)ppadr)[24];
    if (!flag.count() || (v8 != 0 || flag[impl->cos_id]) && (v8 != 1 || !flag[impl->cos_id])) {
        int16_t end_frame = ((int16_t*)ppadr)[0];
        uint16_t part = ((uint16_t*)ppadr)[1];
        int16_t mode = ((int16_t*)ppadr)[2];
        int16_t target_id = ((int16_t*)ppadr)[3];
        float_t ofsx = ((float_t*)ppadr)[2];
        float_t ofsy = ((float_t*)ppadr)[3];
        float_t ofsz = ((float_t*)ppadr)[4];
        int16_t limit = ((int16_t*)ppadr)[5];
        float_t minx = ((float_t*)ppadr)[6];
        float_t miny = ((float_t*)ppadr)[7];
        float_t minz = ((float_t*)ppadr)[8];
        float_t maxx = ((float_t*)ppadr)[9];
        float_t maxy = ((float_t*)ppadr)[10];
        float_t maxz = ((float_t*)ppadr)[11];

        if (base->robmot.flag.bit.mirror) {
            part = leaf_ctrl_part_mirror_tbl[part];
            target_id = leaf_ctrl_target_mirror_tbl[target_id];
            if (leaf_ctrl_tagofs_mirror_flip_tbl[target_id][0]) {
                ofsx = -ofsx;
                minx = -minx;
                maxx = -maxx;
            }

            if (leaf_ctrl_tagofs_mirror_flip_tbl[target_id][1]) {
                ofsy = -ofsy;
                miny = -miny;
                maxy = -maxy;
            }

            if (leaf_ctrl_tagofs_mirror_flip_tbl[target_id][2]) {
                ofsz = -ofsz;
                minz = -minz;
                maxz = -maxz;
            }
        }

        MotLeafCtrl& leaf_ctrl = base->motdata.leaf_ctrl[part];
        leaf_ctrl.mode = mode;
        leaf_ctrl.start_frame = (float_t)list->frame;
        leaf_ctrl.end_frame = (float_t)end_frame;
        if (mode != 1) {
            leaf_ctrl.target_id = target_id;
            leaf_ctrl.limit = limit;
            leaf_ctrl.ofs.x = ofsx;
            leaf_ctrl.ofs.y = ofsy;
            leaf_ctrl.ofs.z = ofsz;
            leaf_ctrl.min.x = minx;
            leaf_ctrl.min.y = miny;
            leaf_ctrl.min.z = minz;
            leaf_ctrl.max.x = maxx;
            leaf_ctrl.max.y = maxy;
            leaf_ctrl.max.z = maxz;
        }
    }
}

void RobMhPp::func_30(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    base->position.spd.x = 0.0f;
    base->position.spd.z = 0.0f;
}

void RobMhPp::func_31(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

static int32_t sub_140533440(int32_t a1) {
    static const uint8_t byte_140A2D538[] = {
       ROB_COLLI_ID_KOSHI, ROB_COLLI_ID_MUNE_L, ROB_COLLI_ID_MUNE_R, ROB_COLLI_ID_KUBI, ROB_COLLI_ID_KAO,
       ROB_COLLI_ID_KATA_L1, ROB_COLLI_ID_KATA_L2, ROB_COLLI_ID_UDE_L1, ROB_COLLI_ID_UDE_L2, ROB_COLLI_ID_TE_L,
       ROB_COLLI_ID_KATA_R1, ROB_COLLI_ID_KATA_R2, ROB_COLLI_ID_UDE_R1, ROB_COLLI_ID_UDE_R2, ROB_COLLI_ID_TE_R,
       ROB_COLLI_ID_MOMO_L1, ROB_COLLI_ID_MOMO_L2, ROB_COLLI_ID_SUNE_L1,
       ROB_COLLI_ID_SUNE_L2, ROB_COLLI_ID_ASI_L, ROB_COLLI_ID_TOE_L,
       ROB_COLLI_ID_MOMO_R1, ROB_COLLI_ID_MOMO_R2, ROB_COLLI_ID_SUNE_R1,
       ROB_COLLI_ID_SUNE_R2, ROB_COLLI_ID_ASI_R, ROB_COLLI_ID_TOE_R,
    };

    int32_t v3 = a1 & 0xF800001F;
    for (int32_t i = 0; i < ROB_COLLI_ID_MAX; i++)
        if (a1 & (1 << i))
            v3 |= 1 << byte_140A2D538[i];
    return v3;
}

void RobMhPp::func_32(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    float_t v8 = (float_t)((int16_t*)ppadr)[0];
    int32_t v5 = ((int32_t*)ppadr)[1];
    float_t v9 = ((float_t*)ppadr)[2];
    int32_t v10 = ((int32_t*)ppadr)[3];

    if (v8 == (int16_t)0xFA0C && v10 == 0xD62721C5) { // X
        float_t value = v9;
        float_t duration = (float_t)v5;
        base->motdata.arm_adjust_next_value = value;
        base->motdata.arm_adjust_prev_value = base->arm_adjust_scale;
        base->motdata.arm_adjust_start_frame = list->frame;
        base->motdata.arm_adjust_duration = max_def(duration, 0.0f);
        return;
    }

    if (base->robmot.flag.bit.mirror)
        v5 = sub_140533440(v5);

    float_t v7 = base->motdata.frame - 1.0f - base->robmot.frame.f;
    if (v7 >= v8)
        v7 = v8;

    for (int32_t i = 0; i < 3; i++) {
        if (!(v10 & (1 << i)))
            continue;

        for (int32_t j = 0; j < ROB_COLLI_ID_MAX; j++) {
            if (!(v5 & (1 << j)))
                continue;

            base->motdata.colliball_flag[i][j] = true;
            base->motdata.colliball_ratio[i][j] = v9;
            base->motdata.colliball_timer[i][j] = v7;
        }
    }
}

void RobMhPp::func_33(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    prj::BitArrayExt<10> flag(&((uint32_t*)ppadr)[4]);
    if (flag.count() && !flag[impl->cos_id])
        return;

    prj::BitArrayExt<10> flag_enemy(&((uint32_t*)ppadr)[5]);
    rob_chara* enemy = impl->enemy;
    if (flag_enemy.count() && !(enemy && flag_enemy[impl->cos_id]))
        return;

    RobMotData& motdata = base->motdata;
    motdata.mot_adjust_start = (float_t)list->frame;
    motdata.mot_adjust_end = (float_t)((int16_t*)ppadr)[0];
    motdata.mot_adjust_flag = ((uint8_t*)ppadr)[2];
    motdata.mot_adjust_type = (MotAdjustType)((int32_t*)ppadr)[1];
    motdata.mot_adjust_scale = ((float_t*)ppadr)[2];
    motdata.pre_adjust_scale = base->robmot.mot_adjust_scale;
    motdata.mot_adjust_base_ypos = ((float_t*)ppadr)[3];
    motdata.pre_adjust_base_ypos = base->robmot.mot_adjust_base_pos.y;
}

void RobMhPp::func_34(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_35(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_36(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_37(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_38(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_39(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_40(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_41(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_42(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_43(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_44(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_45(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_46(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_47(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_48(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_49(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

static void rob_chara_set_face_motion_id(rob_chara* rob_chr, uint32_t motnum,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state, float_t blend_offset, const motion_database* mot_db);

void RobMhPp::func_50_set_face_motion_id(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    rob_chara_set_face_motion_id(impl, ((int32_t*)ppadr)[0],
        ((float_t*)ppadr)[1], ROB_PARTIAL_MOTION_PLAYBACK_STOP, 0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

void RobMhPp::func_51(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_52(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_53_set_face_mottbl_motion(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
     impl->set_face_mottbl_motion(0, (MOTTABLE_TYPE)((uint32_t*)ppadr)[0], ((float_t*)ppadr)[1],
         (rob_partial_motion_playback_state)((int32_t*)ppadr)[3], ((float_t*)ppadr)[2] * 6.0f,
         ((float_t*)ppadr)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, false, mot_db);
}

void RobMhPp::func_54_set_hand_r_mottbl_motion(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_hand_r_mottbl_motion(0, (MOTTABLE_TYPE)((uint32_t*)ppadr)[0], ((float_t*)ppadr)[1],
        (rob_partial_motion_playback_state)((int32_t*)ppadr)[3], ((float_t*)ppadr)[2] * 12.0f,
        ((float_t*)ppadr)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

void RobMhPp::func_55_set_hand_l_mottbl_motion(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_hand_l_mottbl_motion(0, (MOTTABLE_TYPE)((uint32_t*)ppadr)[0], ((float_t*)ppadr)[1],
        (rob_partial_motion_playback_state)((int32_t*)ppadr)[3], ((float_t*)ppadr)[2] * 12.0f,
        ((float_t*)ppadr)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

void RobMhPp::func_56_set_mouth_mottbl_motion(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_mouth_mottbl_motion(0, (MOTTABLE_TYPE)((uint32_t*)ppadr)[0], ((float_t*)ppadr)[1],
        (rob_partial_motion_playback_state)((int32_t*)ppadr)[3], ((float_t*)ppadr)[2] * 6.0f,
        ((float_t*)ppadr)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

void RobMhPp::func_57_set_eyes_mottbl_motion(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_eyes_mottbl_motion(0, (MOTTABLE_TYPE)((uint32_t*)ppadr)[0], ((float_t*)ppadr)[1],
        (rob_partial_motion_playback_state)((int32_t*)ppadr)[3], ((float_t*)ppadr)[2] * 6.0f,
        ((float_t*)ppadr)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

void RobMhPp::func_58_set_eyelid_mottbl_motion(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_eyelid_mottbl_motion(0, (MOTTABLE_TYPE)((uint32_t*)ppadr)[0], ((float_t*)ppadr)[1],
        (rob_partial_motion_playback_state)((int32_t*)ppadr)[3], ((float_t*)ppadr)[2] * 6.0f,
        ((float_t*)ppadr)[4], 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_eyelid_motion_id(rob_chara* rob_chr, uint32_t motnum,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state, float_t blend_offset, const motion_database* mot_db);
static void rob_chara_set_eyes_motion_id(rob_chara* rob_chr, uint32_t motnum,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state, float_t blend_offset, const motion_database* mot_db);

static void rob_chara_set_head_object(rob_chara* rob_chr, int32_t head_object_id, const motion_database* mot_db) {
    rob_chr->rob_base.robmot.field_150.head_object = rob_chara_get_head_object(rob_chr, head_object_id);
    rob_chara_set_face_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
    rob_chara_set_eyelid_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
    rob_chara_set_eyes_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
        0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
    rob_chr->bone_data->look_anim.disable = true;
}

void RobMhPp::func_59_set_rob_chara_head_object(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    rob_chara_set_head_object(impl, ((int32_t*)ppadr)[0], mot_db);
}

void RobMhPp::func_60_set_look_camera(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_look_camera(true, ((int32_t*)ppadr)[0] != 0,
        ((float_t*)ppadr)[1], ((float_t*)ppadr)[2], ((float_t*)ppadr)[3] * 6.0f, 0.25f, 0.0f, false);
}

void RobMhPp::func_61_set_eyelid_motion_from_face(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_eyelid_mottbl_motion_from_face(
        ((int32_t*)ppadr)[0], ((float_t*)ppadr)[1] * 6.0f, -1.0f, 0.0f, mot_db);
}

static void rob_chara_set_parts_adjust_by_index(rob_chara* rob_chr,
    rob_osage_parts index, rob_chara_data_adjust* adjust) {
    if (index >= ROB_OSAGE_PARTS_LEFT && index <= ROB_OSAGE_PARTS_ANGEL_R)
        rob_chara_set_adjust(rob_chr, adjust,
            &rob_chr->rob_base.robmot.parts_adjust[index],
            &rob_chr->rob_base.robmot.parts_adjust_prev[index]);
}

void RobMhPp::func_62_rob_parts_adjust(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = impl;

    rob_chara_data_adjust v16;
    v16.reset();

    int8_t type = ((int8_t*)ppadr)[5];
    if (type >= 0 && type < 6) {
        float_t set_frame = (float_t)list->frame;
        v16.enable = true;
        v16.frame = rob_chr->rob_base.robmot.frame.f - set_frame;
        v16.transition_frame = rob_chr->rob_base.robmot.frame.f - set_frame;
        v16.motnum = rob_chr->rob_base.robmot.num;
        v16.set_frame = set_frame;
        v16.transition_duration = (float_t)((int32_t*)ppadr)[0];
        v16.type = type;
        v16.cycle_type = ((int8_t*)ppadr)[7];
        v16.ignore_gravity = !!((uint8_t*)ppadr)[6];
        v16.ex_force.x = ((float_t*)ppadr)[2];
        v16.ex_force.y = ((float_t*)ppadr)[3];
        v16.ex_force.z = ((float_t*)ppadr)[4];
        v16.ex_force_cycle_strength.x = ((float_t*)ppadr)[5];
        v16.ex_force_cycle_strength.y = ((float_t*)ppadr)[6];
        v16.ex_force_cycle_strength.z = ((float_t*)ppadr)[7];
        v16.ex_force_cycle.x = ((float_t*)ppadr)[8];
        v16.ex_force_cycle.y = ((float_t*)ppadr)[9];
        v16.ex_force_cycle.z = ((float_t*)ppadr)[10];
        v16.cycle = ((float_t*)ppadr)[11];
        v16.force = ((float_t*)ppadr)[13];
        v16.phase = ((float_t*)ppadr)[12];
        v16.strength = ((float_t*)ppadr)[14];
        v16.strength_transition = (float_t)((int32_t*)ppadr)[15];
    }
    rob_chara_set_parts_adjust_by_index(rob_chr, (rob_osage_parts)((uint8_t*)ppadr)[4], &v16);
}

void RobMhPp::func_63(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {

}

void RobMhPp::func_64_osage_reset(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_osage_reset();
}

void RobMhPp::func_65_motion_skin_param(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = impl;
    uint32_t motnum = rob_chr->rob_base.robmot.num;
    float_t _frame = (float_t)list->frame;
    rob_chr->set_motion_skin_param(motnum, _frame);
    rob_chr->set_motion_reset_data(motnum, _frame);
}

void RobMhPp::func_66_osage_step(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_osage_step(((float_t*)ppadr)[0]);
}

void RobMhPp::func_67_sleeve_adjust(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    bool v5 = false;
    bool v6 = false;
    switch (((int32_t*)ppadr)[0]) {
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

    rob_chara_bone_data* rob_bone_data = impl->bone_data;
    rob_bone_data->sleeve_adjust.enable1 = v6;
    rob_bone_data->sleeve_adjust.enable2 = v5;
    rob_bone_data->sleeve_adjust.radius = ((float_t*)ppadr)[1];
}

void RobMhPp::func_68(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    motion_blend_mot* mot = impl->bone_data->motion_loaded.front();
    mot->mot_key_data.frame = -1.0f;
    mot->mot_key_data.field_68.field_0 = ((int32_t*)ppadr)[0];
    mot->mot_key_data.field_68.field_4 = ((float_t*)ppadr)[1];
}

void RobMhPp::func_69_motion_max_frame(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    float_t max_frame = (float_t)*(int32_t*)ppadr;
    impl->bone_data->set_motion_max_frame(max_frame);
    /*pv_game* v6 = pv_game_get();
    if (v6)
        pv_game::set_data_itmpv_max_frame(v6, impl->idnm, max_frame);*/
}

void RobMhPp::func_70_camera_max_frame(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    /*pv_game* v5 = pv_game_data_get();
    if (!v5)
        return;

    int32_t v6 = v5->ppadr.camera_auth_3d_uid;
    if (v6 == -1)
        return;

    int32_t id;
    pv_game::get_data_auth_3d(v5, &id, v6);
    if (auth_3d_data_check_id_not_empty(&id))
        auth_3d_data_set_max_frame(&id, (float_t)((int32_t*)ppadr)[0]);*/
}

void RobMhPp::func_71_osage_move_cancel(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_osage_move_cancel(((uint8_t*)ppadr)[0], ((float_t*)ppadr)[1]);
}

void RobMhPp::func_72(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    RobAdjust* v6 = &impl->rob_base.adjust;
    v6->offset.x = ((float_t*)ppadr)[0];
    v6->offset.y = ((float_t*)ppadr)[1];
    v6->offset.z = ((float_t*)ppadr)[2];
    v6->offset_x = !!((uint8_t*)ppadr)[24];
    v6->offset_y = !!((uint8_t*)ppadr)[25];
    v6->offset_z = !!((uint8_t*)ppadr)[26];
}

void RobMhPp::func_73_rob_hand_adjust(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    RobMotion& motion = impl->rob_base.robmot;
    int32_t hand = ((int16_t*)ppadr)[0];

    rob_chara_data_hand_adjust* hand_adjust = &motion.hand_adjust[hand];
    rob_chara_data_hand_adjust* hand_adjust_prev = &motion.hand_adjust_prev[hand];
    *hand_adjust_prev = *hand_adjust;

    hand_adjust->reset();
    hand_adjust->scale_select = ((int16_t*)ppadr)[1];
    hand_adjust->duration = ((float_t*)ppadr)[1];
    hand_adjust->type = (rob_chara_data_hand_adjust_type)((int16_t*)ppadr)[4];
    hand_adjust->scale = ((float_t*)ppadr)[3];
    hand_adjust->current_time = motion.frame.f - (float_t)list->frame;

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
        hand_adjust->init_cnt = hand_adjust_prev->init_cnt;
        break;
    case 1:
        hand_adjust->enable = true;
        hand_adjust->rotation_blend = ((float_t*)ppadr)[4];
        hand_adjust->offset.x = ((float_t*)ppadr)[5];
        hand_adjust->offset.y = ((float_t*)ppadr)[6];
        hand_adjust->offset.z = ((float_t*)ppadr)[7];
        hand_adjust->enable_scale = !!((uint8_t*)ppadr)[32];
        hand_adjust->disable_x = !!((uint8_t*)ppadr)[33];
        hand_adjust->disable_y = !!((uint8_t*)ppadr)[34];
        hand_adjust->disable_z = !!((uint8_t*)ppadr)[35];
        hand_adjust->scale_blend = ((float_t*)ppadr)[9];
        hand_adjust->arm_length = ((float_t*)ppadr)[10];
        hand_adjust->init_cnt = ((int32_t*)ppadr)[11];
        break;
    }
}

void RobMhPp::func_74_disable_collision(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->set_disable_collision((rob_osage_parts)((uint8_t*)ppadr)[0], !!((uint8_t*)ppadr)[1]);
}

static void rob_chara_set_adjust_global(rob_chara* rob_chr, rob_chara_data_adjust* a2) {
    rob_chara_set_adjust(rob_chr, a2,
        &rob_chr->rob_base.robmot.adjust_global, &rob_chr->rob_base.robmot.adjust_global_prev);
}

void RobMhPp::func_75_rob_adjust_global(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = impl;

    rob_chara_data_adjust v14;
    v14.reset();

    int8_t type = ((int8_t*)ppadr)[4];
    if (type >= 0 && type < 6) {
        float_t set_frame = (float_t)list->frame;
        v14.enable = true;
        v14.frame = rob_chr->rob_base.robmot.frame.f - set_frame;
        v14.transition_frame = rob_chr->rob_base.robmot.frame.f - set_frame;
        v14.motnum = rob_chr->rob_base.robmot.num;
        v14.set_frame = set_frame;
        v14.transition_duration = (float_t)((int32_t*)ppadr)[0];
        v14.type = type;
        v14.cycle_type = ((int8_t*)ppadr)[5];
        v14.ex_force.x = ((float_t*)ppadr)[2];
        v14.ex_force.y = ((float_t*)ppadr)[3];
        v14.ex_force.z = ((float_t*)ppadr)[4];
        v14.ex_force_cycle_strength.x = ((float_t*)ppadr)[5];
        v14.ex_force_cycle_strength.y = ((float_t*)ppadr)[6];
        v14.ex_force_cycle_strength.z = ((float_t*)ppadr)[7];
        v14.ex_force_cycle.x = ((float_t*)ppadr)[8];
        v14.ex_force_cycle.y = ((float_t*)ppadr)[9];
        v14.ex_force_cycle.z = ((float_t*)ppadr)[10];
        v14.cycle = ((float_t*)ppadr)[11];
        v14.phase = ((float_t*)ppadr)[12];
    }
    rob_chara_set_adjust_global(rob_chr, &v14);
}

void RobMhPp::func_76_rob_arm_adjust(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    rob_chara* rob_chr = impl;

    rob_chara_data_arm_adjust* arm_adjust = &rob_chr->rob_base.robmot.arm_adjust[((int16_t*)ppadr)[0]];
    arm_adjust->enable = true;
    arm_adjust->prev_value = arm_adjust->value;
    arm_adjust->value = 0.0f;
    arm_adjust->next_value = ((float_t*)ppadr)[2];
    arm_adjust->duration = ((float_t*)ppadr)[1];
}

void RobMhPp::func_77_disable_eye_motion(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    impl->bone_data->set_disable_eye_motion(((uint8_t*)ppadr)[0] != 0);
}

void RobMhPp::func_78(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    RobBase* rob_base = base;
    uint16_t part = ((uint8_t*)ppadr)[2];
    if (rob_base->robmot.flag.bit.mirror)
        part = leaf_ctrl_part_mirror_tbl[part];

    MotLeafCtrl& leaf_ctrl = rob_base->motdata.leaf_ctrl[part];
    float_t v12 = leaf_ctrl.field_34.x;
    float_t v8 = leaf_ctrl.field_34.y;
    float_t v9 = leaf_ctrl.field_34.z;
    float_t v10 = leaf_ctrl.field_40.x;
    float_t v13 = leaf_ctrl.field_40.y;
    float_t v11 = leaf_ctrl.field_40.z;

    leaf_ctrl.mode = -1;
    leaf_ctrl.start_frame = -1.0f;
    leaf_ctrl.end_frame = -1.0f;
    leaf_ctrl.target_id = 0;
    leaf_ctrl.limit = 0;

    leaf_ctrl.ofs = 0.0f;
    leaf_ctrl.min = 0.0f;
    leaf_ctrl.max = 0.0f;
    leaf_ctrl.field_34 = 0.0f;
    leaf_ctrl.field_40 = 0.0f;

    uint8_t mode = ((uint8_t*)ppadr)[3];
    if (mode != LEAF_CTRL_OFF) {
        v12 = ((float_t*)ppadr)[1];
        v8 = ((float_t*)ppadr)[2];
        v9 = ((float_t*)ppadr)[3];
        v10 = ((float_t*)ppadr)[4];
        v13 = ((float_t*)ppadr)[5];
        v11 = ((float_t*)ppadr)[6];
    }

    leaf_ctrl.mode = mode;
    leaf_ctrl.start_frame = (float_t)list->frame;
    leaf_ctrl.end_frame = (float_t)((int16_t*)ppadr)[1];
    leaf_ctrl.target_id = 26;
    leaf_ctrl.field_34.x = v12;
    leaf_ctrl.field_34.y = v8;
    leaf_ctrl.field_34.z = v9;
    leaf_ctrl.field_40.x = v10;
    leaf_ctrl.field_40.y = v13;
    leaf_ctrl.field_40.z = v11;
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
    RobDisp_set_ring(rob_chr->disp, ring);
}

void RobMhPp::func_79_rob_chara_coli_ring(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    rob_chara_set_coli_ring(impl, ((int8_t*)ppadr)[0]);
}

void RobMhPp::func_80_adjust_get_global_pos(
    const void* ppadr, MhpList* list, int32_t frame, const motion_database* mot_db) {
    base->adjust.get_global_pos = ((uint8_t*)ppadr)[0];
}

// 0x140536D30
void RobMhSmd::exec_func(int32_t type, MhdList* list) {
    (this->*functbl[type])(list->data, list);
}

void RobMhSmd::smd_next(const void* mhadr, MhdList* list) {
    if (motd->next_type != MH_NEXT_NONE)
        return;

    std::list<std::pair<const void*, uint32_t>> v23;
    uint32_t v8 = 0;
    if (!list)
        return;

    bool v9 = list->type == 0;
    if (list->type < 0)
        return;

    while (!v9) {
        list++;
        v9 = list->type == 0;
        if (list->type < 0)
            return;
    }

    const void* v10 = list->data;
    if (!v10)
        return;

LABEL_8:
    v8 += ((uint16_t*)v10)[11];
    v23.push_back({ v10, v8 });

    if (list) {
        MhdList* v13 = list + 1;
        bool v9 = v13 == 0;
        if (!v9) {
            while (v13->type >= 0) {
                if (!v13->type) {
                    list = v13;
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
                mhadr = i.first;
                break;
            }
    }

    v23.clear();

    RobMotData* v19 = motd;
    v19->next_type = (MhNextType)((int16_t*)mhadr)[0];
    v19->next.set(((uint32_t*)mhadr)[1], ((uint32_t*)mhadr)[2]);
    v19->next_end.set(((int32_t*)mhadr)[3], ((int32_t*)mhadr)[4]);
    v19->next_limit = ((int16_t*)mhadr)[10];
    v19->loop_begin = (float_t)((int16_t*)mhadr)[12];
    v19->loop_end = (float_t)((int16_t*)mhadr)[13];
}

void RobMhSmd::func_1(const void* mhadr, MhdList* list) {
    motd->main_mot_frame = (float_t)((int16_t*)mhadr)[0];
    if (((int16_t*)mhadr)[1] >= 0)
        motd->follow1_frame = (float_t)((int16_t*)mhadr)[1];
    if (((int16_t*)mhadr)[2] >= 0)
        motd->follow2_frame = (float_t)((int16_t*)mhadr)[2];
    motd->motkind.set(MK_BEGIN);
    base->robmot.basic_step = BSTEP_BEGIN;
}

void RobMhSmd::func_2(const void* mhadr, MhdList* list) {
    motd->air_kind.init(&((uint32_t*)mhadr)[0]);
    motd->jump_frame = (float_t)((int16_t*)mhadr)[2];
    motd->stop_frame = (float_t)((int16_t*)mhadr)[3];
    motd->land_frame = (float_t)((int16_t*)mhadr)[4];
    motd->ev_land_frame = (float_t)((int16_t*)mhadr)[5];
    motd->land_ypos = ((float_t*)mhadr)[3];
    motd->gravity += ((float_t*)mhadr)[4];
    base->action.yarare.efc_spd.y = -(get_gravity() * ((float_t*)mhadr)[4]);

    if (motd->land_frame < motd->stop_frame)
        motd->land_frame = motd->stop_frame;
}

void RobMhSmd::func_3(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_4(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_5(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_6(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_7(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_8(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_9(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_10(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_11(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_12(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_13(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_14(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_15(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_16(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_17(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_18(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_19(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_20(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_21(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_22(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_23(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_24(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_25(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_26(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_27(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_28(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_29(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_30(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_31(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_32(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_33(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_34(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_35(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_36(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_37(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_38(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_39(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_40(const void* mhadr, MhdList* list) {
    motd->tag_jumph = ((int16_t*)mhadr)[0];
    if (base->robmot.flag.bit.mirror)
        motd->tag_xofs = -((float_t*)mhadr)[1];
    else
        motd->tag_xofs = ((float_t*)mhadr)[1];
    motd->tag_zofs = ((float_t*)mhadr)[2];
    base->action.yarare.efc_spd.y = -(get_gravity() * ((float_t*)mhadr)[3]);
}

void RobMhSmd::smd_shift(const void* mhadr, MhdList* list) {
    if (motd->shift_num <= 0)
        motd->shift_adr = list;
    motd->shift_num++;
}

void RobMhSmd::func_42(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_43(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_44(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_45(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_46(const void* mhadr, MhdList* list) {
    if (motd->motkind_fix[MK_TURN])
        if (base->robmot.flag.bit.mirror)
            motd->mov_yang = -((int16_t*)mhadr)[0];
        else
            motd->mov_yang = ((int16_t*)mhadr)[0];
}

void RobMhSmd::func_47(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_48(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_49(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_50(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_51(const void* mhadr, MhdList* list) {
    if (base->robmot.flag.bit.mirror)
        motd->dturn_yang = -((int16_t*)mhadr)[0];
    else
        motd->dturn_yang = ((int16_t*)mhadr)[0];
}

void RobMhSmd::func_52(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_53(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_54(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_55(const void* mhadr, MhdList* list) {
    motd->smooth_flag.init(&((uint32_t*)mhadr)[0]);
    motd->smooth_f_length = ((float_t*)mhadr)[1];
    motd->smooth_r_length = ((float_t*)mhadr)[2];
}

void RobMhSmd::func_56(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_57(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_58(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_59(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_60(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_61(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_62(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_63(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_64(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_65(const void* mhadr, MhdList* list) {
    if (base->robmot.flag.bit.mirror)
        motd->trans_xofs = -((float_t*)mhadr)[0];
    else
        motd->trans_xofs = ((float_t*)mhadr)[0];
    motd->trans_yofs = ((float_t*)mhadr)[1];
    motd->trans_zofs = ((float_t*)mhadr)[2];
}

void RobMhSmd::func_66(const void* mhadr, MhdList* list) {
    motd->jump_zspd = *(float_t*)mhadr;
    if (motd->jump_zspd < 0.0f)
        motd->jump_zspd *= -1.0f;
}

void RobMhSmd::func_67(const void* mhadr, MhdList* list) {

}

void RobMhSmd::func_68(const void* mhadr, MhdList* list) {
    int32_t init_cnt = *(int32_t*)mhadr;
    if (init_cnt > 0)
        motd->init_cnt = init_cnt;
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

static PvOsageManager* pv_osage_manager_array_get(ROB_ID rob_id) {
    if (rob_id < 0 || rob_id >= ROB_ID_MAX)
        rob_id = ROB_ID_1P;
    return &pv_osage_manager_array[rob_id];
}

static void rob_base_rob_chara_init(rob_chara* rob_chr) {
    //sub_14054F4A0(&rob_chr->rob_touch);
    //sub_14054F830(&rob_chr->rob_touch, rob_chr->rob_init.field_70 != 0);
}

static mat4* sub_140504E80(rob_chara* rob_chr) {
    mat4 mat;
    mat4_rotate_y(rob_chr->rob_base.position.yang.get_rad(), &mat);
    mat4_set_translation(&mat, &rob_chr->rob_base.position.pos);
    rob_chr->rob_base.position.rob_mat = mat;
    return &rob_chr->rob_base.position.rob_mat;
}

static void sub_140414900(MotionSmooth* a1, const mat4* mat) {
    if (mat) {
        a1->base_mtx_set = true;
        a1->base_mtx = *mat;
    }
    else {
        a1->base_mtx_set = false;
        a1->base_mtx = mat4_identity;
    }
}

static void sub_14041DA50(rob_chara_bone_data* rob_bone_data, mat4* mat) {
    for (RobNode& i : rob_bone_data->node_vec)
        mat4_mul(i.mat_ptr, mat, i.mat_ptr);

    sub_140414900(&rob_bone_data->motion_loaded.front()->smooth, mat);
}

static void sub_140507F60(rob_chara* rob_chr) {
    sub_14041DA50(rob_chr->bone_data, sub_140504E80(rob_chr));
    rob_chr->rob_base.position.old_gpos = rob_chr->rob_base.position.gpos;

    mat4* n_hara_mat = rob_chr->bone_data->get_mats_mat(BONE_ID_N_HARA);
    mat4_get_translation(n_hara_mat, &rob_chr->rob_base.position.gpos);
    rob_chr->rob_base.position.velocity = rob_chr->rob_base.position.gpos
        - rob_chr->rob_base.position.old_gpos;

    mat4* n_hara_cp_mat = rob_chr->bone_data->get_mats_mat(BONE_ID_N_HARA_CP);
    vec3 v10 = { 0.0f, 0.0f, 1.0f };
    mat4_transform_vector(n_hara_cp_mat, &v10, &v10);
    rob_chr->rob_base.position.hara_yang.set_rad(atan2f(v10.x, v10.z));
}

static void sub_140505B20(rob_chara* rob_chr) {
    if (rob_chr->rob_base.motdata.motkind[MK_TURN])
        rob_chr->rob_base.position.act_yang = rob_chr->rob_base.position.yang + rob_chr->rob_base.motdata.mov_yang;
    else
        rob_chr->rob_base.position.act_yang = rob_chr->rob_base.position.yang;

    if (rob_chr->rob_base.flag.bit.fix_hara && rob_chr->rob_base.position.spd.y > -0.000001f)
        rob_chr->rob_base.flag.bit.jump_rise = 1;
    else
        rob_chr->rob_base.flag.bit.jump_rise = 0;
}

static void rob_base_rob_chara_ctrl(rob_chara* rob_chr) {
    sub_140507F60(rob_chr);
    rob_chr->adjust_ctrl();
    sub_140505B20(rob_chr);
    //rob_chara* v2 = rob_chr;
    //PostEvent(EVENT_TYPE_ROB_ACTION_FINISH, &v2);
    rob_chr->set_data_adjust_mat(&rob_chr->rob_base.adjust);
}

static void sub_140514680(rob_chara* rob_chr) {
    rob_chr->rob_base.action.step = 0;
    rob_chr->rob_base.action.action = rob_chr->rob_base.action.command;
}

static void sub_140514540(rob_chara* rob_chr) {
    rob_chr->rob_base.action.old_action = rob_chr->rob_base.action.action;
    rob_chr->rob_base.action.bak_command = rob_chr->rob_base.action.command;
    if (rob_chr->rob_base.action.command.name == ROB_ACT_MOTION)
        sub_140514680(rob_chr);
    rob_chr->rob_base.action.command.name = ROB_ACT_NONE;
}

static void sub_1405145F0(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
    if (rob_chr->rob_base.action.step)
        return;

    float_t frame = 0.0f;
    if (rob_chr->rob_base.robmot.flag.bit.ext_frame_req)
        frame = rob_chr->rob_base.robmot.frame.req_f;
    rob_chr->set_rob_motion(rob_chr->rob_base.action.action.motnum,
        rob_chr->rob_base.action.action.mirror, frame, MOTION_BLEND, bone_data, mot_db);
    rob_chr->bone_data->set_motion_blend_duration(0.0f, 1.0f, 1.0f);
    rob_chr->set_motion_reset_data(rob_chr->rob_base.action.action.motnum, 0.0f);
    rob_chr->rob_base.action.step = 1;
}

static void sub_1405144C0(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
    if (rob_chr->rob_base.action.action.name == ROB_ACT_MOTION) {
        pv_expression_array_ctrl(rob_chr);
        sub_1405145F0(rob_chr, bone_data, mot_db);
    }
}

static void sub_140514520(rob_chara* rob_chr, const bone_database* bone_data, const motion_database* mot_db) {
    sub_140514540(rob_chr);
    sub_1405144C0(rob_chr, bone_data, mot_db);
}

static void sub_14054BC70(RobSubAction* a1) {
    if (!a1->field_20)
        return;

    switch (a1->field_20->type) {
    case SUB_ACTION_EXECUTE_CRY:
        a1->field_8 = &a1->cry;
        break;
    case SUB_ACTION_EXECUTE_SHAKE_HAND:
        a1->field_8 = &a1->shake_hand;
        break;
    case SUB_ACTION_EXECUTE_EMBARRASSED:
        a1->field_8 = &a1->embarrassed;
        break;
    case SUB_ACTION_EXECUTE_ANGRY:
        a1->field_8 = &a1->angry;
        break;
    case SUB_ACTION_EXECUTE_LAUGH:
        a1->field_8 = &a1->laugh;
        break;
    case SUB_ACTION_EXECUTE_COUNT_NUM:
        a1->field_8 = &a1->count_num;
        break;
    default:
        a1->field_8 = 0;
        break;
    }

    if (a1->field_8)
        a1->field_8->set(a1->param);

    if (a1->field_20)
        delete a1->field_20;
    a1->field_20 = 0;
}

static void sub_14054CC80(rob_chara* a1) {
    sub_14054BC70(&a1->rob_base.sub_action);
    if (a1->rob_base.sub_action.field_8) {
        a1->rob_base.sub_action.field_8->Field_18(a1);
        a1->rob_base.sub_action.field_8 = 0;
    }

    if (a1->rob_base.sub_action.field_10)
        a1->rob_base.sub_action.field_10->Field_20(a1);
}

static const void* sub_140551F60(rob_chara* rob_chr, const prj::BitArray<23>& shift_req) {
    int16_t v2 = rob_chr->rob_base.motdata.shift_num;
    MhdList* v3 = rob_chr->rob_base.motdata.shift_adr;
    if (!v2)
        return 0;

    while (v3) {
        if (v3->type == MHD_SHIFT) {
            const void* v5 = v3->data;
            prj::BitArray<23> v8(&((uint32_t*)v5)[0]);
            if (v8[4]) {
                if (v8.count() == 1)
                    return v5;
            }
            else {
                if (v8 == shift_req)
                    return v5;

                v8 &= shift_req;
                if (v8.count())
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
    const void* data = sub_140551F60(rob_chr, rob_chr->rob_base.robmot.shift_req);
    if (!data)
        rob_chr->rob_base.robmot.shift_req.reset();
    else if ((float_t)((int16_t*)data)[2] <= rob_chr->rob_base.robmot.frame.f) {
        int32_t motnum = ((int32_t*)data)[2];

        prj::BitArray<23> flag(&((uint32_t*)data)[0]);
        if (flag[22] && flag.count() == 1)
            motnum = rob_chr->get_common_mot(MTP_NONE);
        if (motnum != -1) {
            rob_chr->set_rob_motion(motnum, !!(((uint8_t*)data)[12] & 0x01), 0.0f, MOTION_BLEND, bone_data, mot_db);

            //rob_chara* rob_impl = rob_chr;
            //PostEvent(EVENT_TYPE_ROB_MH_SHIFT_MOTION, &rob_impl);
        }
        rob_chr->rob_base.robmot.shift_req.reset();
    }
}

static void rob_chara_set_face_motion(rob_chara* rob_chr,
    RobFaceMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140553420
static void rob_chara_set_face_motion_id(rob_chara* rob_chr, uint32_t motnum,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobFaceMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motnum != -1) {
        motion.data.motnum = motnum;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_max = get_mot_frame_max(motnum, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_ptr = &rob_chr->rob_base.robmot.frame;
            motion.data.step_ptr = &rob_chr->rob_base.robmot.step;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_face_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_1405509D0(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->rob_base.robmot.field_150.face.CheckEnded())
        rob_chara_set_face_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            6.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_hand_l_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140553E30
static void rob_chara_set_hand_l_motion_id(rob_chara* rob_chr, uint32_t motnum,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motnum != -1) {
        motion.data.motnum = motnum;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_max = get_mot_frame_max(motnum, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_ptr = &rob_chr->rob_base.robmot.frame;
            motion.data.step_ptr = &rob_chr->rob_base.robmot.step;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_hand_l_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550A40(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->rob_base.robmot.field_150.hand_l.CheckEnded())
        rob_chara_set_hand_l_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            12.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_hand_r_motion(rob_chara* rob_chr,
    RobHandMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140554370
static void rob_chara_set_hand_r_motion_id(rob_chara* rob_chr, uint32_t motnum,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobHandMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motnum != -1) {
        motion.data.motnum = motnum;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_max = get_mot_frame_max(motnum, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_ptr = &rob_chr->rob_base.robmot.frame;
            motion.data.step_ptr = &rob_chr->rob_base.robmot.step;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_hand_r_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550AB0(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->rob_base.robmot.field_150.hand_r.CheckEnded())
        rob_chara_set_hand_r_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            12.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_mouth_motion(rob_chara* rob_chr,
    RobMouthMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140554C40
static void rob_chara_set_mouth_motion_id(rob_chara* rob_chr, uint32_t motnum,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobMouthMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motnum != -1) {
        motion.data.motnum = motnum;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_max = get_mot_frame_max(motnum, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_ptr = &rob_chr->rob_base.robmot.frame;
            motion.data.step_ptr = &rob_chr->rob_base.robmot.step;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_mouth_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550B20(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->rob_base.robmot.field_150.mouth.CheckEnded())
        rob_chara_set_mouth_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            6.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_eyes_motion(rob_chara* rob_chr,
    RobEyesMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140552F10
static void rob_chara_set_eyes_motion_id(rob_chara* rob_chr, uint32_t motnum,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobEyesMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motnum != -1) {
        motion.data.motnum = motnum;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_max = get_mot_frame_max(motnum, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_ptr = &rob_chr->rob_base.robmot.frame;
            motion.data.step_ptr = &rob_chr->rob_base.robmot.step;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_eyes_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_140550960(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->rob_base.robmot.field_150.eyes.CheckEnded())
        rob_chara_set_eyes_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            6.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static void rob_chara_set_eyelid_motion(rob_chara* rob_chr,
    RobEyelidMotion* motion, int32_t type, const motion_database* mot_db);

// 0x140552A90
static void rob_chara_set_eyelid_motion_id(rob_chara* rob_chr, uint32_t motnum,
    float_t frame, rob_partial_motion_playback_state playback_state, float_t blend_duration, float_t play_duration,
    float_t step, rob_partial_motion_loop_state loop_state,
    float_t blend_offset, const motion_database* mot_db) {
    RobEyelidMotion motion;
    motion.data.blend_duration = blend_duration;
    motion.data.blend_offset = blend_offset;
    if (motnum != -1) {
        motion.data.motnum = motnum;
        motion.data.playback_state = playback_state;
        motion.data.frame = frame;
        motion.data.step = step;
        motion.data.frame_max = get_mot_frame_max(motnum, mot_db);
        motion.data.play_duration = play_duration;
        if (motion.CheckPlaybackStateCharaMotion()) {
            motion.data.frame_ptr = &rob_chr->rob_base.robmot.frame;
            motion.data.step_ptr = &rob_chr->rob_base.robmot.step;
        }
        motion.data.loop_state = loop_state;
    }
    rob_chara_set_eyelid_motion(rob_chr, &motion, 0, mot_db);
}

static void sub_1405508F0(rob_chara* rob_chr, const motion_database* mot_db) {
    if (rob_chr->rob_base.robmot.field_150.eyelid.CheckEnded())
        rob_chara_set_eyelid_motion_id(rob_chr, -1, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            6.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, mot_db);
}

static bool sub_1404190E0(rob_chara_bone_data* rob_bone_data) {
    return rob_bone_data->eyelid.blend.field_8 || rob_bone_data->eyelid.blend.enable;
}

static void sub_140555F70(rob_chara* rob_chr, const motion_database* mot_db) {
    if (sub_1404190E0(rob_chr->bone_data) || (rob_chr->rob_base.robmot.flag.u32 & 0x8000))
        return;

    int32_t v3 = rob_chr->rob_base.robmot.field_150.field_1C0;
    if (v3 == 1) {
        if (rob_chr->rob_base.robmot.flag.u32 & 0x0200) {
            rob_chr->set_eyelid_mottbl_motion_from_face(2, 3.0f, -1.0f, 1.0f, mot_db);
            rob_chr->rob_base.robmot.flag.u32 |= 0x0200;
            return;
        }
        return;
    }
    else if (v3 == 2) {
        rob_chr->rob_base.robmot.field_150.field_1C0 = 0;
        rob_chr->rob_base.robmot.flag.u32 &= ~0x0200;
    }
    else if (v3 != 0)
        return;

    sub_1405500F0(rob_chr);
    if (!(rob_chr->rob_base.robmot.flag.u32 & 0x020000) && rob_chr->rob_base.robmot.flag.bit.ext_frame_req) {
        rob_chr->rob_base.robmot.field_150.time += rand_state_array_get_float(1)
            * rob_chr->frame_speed * 2.0f * (float_t)(1.0 / 60.0);
        if (rob_chr->rob_base.robmot.field_150.time >= 5.0f) {
            rob_chr->set_eyelid_mottbl_motion_from_face(1, 3.0f, -1.0f, 1.0f, mot_db);
            rob_chr->rob_base.robmot.flag.u32 |= 0x0200;
        }
    }
}

static void sub_14041CA70(rob_chara_bone_data* rob_bone_data, float_t a2) {
    rob_bone_data->motion_loaded.front()->smooth.field_C0 = a2;
}

static void sub_14041CAB0(rob_chara_bone_data* rob_bone_data, float_t a2) {
    rob_bone_data->motion_loaded.front()->smooth.field_C4 = a2;
}

static void sub_14041CA10(rob_chara_bone_data* rob_bone_data, vec3* a2) {
    rob_bone_data->motion_loaded.front()->smooth.field_C8 = *a2;
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
    if (rob_chr->rob_base.robmot.flag.u32 & 0x0400)
        return rob_chr->rob_base.robmot.field_3B0.head_object;
    else
        return rob_chr->rob_base.robmot.field_150.head_object;
}

static object_info sub_140550350(rob_chara* rob_chr){
    if (rob_chr->rob_base.robmot.flag.u32 & 0x0800)
        return rob_chr->rob_base.robmot.field_3B0.hand_l_object;
    else if (rob_chr->rob_base.robmot.flag.u32 & 0x040000)
        return rob_chr->rob_base.robmot.hand_l_object;
    return rob_chr->rob_base.robmot.field_150.hand_l_object;
}

static object_info sub_140550380(rob_chara* rob_chr) {
    if (rob_chr->rob_base.robmot.flag.u32 & 0x1000)
        return rob_chr->rob_base.robmot.field_3B0.hand_r_object;
    else if (rob_chr->rob_base.robmot.flag.u32 & 0x080000)
        return rob_chr->rob_base.robmot.hand_r_object;
    return rob_chr->rob_base.robmot.field_150.hand_r_object;
}

static object_info sub_140550310(rob_chara* rob_chr) {
    if (rob_chr->rob_base.robmot.flag.u32 & 0x010000)
        return rob_chr->rob_base.robmot.field_3B0.face_object;
    return rob_chr->rob_base.robmot.field_150.face_object;
}

static void sub_140419800(rob_chara_bone_data* rob_bone_data, vec3* position) {
    rob_bone_data->motion_loaded.front()->get_n_hara_cp_position(*position);
}

static float_t sub_1405501F0(rob_chara* rob_chr) {
    if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.motion_body_type == 1)
        return rob_chr->rob_base.robmot.mot_adjust_scale / rob_chr->bone_data->adjust_scale.base;
    return rob_chr->rob_base.robmot.mot_adjust_scale;
}

static float_t sub_1405503B0(rob_chara* rob_chr) {
    if (rob_chr->bone_data->motion_loaded.front()->mot_key_data.motion_body_type == 1)
        return rob_chr->rob_base.robmot.mot_xz_adjust_scale / rob_chr->bone_data->adjust_scale.base;
    return rob_chr->rob_base.robmot.mot_xz_adjust_scale;
}

static void sub_140504F00(rob_chara* rob_chr) {
    if (rob_chr->rob_base.motdata.motkind[MK_NO_TRANS] && !rob_chr->rob_base.motdata.motkind[MK_Y_TRANS])
        return;

    vec3 v9 = 0.0f;
    sub_140419800(rob_chr->bone_data, &v9);
    float_t v3 = sub_1405503B0(rob_chr) * rob_chr->rob_base.robmot.step.f;
    float_t v4 = sub_1405501F0(rob_chr) * rob_chr->rob_base.robmot.step.f;
    v9.x -= v3 * rob_chr->rob_base.motdata.trans_xofs;
    v9.y -= v4 * rob_chr->rob_base.motdata.trans_yofs;
    v9.z -= v3 * rob_chr->rob_base.motdata.trans_zofs;

    mat4 mat;
    mat4_rotate_y(rob_chr->rob_base.position.yang.get_rad(), &mat);
    mat4_transform_vector(&mat, &v9, &v9);

    if (!rob_chr->rob_base.motdata.motkind[MK_NO_TRANS]) {
        rob_chr->rob_base.motdata.motkind.set(MK_NO_TRANS);
        rob_chr->rob_base.position.pos.x = rob_chr->rob_base.position.gpos.x - v9.x;
        rob_chr->rob_base.position.pos.z = rob_chr->rob_base.position.gpos.z - v9.z;
    }

    if (rob_chr->rob_base.motdata.motkind[MK_Y_TRANS]) {
        rob_chr->rob_base.motdata.motkind.reset(MK_Y_TRANS);
        rob_chr->rob_base.position.pos.y = rob_chr->rob_base.position.gpos.y - v9.y;
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
    sub_14041C9D0(rob_chr->bone_data, rob_chr->rob_base.robmot.flag.bit.mirror);
    sub_14041D2D0(rob_chr->bone_data, rob_chr->rob_base.robmot.flag.bit.next_mirror);
    sub_14041C680(rob_chr->bone_data, rob_chr->rob_base.flag.bit.fix_hara);
    sub_14041D270(rob_chr->bone_data, rob_chr->rob_base.motdata.mov_yang.get_rad());
    sub_14041CA70(rob_chr->bone_data, rob_chr->rob_base.robmot.mot_adjust_scale);
    sub_14041CAB0(rob_chr->bone_data, rob_chr->rob_base.robmot.mot_xz_adjust_scale);
    sub_14041CA10(rob_chr->bone_data, &rob_chr->rob_base.robmot.mot_adjust_base_pos);
    sub_14041DA00(rob_chr->bone_data);

    RobFaceMotion* face = &rob_chr->rob_base.robmot.field_150.face;
    if ((rob_chr->rob_base.robmot.flag.u32 & 0x0400))
        face = &rob_chr->rob_base.robmot.field_3B0.face;
    face->GetFrameStep();
    rob_chr->bone_data->set_face_frame(face->data.frame);
    rob_chr->bone_data->set_face_step(face->data.step);

    RobHandMotion* hand_l = &rob_chr->rob_base.robmot.field_150.hand_l;
    if (rob_chr->rob_base.robmot.flag.u32 & 0x0800)
        hand_l = &rob_chr->rob_base.robmot.field_3B0.hand_l;
    else if (rob_chr->rob_base.robmot.flag.u32 & 0x040000)
        hand_l = &rob_chr->rob_base.robmot.hand_l;
    hand_l->GetFrameStep();
    rob_chr->bone_data->set_hand_l_frame(hand_l->data.frame);
    rob_chr->bone_data->set_hand_l_step(hand_l->data.step);

    RobHandMotion* hand_r = &rob_chr->rob_base.robmot.field_150.hand_r;
    if (rob_chr->rob_base.robmot.flag.u32 & 0x1000)
        hand_r = &rob_chr->rob_base.robmot.field_3B0.hand_r;
    else if (rob_chr->rob_base.robmot.flag.u32 & 0x080000)
        hand_r = &rob_chr->rob_base.robmot.hand_r;
    hand_r->GetFrameStep();
    rob_chr->bone_data->set_hand_r_frame(hand_r->data.frame);
    rob_chr->bone_data->set_hand_r_step(hand_r->data.step);

    RobMouthMotion* mouth = &rob_chr->rob_base.robmot.field_150.mouth;
    if (rob_chr->rob_base.robmot.flag.u32 & 0x2000)
        mouth = &rob_chr->rob_base.robmot.field_3B0.mouth;
    mouth->GetFrameStep();
    rob_chr->bone_data->set_mouth_frame(mouth->data.frame);
    rob_chr->bone_data->set_mouth_step(mouth->data.step);

    RobEyesMotion* eyes = &rob_chr->rob_base.robmot.field_150.eyes;
    if (rob_chr->rob_base.robmot.flag.u32 & 0x4000)
        eyes = &rob_chr->rob_base.robmot.field_3B0.eyes;
    eyes->GetFrameStep();
    rob_chr->bone_data->set_eyes_frame(eyes->data.frame);
    rob_chr->bone_data->set_eyes_step(eyes->data.step);

    RobEyelidMotion* eyelid = &rob_chr->rob_base.robmot.field_150.eyelid;
    if (rob_chr->rob_base.robmot.flag.u32 & 0x8000)
        eyelid = &rob_chr->rob_base.robmot.field_3B0.eyelid;
    eyelid->GetFrameStep();
    rob_chr->bone_data->set_eyelid_frame(eyelid->data.frame);
    rob_chr->bone_data->set_eyelid_step(eyelid->data.step);

    object_info v8 = sub_140550330(rob_chr);
    if (rob_chara_get_object_info(rob_chr, RPK_ATAMA) != v8)
        rob_chr->set_base(RPK_ATAMA, v8, false, bone_data, data, obj_db);
    object_info v9 = rob_chara_get_head_object(rob_chr, 1);
    object_info v10 = rob_chara_get_head_object(rob_chr, 7);

    bool v11 = v8 == v9 || v8 == v10;
    rob_chr->bone_data->eyes.disable = v11;
    rob_chr->bone_data->look_anim.disable = v11;

    object_info v12 = sub_140550350(rob_chr);
    if (rob_chara_get_object_info(rob_chr, RPK_TE_L) != v12)
        rob_chr->set_base(RPK_TE_L, v12, true, bone_data, data, obj_db);

    object_info v13 = sub_140550380(rob_chr);
    if (rob_chara_get_object_info(rob_chr, RPK_TE_R) != v13)
        rob_chr->set_base(RPK_TE_R, v13, true, bone_data, data, obj_db);

    object_info v14 = sub_140550310(rob_chr);
    if (rob_chara_get_object_info(rob_chr, RPK_HARA) != v14)
        rob_chr->set_base(RPK_HARA, v14, false, bone_data, data, obj_db);

    rob_chr->bone_data->interpolate();
    rob_chr->bone_data->update(0);
    sub_140504F00(rob_chr);

    rob_chr->rob_base.robmot.flag.u32 &= ~0x80;
}

static void sub_140504AC0(rob_chara* rob_chr) {
    vec3 v4 = rob_chr->rob_base.position.spd;
    vec3 v20 = 0.0f;
    if (rob_chr->rob_base.flag.bit.fix_hara) {
        v20 = v4 + rob_chr->rob_base.position.adjust_spd + rob_chr->rob_base.action.yarare.efc_spd;
        v4.y = rob_chr->rob_base.action.yarare.efc_spd.y - get_gravity() * rob_chr->rob_base.motdata.gravity;
    }
    else if (!rob_chr->rob_base.flag.bit.jump_rise) {
        v20 = rob_chr->rob_base.position.adjust_spd;
        v20.x += v4.x + rob_chr->rob_base.action.yarare.efc_spd.x;
        v20.z += v4.z + rob_chr->rob_base.action.yarare.efc_spd.z;

        float_t v7 = 0.4f;
        if (rob_chr->rob_base.motdata.motkind[MK_DOWN_POSE]
            || rob_chr->rob_base.motdata.motkind[MK_DOWN])
            v7 = 0.6f;

        float_t v13 = powf(v7, 1.0f);
        if (rob_chr->rob_base.motdata.target.target_flag != ROB_TARGET_NONE)
            v13 = 1.0f;

        v4.x *= v13;
        v4.y = 0.0f;
        v4.z *= v13;

        rob_chr->rob_base.action.yarare.efc_spd.x *= v13;
        rob_chr->rob_base.action.yarare.efc_spd.z *= v13;

        if (rob_chr->rob_base.flag.bit.ringout
            && rob_chr->rob_base.motdata.jump_frame > rob_chr->rob_base.robmot.frame.f) {
            v20.x += rob_chr->rob_base.ringout.ringout_spd.x;
            v20.z += rob_chr->rob_base.ringout.ringout_spd.z;
        }

        if (rob_chr->rob_base.motdata.target.target_flag == ROB_TARGET_RINGOUT) {
            v20 = 0.0f;
            if (rob_chr->rob_base.motdata.target.target_end >= rob_chr->rob_base.robmot.frame.f)
                v20.y = rob_chr->rob_base.motdata.target.target_pos.y;
            else
                rob_chr->rob_base.motdata.target.target_flag = ROB_TARGET_NONE;
        }
        else if (rob_chr->rob_base.motdata.target.target_flag) {
            v20.x = 0.0f;
            v20.z = 0.0f;
            if (rob_chr->rob_base.robmot.frame.f > rob_chr->rob_base.motdata.target.target_end
                || rob_chr->rob_base.collision.wall_hit_num
                && rob_chr->rob_base.motdata.target.target_flag == ROB_TARGET_WALL)
                rob_chr->rob_base.motdata.target.target_flag = ROB_TARGET_NONE;
            else {
                float_t v17 = rob_chr->rob_base.motdata.target.target_end
                    - rob_chr->rob_base.robmot.frame.f;
                if (v17 < 0.0f)
                    v17 = 0.0f;

                float_t v18 = 1.0f / (v17 + 1.0f);
                v20.x = v18 * (rob_chr->rob_base.motdata.target.target_pos.x
                    - rob_chr->rob_base.position.gpos.x);
                v20.z = v18 * (rob_chr->rob_base.motdata.target.target_pos.z
                    - rob_chr->rob_base.position.gpos.z);
            }
        }
    }

    if (rob_chr->rob_base.motdata.motkind[MK_BACK_SPEED_OFF]
        && (rob_chr->rob_base.robinfo.old_en_flag.bit.en_front
            && rob_chr->rob_base.robinfo.en_flag.bit.en_down_nage
            || rob_chr->rob_base.robinfo.old_en_flag.bit.en_down_nage
            && rob_chr->rob_base.robinfo.en_flag.bit.en_front)) {
        v4.x = 0.0f;
        v4.z = 0.0f;
    }

    rob_chr->rob_base.position.spd = v4;
    rob_chr->rob_base.position.adjust_spd = 0.0f;
    rob_chr->rob_base.position.pos = v20 + rob_chr->rob_base.position.pos;
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
    rob_chr->rob_base.sub_action.dest();
    rob_chara_bone_data_motion_blend_mot_free(rob_chr->bone_data);
}

static void rob_disp_rob_chara_init(rob_chara* rob_chr,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    object_database* aft_obj_db = &aft_data->data_ft.obj_db;

    RobDisp* disp = rob_chr->disp;
    const RobData* rob_data = rob_chr->rob_data;
    RobNode* v3 = rob_chara_bone_data_get_node(rob_chr->bone_data, BONE_NODE_N_HARA_CP);
    disp->reset_init_data(v3);
    disp->set_one_skin(rob_chr->cos_id == 501);
    disp->set_skin({}, RPK_BODY, false, bone_data, data, obj_db);
    for (int32_t i = RPK_BASE_BEGIN; i <= RPK_BASE_END; i++)
        disp->set_base((ROB_PARTS_KIND)i, rob_data->body_obj_uid[i], false,
            aft_bone_data, aft_data, aft_obj_db);
    disp->set_shadow_group(rob_chr->idnm);
    rob_chr->disp->shadow_flag = 0x05;
    rob_chr->item.equip(rob_chr->idnm, bone_data, data, obj_db);
    if (rob_chr->item.check_for_npr_flag())
        rob_chr->disp->npr_flag = true;

    if (rob_chr->check_for_ageageagain_module()) {
        rob_chara_age_age_array_load(rob_chr->idnm, 1);
        rob_chara_age_age_array_load(rob_chr->idnm, 2);
    }
}

static void rob_disp_rob_chara_ctrl(rob_chara* rob_chr) {
    if (!rob_chr->disp_pos_reset)
        return;

    rob_chr->disp_pos_reset = false;

    if (rob_chr->disp_pos_reset_forbidden) {
        rob_chr->disp_pos_reset_forbidden = false;
        return;
    }

    rob_chr->pos_reset();

    effect_fog_ring_data_reset();
    effect_splash_data_reset();
}

static void rob_disp_rob_chara_ctrl_thread_main(rob_chara* rob_chr) {
    if (rob_chr->bone_data->get_frame() < 0.0f)
        rob_chr->bone_data->set_frame(0.0f);

    float_t frame_max = rob_chr->bone_data->get_frame_max();
    if (rob_chr->bone_data->get_frame() > frame_max)
        rob_chr->bone_data->set_frame(frame_max);

    rob_chr->disp->set_opd_blend_data(&rob_chr->bone_data->motion_loaded);

    vec3 pos = 0.0f;
    rob_chr->get_pos_scale(ROB_COLLI_ID_KOSHI, pos);
    rob_chr->disp->position = pos;
    RobDisp_ctrl(rob_chr->disp);
    if (rob_chr->check_for_ageageagain_module()) {
        rob_chara_age_age_array_set_step(rob_chr->idnm, 1, rob_chr->disp->osage_step);
        rob_chara_age_age_ctrl(rob_chr, 1, "j_tail_l_006_wj");
        rob_chara_age_age_array_set_step(rob_chr->idnm, 2, rob_chr->disp->osage_step);
        rob_chara_age_age_ctrl(rob_chr, 2, "j_tail_r_006_wj");
    }
}

static void rob_disp_rob_chara_disp(rob_chara* rob_chr) {
    rob_chr->disp->skin_color = 1.0f;
    rob_chr->disp->mat = rob_chr->rob_base.adjust.mat;
    rob_chr->disp->disp(rob_chr->idnm, rctx_ptr);
}

static void rob_disp_rob_chara_free(rob_chara* rob_chr) {
    rob_chara_age_age_array_reset(rob_chr->idnm);
    rob_chr->disp->reset();
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
    prj::sys_vector<RobBlock>& block_vec, const mat4& cur_mat, float_t step) {
    vec3 v69;
    mat4_get_translation(block_vec[BLK_CL_KAO].node[2].mat_ptr, &v69);
    if (!look_anim->field_190 && !look_anim->field_191)
        return;

    float_t v14 = step * 0.25f;
    float_t v15 = look_anim->field_15C.field_8;
    float_t v16 = look_anim->field_15C.field_C;
    const mat4* c_kata_r_mat = block_vec[BLK_C_KATA_R].node[0].mat_ptr;
    const mat4* c_kata_l_mat = block_vec[BLK_C_KATA_L].node[0].mat_ptr;
    const mat4* n_kao_mat = block_vec[BLK_N_KAO].node[0].mat_ptr;

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

    mat4 cl_kao_mat = *block_vec[BLK_CL_KAO].node[1].mat_ptr;
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

    mat4 cl_kao_mat_backup = block_vec[BLK_CL_KAO].chain_rot[1];

    mat4 v74;
    mat4_invert(block_vec[BLK_CL_KAO].node[0].mat_ptr, &v74);
    mat4_clear_trans(&v74, &v74);
    mat4_clear_trans(&cl_kao_mat, &cl_kao_mat);
    mat4_mul(&cl_kao_mat, &v74, &block_vec[BLK_CL_KAO].chain_rot[1]);

    for (int32_t i = BLK_CL_KAO; i <= BLK_N_KUBI_WJ_EX; i++)
        block_vec[i].recalc_fk_block(cur_mat, block_vec.data(), true);

    block_vec[BLK_CL_KAO].chain_rot[1] = cl_kao_mat_backup;
}

static void sub_1404189A0(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
    sub_140407280(&rob_bone_data->look_anim, mot->bone_data.block_vec,
        mot->smooth.base_mtx, mot->mot_play_data.frame_data.step);
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
    prj::sys_vector<RobBlock>& block_vec, const mat4& mat, float_t step) {
    if (look_anim->disable)
        return;

    look_anim->block_vec = &block_vec;
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

    const mat4* kl_eye_l_parent_mat = block_vec[BLK_KL_EYE_L].inherit_mat_ptr;
    const mat4* kl_eye_r_parent_mat = block_vec[BLK_KL_EYE_R].inherit_mat_ptr;

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
    sub_140406FC0(look_anim, &block_vec[BLK_KL_EYE_L],
        &look_anim->left_eye_mat, pos_left - look_anim->param.pos,
        rot_neg_left, rot_pos_left, eyes_rot_anim, eyes_rot_blend, eyes_rot_step);

    const vec3 _xor = vec3(-0.0f, 0.0f, 0.0f);
    vec3 rot_neg_right = vec3(xrot_neg, yrot_neg_right, 0.0f);
    vec3 rot_pos_right = vec3(xrot_pos, yrot_pos_right, 0.0f);
    vec3 pos_right;
    mat4_inverse_transform_point(kl_eye_r_parent_mat, &look_anim->view_point, &pos_right);
    sub_140406FC0(look_anim, &block_vec[BLK_KL_EYE_R],
        &look_anim->right_eye_mat, pos_right - (look_anim->param.pos ^ _xor),
        rot_neg_right, rot_pos_right, eyes_rot_anim, eyes_rot_blend, eyes_rot_step);

    for (int32_t i = BLK_KL_EYE_L; i <= BLK_KL_HIGHLIGHT_L_WJ; i++)
        block_vec[i].recalc_fk_block(mat, block_vec.data(), true);

    for (int32_t i = BLK_KL_EYE_R; i <= BLK_KL_HIGHLIGHT_R_WJ; i++)
        block_vec[i].recalc_fk_block(mat, block_vec.data(), true);
}

static void sub_14041A160(rob_chara_bone_data* rob_bone_data, const mat4& adjust_mat) {
    motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
    sub_140409170(&rob_bone_data->look_anim, adjust_mat, mot->bone_data.block_vec,
        mot->smooth.base_mtx, mot->mot_play_data.frame_data.step);
}

static void rob_chara_head_adjust(rob_chara* rob_chr) {
    if (rob_chr->type != ROB_TYPE_AUTH)
        return;

    bool v4 = !rob_chr->rob_base.flag.bit.dmy_yokerare;
    rob_chr->rob_base.flag.bit.dmy_yokerare = 0;

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

    sub_14041C620(rob_bone_data, target_view_point, &rob_chr->rob_base.adjust.mat, v4);
    sub_1404189A0(rob_bone_data);
    sub_14041A160(rob_bone_data, rob_chr->rob_base.adjust.mat);
}

static void rob_chara_set_hand_adjust(rob_chara* rob_chr,
    rob_chara_data_hand_adjust* adjust, rob_chara_data_hand_adjust* prev_adjust) {
    if (!adjust->enable)
        return;

    float_t chara_scale = rob_chr->rob_base.adjust.scale;
    RobManagement* rob_man = get_rob_management();
    float_t opposite_chara_scale = rob_man->get_adjust_scale(rob_chr->idnm ? ROB_ID_1P : ROB_ID_2P);
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
        adjust->scale = rob_man->get_adjust_scale(ROB_ID_1P);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_2P:
        adjust->scale = rob_man->get_adjust_scale(ROB_ID_2P);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_3P:
        adjust->scale = rob_man->get_adjust_scale(ROB_ID_3P);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_4P:
        adjust->scale = rob_man->get_adjust_scale(ROB_ID_4P);
        break;
    case ROB_CHARA_DATA_HAND_ADJUST_ITEM: // X
        adjust->scale = rob_chr->rob_base.adjust.item_scale;
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
        adjust->current_time += rob_chr->rob_base.robmot.step.f;
    }
}

static void rob_chara_set_hands_adjust(rob_chara* rob_chr) {
    for (int32_t i = 0; i < 2; i++)
        rob_chara_set_hand_adjust(rob_chr, &rob_chr->rob_base.robmot.hand_adjust[i],
            &rob_chr->rob_base.robmot.hand_adjust_prev[i]);
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

static void sub_140406A70(Motion::AshiOidashiColle* a1, prj::sys_vector<RobBlock>& block_vec, const mat4& mat, const vec3& target,
    const BONE_BLK* a5, float_t rotation_blend, float_t arm_length, bool rot) {
    RobBlock* v14 = &block_vec[a5[0]];

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
    v14->solve_ik(1);

    if (a5[1] == BLK_DUMMY)
        return;

    RobBlock* v25 = &block_vec[a5[1]];
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

    if (rot) {
        a5++;
        while (*a5 != BLK_DUMMY)
            block_vec[*a5++].recalc_fk_block(mat, block_vec.data(), true);
    }
    else
        v25->recalc_fk_block(mat, block_vec.data(), false);
}

static void sub_140418A00(rob_chara_bone_data* rob_bone_data, const vec3& target,
    const BONE_BLK* a3, float_t rotation_blend, float_t arm_length, bool rot) {
    motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
    mat4 cur_mat = mot->smooth.base_mtx;
    sub_140406A70(&rob_bone_data->ashi_oidashi, mot->bone_data.block_vec,
        cur_mat, target, a3, rotation_blend, arm_length, rot);
}

static bool sub_14053ACA0(rob_chara* rob_chr, int32_t hand) {
    if (hand >= 2 || !rob_chr->rob_base.robmot.hand_adjust[hand].enable)
        return false;

    float_t rotation_blend = rob_chr->rob_base.robmot.hand_adjust[hand].rotation_blend;
    float_t chara_scale = rob_chr->rob_base.adjust.scale;
    float_t adjust_scale = rob_chr->rob_base.robmot.hand_adjust[hand].current_scale;

    RobAdjust adjust = rob_chr->rob_base.adjust;
    float_t scale = adjust_scale / chara_scale;
    adjust.scale = scale;
    adjust.item_scale = scale; // X
    rob_chr->set_data_adjust_mat(&adjust, false);

    int32_t v15 = 0;
    bool rot = true;
    if (rob_chr->rob_base.robmot.hand_adjust[hand].init_cnt > 0
        && fabsf(rotation_blend - 1.0f) > 0.000001f
        && (fabsf(rob_chr->rob_base.robmot.hand_adjust[hand].offset.x) > 0.000001f
            || fabsf(rob_chr->rob_base.robmot.hand_adjust[hand].offset.y) > 0.000001f
            || fabsf(rob_chr->rob_base.robmot.hand_adjust[hand].offset.z) > 0.000001f)) {
        v15 = rob_chr->rob_base.robmot.hand_adjust[hand].init_cnt;
        rot = false;
    }

    mat4* v40 = rob_chr->bone_data->get_mats_mat(lc_target_arm_tbl[hand]);
    mat4 v42;
    mat4_mul(v40, &adjust.mat, &v42);

    vec3 v37 = rob_chr->rob_base.robmot.hand_adjust[hand].offset * (1.0f - chara_scale / adjust_scale);

    vec3 v38;
    mat4_transform_point(&v42, &v37, &v38);

    vec3 v27 = 0.0f;
    if (rob_chr->rob_base.robmot.hand_adjust[hand].disable_x)
        v27.x = v40->row3.x - v38.x;
    if (rob_chr->rob_base.robmot.hand_adjust[hand].disable_y)
        v27.y = v40->row3.y - v38.y;
    if (rob_chr->rob_base.robmot.hand_adjust[hand].disable_z)
        v27.z = v40->row3.z - v38.z;

    vec3 target = v38 + v27;

    vec3 v18 = 0.0f;
    if (v15 > 0) {
        mat4_transform_point(&v42, &rob_chr->rob_base.robmot.hand_adjust[hand].offset, &v38);
        v18 = v38 + v27;
    }

    sub_140418A00(rob_chr->bone_data, target, recalc_flip_ik_block_tbl[hand], rotation_blend,
        rob_chr->rob_base.robmot.hand_adjust[hand].arm_length, rot);

    while (v15 > 0) {
        mat4* v40 = rob_chr->bone_data->get_mats_mat(lc_target_arm_tbl[hand]);
        mat4_set_translation(v40, &v18);

        vec3 v37 = -rob_chr->rob_base.robmot.hand_adjust[hand].offset;
        mat4_transform_point(v40, &v37, &target);
        if (!--v15) {
            rotation_blend = 1.0f;
            rot = true;
        }

        sub_140418A00(rob_chr->bone_data, target, recalc_flip_ik_block_tbl[hand], rotation_blend,
            rob_chr->rob_base.robmot.hand_adjust[hand].arm_length, rot);
    }

    rob_chr->rob_base.robmot.hand_adjust[hand].target = target;
    return true;
}

// 0x140412860
static void sub_140412860(motion_blend_mot* mot, BONE_BLK block, mat4* out_mat) {
    if (block >= mot->bone_data.block_vec.size())
        return;

    RobBlock* bl = &mot->bone_data.block_vec[block];
    if (!block)
        return;

    switch (bl->ik_type) {
    case IKT_1:
        *out_mat = *bl->node[2].mat_ptr;
        break;
    case IKT_2:
    case IKT_2R:
        *out_mat = *bl->node[3].mat_ptr;
        break;
    }
}

// 0x140419290
static void Motion__get_leaf_matrix(rob_chara_bone_data* rob_bone_data, BONE_BLK blk, mat4* out_mat) {
    sub_140412860(rob_bone_data->motion_loaded.front(), blk, out_mat);
}

static bool sub_14053B580(rob_chara* rob_chr, MotLeafCtrlPart part) {
    if (part < 0 || part >= LCPART_MAX)
        return 0;

    MotLeafCtrl& leaf_ctrl = rob_chr->rob_base.motdata.leaf_ctrl[part];
    int16_t target_id = leaf_ctrl.target_id;
    if (target_id < 0 || target_id >= 27 || !(target_id < LCTAG_OFS || rob_chr->enemy))
        return false;

    switch (leaf_ctrl.mode) {
    case LEAF_CTRL_ON:
        if (rob_chr->rob_base.robmot.frame.f < leaf_ctrl.start_frame)
            return false;
        break;
    case LEAF_CTRL_OFF:
        if (rob_chr->rob_base.robmot.frame.f < leaf_ctrl.start_frame
            || rob_chr->rob_base.robmot.frame.f >= leaf_ctrl.end_frame)
            return false;
        break;
    default:
        return false;
    }

    mat4 mat;
    Motion__get_leaf_matrix(rob_chr->bone_data, lc_part_block_tbl[part], &mat);

    vec3 v13;
    mat4_get_translation(&mat, &v13);

    vec3 v45;
    switch (leaf_ctrl.target_id) {
    case LCTAG_ABS:
        v45 = leaf_ctrl.ofs;
        break;
    case LCTAG_OFS:
        v45 = leaf_ctrl.ofs + v13;
        break;
    case LCTAG_ADJ_EMY: {
        float_t adjust_scale = rob_chr->enemy->bone_data->adjust_scale.base;
        v45 = v13 * adjust_scale;
    } break;
    case LCTAG_ADJ_EMY_BODY: {
        float_t adjust_scale_body = rob_chr->enemy->bone_data->adjust_scale.body;
        v45 = v13 * adjust_scale_body;
    } break;
    case LCTAG_ADJ_EMY_HEIGHT: {
        float_t adjust_scale_height = rob_chr->enemy->bone_data->adjust_scale.height;
        v45 = v13 * adjust_scale_height;
    } break;
    case LCTAG_ADJ_REV: {
        float_t adjust_scale = rob_chr->bone_data->adjust_scale.base;
        v45 = v13 * (1.0f / adjust_scale);
    } break;
    case LCTAG_ADJ_BODY_REV: {
        float_t adjust_scale_body = rob_chr->bone_data->adjust_scale.body;
        v45 = v13 * (1.0f / adjust_scale_body);
    } break;
    case LCTAG_ADJ_HEIGHT_REV: {
        float_t adjust_scale_height = rob_chr->bone_data->adjust_scale.height;
        v45 = v13 * (1.0f / adjust_scale_height);
    } break;
    default:
        if (leaf_ctrl.target_id < LCTAG_OFS) {
            mat4* v26 = rob_chr->enemy->bone_data->get_mats_mat(lc_target_bone_tbl[leaf_ctrl.target_id]);
            mat4_get_translation(v26, &v45);
            v45 += leaf_ctrl.ofs;
        }
        else {
            mat4* v23 = rob_chr->enemy->bone_data->get_mats_mat(lc_target_bone_tbl[leaf_ctrl.target_id]);

            mat4 v49;
            mat4_mul_translate(v23, &leaf_ctrl.ofs, &v49);
            mat4_get_translation(&v49, &v45);
        }
        break;
    }

    float_t v29 = rob_chr->rob_base.robmot.frame.f;
    if (v29 < leaf_ctrl.start_frame)
        v29 = leaf_ctrl.start_frame;
    else if (v29 > leaf_ctrl.end_frame)
        v29 = leaf_ctrl.end_frame;

    float_t v31 = 1.0f;
    float_t v32 = leaf_ctrl.end_frame - leaf_ctrl.start_frame + 1.0f;
    if (fabsf(v32) > 0.000001f) {
        v31 = (leaf_ctrl.end_frame - v29) / v32;
        v31 = clamp_def(v31, 0.0f, 1.0f);
    }

    int32_t v33 = leaf_ctrl.mode;
    vec3 v48 = 0.0f;
    switch (leaf_ctrl.mode) {
    case LEAF_CTRL_ON:
        v48 = vec3::lerp(v45, v13, v31);
        break;
    case LEAF_CTRL_OFF:
        v48 = vec3::lerp(v13, v45, v31);
        break;
    }

    switch (leaf_ctrl.limit) {
    case LCLIMIT_X:
        v48.y = v13.y;
        v48.z = v13.z;
        break;
    case LCLIMIT_Y:
        v48.x = v13.x;
        v48.z = v13.z;
        break;
    case LCLIMIT_Z:
        v48.x = v13.x;
        v48.y = v13.y;
        break;
    case LCLIMIT_XY:
        v48.z = v13.z;
        break;
    case LCLIMIT_XZ:
        v48.y = v13.y;
        break;
    case LCLIMIT_YZ:
        v48.x = v13.z;
        break;
    }

    vec3 v36 = v48 - v13;
    if (leaf_ctrl.min.x != 0.0f && v36.x < leaf_ctrl.min.x)
        v48.x = leaf_ctrl.min.x + v13.x;
    else if (leaf_ctrl.max.x != 0.0f && v36.x > leaf_ctrl.max.x)
        v48.x = leaf_ctrl.max.x + v13.x;

    if (leaf_ctrl.min.y != 0.0f && v36.y < leaf_ctrl.min.y)
        v48.y = leaf_ctrl.min.y + v13.y;
    else if (leaf_ctrl.max.y != 0.0f && v36.y > leaf_ctrl.max.y)
        v48.y = leaf_ctrl.max.y + v13.y;

    if (leaf_ctrl.min.z != 0.0f && v36.z < leaf_ctrl.min.z)
        v48.z = leaf_ctrl.min.z + v13.z;
    else if (leaf_ctrl.max.z != 0.0f && v36.z > leaf_ctrl.max.z)
        v48.z = leaf_ctrl.max.z + v13.z;

    sub_140418A00(rob_chr->bone_data, v48, recalc_ik_block_tbl[part], 0.0f, 0.0f, true);
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
    mat4* v4 = (*a1->block_vec)[BLK_C_KATA_L].node[3].mat_ptr;
    mat4* v5 = (*a1->block_vec)[BLK_C_KATA_R].node[3].mat_ptr;

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
        mat4 mat = *(*a1->block_vec)[BLK_C_KATA_L].node[2].mat_ptr;
        sub_14040AE10(&mat, v42 + v45);
        *(*a1->block_vec)[BLK_C_KATA_L].node[2].mat_ptr = mat;
    }

    if (v19 || v18) {
        mat4 mat = *(*a1->block_vec)[BLK_C_KATA_R].node[2].mat_ptr;
        sub_14040AE10(&mat, v43 + v44);
        *(*a1->block_vec)[BLK_C_KATA_R].node[2].mat_ptr = mat;
    }
}

static void sub_1403F9B20(rob_chara_sleeve_adjust* a1, BONE_BLK blk) {
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
    if (blk == BLK_C_KATA_L) {
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
    else if (blk == BLK_C_KATA_R) {
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

    RobBlock* block_top = a1->block_vec->data();
    RobNode* v18 = block_top[blk].node;
    mat4* v19 = v18[BLK_KL_HARA_ETC].mat_ptr;

    vec3 v39;
    mat4_get_translation(v19, &v39);

    vec3 v37;
    sub_1403F9A40(&v37, &v39, v19, sleeve_cyofs, sleeve_czofs);

    vec3 v40;
    mat4* v23 = block_top[BLK_KL_MUNE_B_WJ].node->mat_ptr;
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

    mat4* v42 = block_top[blk].node[2].mat_ptr;
    sub_14040AE10(v42, v39 + v41);
}

static void sub_1403FAF30(rob_chara_sleeve_adjust* a1, prj::sys_vector<RobBlock>& block_vec, float_t step) {
    a1->step = step;
    a1->block_vec = &block_vec;
    if (a1->enable1)
        sub_1403FA770(a1);

    if (a1->enable2) {
        sub_1403F9B20(a1, BLK_C_KATA_L);
        sub_1403F9B20(a1, BLK_C_KATA_R);
    }
}

static void sub_14041AB50(rob_chara_bone_data* rob_bone_data) {
    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    sub_1403FAF30(&rob_bone_data->sleeve_adjust, v2->bone_data.block_vec, v2->mot_play_data.frame_data.step);
}

static bool rob_chara_hands_adjust(rob_chara* rob_chr) {
    rob_chara_set_hands_adjust(rob_chr);
    bool v2 = false;
    if (sub_14053ACA0(rob_chr, 0) || sub_14053B580(rob_chr, LCPART_TE_L))
        v2 = true;
    if (sub_14053ACA0(rob_chr, 1) || sub_14053B580(rob_chr, LCPART_TE_R))
        v2 = true;
    sub_14041AB50(rob_chr->bone_data);
    return v2;
}

// 0x140406E90
static float_t get_ashi_pos(const RobBlock& bl_momo, const RobBlock& bl_toe, float_t hh, vec3& pos) {
    vec3 asi_pos = 0.0f;
    mat4_transform_point(bl_momo.node[3].mat_ptr, &asi_pos, &asi_pos);
    pos = asi_pos;

    vec3 toe_pos(0.01f, -0.05f, 0.0f);
    mat4_transform_point(bl_toe.node[0].mat_ptr, &toe_pos, &toe_pos);
    return min_def(asi_pos.y - hh, toe_pos.y);
}

// 0x140406920
void Motion::AshiOidashiColle::calc_sub(RobBlock& bl_momo, const RobBlock& bl_toe,
    float_t hh, Motion::AshiOidashi& ashi, float_t step) {
    vec3 ashi_pos;
    float_t toe_pos_y = get_ashi_pos(bl_momo, bl_toe, hh, ashi_pos);

    float_t y = task_stage_current_get_floor_height(ashi_pos, hh);
    if (ashi.y_old != y) {
        ashi.y_bef = ashi.y_old;
        ashi.scale = 0.0f;
    }
    ashi.y_old = y;

    ashi.scale += step * 10.0f;
    if (ashi.scale > 1.0f) {
        ashi.y_bef = y;
        ashi.scale = 1.0f;
    }

    float_t y_diff = y - toe_pos_y;
    float_t y_bef_diff = ashi.y_bef - toe_pos_y;
    if (y_diff < 0.0f)
        y_diff = 0.0f;
    if (y_bef_diff < 0.0f)
        y_bef_diff = 0.0f;

    bl_momo.leaf_pos[0].x = ashi_pos.x;
    bl_momo.leaf_pos[0].y = ashi_pos.y + lerp_def(y_bef_diff, y_diff, ashi.scale);
    bl_momo.leaf_pos[0].z = ashi_pos.z;
}

// 0x1404065B0
void Motion::AshiOidashiColle::calc(prj::sys_vector<RobBlock>& block_vec, const mat4& cur_mat,
    float_t step, BONE_KIND kind, const BONE_BLK* c_momo_l_ik_blk, const BONE_BLK* c_momo_r_ik_blk) {
    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;

    const RobBlock* block_top = block_vec.data();
    const float_t* hh = aft_bone_data->get_heel_height(kind);
    if (!hh)
        return;

    calc_sub(block_vec[c_momo_l_ik_blk[0]], block_vec[BLK_KL_TOE_L_WJ], *hh, l, step);
    calc_sub(block_vec[c_momo_r_ik_blk[0]], block_vec[BLK_KL_TOE_R_WJ], *hh, r, step);

    RobBlock& bl_momo_l = block_vec[c_momo_l_ik_blk[0]];
    bl_momo_l.chain_rot[1] = mat4_identity;
    bl_momo_l.chain_rot[2] = mat4_identity;
    bl_momo_l.solve_ik(1);

    RobBlock& bl_momo_r = block_vec[c_momo_r_ik_blk[0]];
    bl_momo_r.chain_rot[1] = mat4_identity;
    bl_momo_r.chain_rot[2] = mat4_identity;
    bl_momo_r.solve_ik(1);

    c_momo_l_ik_blk++;
    while (*c_momo_l_ik_blk != BLK_DUMMY)
        block_vec[*c_momo_l_ik_blk++].recalc_fk_block(cur_mat, block_top, true);

    c_momo_r_ik_blk++;
    while (*c_momo_r_ik_blk != BLK_DUMMY)
        block_vec[*c_momo_r_ik_blk++].recalc_fk_block(cur_mat, block_top, true);
}

static void sub_140418810(rob_chara_bone_data* rob_bone_data, const BONE_BLK* c_momo_l_ik_blk, const BONE_BLK* c_momo_r_ik_blk) {
    motion_blend_mot* mot = rob_bone_data->motion_loaded.front();
    mat4 cur_mat = mot->smooth.base_mtx;
    rob_bone_data->ashi_oidashi.calc(
        mot->bone_data.block_vec, cur_mat, mot->mot_play_data.frame_data.step,
        rob_bone_data->disp_kind, c_momo_l_ik_blk, c_momo_r_ik_blk);
}

static void sub_14053B260(rob_chara* rob_chr) {
    if (rob_chr->rob_base.motdata.motkind[MK_CATCH_SABAKI_DISABLE])
        sub_140418810(rob_chr->bone_data, recalc_ik_block_tbl[3], recalc_ik_block_tbl[2]);
}

static void rob_chara_set_hand_l_object(rob_chara* rob_chr, object_info obj_info, int32_t type) {
    switch (type) {
    case 0:
    default:
        rob_chr->rob_base.robmot.field_150.hand_l_object = obj_info;
        break;
    case 1:
        rob_chr->rob_base.robmot.hand_l_object = obj_info;
        break;
    case 2:
        rob_chr->rob_base.robmot.field_3B0.hand_l_object = obj_info;
        break;
    }
}

static void rob_chara_set_hand_r_object(rob_chara* rob_chr, object_info obj_info, int32_t type) {
    switch (type) {
    case 0:
    default:
        rob_chr->rob_base.robmot.field_150.hand_r_object = obj_info;
        break;
    case 1:
        rob_chr->rob_base.robmot.hand_r_object = obj_info;
        break;
    case 2:
        rob_chr->rob_base.robmot.field_3B0.hand_r_object = obj_info;
        break;
    }
}

static void rob_chara_age_age_ctrl(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, mat4& mat) {
    arr[rob_id * 3 + part].ctrl(mat);
}

static void rob_chara_age_age_disp(rob_chara_age_age* arr,
    render_context* rctx, int32_t rob_id, bool reflect, bool chara_color) {
    bool npr = !!rctx->render_manager->npr_param;
    mat4& view = rctx->camera->view;
    vec3 v11 = { view.row0.z, view.row1.z, view.row2.z };
    arr[rob_id * 3 + 0].disp(rctx, rob_id, npr, reflect, v11, chara_color);
    arr[rob_id * 3 + 1].disp(rctx, rob_id, npr, reflect, v11, chara_color);
    arr[rob_id * 3 + 2].disp(rctx, rob_id, npr, reflect, v11, chara_color);
}

static void rob_chara_age_age_load(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part) {
    if (part != 1 && part != 2)
        return;

    rob_chara_age_age_data data[10];
    for (rob_chara_age_age_data& i : data) {
        i.index = (int32_t)(&i - data);
        i.remaining = -1.0f;
        i.part = part;
    }

    arr[rob_id * 3 + part].load({ 0, 3291 }, data, 10);
}

static void rob_chara_age_age_reset(rob_chara_age_age* arr, int32_t rob_id) {
    arr[rob_id * 3 + 0].reset();
    arr[rob_id * 3 + 1].reset();
    arr[rob_id * 3 + 2].reset();
}

static void rob_chara_age_age_set_alpha(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, float_t value) {
    arr[rob_id * 3 + part].set_alpha(value);
}

static void rob_chara_age_age_set_disp(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, bool value) {
    arr[rob_id * 3 + part].set_disp(value);
}

static void rob_chara_age_age_set_move_cancel(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, float_t value) {
    arr[rob_id * 3 + part].set_move_cancel(value);
}

static void rob_chara_age_age_set_npr(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, bool value) {
    arr[rob_id * 3 + part].set_npr(value);
}

static void rob_chara_age_age_set_speed(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, float_t value) {
    arr[rob_id * 3 + part].set_speed(value);
}

static void rob_chara_age_age_set_skip(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part) {
    arr[rob_id * 3 + part].set_skip();
}

static void rob_chara_age_age_set_step(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part, float_t value) {
    arr[rob_id * 3 + part].set_step(value);
}

static void rob_chara_age_age_set_step_full(rob_chara_age_age* arr,
    int32_t rob_id, int32_t part) {
    arr[rob_id * 3 + part].set_step_full();
}

static void rob_chara_bone_data_calculate_bones(rob_chara_bone_data* rob_bone_data,
    const std::vector<BODYTYPE>* body_type_table) {
    bone_database_bones_calculate_count(body_type_table, rob_bone_data->mat_max,
        rob_bone_data->block_max, rob_bone_data->node_max,
        rob_bone_data->leaf_pos_max, rob_bone_data->chain_pos_max);
}

static void rob_chara_bone_data_get_adjust_scale(
    rob_chara_bone_data* rob_bone_data, const bone_database* bone_data) {
    if (rob_bone_data->motion_loaded.size() < 0)
        return;

    motion_blend_mot* v2 = rob_bone_data->motion_loaded.front();
    rob_chara_bone_data_adjust_scale_calculate(&rob_bone_data->adjust_scale, v2->bone_data.block_vec,
        rob_bone_data->kind, rob_bone_data->disp_kind, bone_data);
    float_t base = rob_bone_data->adjust_scale.base;
    v2->smooth.field_C0 = base;
    v2->smooth.field_C4 = base;
    v2->smooth.field_C8 = 0.0f;
}

static mat4* rob_chara_bone_data_get_mat(rob_chara_bone_data* rob_bone_data, size_t index) {
    RobNode* node = rob_chara_bone_data_get_node(rob_bone_data, index);
    if (node)
        return node->mat_ptr;
    return 0;
}

static RobNode* rob_chara_bone_data_get_node(rob_chara_bone_data* rob_bone_data, size_t index) {
    if (index < rob_bone_data->node_vec.size())
        return &rob_bone_data->node_vec[index];
    return 0;
}

static void rob_chara_bone_data_adjust_scale_calculate(
    rob_chara_bone_data_adjust_scale* adjust_scale, prj::sys_vector<RobBlock>& block_vec,
    BONE_KIND kind, BONE_KIND disp_kind, const bone_database* bone_data) {
    const float_t* heel_height = bone_data->get_heel_height(kind);
    const float_t* disp_heel_height = bone_data->get_heel_height(disp_kind);
    if (!heel_height || !disp_heel_height)
        return;

    RobBlock* b_cl_mune = &block_vec[BLK_CL_MUNE];
    RobBlock* b_kl_kubi = &block_vec[BLK_KL_KUBI];
    RobBlock* b_c_kata_l = &block_vec[BLK_C_KATA_L];
    RobBlock* b_cl_momo_l = &block_vec[BLK_CL_MOMO_L];

    float_t height = fabsf(b_cl_momo_l->chain_pos[0].y)
        + b_cl_momo_l->len[0][0] + b_cl_momo_l->len[1][0] + *heel_height;
    float_t disp_height = fabsf(b_cl_momo_l->chain_pos[1].y)
        + b_cl_momo_l->len[0][1] + b_cl_momo_l->len[1][1] + *disp_heel_height;
    adjust_scale->base = disp_height / height;
    adjust_scale->body
        = (fabsf(b_kl_kubi->chain_pos[1].y) + b_cl_mune->len[0][1])
        / (fabsf(b_kl_kubi->chain_pos[0].y) + b_cl_mune->len[0][0]);
    adjust_scale->arm = (b_c_kata_l->len[0][1] + b_c_kata_l->len[1][1])
        / (b_c_kata_l->len[0][0] + b_c_kata_l->len[1][0]);
    adjust_scale->height
        = (fabsf(b_kl_kubi->chain_pos[1].y) + b_cl_mune->len[0][1] + disp_height)
        / (fabsf(b_kl_kubi->chain_pos[0].y) + b_cl_mune->len[0][0] + height);
}

static bool motion_blend_mot_check(BONE_BLK blk) {
    return true;
}

static bool partial_motion_blend_mot_face_check(BONE_BLK blk) {
    return blk >= BLK_FACE_ROOT && blk <= BLK_TL_TOOTH_UPPER_WJ;
}

static bool partial_motion_blend_mot_hand_l_check(BONE_BLK blk) {
    return blk >= BLK_N_HITO_L_EX && blk <= BLK_NL_OYA_C_L_WJ;
}

static bool partial_motion_blend_mot_hand_r_check(BONE_BLK blk) {
    return blk >= BLK_N_HITO_R_EX && blk <= BLK_NL_OYA_C_R_WJ;
}

static bool partial_motion_blend_mot_mouth_check(BONE_BLK blk) {
    return blk >= BLK_N_AGO && blk <= BLK_TL_TOOTH_UNDER_WJ
        || blk >= BLK_N_KUTI_D && blk <= BLK_TL_KUTI_U_R_WJ
        || blk >= BLK_N_TOOTH_UPPER && blk <= BLK_TL_TOOTH_UPPER_WJ;
}

static bool partial_motion_blend_mot_eyes_check(BONE_BLK blk) {
    return blk >= BLK_N_EYE_L && blk <= BLK_KL_HIGHLIGHT_R_WJ;
}

static bool partial_motion_blend_mot_eyelid_check(BONE_BLK blk) {
    if (blk >= BLK_N_MABU_L_D_A && blk <= BLK_TL_MABU_R_U_C_WJ
        || blk >= BLK_N_EYELID_L_A && blk <= BLK_TL_EYELID_R_B_WJ)
        return true;
    return partial_motion_blend_mot_eyes_check(blk);
}

static void rob_chara_bone_data_init_data(rob_chara_bone_data* rob_bone_data,
    BONE_KIND base_type, BONE_KIND type, const bone_database* bone_data) {
    rob_chara_bone_data_init_skeleton(rob_bone_data, base_type, type, bone_data);
    for (motion_blend_mot*& i : rob_bone_data->motions)
        i->init(rob_bone_data, motion_blend_mot_check, bone_data);

    size_t block_max = rob_bone_data->block_max;
    rob_bone_data->face.init(base_type, partial_motion_blend_mot_face_check, block_max, bone_data);
    rob_bone_data->hand_l.init(base_type, partial_motion_blend_mot_hand_l_check, block_max, bone_data);
    rob_bone_data->hand_r.init(base_type, partial_motion_blend_mot_hand_r_check, block_max, bone_data);
    rob_bone_data->mouth.init(base_type, partial_motion_blend_mot_mouth_check, block_max, bone_data);
    rob_bone_data->eyes.init(base_type, partial_motion_blend_mot_eyes_check, block_max, bone_data);
    rob_bone_data->eyelid.init(base_type, partial_motion_blend_mot_eyelid_check, block_max, bone_data);
}

static void rob_chara_bone_data_init_skeleton(rob_chara_bone_data* rob_bone_data,
    BONE_KIND kind, BONE_KIND disp_kind, const bone_database* bone_data) {
    if (rob_bone_data->kind == kind
        && rob_bone_data->disp_kind == disp_kind)
        return;

    const std::vector<BODYTYPE>* body_type_table = bone_data->get_body_type_table(kind);
    const std::vector<uint16_t>* node_parent_table = bone_data->get_node_parent_table(kind);
    const std::vector<std::string>* bone_node_name_table = bone_data->get_bone_node_name_table(kind);
    if (!body_type_table || !node_parent_table || !bone_node_name_table)
        return;

    rob_chara_bone_data_calculate_bones(rob_bone_data, body_type_table);
    rob_chara_bone_data_reserve(rob_bone_data);
    rob_chara_bone_data_set_mats(rob_bone_data, body_type_table, bone_node_name_table->data());
    rob_chara_bone_data_set_parent_mats(rob_bone_data, node_parent_table->data());
    rob_bone_data->kind = kind;
    rob_bone_data->disp_kind = disp_kind;
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

static bool sub_140413630(MotionSmooth* a1) {
    return (a1->field_4 & 2) != 0;
}

static bool sub_140413790(MotionSmooth* a1) {
    return (a1->field_0 & 2) != 0;
}

static bool sub_1404137A0(MotionSmooth* a1) {
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
    if (!a1->smooth.root_xzpos)
        v7.field_0 = !sub_140413630(&a1->smooth);
    if (!a1->smooth.root_ypos)
        v7.field_2 = !sub_140413790(&a1->smooth);
    v7.field_1 = sub_1404137A0(&a1->smooth);
    v7.frame = a1->mot_key_data.frame;
    v7.move_yang = a1->smooth.move_yang;
    v7.move_yang_bef = a1->smooth.move_yang_bef;
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
    v3.move_yang = 0.0f;
    v3.move_yang_bef = 0.0f;
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
    rob_bone_data->mat_vec.clear();
    rob_bone_data->mat_vec.resize(rob_bone_data->mat_max);

    for (mat4& i : rob_bone_data->mat_vec)
        i = mat4_identity;

    rob_bone_data->mat2_vec.clear();
    rob_bone_data->mat2_vec.resize(rob_bone_data->node_max - rob_bone_data->mat_max);

    for (mat4& i : rob_bone_data->mat2_vec)
        i = mat4_identity;

    rob_bone_data->node_vec.clear();
    rob_bone_data->node_vec.resize(rob_bone_data->node_max);
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

static void rob_chara_bone_data_motion_load(rob_chara_bone_data* rob_bone_data, uint32_t motnum,
    MotionBlendType blend_type, const bone_database* bone_data, const motion_database* mot_db) {
    if (!rob_bone_data->motion_loaded.size())
        return;

    if (blend_type == MOTION_BLEND_FREEZE) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        rob_bone_data->motion_loaded.front()->load_file(
            motnum, MOTION_BLEND_FREEZE, 1.0f, bone_data, mot_db);
        sub_14041BA60(rob_bone_data);
        return;
    }

    if (blend_type != MOTION_BLEND_CROSS) {
        if (blend_type == MOTION_BLEND_COMBINE)
            blend_type = MOTION_BLEND;
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        rob_bone_data->motion_loaded.front()->load_file(
            motnum, blend_type, 1.0f, bone_data, mot_db);
        return;
    }
    rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 2);

    if (rob_bone_data->motion_indices.size()) {
        rob_chara_bone_data_motion_blend_mot_list_init(rob_bone_data);
        rob_bone_data->motion_loaded.front()->copy_rot_trans();
        rob_bone_data->motion_loaded.front()->load_file(
            motnum, MOTION_BLEND_CROSS, 1.0f, bone_data, mot_db);
        return;
    }

    motion_blend_mot* mot = new motion_blend_mot();
    mot->init(rob_bone_data, mot->enabled_bones.func, bone_data);
    if (!mot) {
        rob_chara_bone_data_motion_blend_mot_list_free(rob_bone_data, 1);
        sub_14041AE40(rob_bone_data);
        rob_bone_data->motion_loaded.front()->load_file(
            motnum, MOTION_BLEND_FREEZE, 1.0f, bone_data, mot_db);
        sub_14041BA60(rob_bone_data);
        return;
    }

    rob_bone_data->motion_loaded.push_back(mot);
    rob_bone_data->motion_loaded_indices.push_back(rob_bone_data->motions.size());
    rob_bone_data->motion_loaded.front()->copy_rot_trans();
    rob_bone_data->motion_loaded.front()->load_file(
        motnum, MOTION_BLEND_CROSS, 1.0f, bone_data, mot_db);
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
        RobBlock* block_top = i->bone_data.block_vec.data();
        block_top[BLK_KL_EYE_L].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        block_top[BLK_KL_EYE_L].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
        block_top[BLK_KL_EYE_R].eyes_xrot_adjust_neg = eyes_xrot_adjust_neg;
        block_top[BLK_KL_EYE_R].eyes_xrot_adjust_pos = eyes_xrot_adjust_pos;
    }

    rob_bone_data->look_anim.set_eyes_xrot_adjust(eyes_xrot_adjust_neg, eyes_xrot_adjust_pos);
}

static void rob_chara_bone_data_set_mats(rob_chara_bone_data* rob_bone_data,
    const std::vector<BODYTYPE>* body_type_table, const std::string* bone_node_name_table) {
    size_t node = 0;
    size_t mat = 0;
    size_t mat2 = 0;
    size_t leaf_pos = 0;
    size_t chain_pos = 0;

    RobNode* n = rob_bone_data->node_vec.data();
    for (const BODYTYPE& i : *body_type_table) {
        chain_pos++;

        switch (i.ik_type) {
        case IKT_0:
        case IKT_0N:
        case IKT_0T:
            n->mat_ptr = &rob_bone_data->mat_vec[mat++];
            n->name = bone_node_name_table[node].c_str();
            n++;
            break;
        case IKT_ROOT:
            n->mat_ptr = &rob_bone_data->mat_vec[mat++];
            n->name = bone_node_name_table[node].c_str();
            n++;

            leaf_pos++;
            break;
        case IKT_1:
            n->mat_ptr = &rob_bone_data->mat2_vec[mat2++];
            n->name = bone_node_name_table[node].c_str();
            n++;

            n->mat_ptr = &rob_bone_data->mat_vec[mat++];
            n->name = bone_node_name_table[node + 1].c_str();
            n++;

            n->mat_ptr = &rob_bone_data->mat2_vec[mat2++];
            n->name = bone_node_name_table[node + 2].c_str();
            n++;

            chain_pos++;
            leaf_pos++;
            node += 2;
            break;
        case IKT_2:
        case IKT_2R:
            n->mat_ptr = &rob_bone_data->mat_vec[mat++];
            n->name = bone_node_name_table[node].c_str();
            n++;

            n->mat_ptr = &rob_bone_data->mat_vec[mat++];
            n->name = bone_node_name_table[node + 1].c_str();
            n++;

            n->mat_ptr = &rob_bone_data->mat_vec[mat++];
            n->name = bone_node_name_table[node + 2].c_str();
            n++;

            n->mat_ptr = &rob_bone_data->mat2_vec[mat2++];
            n->name = bone_node_name_table[node + 3].c_str();
            n++;

            chain_pos += 2;
            leaf_pos++;
            node += 3;
            break;
        }
        node++;
    }

    for (RobNode& i : rob_bone_data->node_vec)
        i.no_scale_mat = i.mat_ptr;

    if (node != rob_bone_data->node_max)
        printf_debug_error("Node mismatch");
    if (mat != rob_bone_data->mat_max)
        printf_debug_error("Matrix mismatch");
    if (mat2 != rob_bone_data->node_max - rob_bone_data->mat_max)
        printf_debug_error("Matrix2 mismatch");
    if (leaf_pos != rob_bone_data->leaf_pos_max)
        printf_debug_error("LeafPos mismatch");
    if (chain_pos != rob_bone_data->chain_pos_max)
        printf_debug_error("ChainPos mismatch");
    rob_bone_data->field_1 = true;
}

static void rob_chara_bone_data_set_parent_mats(rob_chara_bone_data* rob_bone_data,
    const uint16_t* node_parent_table) {
    if (rob_bone_data->node_vec.size() < 1)
        return;

    size_t node = 1;
    RobNode* i_begin = rob_bone_data->node_vec.data();
    RobNode* i_begin_1 = rob_bone_data->node_vec.data() + 1;
    RobNode* i_end = rob_bone_data->node_vec.data() + rob_bone_data->node_vec.size();
    for (RobNode* i = i_begin_1; i != i_end; i++)
        i->parent = &i_begin[node_parent_table[node++]];
}

static void rob_chara_bone_data_set_yrot(rob_chara_bone_data* rob_bone_data, float_t value) {
    rob_bone_data->motion_loaded.front()->bone_data.yrot = value;
}

static void rob_chara_bone_data_set_step(rob_chara_bone_data* rob_bone_data, float_t value) {
    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        i->set_step(value);
}

static void sub_140413EB0(MotionSmooth* a1) {
    a1->base_mtx_set = false;
    a1->base_mtx_bef = a1->base_mtx;
    a1->base_mtx = mat4_identity;
}

static void sub_1404117F0(motion_blend_mot* a1) {
    sub_140413EB0(&a1->smooth);
    int32_t motion_body_type = a1->mot_key_data.motion_body_type;
    for (RobBlock& i : a1->bone_data.block_vec)
        i.get_mat(motion_body_type);
}

static void sub_14040FBF0(motion_blend_mot* a1, float_t a2) {
    RobBlock* b_n_hara_cp = &a1->bone_data.block_vec[BLK_N_HARA_CP];
    RobBlock* b_kg_hara_y = &a1->bone_data.block_vec[BLK_KG_HARA_Y];
    RobBlock* b_kl_hara_xz = &a1->bone_data.block_vec[BLK_KL_HARA_XZ];
    RobBlock* b_kl_hara_etc = &a1->bone_data.block_vec[BLK_KL_HARA_ETC];
    a1->smooth.field_90 = 0.0f;

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

    float_t v8 = a1->smooth.field_C0;
    float_t v9 = a1->smooth.field_C4;
    vec3 v10 = a1->smooth.field_C8;
    a1->smooth.field_A8 = b_n_hara_cp->leaf_pos[1];
    if (!a1->mot_key_data.motion_body_type) {
        if (a2 != v9) {
            b_n_hara_cp->leaf_pos[1].x = (b_n_hara_cp->leaf_pos[1].x - v10.x) * v9 + v10.x;
            b_n_hara_cp->leaf_pos[1].z = (b_n_hara_cp->leaf_pos[1].z - v10.z) * v9 + v10.z;
        }
        b_n_hara_cp->leaf_pos[1].y = ((b_n_hara_cp->leaf_pos[1].y - v10.y) * v8) + v10.y;
    }
    else {
        if (a2 != v9) {
            v9 /= a2;
            b_n_hara_cp->leaf_pos[1].x = (b_n_hara_cp->leaf_pos[1].x - v10.x) * v9 + v10.x;
            b_n_hara_cp->leaf_pos[1].z = (b_n_hara_cp->leaf_pos[1].z - v10.z) * v9 + v10.z;
        }

        if (a2 != v8) {
            v8 /= a2;
            b_n_hara_cp->leaf_pos[1].y = (b_n_hara_cp->leaf_pos[1].y - v10.y) * v8 + v10.y;
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

    v3->Field_20(&a1->bone_data.block_vec, a2);

    for (RobBlock& i : a1->bone_data.block_vec) {
        if (!a1->enabled_bones.arr[i.block_id])
            continue;

        RobBlock* v6 = 0;
        if (a2)
            v6 = &a2->data()[i.block_id];
        v3->Blend(&a1->bone_data.block_vec[i.block_id], v6);
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
        sub_140410A40(*i, &(*v4)->bone_data.block_vec, &(*i)->bone_data.block_vec);
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
            bones = &(*v5)->bone_data.block_vec;
        }
        else if ((*j)->get_type() != MOTION_BLEND_FREEZE)
            continue;

        sub_140410B70(*j, bones);
    }

    sub_140410CB0(&rob_bone_data->face, &v3->bone_data.block_vec);
    sub_140410CB0(&rob_bone_data->hand_l, &v3->bone_data.block_vec);
    sub_140410CB0(&rob_bone_data->hand_r, &v3->bone_data.block_vec);
    sub_140410CB0(&rob_bone_data->mouth, &v3->bone_data.block_vec);
    sub_140410CB0(&rob_bone_data->eyes, &v3->bone_data.block_vec);
    sub_140410CB0(&rob_bone_data->eyelid, &v3->bone_data.block_vec);

    RobBlock* v7 = &v3->bone_data.block_vec[0];
    v3->smooth.field_9C = v7->leaf_pos[1];
    if (sub_140413790(&v3->smooth)) { // WTF???
        v3->smooth.field_90 = v7->leaf_pos[1];
        v7->leaf_pos[1] -= v7->leaf_pos[1];
    }
}

void sub_14041B9F0(rob_chara_bone_data* rob_bone_data) {
    for (motion_blend_mot*& i : rob_bone_data->motion_loaded)
        i->enabled_bones.check();

    rob_bone_data->face.enabled_bones.check();
    rob_bone_data->eyelid.enabled_bones.check();
}

static float_t sub_140512F60(RobDisp* disp) {
    const mat4* motion_matrix = disp->motion_matrix;
    float_t y = 1.0f;
    y = min_def(y, motion_matrix[BONE_ID_N_HARA_CP     ].row3.y);
    y = min_def(y, motion_matrix[BONE_ID_KL_ASI_L_WJ_CO].row3.y);
    y = min_def(y, motion_matrix[BONE_ID_KL_ASI_R_WJ_CO].row3.y);
    y = min_def(y, motion_matrix[BONE_ID_KL_TE_L_WJ    ].row3.y);
    y = min_def(y, motion_matrix[BONE_ID_KL_TE_R_WJ    ].row3.y);
    return y;
}

static void rob_chara_add_motion_reset_data(rob_chara* rob_chr,
    const uint32_t& motnum, const float_t& frame, int32_t init_cnt) {
    if (motnum != -1)
        rob_chr->disp->add_motion_reset_data(motnum, frame, init_cnt);
}

static void rob_chara_age_age_ctrl(rob_chara* rob_chr, int32_t part, const char* name) {
    mat4 mat = *rob_chr->disp->get_ex_data_bone_node_mat(RPK_KAMI, name);
    if (vec3::length(rob_chr->rob_base.adjust.pos_adjust) > 0.000001f) {
        mat4 temp;
        mat4_translate(&rob_chr->rob_base.adjust.pos_adjust, &temp);
        mat4_mul(&mat, &temp, &mat);
    }
    rob_chara_age_age_array_ctrl(rob_chr->idnm, part, mat);
}

static object_info rob_chara_get_head_object(rob_chara* rob_chr, int32_t head_object_id) {
    if (head_object_id < 0 || head_object_id > 8)
        return {};
    object_info obj_info = rob_chr->item.get_head_object_replace(head_object_id);
    if (obj_info.not_null())
        return obj_info;
    return rob_chr->rob_data->head_objects[head_object_id];
}

static object_info rob_chara_get_object_info(rob_chara* rob_chr, ROB_PARTS_KIND rpk) {
    return rob_chr->disp->get_objid(rpk);
}

static void rob_chara_load_default_motion(rob_chara* rob_chr,
    const bone_database* bone_data, const motion_database* mot_db) {
    uint32_t motnum = rob_chr->get_common_mot(MTP_NONE);
    rob_chara_load_default_motion_sub(rob_chr, 1, motnum, bone_data, mot_db);
}

static void sub_140419820(rob_chara_bone_data* rob_bone_data, int32_t target) {
    rob_bone_data->motion_loaded.front()->get_smooth_target(target);
}

static void rob_chara_load_default_motion_sub(rob_chara* rob_chr, int32_t motion_body_type,
    uint32_t motnum, const bone_database* bone_data, const motion_database* mot_db) {
    rob_chr->bone_data->load_face_motion(-1, mot_db);
    rob_chr->bone_data->load_hand_l_motion(-1, mot_db);
    rob_chr->bone_data->load_hand_r_motion(-1, mot_db);
    rob_chr->bone_data->load_mouth_motion(-1, mot_db);
    rob_chr->bone_data->load_eyes_motion(-1, mot_db);
    rob_chr->bone_data->load_eyelid_motion(-1, mot_db);
    rob_chara_bone_data_motion_load(rob_chr->bone_data, motnum, MOTION_BLEND_FREEZE, bone_data, mot_db);
    rob_chr->bone_data->set_frame(0.0f);
    sub_14041C680(rob_chr->bone_data, false);
    sub_14041C9D0(rob_chr->bone_data, false);
    sub_14041D2D0(rob_chr->bone_data, false);
    sub_14041BC40(rob_chr->bone_data, false);
    sub_14041D270(rob_chr->bone_data, 0.0f);
    sub_14041D2A0(rob_chr->bone_data, 0.0f);
    rob_chara_bone_data_set_yrot(rob_chr->bone_data, 0.0f);
    rob_chr->bone_data->set_motion_blend_duration(0.0f, 1.0f, 1.0f);
    sub_14041D310(rob_chr->bone_data, 0.0f, 0.0f, 2);
    rob_chr->bone_data->interpolate();
    rob_chr->bone_data->update(0);
    rob_chr->sub_140509D30();
    sub_140419820(rob_chr->bone_data, motion_body_type);
}

static bool rob_chara_data_adjust_compare(rob_chara_data_adjust* left, rob_chara_data_adjust* right) {
    return right->motnum == left->motnum
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

static void opd_chara_data_array_add_frame_data(ROB_ID rob_id) {
    opd_chara_data_array_get(rob_id)->add_frame_data();
}

static void opd_chara_data_array_encode_data(ROB_ID rob_id) {
    opd_chara_data_array_get(rob_id)->encode_data();
}

static void opd_chara_data_array_encode_init_data(ROB_ID rob_id, uint32_t motnum) {
    opd_chara_data_array_get(rob_id)->encode_init_data(motnum);
}

static void opd_chara_data_array_fs_copy_file(ROB_ID rob_id) {
    opd_chara_data_array_get(rob_id)->fs_copy_file();
}

static opd_chara_data* opd_chara_data_array_get(ROB_ID rob_id) {
    if (rob_id < 0 || rob_id >= ROB_ID_MAX)
        rob_id = ROB_ID_1P;
    return &opd_chara_data_array[rob_id];
}

static void opd_chara_data_array_init_data(ROB_ID rob_id, uint32_t motnum) {
    opd_chara_data_array_get(rob_id)->init_data(motnum);
}

static void opd_chara_data_array_open_opd_file(ROB_ID rob_id) {
    opd_chara_data_array_get(rob_id)->open_opd_file();
}

static void opd_chara_data_array_open_opdi_file(ROB_ID rob_id) {
    opd_chara_data_array_get(rob_id)->open_opdi_file();
}

static void opd_chara_data_array_write_file(ROB_ID rob_id) {
    opd_chara_data_array_get(rob_id)->write_file();
}

static const RobTaskList* get_rob_manager_list_before(TaskRobManager* task_rob_mgr) {
    static RobTaskList list_before[] = {
        RobTaskList(&task_rob_mgr->task_rob_prepare_ctrl, "ROB_PREPARE_CONTROL"),
        RobTaskList(),
    };

    return list_before;
}

static const RobTaskList* get_rob_manager_list_after(TaskRobManager* task_rob_mgr) {
    static RobTaskList list_after[] = {
        RobTaskList(&task_rob_mgr->task_rob_prepare_act    , "ROB_PREPARE_ACTION" ),
        RobTaskList(&task_rob_mgr->task_rob_base           , "ROB_BASE"           ),
        RobTaskList(&task_rob_mgr->task_rob_motion_modifier, "ROB_MOTION_MODIFIER"),
        RobTaskList(&task_rob_mgr->task_rob_collision      , "ROB_COLLISION"      ),
        RobTaskList(&task_rob_mgr->task_rob_info           , "ROB_INFO"           ),
        RobTaskList(&task_rob_mgr->task_rob_disp           , "ROB_DISP"           ),
        RobTaskList(),
    };

    return list_after;
}

RobManagement* get_rob_management() {
    return &s_rob_management;
}

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

void opd_make_manager_open(const OpdMakeManagerArgs& args) {
    opd_make_manager->open(args);
}

bool opd_make_manager_check_alive() {
    return opd_make_manager->check_alive();
}

bool opd_make_manager_close() {
    return opd_make_manager->close();
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
    opd_make_manager->open(args);
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
    opd_make_manager_close();
}

bool osage_play_data_manager_open() {
    return osage_play_data_manager->open();
}

void osage_play_data_manager_append_chara_motion_id(rob_chara* rob_chr, uint32_t motnum) {
    std::vector<uint32_t> motion_ids;
    motion_ids.push_back(motnum);
    osage_play_data_manager->AppendCharaMotionId(rob_chr, motion_ids);
}

void osage_play_data_manager_append_chara_motion_ids(
    rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids) {
    osage_play_data_manager->AppendCharaMotionId(rob_chr, motion_ids);
}

bool osage_play_data_manager_check_alive() {
    return osage_play_data_manager->CheckTaskReady();
}

void osage_play_data_manager_get_opd_file_data(object_info obj_info,
    uint32_t motnum, const float_t*& data, uint32_t& count) {
    osage_play_data_manager->GetOpdFileData(obj_info, motnum, data, count);
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

void pv_osage_manager_array_reset(ROB_ID rob_id) {
    pv_osage_manager_array_get(rob_id)->Reset();
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

void rob_chara_age_age_array_ctrl(int32_t rob_id, int32_t part, mat4& mat) {
    rob_chara_age_age_ctrl(rob_chara_age_age_array, rob_id, part, mat);
}

void rob_chara_age_age_array_disp(render_context* rctx,
    int32_t rob_id, bool reflect, bool chara_color) {
    mdl::ObjFlags obj_flags = rctx->disp_manager->get_obj_flags();
    rob_chara_age_age_disp(rob_chara_age_age_array, rctx, rob_id, reflect, chara_color);
    rctx->disp_manager->set_obj_flags(obj_flags);
}

void rob_chara_age_age_array_load(int32_t rob_id, int32_t part) {
    rob_chara_age_age_load(rob_chara_age_age_array, rob_id, part);
}

void rob_chara_age_age_array_reset(int32_t rob_id) {
    rob_chara_age_age_reset(rob_chara_age_age_array, rob_id);
}

void rob_chara_age_age_array_set_alpha(int32_t rob_id, int32_t part, float_t alpha) {
    rob_chara_age_age_set_alpha(rob_chara_age_age_array, rob_id, part, alpha);
}

void rob_chara_age_age_array_set_disp(int32_t rob_id, int32_t part, bool value) {
    rob_chara_age_age_set_disp(rob_chara_age_age_array, rob_id, part, value);
}

void rob_chara_age_age_array_set_move_cancel(int32_t rob_id, int32_t part, float_t value) {
    rob_chara_age_age_set_move_cancel(rob_chara_age_age_array, rob_id, part, value);
}

void rob_chara_age_age_array_set_npr(int32_t rob_id, int32_t part, bool value) {
    rob_chara_age_age_set_npr(rob_chara_age_age_array, rob_id, part, value);
}

void rob_chara_age_age_array_set_params(int32_t rob_id, int32_t part,
    int32_t npr, int32_t speed, int32_t skip, int32_t disp) {
    if (!npr || npr == 1)
        rob_chara_age_age_array_set_npr(rob_id, part, npr);
    if (speed != -1)
        rob_chara_age_age_array_set_speed(rob_id, part, (float_t)speed * 0.001f);
    if (skip != -1)
        rob_chara_age_age_array_set_skip(rob_id, part);
    if (!disp || disp == 1)
        rob_chara_age_age_array_set_disp(rob_id, part, !!disp);
}

void rob_chara_age_age_array_set_speed(int32_t rob_id, int32_t part, float_t value) {
    rob_chara_age_age_set_speed(rob_chara_age_age_array, rob_id, part, value);
}

void rob_chara_age_age_array_set_skip(int32_t rob_id, int32_t part) {
    rob_chara_age_age_set_skip(rob_chara_age_age_array, rob_id, part);
}

void rob_chara_age_age_array_set_step(int32_t rob_id, int32_t part, float_t step) {
    rob_chara_age_age_set_step(rob_chara_age_age_array, rob_id, part, step);
}

void rob_chara_age_age_array_set_step_full(int32_t rob_id, int32_t part) {
    rob_chara_age_age_set_step_full(rob_chara_age_age_array, rob_id, part);
}

rob_chara* rob_chara_array_get(ROB_ID rob_id) {
    if (rob_id < 0 || rob_id >= ROB_ID_MAX)
        return 0;

    if (get_rob_management()->is_init(rob_id))
        return &RobManagement::rob_impl[rob_id];
    return 0;
}

void rob_chara_array_reset_pv_data(ROB_ID rob_id) {
    RobManagement::rob_init[rob_id].rob_type = ROB_TYPE_NULL;
}

void rob_chara_array_reset_bone_data_item_equip(ROB_ID rob_id) {
    RobDisp* disp = RobManagement::rob_impl[rob_id].disp;
    disp->reset_init_data(rob_chara_bone_data_get_node(
        RobManagement::rob_impl[rob_id].bone_data, BONE_NODE_N_HARA_CP));
    disp->set_shadow_group(rob_id);
    disp->shadow_flag = 0x05;
}

void rob_chara_array_set_alpha_obj_flags(ROB_ID rob_id, float_t alpha, mdl::ObjFlags flags) {
    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    if (!rob_chr)
        return;

    rob_chr->disp->set_alpha_obj_flags(alpha, flags);

    if (rob_chr->check_for_ageageagain_module()) {
        rob_chara_age_age_array_set_alpha(rob_id, 1, alpha);
        rob_chara_age_age_array_set_alpha(rob_id, 2, alpha);
    }
}

bool pv_osage_manager_array_get_disp() {
    bool disp = false;
    for (int32_t i = 0; i < ROB_ID_MAX; i++)
        disp |= pv_osage_manager_array_get((ROB_ID)i)->GetDisplay();
    return disp;
}

bool pv_osage_manager_array_get_disp(ROB_ID rob_id) {
    return pv_osage_manager_array_get(rob_id)->GetDisplay();
}

void pv_osage_manager_array_set_motion_ids(
    ROB_ID rob_id, const std::vector<uint32_t>& motion_ids) {
    std::vector<pv_data_set_motion> set_motion;
    for (const uint32_t& i : motion_ids)
        set_motion.push_back({ i });
    pv_osage_manager_array_get(rob_id)->SetPvSetMotion(set_motion);
}

void pv_osage_manager_array_set_not_reset_true() {
    for (int32_t i = 0; i < ROB_ID_MAX; i++)
        pv_osage_manager_array_get((ROB_ID)i)->SetNotReset(true);
}

void pv_osage_manager_array_set_pv_id(ROB_ID rob_id, int32_t pv_id, bool reset) {
    pv_osage_manager_array_get(rob_id)->SetPvId(pv_id, rob_id, reset);
}

void pv_osage_manager_array_set_pv_set_motion(
    ROB_ID rob_id, const std::vector<pv_data_set_motion>& set_motion) {
    pv_osage_manager_array_get(rob_id)->SetPvSetMotion(set_motion);
}

bool rob_chara_check_for_ageageagain_module(CHARA_NUM cn, int32_t cos_id) {
    return cn == CN_MIKU && cos_id == 148;
}

void rob_sleeve_handler_data_get_sleeve_data(
    CHARA_NUM cn, int32_t cos, rob_sleeve_data& l, rob_sleeve_data& r) {
    rob_sleeve_handler_data->get_sleeve_data(cn, cos, l, r);
}

bool rob_sleeve_handler_data_load() {
    return rob_sleeve_handler_data->load();
}

void rob_sleeve_handler_data_read() {
    rob_sleeve_handler_data->read();
}

bool task_rob_load_open() {
    return task_rob_load->open(0, "ROB LOAD", app::TASK_PRIO_HIGH);
}

bool task_rob_load_append_free_req_data(CHARA_NUM cn) {
    return task_rob_load->AppendFreeReqData(cn);
}

bool task_rob_load_append_free_req_data_obj(CHARA_NUM cn, const RobItemEquip* item_set) {
    return task_rob_load->AppendFreeReqDataObj(cn, item_set);
}

bool task_rob_load_append_load_req_data(CHARA_NUM cn) {
    return task_rob_load->AppendLoadReqData(cn);
}

bool task_rob_load_append_load_req_data_obj(CHARA_NUM cn, const RobItemEquip* item_set) {
    return task_rob_load->AppendLoadReqDataObj(cn, item_set);
}

bool task_rob_load_check_load_req_data() {
    if (task_rob_load->field_F0 == 2 && !task_rob_load->load_req_data_obj.size())
        return task_rob_load->load_req_data.size() != 0;
    return true;
}

bool task_rob_load_close() {
    task_rob_load->close();
    return true;
}

bool task_rob_manager_open() {
    return task_rob_manager->open("ROB_MANAGER TASK");
}

bool task_rob_manager_check_chara_loaded(ROB_ID rob_id) {
    if (rob_id < 0 || rob_id >= ROB_ID_MAX)
        return false;
    return task_rob_manager->CheckCharaLoaded(&RobManagement::rob_impl[rob_id]);
}

bool task_rob_manager_check_alive() {
    return task_rob_manager->check_alive();
}

bool task_rob_manager_get_free_chara_list_empty() {
    return task_rob_manager->GetFreeCharaListEmpty();
}

bool task_rob_manager_get_wait(ROB_ID rob_id) {
    if (rob_id < 0 || rob_id >= ROB_ID_MAX)
        return false;

    return task_rob_manager->GetWait(&RobManagement::rob_impl[rob_id]);
}

bool task_rob_manager_restart() {
    return task_rob_manager->restart();
}

bool task_rob_manager_suspend() {
    return task_rob_manager->suspend();
}

bool task_rob_manager_close() {
    if (!task_rob_manager->check_alive())
        return true;

    task_rob_manager->close();
    return false;
}

MOTTABLE_TYPE expression_id_to_mottbl_type(int32_t id) {
    static const MOTTABLE_TYPE expression_id_to_mottbl_type_table[] = {
        MTP_FACE_SAD,
        MTP_FACE_LAUGH,
        MTP_FACE_CRY,
        MTP_FACE_SURPRISE,
        MTP_FACE_WINK_OLD,
        MTP_FACE_ADMIRATION,
        MTP_FACE_SMILE,
        MTP_FACE_SETTLED,
        MTP_FACE_DAZZLING,
        MTP_FACE_LASCIVIOUS,
        MTP_FACE_STRONG,
        MTP_FACE_CLARIFYING,
        MTP_FACE_GENTLE,
        MTP_FACE_NAGASI,
        MTP_FACE_RESET,
        MTP_FACE_KIRI,
        MTP_FACE_UTURO,
        MTP_FACE_OMOU,
        MTP_FACE_SETUNA,
        MTP_FACE_GENKI,
        MTP_FACE_YARU,
        MTP_FACE_RESET,
        MTP_FACE_CLOSE,
        MTP_FACE_NULL,
        MTP_FACE_MOT_SLOT_1,
        MTP_FACE_MOT_SLOT_2,
        MTP_FACE_MOT_SLOT_3,
        MTP_FACE_MOT_SLOT_4,
        MTP_FACE_MOT_SLOT_5,
        MTP_FACE_MOT_SLOT_6,
        MTP_FACE_MOT_SLOT_7,
        MTP_FACE_MOT_SLOT_8,
        MTP_FACE_MOT_SLOT_9,
        MTP_FACE_MOT_SLOT_10,
        MTP_FACE_COOL,
        MTP_FACE_KOMARIWARAI,
        MTP_FACE_KUMON,
        MTP_FACE_KUTSUU,
        MTP_FACE_NAKI,
        MTP_FACE_NAYAMI,
        MTP_FACE_SUPSERIOUS,
        MTP_FACE_TSUYOKIWARAI,
        MTP_FACE_WINK_L,
        MTP_FACE_WINK_R,
        MTP_FACE_WINKG_L,
        MTP_FACE_WINKG_R,
        MTP_FACE_RESET1,
        MTP_FACE_RESET2,
        MTP_FACE_RESET3,
        MTP_FACE_RESET4,
        MTP_FACE_RESET5,
        MTP_FACE_WINK_FT_OLD,
        MTP_FACE_SAD_OLD,
        MTP_FACE_SURPRISE_OLD,
        MTP_FACE_SMILE_OLD,
        MTP_FACE_DAZZLING_OLD,
        MTP_FACE_LASCIVIOUS_OLD,
        MTP_FACE_STRONG_OLD,
        MTP_FACE_CLARIFYING_OLD,
        MTP_FACE_GENTLE_OLD,
        MTP_FACE_NAGASI_OLD,
        MTP_FACE_KIRI_OLD,
        MTP_FACE_OMOU_OLD,
        MTP_FACE_SETUNA_OLD,
        MTP_FACE_NEW_IKARI_OLD,
        MTP_FACE_CRY_OLD,
        MTP_FACE_LAUGH_OLD,
        MTP_FACE_YARU_OLD,
        MTP_FACE_ADMIRATION_OLD,
        MTP_FACE_GENKI_OLD,
        MTP_FACE_SETTLED_OLD,
        MTP_FACE_UTURO_OLD,
        MTP_FACE_RESET_OLD,
        MTP_FACE_CLOSE_OLD,
        MTP_FACE_EYEBROW_UP_RIGHT,
        MTP_FACE_EYEBROW_UP_LEFT,
        MTP_FACE_KOMARIEGAO,
        MTP_FACE_KONWAKU,
    };

    if (id >= 0 && id < sizeof(expression_id_to_mottbl_type_table) / sizeof(int32_t))
        return expression_id_to_mottbl_type_table[id];
    return MTP_FACE_NULL;
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

MOTTABLE_TYPE hand_anim_id_to_mottbl_type(int32_t id) {
    static const MOTTABLE_TYPE hand_anim_id_to_mottbl_type_table[] = {
        MTP_HAND_OPEN,
        MTP_HAND_CLOSE,
        MTP_HAND_NORMAL,
        MTP_HAND_PEACE,
        MTP_HAND_NEGI,
        MTP_HAND_GOOD,
        MTP_HAND_ONE,
        MTP_HAND_SIZEN,
        MTP_HAND_PICK,
        MTP_HAND_NULL,
        MTP_HAND_THREE,
        MTP_HAND_MIC,
        MTP_HAND_MIC,
        MTP_HAND_MIC,
        MTP_HAND_RESET,
    };

    if (id >= 0 && id < sizeof(hand_anim_id_to_mottbl_type_table) / sizeof(int32_t))
        return hand_anim_id_to_mottbl_type_table[id];
    return MTP_HAND_NULL;
}

MOTTABLE_TYPE look_anim_id_to_mottbl_type(int32_t id) {
    static const MOTTABLE_TYPE look_anim_id_to_mottbl_type_table[] = {
        MTP_EYES_UP,
        MTP_EYES_DOWN,
        MTP_EYES_RIGHT,
        MTP_EYES_LEFT,
        MTP_EYES_UP_RIGHT,
        MTP_EYES_UP_LEFT,
        MTP_EYES_DOWN_RIGHT,
        MTP_EYES_DOWN_LEFT,
        MTP_EYES_RESET,
        MTP_EYES_NULL,
        MTP_EYES_BASE_MOT,
        MTP_EYES_UP_OLD,
        MTP_EYES_DOWN_OLD,
        MTP_EYES_RIGHT_OLD,
        MTP_EYES_LEFT_OLD,
        MTP_EYES_UP_RIGHT_OLD,
        MTP_EYES_UP_LEFT_OLD,
        MTP_EYES_DOWN_RIGHT_OLD,
        MTP_EYES_DOWN_LEFT_OLD,
        MTP_EYES_RESET_OLD,
    };

    if (id >= 0 && id < sizeof(look_anim_id_to_mottbl_type_table) / sizeof(int32_t))
        return look_anim_id_to_mottbl_type_table[id];
    return MTP_EYES_NULL;
}

MOTTABLE_TYPE mouth_anim_id_to_mottbl_type(int32_t id) {
    static const MOTTABLE_TYPE mouth_anim_id_to_mottbl_type_table[] = {
        MTP_KUCHI_A,
        MTP_KUCHI_E,
        MTP_KUCHI_O,
        MTP_KUCHI_SURPRISE,
        MTP_KUCHI_HE,
        MTP_KUCHI_SMILE,
        MTP_KUCHI_NIYA,
        MTP_KUCHI_CHU,
        MTP_KUCHI_RESET,
        MTP_KUCHI_NULL,
        MTP_KUCHI_I,
        MTP_KUCHI_U,
        MTP_KUCHI_E_DOWN,
        MTP_KUCHI_HAMISE,
        MTP_KUCHI_HAMISE_DOWN,
        MTP_KUCHI_HE_S,
        MTP_KUCHI_HERAHERA,
        MTP_KUCHI_MOGUMOGU,
        MTP_KUCHI_NEKO,
        MTP_KUCHI_SAKEBI,
        MTP_KUCHI_SAKEBI_L,
        MTP_KUCHI_SMILE_L,
        MTP_KUCHI_NEUTRAL,
        MTP_KUCHI_NIYA_OLD,
        MTP_KUCHI_A_OLD,
        MTP_KUCHI_O_OLD,
        MTP_KUCHI_SURPRISE_OLD,
        MTP_KUCHI_HE_OLD,
        MTP_KUCHI_RESET_OLD,
        MTP_KUCHI_I_OLD,
        MTP_KUCHI_U_OLD,
        MTP_KUCHI_E_OLD,
        MTP_KUCHI_SMILE_OLD,
        MTP_KUCHI_CHU_OLD,
        MTP_KUCHI_PSP_A,
        MTP_KUCHI_PSP_E,
        MTP_KUCHI_PSP_O,
        MTP_KUCHI_PSP_SURPRISE,
        MTP_KUCHI_PSP_NIYA,
        MTP_KUCHI_PSP_NIYARI,
        MTP_KUCHI_HAMISE_E,
        MTP_KUCHI_SANKAKU,
        MTP_KUCHI_SHIKAKU,
    };

    if (id >= 0 && id < sizeof(mouth_anim_id_to_mottbl_type_table) / sizeof(int32_t))
        return mouth_anim_id_to_mottbl_type_table[id];
    return MTP_KUCHI_NULL;
}

bone_data_parent::bone_data_parent() : rob_bone_data(),
block_max(), leaf_pos_max(), chain_pos_max(),
bone_key_set_count(), global_key_set_count(), yrot() {

}

bone_data_parent::~bone_data_parent() {

}

// 0x1404102E0
void bone_data_parent::ik_init(const std::vector<BODYTYPE>* body_type_table,
    const CHAINPOSRADIUS* joint_table, const CHAINPOSRADIUS* disp_joint_table) {
    rob_chara_bone_data* rob_bone_data = this->rob_bone_data;
    size_t chain_pos = 0;
    size_t node_count = 0;
    size_t leaf_pos = 0;

    RobBlock* block = block_vec.data();
    for (const BODYTYPE& i : *body_type_table) {
        block->block_id = (BONE_BLK)(&i - body_type_table->data());
        block->ik_type = i.ik_type;
        block->flip_block_id = (BONE_BLK)i.flip_block_id;
        block->inherit_mat_id = (BONE_BLK)i.inherit_mat_id;
        block->expression_id = i.expression_id;
        block->key_set_count = i.ik_type >= IKT_ROOT ? 6 : 3;
        block->chain_pos[0] = (*joint_table++).chain_pos;
        block->chain_pos[1] = (*disp_joint_table++).chain_pos;
        block->node = &rob_bone_data->node_vec[node_count];
        block->inherit_type = i.inherit_type;
        block->inherit_mat_ptr = i.inherit_type ? &rob_bone_data->mat_vec[i.inherit_mat_id] : 0;

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
            block->len[0][0] = (*joint_table++).chain_pos.x;
            block->len[0][1] = (*disp_joint_table++).chain_pos.x;

            chain_pos += 2;
            leaf_pos++;
            node_count += 3;
            break;
        case IKT_2:
        case IKT_2R:
            block->len[0][0] = (*joint_table++).chain_pos.x;
            block->len[0][1] = (*disp_joint_table++).chain_pos.x;
            block->len[1][0] = (*joint_table++).chain_pos.x;
            block->len[1][1] = (*disp_joint_table++).chain_pos.x;

            mat4* up_vector_mat = 0;
            if (i.up_vector_id)
                up_vector_mat = &rob_bone_data->mat_vec[i.up_vector_id];
            block->up_vector_mat_ptr = up_vector_mat;

            chain_pos += 3;
            leaf_pos++;
            node_count += 4;
            break;
        }
        block++;
    }

    if (node_count != rob_bone_data->node_max)
        printf_debug_error("Node mismatch");
    if (leaf_pos != leaf_pos_max)
        printf_debug_error("LeafPos mismatch");
    if (chain_pos != chain_pos_max)
        printf_debug_error("ChainPos mismatch");
}

void mot_play_frame_data::reset() {
    frame = -1.0f;
    step = 1.0f;
    step_prev = 1.0f;
    frame_max = 0.0f;
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

MotionSmooth::MotionSmooth() : field_0(), field_4(), field_8(), base_mtx_set(),
move_yang(), move_yang_bef(), root_ypos(), root_xzpos(), field_C0(), field_C4() {

}

void MotionSmooth::reset() {
    field_0 = 0;
    field_8 = 0;
    base_mtx = mat4_identity;
    base_mtx_bef = mat4_identity;
    base_mtx_set = false;
    move_yang = 0.0f;
    move_yang_bef = 0.0f;
    field_90 = 1.0f;
    field_9C = 1.0f;
    field_A8 = 1.0f;
    root_ypos = 0;
    root_xzpos = 0;
    field_C0 = 1.0f;
    field_C4 = 1.0f;
    field_C8 = 0.0f;
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
        mat4_rotate_y(-a2->move_yang_bef, &rot_y_mat);
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
        curr->leaf_pos[1] = vec3::lerp(prev->leaf_pos[1], curr->leaf_pos[1], blend);
        break;
    case IKT_ROOT:
        if (trans_xz) {
            curr->leaf_pos[1].x = lerp_def(prev->leaf_pos[1].x, curr->leaf_pos[1].x, blend);
            curr->leaf_pos[1].z = lerp_def(prev->leaf_pos[1].z, curr->leaf_pos[1].z, blend);
        }

        if (trans_y)
            curr->leaf_pos[1].y = lerp_def(prev->leaf_pos[1].y, curr->leaf_pos[1].y, blend);
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
        mat4_rotate_y(-a2->move_yang_bef, &rot_y_mat);
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
            mat4_rotate_y(a2->move_yang, &mat);
        else
            mat4_rotate_y(-a2->move_yang_bef, &mat);
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
        curr->leaf_pos[1] = vec3::lerp(curr->smooth_pos[field_24], curr->leaf_pos[1], blend);
        break;
    case IKT_ROOT:
        if (trans_xz) {
            curr->leaf_pos[1].x = lerp_def(curr->smooth_pos[field_24].x, curr->leaf_pos[1].x, blend);
            curr->leaf_pos[1].z = lerp_def(curr->smooth_pos[field_24].z, curr->leaf_pos[1].z, blend);
        }

        if (trans_y)
            curr->leaf_pos[1].y = lerp_def(curr->smooth_pos[field_24].y, curr->leaf_pos[1].y, blend);
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
        curr->leaf_pos[1] = vec3::lerp(curr->smooth_pos[0], curr->leaf_pos[1], blend);
        break;
    case IKT_ROOT:
        mat4_lerp_rotation(&curr->smooth_rot[0][0], &curr->chain_rot[0], &curr->chain_rot[0], blend);
        curr->leaf_pos[1] = vec3::lerp(curr->smooth_pos[0], curr->leaf_pos[1], blend);
        break;
    case IKT_1:
        mat4_mul(&curr->smooth_rot[1][0], &curr->smooth_rot[0][0], &prev_mat);
        mat4_mul(&curr->chain_rot[1], &curr->chain_rot[0], &mat);
        mat4_lerp_rotation(&prev_mat, &mat, &mat, blend);
        mat4_transpose(&curr->chain_rot[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->chain_rot[1]);
        break;
    case IKT_2:
    case IKT_2R:
        mat4_lerp_rotation(&curr->smooth_rot[2][0], &curr->chain_rot[2], &curr->chain_rot[2], blend);
        mat4_mul(&curr->smooth_rot[1][0], &curr->smooth_rot[0][0], &prev_mat);
        mat4_mul(&curr->chain_rot[1], &curr->chain_rot[0], &mat);
        mat4_lerp_rotation(&prev_mat, &mat, &mat, blend);
        mat4_transpose(&curr->chain_rot[0], &ik_mat);
        mat4_mul(&mat, &ik_mat, &curr->chain_rot[1]);
        break;
    }
}

motion_blend_mot_enabled_bones::motion_blend_mot_enabled_bones() : func(), arr(0, false), count() {

}

motion_blend_mot_enabled_bones::~motion_blend_mot_enabled_bones() {

}

// 0x1404146F0
void motion_blend_mot_enabled_bones::check() {
    int32_t blk = BLK_N_HARA_CP;
    for (auto i = arr.begin(); i != arr.end(); i++) {
        if (func((BONE_BLK)blk))
            *i = true;
        else
            *i = false;
        blk++;
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
mot_key_data(), mot_play_data(), smooth(), field_5CC(), blend() {
    mot_key_data.reset();
    mot_play_data.reset();
    smooth.field_C0 = 1.0f;
    smooth.field_C4 = 1.0f;
}

motion_blend_mot::~motion_blend_mot() {

}

// 0x140415430
void motion_blend_mot::apply_global_transform() {
    mat4 mat;
    mat4_rotate_y(bone_data.yrot, &mat);
    mat4_mul_translate(&mat, &bone_data.gblctr_pos, &mat);
    mat4_mul_rotate_zyx(&mat, &bone_data.gblctr_rot, &mat);

    for (RobBlock& i : bone_data.block_vec)
        switch (i.ik_type) {
        case IKT_0N:
            mat4_transform_point(&mat, &i.leaf_pos[1], &i.leaf_pos[1]);
            break;
        case IKT_ROOT: {
            mat4 chain_rot;
            mat4_clear_trans(&mat, &chain_rot);
            i.chain_rot[0] = chain_rot;
            mat4_transform_point(&mat, &i.leaf_pos[1], &i.leaf_pos[1]);
        } break;
        case IKT_1:
        case IKT_2:
        case IKT_2R:
            mat4_transform_point(&mat, &i.leaf_pos[0], &i.leaf_pos[0]);
            break;
        }
}

void motion_blend_mot::copy_rot_trans() {
    copy_rot_trans(bone_data.block_vec);
}

// 0x140412BB0
void motion_blend_mot::copy_rot_trans(const prj::sys_vector<RobBlock>& block_vec) {
    for (RobBlock& i : bone_data.block_vec)
        i.copy_rot_trans(block_vec[i.block_id]);
}

bool motion_blend_mot::get_blend_enable() {
    if (blend && !blend->Field_30())
        return blend->enable;
    return false;
}

// 0x140412DA0
void motion_blend_mot::get_n_hara_cp_position(vec3& position) {
    position = bone_data.block_vec[BLK_N_HARA_CP].leaf_pos[1];
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
        this->bone_data.rob_bone_data->kind,
        this->bone_data.block_max,
        this->bone_data.leaf_pos_max);
    enabled_bones.init(check_func, this->bone_data.block_max);
}

void motion_blend_mot::interpolate() {
    const vec3* fc_value = (const vec3*)mot_key_data.fc_value.data();
    bool flip = motion_blend_mot::interpolate_get_flip(smooth);
    float_t frame = mot_play_data.frame_data.frame;

    BONE_KIND kind = bone_data.rob_bone_data->kind;
    RobBlock* block_top = bone_data.block_vec.data();
    for (uint16_t& i : bone_data.bone_indices) {
        RobBlock* bl = &block_top[i];
        bool get_data = enabled_bones.arr[bl->block_id];
        if (flip && bl->flip_block_id != 0xFF)
            bl = &block_top[bl->flip_block_id];

        if (get_data && frame != bl->frame) {
            mot_key_data.interpolate(frame, bl->key_set_offset, bl->key_set_count);
            bl->frame = frame;
        }

        fc_value = bl->set_global_leaf_sub(fc_value, kind, get_data, flip);
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

void motion_blend_mot::load_file(uint32_t motnum, MotionBlendType blend_type, float_t blend,
    const bone_database* bone_data, const motion_database* mot_db) {
    set_blend(blend_type, blend);
    const mot_data* v6 = mot_key_data_load_file(&mot_key_data, motnum, mot_db);
    bone_data_parent* v7 = &this->bone_data;
    if (v6) {
        bone_data_parent_load_bone_indices_from_mot(v7, v6, bone_data, mot_db);
        RobBlock* bone = this->bone_data.block_vec.data();
        for (uint16_t& i : this->bone_data.bone_indices)
            bone[i].frame = -1.0f;
    }
    else {
        v7->rob_bone_data->set_mats_identity();
        this->bone_data.bone_indices.clear();
    }

    sub_140413EB0(&smooth);
}

void motion_blend_mot::mult_mat(const mat4* mat) {
    sub_140414900(&smooth, mat);

    mat4& base_mtx = smooth.base_mtx;
    RobBlock* bones = bone_data.block_vec.data();
    for (RobBlock& bone : bone_data.block_vec)
        bone.recalc_fk_block(base_mtx, bones, true);
}

void motion_blend_mot::reset() {
    mot_key_data.reset();
    mot_play_data.reset();
    smooth.reset();
}

inline void motion_blend_mot::set_arm_length(BONE_BLK blk, float_t value) {
    bone_data.block_vec[blk].arm_length = value;
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
        get_smooth_target(0);
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
void motion_blend_mot::get_smooth_target(int32_t target) {
    for (RobBlock& i : bone_data.block_vec)
        if (enabled_bones.arr[i.block_id]) {
            i.get_smooth_target(target);
            if (i.ik_type == IKT_ROOT && (smooth.field_0 & 0x02))
                i.smooth_pos[target] += smooth.field_90;
        }
}

bool motion_blend_mot::interpolate_get_flip(MotionSmooth& a1) {
    return a1.field_0 & 0x01 && !(a1.field_0 & 0x08) || !(a1.field_0 & 0x01) && a1.field_0 & 0x08;
}

rob_chara_bone_data_adjust_scale::rob_chara_bone_data_adjust_scale() {
    base = 1.0f;
    body = 1.0f;
    arm = 1.0f;
    height = 1.0f;
}

partial_motion_blend_mot::partial_motion_blend_mot() : disable(), mot_key_data(), mot_play_data() {

}

partial_motion_blend_mot::~partial_motion_blend_mot() {

}

void partial_motion_blend_mot::init(BONE_KIND kind, PFNMOTIONBONECHECKFUNC check_func,
    size_t block_max, const bone_database* bone_data) {
    mot_key_data_init(&mot_key_data, kind, bone_data);
    enabled_bones.init(check_func, block_max);
}

void partial_motion_blend_mot::interpolate(prj::sys_vector<RobBlock>& block_vec,
    const prj::sys_vector<uint16_t>* bone_indices, BONE_KIND kind) {
    if (!mot_key_data.key_sets_ready || !mot_key_data.mot_data || disable)
        return;

    float_t frame = mot_play_data.frame_data.frame;
    const vec3* fc_value = (const vec3*)mot_key_data.fc_value.data();
    for (const uint16_t& i : *bone_indices) {
        RobBlock* block = &block_vec[i];
        bool get_data = enabled_bones.arr[block->block_id];
        if (get_data) {
            mot_key_data.interpolate(frame, block->key_set_offset, block->key_set_count);
            block->frame = frame;
        }
        fc_value = block->set_global_leaf_sub(fc_value, kind, get_data, false);
    }
}

void partial_motion_blend_mot::load_file(uint32_t motnum, const motion_database* mot_db) {
    mot_key_data_load_file(&mot_key_data, motnum, mot_db);
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
void partial_motion_blend_mot::get_smooth_target(prj::sys_vector<RobBlock>& block_vec) {
    for (RobBlock& i : block_vec)
        if (enabled_bones.arr[i.block_id])
            i.get_smooth_target(0);
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

Motion::AshiOidashi::AshiOidashi() : y_bef(), y_old(), scale() {

}

void Motion::AshiOidashi::reset() {
    y_bef = 0.0f;
    y_old = 0.0f;
    scale = 0.0f;
}

Motion::AshiOidashiColle::AshiOidashiColle() {

}

void Motion::AshiOidashiColle::reset() {
    l.reset();
    r.reset();
}

rob_chara_look_anim::rob_chara_look_anim() : block_vec(), update_view_point(), init_head_rotation(),
head_rotation(), init_eyes_rotation(), eyes_rotation(), disable(), head_rot_strength(),
eyes_rot_strength(), eyes_rot_step(), duration(), eyes_rot_frame(), step(), head_rot_frame(), field_B0(),
ext_head_rotation(), ext_head_rot_strength(), ext_head_rot_y_angle(), ext_head_rot_x_angle(),
field_15C(), field_190(), field_191(), field_192(), field_193(), field_194(), field_195(), head_rot_blend(),
field_19C(), field_1AC(), eyes_xrot_adjust_neg(), eyes_xrot_adjust_pos(), ft(), type() {
    reset();
}

void rob_chara_look_anim::reset() {
    block_vec = 0;
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

rob_chara_sleeve_adjust::rob_chara_sleeve_adjust() : sleeve_l(), sleeve_r(), enable1(), enable2(),
field_5C(), field_68(), field_74(), field_80(), radius(), block_vec(), step() {
    reset();
}

void rob_chara_sleeve_adjust::reset() {
    sleeve_l = {};
    sleeve_r = {};
    enable1 = false;
    enable2 = false;
    radius = 1.0f;
    block_vec = 0;
    step = 1.0f;
    field_5C = 0.0f;
    field_68 = 0.0f;
    field_74 = 0.0f;
    field_80 = 0.0f;
}

rob_chara_bone_data::rob_chara_bone_data() : field_0(), field_1(), mat_max(), node_max(),
block_max(), leaf_pos_max(), chain_pos_max(), disable_eye_motion(), look_anim() {
    kind = BONE_KIND_NONE;
    disp_kind = BONE_KIND_NONE;
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

float_t rob_chara_bone_data::get_frame() const  {
    return motion_loaded.front()->mot_play_data.frame_data.frame;
}

float_t rob_chara_bone_data::get_frame_max() const {
    return (float_t)motion_loaded.front()->mot_key_data.mot.frame_max;
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
    if (index < mat_vec.size())
        return &mat_vec[index];
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

    BONE_KIND kind = this->kind;
    motion_blend_mot* v5 = motion_loaded.front();
    prj::sys_vector<RobBlock>& block_vec = v5->bone_data.block_vec;
    prj::sys_vector<uint16_t>* bone_indices = &v5->bone_data.bone_indices;
    face.interpolate(block_vec, bone_indices, kind);
    hand_l.interpolate(block_vec, bone_indices, kind);
    hand_r.interpolate(block_vec, bone_indices, kind);
    mouth.interpolate(block_vec, bone_indices, kind);
    eyes.interpolate(block_vec, bone_indices, kind);
    eyelid.interpolate(block_vec, bone_indices, kind);
}

void rob_chara_bone_data::load_eyelid_motion(uint32_t motnum, const motion_database* mot_db) {
    sub_14041AD50(this);
    eyelid.get_smooth_target(motion_loaded.front()->bone_data.block_vec);
    eyelid.enabled_bones.check();
    eyelid.load_file(motnum, mot_db);
}

void rob_chara_bone_data::load_eyes_motion(uint32_t motnum, const motion_database* mot_db) {
    eyes.get_smooth_target(motion_loaded.front()->bone_data.block_vec);
    eyes.load_file(motnum, mot_db);
}

void rob_chara_bone_data::load_face_motion(uint32_t motnum, const motion_database* mot_db) {
    sub_14041AD90(this);
    face.get_smooth_target(motion_loaded.front()->bone_data.block_vec);
    face.enabled_bones.check();
    face.load_file(motnum, mot_db);
}

void rob_chara_bone_data::load_hand_l_motion(uint32_t motnum, const motion_database* mot_db) {
    hand_l.get_smooth_target(motion_loaded.front()->bone_data.block_vec);
    hand_l.load_file(motnum, mot_db);
}

void rob_chara_bone_data::load_hand_r_motion(uint32_t motnum, const motion_database* mot_db) {
    hand_r.get_smooth_target(motion_loaded.front()->bone_data.block_vec);
    hand_r.load_file(motnum, mot_db);
}

void rob_chara_bone_data::load_mouth_motion(uint32_t motnum, const motion_database* mot_db) {
    mouth.get_smooth_target(motion_loaded.front()->bone_data.block_vec);
    mouth.load_file(motnum, mot_db);
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
            frame_data->frame = min_def(frame_data->frame, frame_data->frame_max);
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
    mat_max = 0;
    block_max = 0;
    node_max = 0;
    leaf_pos_max = 0;
    chain_pos_max = 0;
    mat_vec.clear();
    mat2_vec.clear();
    node_vec.clear();
    kind = BONE_KIND_NONE;
    disp_kind = BONE_KIND_NONE;

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
    adjust_scale = rob_chara_bone_data_adjust_scale();
    ashi_oidashi.reset();
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
    for (mat4& i : mat_vec)
        i = mat4_identity;

    for (mat4& i : mat2_vec)
        i = mat4_identity;
}

void rob_chara_bone_data::set_motion_blend_duration(float_t duration, float_t step, float_t offset) {
    motion_loaded.front()->set_blend_duration(duration, step, offset);
}

void rob_chara_bone_data::set_motion_frame(float_t frame, float_t step, float_t frame_max) {
    mot_play_data* play_data = &motion_loaded.front()->mot_play_data;
    play_data->frame_data.frame = frame;
    play_data->frame_data.step_prev = play_data->frame_data.step;
    play_data->frame_data.step = step;
    play_data->frame_data.frame_max = frame_max;
    play_data->frame_data.last_frame = frame_max - 1.0f;
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
        sub_14040FBF0(i, adjust_scale.base);
    }

    sub_1404182B0(this);
    sub_14041B9F0(this);

    motion_loaded.front()->mult_mat(mat);
}

eyes_adjust::eyes_adjust() : xrot_adjust(), base_adjust() {
    neg = -1.0f;
    pos = -1.0f;
}

RobInit::RobInit() : rob_type(), disp(), mirror(),
drank_reset(), energy(), sleeve_l(), sleeve_r(), field_70(),
face_mot_slot(), chara_size_index(), height_adjust(), item() {
    init();
}

void RobInit::init() {
    rob_type = ROB_TYPE_AUTH;
    disp = true;
    mirror = false;
    drank_reset = false;
    pos = 0.0f;
    yang = 0;
    energy = 201;
    sleeve_l = {};
    sleeve_r = {};
    field_70 = 0;
    for (uint32_t& i : face_mot_slot)
        i = -1;
    chara_size_index = 1;
    height_adjust = false;
    item = {};
    eyes_adjust = {};
}

RobSkinOfs::RobSkinOfs() : flag() {

}

RobSkinDisp::RobSkinDisp() : index(), motion_matrix(),
obj_uid(), obj_uid_sub(), alpha(), obj_flags(), can_disp(), bone_kind(),
mat(), init_cnt(), motion_node(), field_138(), osage_depends_on_others(),
osage_nodes_count(), use_opd(), skin_ex_data(), skin(), rob_disp() {
    init_members(0x12345678);
}

RobSkinDisp::~RobSkinDisp() {
    init_members(0xDDDDDDDD);
}

void RobSkinDisp::add_motion_reset_data(const uint32_t& motnum, const float_t& frame, int32_t init_cnt) {
    if (init_cnt > 0)
        pos_reset(init_cnt);

    for (ExOsageBlock*& i : osage_blk)
        i->AddMotionResetData(motnum, frame);

    for (ExClothBlock*& i : cloth)
        i->AddMotionResetData(motnum, frame);
}

void RobSkinDisp::check_no_opd(std::vector<opd_blend_data>& opd_blend_data) {
    use_opd = true;
    if (!osage_blk.size() && !cloth.size() || !opd_blend_data.size())
        return;

    for (::opd_blend_data& i : opd_blend_data) {
        const float_t* opd_data = 0;
        uint32_t opd_count = 0;
        osage_play_data_manager_get_opd_file_data(obj_uid, i.motnum, opd_data, opd_count);
        if (!opd_data) {
            use_opd = false;
            break;
        }
    }
}

void RobSkinDisp::dest_ex_node() {
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

void RobSkinDisp::disp(const mat4& mat, render_context* rctx) {
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

int32_t RobSkinDisp::get_node_index(const char* name, const bone_database* bone_data) const {
    int32_t node_index = bone_data->get_bone_node_index(BONE_KIND_CMN, name);
    if (node_index == -1)
        for (auto& i : node_name_map)
            if (!str_utils_compare(name, i.first))
                return 0x8000 | i.second;
    return node_index;
}

const RobNode* RobSkinDisp::get_node(int32_t index) const {
    if (!(index & 0x8000))
        return &motion_node[index & 0x7FFF];
    else if ((index & 0x7FFF) < ex_node.size())
        return &ex_node[index & 0x7FFF];
    return 0;
}

const RobNode* RobSkinDisp::get_node(const char* name, const bone_database* bone_data) const {
    return get_node(get_node_index(name, bone_data));
}

const mat4* RobSkinDisp::get_ex_data_bone_node_mat(const char* name) {
    if (!name || !ex_node.size())
        return &mat4_identity;

    for (RobNode& i : ex_node)
        if (!str_utils_compare(i.name, name))
            return i.mat_ptr;

    return &mat4_identity;
}

RobJointNode* RobSkinDisp::get_normal_ref_osage_node(const std::string& str, size_t* index) {
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

void RobSkinDisp::init_members(size_t index) {
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
    dest_ex_node();
    ex_node.clear();
    matrix.clear();
    no_scale_matrix.clear();
    node_name_map.clear();
    osage_depends_on_others = false;
    use_opd = false;
    osage_nodes_count = 0;
}

// 0x1405F4820
void RobSkinDisp::pos_reset(int32_t init_cnt) {
    if (!ex_node_block.size())
        return;

    auto pos_init = &ExNodeBlock::pos_init;
    auto pos_init_cont = &ExNodeBlock::pos_init_cont;

    for (ExNodeBlock*& i : ex_node_block)
        (i->*pos_init)();

    for (; init_cnt; init_cnt--) {
        if (osage_depends_on_others)
            RobSkinDisp_ctrl_step(this, true);

        for (ExNodeBlock*& i : ex_node_block)
            (i->*pos_init_cont)();
    }
}

void RobSkinDisp::reset_ex_force() {
    for (ExOsageBlock*& i : osage_blk)
        i->reset_ex_force();
    for (ExClothBlock*& i : cloth)
        i->reset_ex_force();
}

void RobSkinDisp::reset_nodes_ex_force(rob_osage_parts_bit parts_bits) {
    for (ExOsageBlock*& i : osage_blk)
        if (i->osage_work.CheckPartsBits(parts_bits)) {
            i->osage_work.SetNodesExternalForce(0, 1.0f);
            i->osage_work.SetNodesForce(1.0f);
        }
}

void RobSkinDisp::set(object_info objuid, const RobNode* mot_node,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    obj_uid = objuid;
    motion_node = mot_node;
    motion_matrix = mot_node->mat_ptr;
    ex_node.clear();
    node_name_map.clear();
    matrix.clear();
    dest_ex_node();

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

void RobSkinDisp::set_alpha_obj_flags(float_t alpha, int32_t flags) {
    obj_flags = (mdl::ObjFlags)flags;
    this->alpha = clamp_def(alpha, 0.0f, 1.0f);
}

bool RobSkinDisp::set_boc(
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

// 0x1405F7240
void RobSkinDisp::set_col_color(const vec3& blend, const vec3& ofs) {
    skn_col.blend_color = blend;
    skn_col.offset_color = ofs;
}

// 0x1405F7240
void RobSkinDisp::set_col_specular(const vec3& blend, const vec3& ofs) {
    skn_col.blend_specular = blend;
    skn_col.offset_specular = ofs;
}

// 0x1405F7270
void RobSkinDisp::set_col_type(int32_t type) {
    skn_col.type = type;
}

void RobSkinDisp::set_collision_target_osage(
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

void RobSkinDisp::set_disable_collision(rob_osage_parts_bit parts_bits, bool disable) {
    for (ExOsageBlock*& i : osage_blk)
        if (i->osage_work.CheckPartsBits(parts_bits))
            i->SetDisableCollision(disable);
}

void RobSkinDisp::set_ex_node(obj_skin_ex_data* ex_data) {
    static size_t ex_node_least = 64;

    ex_node.clear();
    matrix.clear();
    no_scale_matrix.clear();

    ex_node.resize(max_def(ex_node_least, (size_t)ex_data->nb_node_name));
    matrix.resize(ex_node.size());
    no_scale_matrix.resize(ex_node.size());

    RobNode* motion_node = ex_node.data();
    mat4* mat = matrix.data();
    mat4* no_scale_mat = no_scale_matrix.data();
    for (size_t i = 0; i < ex_node.size(); i++)
        motion_node[i].mat_ptr = &mat[i];

    if (ex_data->ex_node_name) {
        node_name_map.clear();
        const char** ex_node_name = ex_data->ex_node_name;
        for (size_t i = 0; i < ex_data->nb_node_name; i++) {
            motion_node[i].init(ex_node_name[i], &mat[i], &no_scale_mat[i]);
            node_name_map.push_back(ex_node_name[i], (uint32_t)i);
        }
    }
}

void RobSkinDisp::set_ex_node_block(obj_skin_ex_data* ex_data,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!ex_data->ex_node_table)
        return;

    prj::vector_pair<uint32_t, RobJointNode*> joint_map;
    std::map<std::string, ExNodeBlock*> node_name_map;
    dest_ex_node();

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
            cls->set_name(block->cloth->omote_name);
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
            osg->set_data(this, block->osage,
                &skin_ex_data->osage_joint[block->osage->joint_ofs],
                motion_node, this->ex_node.data(), skin);
            osg->make_joint_map(block->osage,
                &skin_ex_data->osage_joint[block->osage->joint_ofs], joint_map, node_name_map);
            osg->block_idx = osage_count;
            osg->set_name(ex_node_name[block->osage->root_idx & 0x7FFF]);
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
            auto elem = node_name_map.find(parent->name);
            if (elem != node_name_map.end())
                ex_node->parent_node = elem->second;
        }

        node_name_map.insert({ ex_node->name, ex_node });
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

            auto node_elem = joint_map.begin();
            for (; node_elem != joint_map.end(); node_elem++)
                if (node_elem->first == dst_joint)
                    break;

            auto distance_elem = joint_map.begin();
            for (; distance_elem != joint_map.end(); distance_elem++)
                if (distance_elem->first == src_joint)
                    break;

            if (node_elem != joint_map.end() && distance_elem != joint_map.end()) {
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

void RobSkinDisp::set_motion_node(const RobNode* mot_node, const bone_database* bone_data) {
    motion_node = mot_node;
    motion_matrix = mot_node->mat_ptr;
    for (ExNodeBlock*& i : ex_node_block)
        i->parent = get_node(i->parent_name.c_str(), bone_data);
}

void RobSkinDisp::set_motion_reset_data(const uint32_t& motnum, const float_t& frame) {
    for (ExOsageBlock*& i : osage_blk)
        i->SetMotionResetData(motnum, frame);

    for (ExClothBlock*& i : cloth)
        i->SetMotionResetData(motnum, frame);
}

void RobSkinDisp::set_motion_skin_param(int8_t rob_id, uint32_t motnum, int32_t frame) {
    if (obj_uid.is_null())
        return;

    std::vector<skin_param_file_data>* skp_file_data
        = skin_param_manager_get_skin_param_file_data(rob_id, obj_uid, motnum, frame);
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

// 0x1405F92D0
void RobSkinDisp::set_ofs(const vec3& pos, const vec3& rot, const vec3& scale) {
    const vec3 _pos = pos;
    const vec3 _rot = rot * DEG_TO_RAD_FLOAT;
    const vec3 sc = scale;

    skn_ofs.flag = true;
    for (ExNullBlock*& i : null_blk) {
        if (!i || !i->dst_node)
            continue;

        RobTransform& transform = i->dst_node->transform;
        transform.pos = _pos;
        transform.rot = _rot;
        transform.scale = sc;
    }
}

void RobSkinDisp::set_osage_play_data_init(const float_t* opdi_data) {
    for (ExOsageBlock*& i : osage_blk)
        opdi_data = i->SetOsagePlayDataInit(opdi_data);

    for (ExClothBlock*& i : cloth)
        opdi_data = i->SetOsagePlayDataInit(opdi_data);
}

void RobSkinDisp::set_osage_reset() {
    for (ExOsageBlock*& i : osage_blk)
        i->SetOsageReset();

    for (ExClothBlock*& i : cloth)
        i->SetOsageReset();
}

void RobSkinDisp::set_osage_move_cancel(const float_t& mv_ccl) {
    for (ExOsageBlock*& i : osage_blk)
        i->set_move_cancel(mv_ccl);

    for (ExClothBlock*& i : cloth)
        i->set_move_cancel(mv_ccl);
}

// 0x1405F9C30
void RobSkinDisp::set_tex_change(size_t num, const TexChange* texchg) {
    texchg_vec.clear();
    if (num && texchg)
        texchg_vec.insert(texchg_vec.end(), texchg, texchg + num);
}

void RobSkinDisp::skp_load(void* kv, const bone_database* bone_data) {
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

void RobSkinDisp::skp_load(const skin_param_osage_root& skp_root,
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

bool RobSkinDisp::skp_load_boc(
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

void RobSkinDisp::skp_load_file(void* data,
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

bool RobSkinDisp::skp_load_normal_ref(
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

RobDisp::RobDisp() : motion_node(), motion_matrix(), skin_disp(), parts_attr(), one_skin(),
disp_begin(), disp_max(), shadow_flag(), shadow_group(), position(), hyoutan_rpk(),
disable_update(), hyoutan_status(), skin_color(), wet_cloth(), wind_strength(), chara_color(),
npr_flag(), shadow_skin(), use_opd(), parts_short(), parts_append(), parts_white_one_l() {
    skin_disp = new RobSkinDisp[RPK_MAX];
    wind_strength = 1.0f;
    chara_color = true;
    osage_step = 1.0f;

    for (int32_t i = 0; i < RPK_MAX; i++)
        skin_disp[i].rob_disp = this;
}

RobDisp::~RobDisp() {
    reset();

    if (skin_disp) {
        delete[] skin_disp;
        skin_disp = 0;
    }
}

void RobDisp::add_motion_reset_data(const uint32_t& motnum, const float_t& frame, int32_t init_cnt) {
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].add_motion_reset_data(motnum, frame, init_cnt);
}

static void sub_140512C20(RobDisp* disp, render_context* rctx) {
    if (disp->hyoutan_obj.is_null() || disp->hyoutan_rpk == RPK_NONE)
        return;

    mat4 mat = mat4_identity;
    const char* name;
    if (disp->hyoutan_status == HYOUTAN_STAT_RIGHT_HAND) {
        name = "kl_te_r_wj";
        mat4_translate(0.0f, 0.0f, 0.082f, &mat);
        mat4_mul_rotate_zyx(&mat, (float_t)(-90.9 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    if (disp->hyoutan_status == HYOUTAN_STAT_LEFT_HAND) {
        name = "kl_te_l_wj";
        mat4_translate(0.0f, 0.0015f, -0.0812f, &mat);
        mat4_mul_rotate_zyx(&mat, (float_t)(-34.5 * DEG_TO_RAD), 0.0f, (float_t)(-179.5 * DEG_TO_RAD), &mat);
    }
    else
        name = "j_1_hyoutan_000wj";

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;

    const RobNode* node = disp->skin_disp[disp->hyoutan_rpk].get_node(name, aft_bone_data);
    if (!node || !node->mat_ptr)
        return;

    mat4_mul(node->mat_ptr, &mat, &mat);
    int32_t num_texchg = (int32_t)disp->hyoutan_texchg_list.size();
    if (num_texchg)
        rctx->disp_manager->set_texture_pattern(num_texchg, disp->hyoutan_texchg_list.data());
    rctx->disp_manager->entry_obj_by_object_info(mat, disp->hyoutan_obj);
    if (num_texchg)
        rctx->disp_manager->set_texture_pattern();
}

void RobDisp::disp(int32_t rob_id, render_context* rctx) {
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
    disp_manager.set_wet_param(wet_cloth);
    rctx->render_manager->npr_mask |= npr_flag;
    sub_140512C20(this, rctx);
    rob_chara_age_age_array_disp(rctx, rob_id, rctx->chara_reflect, chara_color);

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

// 0x140513130
object_info RobDisp::get_objid(ROB_PARTS_KIND rpk) const {
    if (rpk >= 0 && rpk < RPK_MAX)
        return skin_disp[rpk].obj_uid;
    return {};
}

// Added
RobSkinDisp* RobDisp::get_skin_work(ROB_PARTS_KIND rpk) {
    if (rpk >= 0 && rpk < RPK_MAX)
        return &skin_disp[rpk];
    return 0;
}

// 0x140513210
const RobSkinDisp* RobDisp::get_skin_work(ROB_PARTS_KIND rpk) const {
    if (rpk >= 0 && rpk < RPK_MAX)
        return &skin_disp[rpk];
    return 0;
}

const mat4* RobDisp::get_ex_data_bone_node_mat(ROB_PARTS_KIND rpk, const char* name) {
    return skin_disp[rpk].get_ex_data_bone_node_mat(name);
}

// 0x140512EF0
ROB_PARTS_KIND RobDisp::get_free_item(ROB_PARTS_KIND rpk) const {
    int32_t _rpk = rpk;
    if (rpk < 0 || rpk >= RPK_DISP_END)
        return RPK_NONE;

    for (const RobSkinDisp* skin = &skin_disp[rpk]; skin->obj_uid.not_null(); skin++) {
        if (++_rpk >= RPK_DISP_END)
            return RPK_NONE;
    }
    return (ROB_PARTS_KIND)_rpk;
}

// 0x1405135E0
void RobDisp::pos_reset(uint8_t init_cnt) {
    for (int32_t i = 0; i < RPK_MAX; i++)
        skin_disp[i].pos_reset(init_cnt);
}

void RobDisp::reset() {
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

void RobDisp::reset_ex_force() {
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].reset_ex_force();
}

void RobDisp::reset_init_data(RobNode* mot_node) {
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
    wet_cloth = 0.0f;
    wind_strength = 1.0f;
    chara_color = true;
    npr_flag = false;
    mat = mat4_identity;
}

void RobDisp::reset_nodes_ex_force(rob_osage_parts parts) {
    skin_disp[parts == ROB_OSAGE_PARTS_MUFFLER ? RPK_OUTER : RPK_KAMI]
        .reset_nodes_ex_force((rob_osage_parts_bit)(1 << parts));
}

void RobDisp::set_alpha_obj_flags(float_t alpha, mdl::ObjFlags flags) {
    if (one_skin)
        for (int32_t i = disp_begin; i < disp_max; i++)
            skin_disp[i].set_alpha_obj_flags(alpha, flags);
    else
        for (int32_t i = RPK_DISP_BEGIN; i <= RPK_DISP_END; i++)
            skin_disp[i].set_alpha_obj_flags(alpha, flags);
}

// 0x140513950
void RobDisp::set_base(ROB_PARTS_KIND rpk, object_info obj_uid, bool osage_reset,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (rpk < RPK_BASE_BEGIN && rpk > RPK_BASE_END)
        obj_uid = {};
    set_skin(obj_uid, rpk, osage_reset, bone_data, data, obj_db);
}

// 0x140513970
void RobDisp::set_body(object_info obj_uid,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    set_skin(obj_uid, RPK_BODY, false, bone_data, data, obj_db);
}

// 0x140513980
void RobDisp::set_col_color(ROB_PARTS_KIND rpk, const vec3& blend, const vec3& ofs) {
    skin_disp[rpk].set_col_color(blend, ofs);
}

// 0x1405139A0
void RobDisp::set_col_specular(ROB_PARTS_KIND rpk, const vec3& blend, const vec3& ofs) {
    skin_disp[rpk].set_col_specular(blend, ofs);
}

// 0x1405139C0
void RobDisp::set_col_type(ROB_PARTS_KIND rpk, int32_t type) {
    skin_disp[rpk].set_col_type(type);
}

void RobDisp::set_disable_collision(rob_osage_parts parts, bool disable) {
    skin_disp[parts == ROB_OSAGE_PARTS_MUFFLER ? RPK_OUTER : RPK_KAMI]
        .set_disable_collision((rob_osage_parts_bit)(1 << parts), disable);
}

void RobDisp::set_disp_flag(ROB_PARTS_KIND rpk, bool flag) {
    skin_disp[rpk].can_disp = flag;
}

// 0x140513B90
void RobDisp::set_hyoutan(ROB_PARTS_KIND rpk, object_info obj_uid, object_info obj_uid_sub,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (rpk <= RPK_BODY || rpk >= RPK_MAX) {
        obj_uid_sub = {};
        obj_uid = {};
    }
    hyoutan_obj = obj_uid_sub;
    hyoutan_rpk = rpk;
    set_skin(obj_uid, rpk, false, bone_data, data, obj_db);
    set_disp_flag(rpk, false);
}

// 0x140513BF0
void RobDisp::set_hyoutan_tex_change(const TexChange* texchg, int32_t tex_num) {
    hyoutan_texchg_list.clear();
    if (tex_num && texchg)
        hyoutan_texchg_list.insert(hyoutan_texchg_list.end(), texchg, texchg + tex_num);
}

// 0x140513C40
void RobDisp::set_item(ROB_PARTS_KIND rpk, object_info obj_uid,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (rpk < RPK_ITEM_BEGIN || rpk > RPK_ITEM_END)
        obj_uid = {};
    set_skin(obj_uid, rpk, false, bone_data, data, obj_db);
}

// 0x140513C60
void RobDisp::set_merge(ROB_PARTS_KIND rpk, bool flag) {
    if (flag)
        parts_attr[rpk] &= ~0x04;
    else
        parts_attr[rpk] |= 0x04;
}

void RobDisp::set_motion_node(const RobNode* mot_node, const bone_database* bone_data) {
    motion_node = mot_node;
    motion_matrix = mot_node->no_scale_mat;
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].set_motion_node(mot_node, bone_data);
}

void RobDisp::set_motion_reset_data(const uint32_t& motnum, const float_t& frame) {
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].set_motion_reset_data(motnum, frame);

    task_wind->stage_wind.wind->reset();
}

void RobDisp::set_motion_skin_param(int8_t rob_id, uint32_t motnum, int32_t frame) {
    for (int32_t i = RPK_ITEM_BEGIN; i <= RPK_ITEM_END; i++)
        skin_disp[i].set_motion_skin_param(rob_id, motnum, frame);
}

// 0x140513E00
void RobDisp::set_ofs(ROB_PARTS_KIND rpk, const vec3& pos, const vec3& rot, const vec3& scale) {
    skin_disp[rpk].set_ofs(pos, rot, scale);
}

void RobDisp::set_one_skin(bool value) {
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

void RobDisp::set_opd_blend_data(std::list<motion_blend_mot*>* a2) {
    opd_blend_data.clear();
    if (!a2)
        return;

    for (motion_blend_mot*& i : *a2) {
        ::opd_blend_data v11;
        v11.motnum = i->mot_key_data.motnum;
        v11.frame = i->mot_play_data.frame_data.frame;
        v11.frame_max = (float_t)i->mot_key_data.mot.frame_max - 1.0f;
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

void RobDisp::set_osage_move_cancel(uint8_t id, const float_t& mv_ccl) {
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

void RobDisp::set_osage_play_data_init(ROB_PARTS_KIND rpk, const float_t* opdi_data) {
    if (rpk >= RPK_ITEM_BEGIN && rpk <= RPK_ITEM_END)
        skin_disp[rpk].set_osage_play_data_init(opdi_data);
}

void RobDisp::set_osage_reset() {
    for (int32_t i = disp_begin; i < disp_max; i++)
        skin_disp[i].set_osage_reset();

    task_wind->stage_wind.wind->reset();
}

void RobDisp::set_osage_step(float_t value) {
    osage_step = value;
}

// 0x140514110
void RobDisp::set_reflect(ROB_PARTS_KIND rpk, bool flag) {
    if (flag)
        parts_attr[rpk] &= ~0x02;
    else
        parts_attr[rpk] |= 0x02;
}

// 0x140514130
void RobDisp::set_shadow(ROB_PARTS_KIND rpk, bool flag) {
    if (flag)
        parts_attr[rpk] &= ~0x01;
    else
        parts_attr[rpk] |= 0x01;
}

void RobDisp::set_shadow_group(int32_t rob_id) {
    if (rob_id == ROB_ID_1P)
        shadow_group = SHADOW_GROUP_CHARA;
    else
        shadow_group = SHADOW_GROUP_STAGE;
}

// 0x1405141B0
void RobDisp::set_skin(object_info obj_uid, ROB_PARTS_KIND rpk,
    bool osage_reset, const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (rpk < 0 || rpk >= RPK_MAX || !motion_node)
        return;

    skin_disp[rpk].set(obj_uid, motion_node, osage_reset, bone_data, data, obj_db);
    set_disp_flag(rpk, true);
}

// 0x1405143B0
void RobDisp::set_tex_change(ROB_PARTS_KIND rpk, const TexChange* texchg, int32_t tex_num) {
    skin_disp[rpk].set_tex_change(tex_num, texchg);
}

void RobDisp::skp_load(ROB_PARTS_KIND rpk, const skin_param_osage_root& skp_root,
    std::vector<skin_param_osage_node>& vec, skin_param_file_data* skp_file_data, const bone_database* bone_data) {
    if (rpk >= 0 && rpk < RPK_MAX)
        skin_disp[rpk].skp_load(skp_root, vec, skp_file_data, bone_data);
}

RobItemTXHD::RobItemTXHD() : src(), dst(), dst_copy() {

}

RobItemTXHD::~RobItemTXHD() {

}

// 0x140522990
void RobItem::equip_phase0(RobItemEquip* item_set) {
    if (!get_rob_item_header(m_cn))
        return;

    free_copy_texture_all();
    clear_texchg_list();

    for (int32_t sub_id = 0; sub_id < ROB_ITEM_EQUIP_SUB_ID_MAX; sub_id++) {
        uint32_t item_no = item_set->item_no[sub_id];
        if (!item_no || sub_id == ROB_ITEM_EQUIP_SUB_ID_HADA)
            continue;

        const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
        if (!tbl)
            continue;

        make_chg_tex(item_no, tbl);
        make_nude_attr_map(item_no, tbl);
    }
}
// 0x140522A30
void RobItem::equip_phase1(RobDisp* rdp,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    const RobData* rob_data = get_rob_data(m_cn);
    for (int32_t i = RPK_BASE_BEGIN; i <= RPK_BASE_END; i++) {
        ROB_PARTS_KIND rpk = (ROB_PARTS_KIND)i;
        rdp->set_base(rpk, rob_data->body_obj_uid[i], false, bone_data, data, obj_db);
        auto elem = m_nude_attr_map.find(rpk);
        if (elem != m_nude_attr_map.end())
            set_texture(rdp, elem->second, rpk);
        else
            reset_texture(rdp, rpk);
    }

    for (int32_t i = RPK_ITEM_BEGIN; i <= RPK_ITEM_END; i++) {
        ROB_PARTS_KIND rpk = (ROB_PARTS_KIND)i;
        rdp->set_item(rpk, {}, bone_data, data, obj_db);
        reset_texture(rdp, rpk);
    }
    reset_texture(rdp, RPK_NONE, true);

    m_rpk_map.clear();
    m_rpk_sp_map.clear();
    m_head_map.clear();
}

// 0x140522C60
void RobItem::equip_phase2(RobDisp* rdp, RobItemEquip* item_set,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!get_rob_item_header(m_cn))
        return;

    const ROB_ITEM_EQUIP_SUB_ID* id = equip_sub_id_phase2;
    while (*id != ROB_ITEM_EQUIP_SUB_ID_MAX) {
        uint32_t item_no = item_set->item_no[*id];
        if (item_no) {
            const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
            if (tbl && tbl->type != ROB_ITEM_TYPE_REM && (tbl->attr & ROB_ITEM_ATTR_OBJ))
                set_obj_phase2(rdp, item_no, tbl, *id, bone_data, data, obj_db);
        }
        id++;
    }
}

// 0x140522D00
void RobItem::equip_phase3(RobDisp* rdp, RobItemEquip* item_set,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!get_rob_item_header(m_cn))
        return;

    const ROB_ITEM_EQUIP_SUB_ID* id = equip_sub_id_phase3;
    while (*id != ROB_ITEM_EQUIP_SUB_ID_MAX) {
        uint32_t item_no = item_set->item_no[*id];
        if (item_no) {
            const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
            if (tbl && tbl->type != ROB_ITEM_TYPE_REM && (tbl->attr & ROB_ITEM_ATTR_OBJ))
                set_obj_phase3(rdp, item_no, tbl, bone_data, data, obj_db);
        }
        id++;
    }
}

// 0x140522D90
void RobItem::equip_phase4(RobDisp* rdp, RobItemEquip* item_set,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    if (!get_rob_item_header(m_cn))
        return;

    const ROB_ITEM_EQUIP_SUB_ID* id = equip_sub_id_phase4;
    while (*id != ROB_ITEM_EQUIP_SUB_ID_MAX) {
        uint32_t item_no = item_set->item_no[*id];
        if (!item_no) {
            id++;
            continue;
        }

        const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
        if (!tbl || tbl->type != ROB_ITEM_TYPE_REPLACE)
            continue;

        for (const RobItemDataObj& i : tbl->data.obj) {
            if (i.uid.is_null())
                continue;

            ROB_PARTS_KIND rpk = get_rpk_item(*id);
            rdp->set_item(rpk, i.uid, bone_data, data, obj_db);
            m_rpk_map.insert({ i.uid, rpk });
            if (tbl->attr & (ROB_ITEM_ATTR_COL | ROB_ITEM_ATTR_TEX))
                set_texture(rdp, item_no, rpk);

            if (i.replace_id == RPK_NONE)
                continue;

            rdp->set_base(i.replace_id, {}, false, bone_data, data, obj_db);
            switch (i.replace_id) {
            case RPK_UDE_R:
                hide_rpk_sp(rdp, RPK_SP_UDE_R, bone_data, data, obj_db);
                break;
            case RPK_UDE_L:
                hide_rpk_sp(rdp, RPK_SP_UDE_L, bone_data, data, obj_db);
                break;
            case RPK_TE_R:
                hide_rpk_sp(rdp, RPK_SP_TE_R, bone_data, data, obj_db);
                break;
            case RPK_TE_L:
                hide_rpk_sp(rdp, RPK_SP_TE_L, bone_data, data, obj_db);
                break;
            case RPK_SUNE:
                hide_rpk_sp(rdp, RPK_SP_SUNE, bone_data, data, obj_db);
                break;
            case RPK_ASI:
                hide_rpk_sp(rdp, RPK_SP_ASI, bone_data, data, obj_db);
                break;
            }
        }
        id++;
    }
}

// 0x140522F90
void RobItem::equip_phase5(RobDisp* rdp, RobItemEquip* item_set) {
    if (!get_rob_item_header(m_cn))
        return;

    uint32_t item_no = item_set->item_no[ROB_ITEM_EQUIP_SUB_ID_HADA];
    if (!item_no)
        return;

    const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
    if (!tbl || tbl->data.col.size())
        return;

    vec3 blend_color = tbl->data.col[0].color.blend_color;
    vec3 offset_color = tbl->data.col[0].color.offset_color;
    vec3 blend_specular = 1.0f;
    vec3 offset_specular = 0.0f;
    if (tbl->data.col.size() > 1) {
        blend_specular = tbl->data.col[1].color.blend_color;
        offset_specular = tbl->data.col[1].color.offset_color;
    }

    for (int32_t i = RPK_DISP_BEGIN; i <= RPK_DISP_END; i++) {
        ROB_PARTS_KIND rpk = (ROB_PARTS_KIND)i;
        if (rdp->get_objid(rpk).is_null())
            continue;

        rdp->set_col_type(rpk, 0);
        rdp->set_col_color(rpk, blend_color, offset_color);
        rdp->set_col_specular(rpk, blend_specular, offset_specular);
    }
}

// 0x1405231D0
void RobItem::equip_phase6(RobDisp* rdp, RobItemEquip* item_set) {
    if (!get_rob_item_header(m_cn))
        return;

    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
        equip_phase6(rdp, &m_equip, item_set->item_no[i]);
}

// 0x140523230
void RobItem::equip_phase6(RobDisp* rdp, RobItemEquip* item_set, uint32_t item_no) {
    if (!item_no)
        return;

    const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
    if (!tbl || tbl->type == ROB_ITEM_TYPE_NONE || tbl->type == ROB_ITEM_TYPE_REM)
        return;

    if (tbl->attr & ROB_ITEM_ATTR_OFS)
        for (const RobItemDataOfs& i : tbl->data.ofs) {
            if (item_set->item_no[i.equip_sub_id] != i.item_no)
                continue;

            for (const RobItemDataObj& j : tbl->data.obj) {
                if (j.uid.is_null())
                    break;

                auto elem = m_rpk_map.find(j.uid);
                if (elem != m_rpk_map.end())
                    rdp->set_ofs(elem->second, i.trans, i.rot, i.scale);
            }
            break;
        }

    for (const RobItemDataObj& i : tbl->data.obj) {
        if (i.uid.is_null())
            break;

        auto elem = m_rpk_map.find(i.uid);
        if (elem != m_rpk_map.end()) {
            ROB_PARTS_KIND rpk = elem->second;
            rdp->set_shadow(rpk, !(tbl->attr & ROB_ITEM_ATTR_NO_SDW));
            rdp->set_reflect(rpk, !(tbl->attr & ROB_ITEM_ATTR_NO_REF));
            rdp->set_merge(rpk, !(tbl->attr & ROB_ITEM_ATTR_NO_MRG));
        }
    }
}

// 0x1405234E0
void RobItem::equip_phase7() {
    init_nude_attr_map();
    m_rpk_map.clear();
    m_rpk_sp_map.clear();
}

// 0x140525B90
ROB_PARTS_KIND RobItem::get_rpk_item(ROB_ITEM_EQUIP_SUB_ID id) const {
    return rpk_item[id];
}

// 0x140525D90
void RobItem::hide_rpk_sp(RobDisp* disp, int32_t rpk_sp,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    auto elem = m_rpk_sp_map.find(rpk_sp);
    if (elem != m_rpk_sp_map.end())
        disp->set_item(elem->second, {}, bone_data, data, obj_db);
}

// 0x140526FD0
void RobItem::make_chg_tex(uint32_t item_no, const RobItemTable* tbl) {
    if (!(tbl->attr & (ROB_ITEM_ATTR_COL | ROB_ITEM_ATTR_TEX)))
        return;

    if (!(tbl->attr & ROB_ITEM_ATTR_COL)) {
        std::vector<RobItemTXHD> tex_chg_vec;
        for (const RobItemDataTex& i : tbl->data.tex) {
            RobItemTXHD txhd;
            txhd.src = texture_manager_get_texture(i.org_uid);
            txhd.dst = texture_manager_get_texture(i.chg_uid);
            txhd.dst_copy = false;
            tex_chg_vec.push_back(txhd);
        }
        m_txhd_map.insert({ item_no, tex_chg_vec });
        return;
    }
    else if (tbl->data.col.size() <= 0)
        return;

    std::vector<int32_t> chg_tex_ids;
    if (tbl->attr & ROB_ITEM_ATTR_TEX)
        for (const RobItemDataTex& i : tbl->data.tex)
            chg_tex_ids.push_back(i.chg_uid);
    else
        for (const RobItemDataCol& i : tbl->data.col)
            chg_tex_ids.push_back(i.tex_uid);

    std::vector<RobItemTXHD> tex_chg_vec;
    size_t index = 0;
    for (int32_t& i : chg_tex_ids) {
        size_t j = &i - chg_tex_ids.data();
        texture* tex = texture_manager_get_texture(i);
        if (!tex) {
            index++;
            continue;
        }

        bool dst_copy = false;
        if (tbl->data.col[j].flag.m.is_available) {
            tex = texture_create_copy_texture_apply_color_tone(
                texture_manager_get_copy_id(0x30), tex, &tbl->data.col[j].color);
            dst_copy = true;
        }

        RobItemTXHD txhd;
        txhd.src = texture_manager_get_texture(tbl->data.col[j].tex_uid);
        txhd.dst = tex;
        txhd.dst_copy = dst_copy;
        tex_chg_vec.push_back(txhd);
    }
    m_txhd_map.insert({ item_no, tex_chg_vec });
}

// 0x140525200
void RobItem::free_copy_texture_all() {
    for (auto i = m_txhd_map.begin(); i != m_txhd_map.end(); i++)
        free_copy_texture(i);
    m_txhd_map.clear();
}

// 0x1405267E0
void RobItem::init_nude_attr_map() {
    m_nude_attr_map.clear();
}

// 0x140527280
void RobItem::make_nude_attr_map(uint32_t item_no, const RobItemTable* tbl) {
    if (!(tbl->attr & ROB_ITEM_ATTR_NUDE) || !(tbl->attr & (ROB_ITEM_ATTR_COL | ROB_ITEM_ATTR_TEX)))
        return;

    for (const RobItemDataObj& i : tbl->data.obj) {
        if (i.replace_id == RPK_NONE)
            continue;

        auto elem = m_nude_attr_map.find(i.replace_id);
        if (elem == m_nude_attr_map.end()) {
            std::vector<uint32_t> vec;
            vec.push_back(item_no);
            m_nude_attr_map.insert({ i.replace_id, vec });
        }
        else {
            std::vector<uint32_t> vec = elem->second;
            vec.push_back(item_no);
            m_nude_attr_map.insert_or_assign(i.replace_id, vec);
        }
    }
}

// 0x14052C560
void RobItem::set_obj_phase2(RobDisp* rdp, uint32_t item_no, const RobItemTable* tbl, ROB_ITEM_EQUIP_SUB_ID id,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    for (const RobItemDataObj& i : tbl->data.obj) {
        if (i.uid.is_null())
            break;

        if (tbl->type == ROB_ITEM_TYPE_REPLACE) {
            if (i.replace_id != RPK_NONE) {
                rdp->set_base(i.replace_id, i.uid, false, bone_data, data, obj_db);
                if (tbl->attr & (ROB_ITEM_ATTR_COL | ROB_ITEM_ATTR_TEX))
                    set_texture(rdp, item_no, i.replace_id);
            }
            continue;
        }

        else if (tbl->type)
            continue;

        ROB_PARTS_KIND rpk = get_rpk_item(id);
        if (rpk != RPK_NONE) {
            rdp->set_item(rpk, i.uid, bone_data, data, obj_db);
            m_rpk_map.insert({ i.uid, rpk });
            if (tbl->attr & (ROB_ITEM_ATTR_COL | ROB_ITEM_ATTR_TEX))
                set_texture(rdp, item_no, rpk);
        }
        else {
            ROB_PARTS_KIND rpk = rdp->get_free_item(RPK_ITEM_ETC_BEGIN);
            if (rpk != RPK_NONE && (!(tbl->attr & ROB_ITEM_ATTR_HYOUTAN) || &i - tbl->data.obj.data())) {
                bool is_hyoutan = false;
                if ((tbl->attr & ROB_ITEM_ATTR_HYOUTAN) && &i - tbl->data.obj.data() == 1) {
                    rdp->set_hyoutan(rpk, tbl->data.obj[1].uid,
                        tbl->data.obj[0].uid, bone_data, data, obj_db);
                    is_hyoutan = true;
                }
                else
                    rdp->set_item(rpk, i.uid, bone_data, data, obj_db);
                m_rpk_map.insert({ i.uid, rpk });
                if (tbl->attr & (ROB_ITEM_ATTR_COL | ROB_ITEM_ATTR_TEX))
                    set_texture(rdp, item_no, rpk, is_hyoutan);
            }
        }
    }

    m_head_map.clear();
    if (!(tbl->attr & ROB_ITEM_ATTR_HEAD))
        return;

    const char* chara_name = get_chara_name(m_cn);
    for (int32_t i = 0; i < 9; i++) {
        std::string buf = sprintf_s_string("%sITM%03d_ATAM_HEAD_%02d_SP__DIVSKN", chara_name, item_no, i);
        object_info obj_info = obj_db->get_object_info(buf.c_str());
        if (obj_info.not_null())
            m_head_map.insert({ i, obj_info });
    }
}

// 0x14052C8C0
void RobItem::set_obj_phase3(RobDisp* rdp, uint32_t item_no, const RobItemTable* tbl,
    const bone_database* bone_data, void* data, const object_database* obj_db) {
    for (const RobItemDataObj& i : tbl->data.obj) {
        object_info uid = i.uid;
        ROB_PARTS_KIND replace_id = i.replace_id;
        if (tbl->type == ROB_ITEM_TYPE_REPLACE) {
            if (uid.is_null()) {
                if (replace_id == RPK_NONE)
                    continue;
            }
            else if (replace_id == RPK_NONE);
            else if (i.replace_id >= RPK_SP_UDE_L && i.replace_id <= RPK_SP_ASI) {
                ROB_PARTS_KIND rpk = rdp->get_free_item(RPK_ITEM_BEGIN);
                if (rpk == RPK_NONE)
                    continue;

                rdp->set_item(rpk, uid, bone_data, data, obj_db);
                m_rpk_map.insert({ uid, rpk });
                m_rpk_sp_map.insert({ replace_id, rpk });
                replace_id = rpk;
            }
            else {
                rdp->set_base(replace_id, uid, false, bone_data, data, obj_db);
            }
        }
        else if (!tbl->type) {
            replace_id = rdp->get_free_item(RPK_ITEM_ETC_BEGIN);
            if (replace_id != RPK_NONE) {
                rdp->set_item(replace_id, uid, bone_data, data, obj_db);
                m_rpk_map.insert({ uid, replace_id });
                continue;
            }
        }
        else
            continue;

        if (tbl->attr & (ROB_ITEM_ATTR_COL | ROB_ITEM_ATTR_TEX))
            set_texture(rdp, item_no, replace_id);
    }
}

// 0x14052CCC0
void RobItem::set_texture(RobDisp* rdp,
    const std::vector<uint32_t>& item_nos, ROB_PARTS_KIND rpk, bool is_hyoutan) {
    if (rpk < 0 || rpk >= RPK_MAX)
        return;

    for (const uint32_t& i : item_nos) {
        auto elem = m_txhd_map.find(i);
        if (elem == m_txhd_map.end())
            continue;

        for (RobItemTXHD& j : elem->second)
            if (j.src && j.dst)
                m_texchg_list[rpk].push_back({ j.src->id, j.dst->id });
    }

    if (is_hyoutan)
        rdp->set_hyoutan_tex_change(m_texchg_list[rpk].data(), (int32_t)m_texchg_list[rpk].size());
    else
        rdp->set_tex_change(rpk, m_texchg_list[rpk].data(), (int32_t)m_texchg_list[rpk].size());
}

// 0x14052CB70
void RobItem::set_texture(RobDisp* rdp, uint32_t item_no, ROB_PARTS_KIND rpk, bool is_hyoutan) {
    if (rpk < 0 || rpk >= RPK_MAX)
        return;

    auto elem = m_txhd_map.find(item_no);
    if (elem == m_txhd_map.end())
        return;

    for (RobItemTXHD& i : elem->second)
        if (i.src && i.dst)
            m_texchg_list[rpk].push_back({ i.src->id, i.dst->id });

    if (is_hyoutan)
        rdp->set_hyoutan_tex_change(m_texchg_list[rpk].data(), (int32_t)m_texchg_list[rpk].size());
    else
        rdp->set_tex_change(rpk, m_texchg_list[rpk].data(), (int32_t)m_texchg_list[rpk].size());
}

// 0x14052B4C0
void RobItem::reset_texture(RobDisp* rdp, ROB_PARTS_KIND rpk, bool is_hyoutan) {
    if (is_hyoutan)
        rdp->set_hyoutan_tex_change(0, 0);
    else
        rdp->set_tex_change(rpk, 0, 0);
}

// 0x140521A30
void RobItem::clear_texchg_list() {
    for (std::vector<TexChange>& i : m_texchg_list)
        i.clear();
}

// 0x140522480
void RobItem::disp_obj_internal(object_info obj_uid, mat4& mat, uint32_t item_no, render_context* rctx) {
    static mat4 env_mtx[MATRIX_BUFFER_COUNT];

    TexChange* texchg = 0;
    size_t num_texchg = 0;

    obj_skin* skin = objset_info_storage_get_obj_skin(obj_uid);

    auto elem = m_txhd_map.find(item_no);
    if (elem != m_txhd_map.end()) {
        std::vector<RobItemTXHD> tex_chg_vec = elem->second;
        num_texchg = tex_chg_vec.size();
        if (num_texchg) {
            texchg = new TexChange[num_texchg];
            const RobItemTXHD* item_txhd = tex_chg_vec.data();
            for (size_t i = 0; i < num_texchg; i++) {
                texchg[i].org_texid = item_txhd[i].src->id;
                texchg[i].change_texid = item_txhd[i].dst->id;
            }
        }
    }

    if (!skin)
        rctx->disp_manager->entry_obj_by_object_info(mat4_identity, obj_uid);
    else if (skin->num_bone <= MATRIX_BUFFER_COUNT) {
        for (int32_t i = 0; i < skin->num_bone; i++)
            env_mtx[i] = mat;
        rctx->disp_manager->entry_obj_by_object_info(mat4_identity, obj_uid, env_mtx);
    }

    if (num_texchg) {
        rctx->disp_manager->set_texture_pattern(0, 0);
        delete[] texchg;
    }
}

// 0x14052C430
bool RobItem::s_repair_equip(CHARA_NUM cn, uint32_t item_no, int32_t id, RobItemEquip* rep_item_set) {
    const RobItemEquip* item_set = get_default_costume_data(cn, item_no);
    if (item_set) {
        rep_item_set->item_no[id] = item_set->item_no[id];
        return true;
    }
    return false;
}

RobItem::RobItem() : m_cn(), m_cn_load(), m_equip(), m_equip_load() {

}

RobItem::~RobItem() {
    free_copy_texture_all();
    init_nude_attr_map();
    clear_texchg_list();
    m_rpk_map.clear();
    m_rpk_sp_map.clear();
    m_head_map.clear();
}

// 0x14052C530
void RobItem::set_chara_num(CHARA_NUM cn) {
    m_cn = cn;
}

// 0x14052AE30
void RobItem::req_obj(uint32_t item_no, void* data, const object_database* obj_db) {
    if (!item_no)
        return;

    m_cn_load = m_cn;
    const std::vector<uint32_t>* item_objset = get_rob_item_table_objset(m_cn, item_no);
    if (!item_objset)
        return;

    for (uint32_t i : *item_objset)
        objset_info_storage_load_set(data, obj_db, i);
}

// 0x14052D3D0
bool RobItem::wait_obj(uint32_t item_no) {
    if (!item_no)
        return false;

    const std::vector<uint32_t>* item_objset = get_rob_item_table_objset(m_cn_load, item_no);
    if (!item_objset || !item_objset->size())
        return false;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1 && objset_info_storage_load_obj_set_check_not_read(i))
            return true;
    return false;
}

// 0x1405253A0
void RobItem::free_obj(uint32_t item_no) {
    if (!item_no)
        return;

    const std::vector<uint32_t>* item_objset = get_rob_item_table_objset(m_cn_load, item_no);
    if (!item_objset)
        return;

    for (uint32_t i : *item_objset)
        objset_info_storage_unload_set(i);

    free_copy_texture(item_no);
}

// 0x14052AEA0
void RobItem::req_obj_all(void* data, const object_database* obj_db) {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++) {
        uint32_t item_no = m_equip.item_no[i];
        if (item_no && m_equip_load.item_no[i] != item_no) {
            req_obj(item_no, data, obj_db);
            m_equip_load.item_no[i] = item_no;
        }
    }
}

// 0x14052D460
bool RobItem::wait_obj_all() {
    bool ret = false;
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
        if (m_equip_load.item_no[i])
            ret |= wait_obj(m_equip_load.item_no[i]);
    return ret;
}

// 0x1405254B0
void RobItem::free_obj_all() {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
        if (m_equip_load.item_no[i])
            free_obj(m_equip_load.item_no[i]);
}

// 0x140525500
void RobItem::free_obj_diff() {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++) {
        uint32_t item_no = m_equip_load.item_no[i];
        if (item_no && m_equip.item_no[i] != item_no) {
            free_obj(item_no);
            m_equip_load.item_no[i] = 0;
        }
    }
}

// 0x14052AD90
void RobItem::regist_item(ROB_ITEM_EQUIP_SUB_ID id, uint32_t item_no) {
    m_equip.item_no[id] = item_no;
}

// 0x14052ADF0
void RobItem::regist_item_one(uint32_t item_no) {
    if (!item_no)
        return;

    const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
    if (tbl)
        regist_item(tbl->equip_sub_id, item_no);
}

// 0x14052ADA0
void RobItem::regist_item_all(const RobItemEquip* item_set) {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
        regist_item((ROB_ITEM_EQUIP_SUB_ID)i, item_set->item_no[i]);
}

// 0x140521FA0
void RobItem::delete_item(uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
    if (tbl)
        m_equip.item_no[tbl->equip_sub_id] = 0;
}

void RobItem::equip(int32_t rc, const bone_database* bone_data,
    void* data, const object_database* obj_db) {
    RobDisp* disp = get_rob_management()->get_rob_robdisp_work((ROB_ID)rc);
    equip_phase0(&m_equip);
    equip_phase1(disp, bone_data, data, obj_db);
    equip_phase2(disp, &m_equip, bone_data, data, obj_db);
    equip_phase3(disp, &m_equip, bone_data, data, obj_db);
    equip_phase4(disp, &m_equip, bone_data, data, obj_db);
    equip_phase5(disp, &m_equip);
    equip_phase6(disp, &m_equip);
    equip_phase7();
}

// 0x140525730
ROB_ITEM_EQUIP_ID RobItem::get_equip_id(uint32_t item_no) const {
    if (!get_rob_item_header(m_cn))
        return ROB_ITEM_EQUIP_ID_NONE;

    ROB_ITEM_EQUIP_SUB_ID id = s_get_equip_sub_id(m_cn, item_no);
    if (id != ROB_ITEM_EQUIP_SUB_ID_NONE)
        return equip_id_conv_table[id];
    return ROB_ITEM_EQUIP_ID_NONE;
}

// 0x140525780
ROB_ITEM_EQUIP_SUB_ID RobItem::get_equip_sub_id(uint32_t item_no) const {
    const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
    if (tbl)
        return tbl->equip_sub_id;
    return ROB_ITEM_EQUIP_SUB_ID_NONE;
}

// Missing
uint32_t RobItem::check_exclusive_item(uint32_t item_no) const {
    const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
    if (tbl && tbl->equip_sub_id != -1)
        return m_equip.item_no[tbl->equip_sub_id];
    return 0;
}

// 0x140525AA0
RobItemEquip* RobItem::get_equip() {
    return (RobItemEquip*)((const RobItem*)this)->get_equip();
}

// 0x140525720
const RobItemEquip* RobItem::get_equip() const {
    return &m_equip;
}

// Missing
bool RobItem::is_equipped_item(uint32_t item_no) const {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
        if (m_equip.item_no[i] == item_no)
            return true;
    return false;
}

// 0x140525F80
void RobItem::init_disp_obj(uint32_t item_no) {
    if (!item_no)
        return;

    const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
    if (!tbl)
        return;

    make_chg_tex(item_no, tbl);
}

// 0x1405223C0
void RobItem::disp_obj(uint32_t item_no, render_context* rctx, const mat4& global_mat) {
    if (!item_no)
        return;

    const RobItemTable* tbl = get_rob_item_table(m_cn, item_no);
    if (!tbl)
        return;

    mat4 mat = global_mat;
    for (const RobItemDataObj& i : tbl->data.obj)
        if (i.uid.not_null())
            disp_obj_internal(i.uid, mat, item_no, rctx);
}

void RobItem::get_item_texchange_list(uint32_t item_no, std::vector<TexChange>& tex_chg_list) {
    auto elem = m_txhd_map.find(item_no);
    if (elem != m_txhd_map.end()) {
        std::vector<RobItemTXHD> tex_chg_vec = elem->second;
        for (RobItemTXHD& i : tex_chg_vec)
            tex_chg_list.push_back({ i.src->id, i.dst->id });
    }
}

// Missing
RobItemEquip* RobItem::get_equip_load() {
    return (RobItemEquip*)((const RobItem*)this)->get_equip_load();
}

// Missing
const RobItemEquip* RobItem::get_equip_load() const {
    return &m_equip_load;
}

bool RobItem::check_for_npr_flag() const {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++) {
        const RobItemTable* tbl = get_rob_item_table(m_cn, m_equip.item_no[i]);
        if (tbl && tbl->npr_flag)
            return true;
    }
    return false;
}

object_info RobItem::get_head_object_replace(int32_t head_object_id) const {
    auto elem = m_head_map.find(head_object_id);
    if (elem != m_head_map.end())
        return elem->second;
    return {};
}

float_t RobItem::get_face_depth() const {
    float_t face_depth = 0.0f;
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++) {
        const RobItemTable* tbl = get_rob_item_table(m_cn, m_equip.item_no[i]);
        if (tbl)
            face_depth = max_def(tbl->face_depth, face_depth);
    }
    return face_depth;
}

const char* RobItem::s_get_equip_sub_id_str(ROB_ITEM_EQUIP_SUB_ID id) {
    if (id >= 0 && id < ROB_ITEM_EQUIP_SUB_ID_MAX)
        return rob_item_equip_sub_id_str[id];
    return 0;
}

// 0x140525A40
const char* RobItem::s_get_name(const CHARA_NUM& cn, uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        return tbl->name.c_str();
    return 0;
}

// 0x14052BDC0
ROB_ITEM_EQUIP_ID RobItem::s_get_equip_id(CHARA_NUM cn, uint32_t item_no) {
    if (!get_rob_item_header(cn))
        return ROB_ITEM_EQUIP_ID_NONE;

    ROB_ITEM_EQUIP_SUB_ID id = s_get_equip_sub_id(cn, item_no);
    if (id != ROB_ITEM_EQUIP_SUB_ID_NONE)
        return equip_id_conv_table[id];
    return ROB_ITEM_EQUIP_ID_NONE;
}

// 0x14052BF00
ROB_ITEM_EQUIP_SUB_ID RobItem::s_get_equip_sub_id(CHARA_NUM cn, uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        return tbl->equip_sub_id;
    return ROB_ITEM_EQUIP_SUB_ID_NONE;
}

// Missing
void RobItem::s_get_equip_sub_group(int32_t id, std::vector<int32_t>* sub_group) {
    if (!sub_group)
        return;

    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
        if (id != equip_id_conv_table[i])
            sub_group->push_back(i);
}

// 0x14052CAC0
void RobItem::regist_item_all(const RobItemEquipInit* item_set) {
    for (int32_t i = 0; i < 4; i++)
        regist_item_one(item_set->item_no[i]);
}

// 0x140525160
void RobItem::free_copy_texture(uint32_t item_no) {
    auto elem = m_txhd_map.find(item_no);
    if (elem != m_txhd_map.end()) {
        free_copy_texture(elem);
        m_txhd_map.erase(elem);
    }
}

// 0x140525340
void RobItem::free_copy_texture(std::map<uint32_t, std::vector<RobItemTXHD>>::iterator elem) {
    for (RobItemTXHD& i : elem->second)
        if (i.dst_copy)
            texture_release(i.dst);
    elem->second.clear();
}

// 0x14052BB30
bool RobItem::s_is_replace_part(CHARA_NUM cn, uint32_t item_no, ROB_ITEM_EQUIP_SUB_ID id) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl) {
        if (id == ROB_ITEM_EQUIP_SUB_ID_HADA || tbl->type == ROB_ITEM_TYPE_REM)
            return true;
        return get_rob_item_table_objset(cn, item_no);
    }
    return false;
}

// 0x14052BBA0
bool RobItem::s_equip_ok(CHARA_NUM cn, uint32_t item_no, ROB_ITEM_EQUIP_SUB_ID id) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        return id == tbl->equip_sub_id;
    return false;
}

// 0x14052BF40
uint32_t RobItem::s_get_exclusion(CHARA_NUM cn, uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        return tbl->exclusion;
    return 0;
}

// 0x14052C370
uint32_t RobItem::s_get_point(CHARA_NUM cn, uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl) {
        int32_t ret = s_check_ng_item_point(cn, item_no);
        if (ret < 0)
            ret = tbl->point;
        return ret;
    }
    return 0;
}

// 0x14052C2A0
void RobItem::s_get_offset_list(CHARA_NUM cn, uint32_t item_no, std::vector<RobItemDataOfs>& ofs_list, bool clear) {
    if (clear)
        ofs_list.clear();

    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        ofs_list.insert(ofs_list.end(), tbl->data.ofs.begin(), tbl->data.ofs.end());
}

// 0x14052C410
ROB_ITEM_TYPE RobItem::s_get_type(CHARA_NUM cn, uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        return tbl->type;
    return ROB_ITEM_TYPE_NONE;
}

// 0x14052C410
bool RobItem::s_is_texorg(CHARA_NUM cn, uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        return tbl->flag.m.is_texorg;
    return 0;
}

// 0x14052C3F0
bool RobItem::s_is_objorg(CHARA_NUM cn, uint32_t item_no) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        return tbl->flag.m.is_objorg;
    return 0;
}

// 0x14052BC80
std::string RobItem::s_check_ng_item_name(CHARA_NUM cn, uint32_t item_no) {
    // NOT IMPL!
    return "";
}

// 0x14052BD40
int32_t RobItem::s_check_ng_item_point(CHARA_NUM cn, uint32_t item_no) {
    // NOT IMPL!
    return -1;
}

// 0x1405256C0
uint32_t RobItem::get_dbgset_num(CHARA_NUM cn) {
    const RobItemHeader* tbl = get_rob_item_header(cn);
    if (tbl)
        return (uint32_t)tbl->dbgset.size();
    return 0;
}

// 0x1405256A0
const RobItemDbgSet* RobItem::get_dbgset(CHARA_NUM cn) {
    const RobItemHeader* tbl = get_rob_item_header(cn);
    if (tbl)
        return &tbl->dbgset;
    return 0;
}

// 0x140525FC0
void RobItem::init_have_dbg() {
    if (s_have_dbg_ref)
        s_have_dbg_ref++;
    else {
        for (int32_t i = 0; i < CN_MAX; i++) {
            CHARA_NUM cn = (CHARA_NUM)i;
            const RobItemHeader* tbl = get_rob_item_header(cn);
            if (!tbl)
                continue;

            bool found = false;
            for (int32_t j = 0; j < ROB_ITEM_EQUIP_ID_MAX; j++)
                if (s_have_dbg[i].part[j].item_no.size()) {
                    found = true;
                    break;
                }

            if (found)
                break;

            for (auto& j : tbl->table) {
                ROB_ITEM_EQUIP_ID equip = s_get_equip_id(cn, j.first);
                if (equip != ROB_ITEM_EQUIP_ID_NONE)
                    s_have_dbg[cn].part[equip].item_no.push_back(j.first);
            }
        }
        s_have_dbg_ref = 1;
    }
}

// 0x140522000
void RobItem::dest_have_dbg() {
    if (--s_have_dbg_ref < 0)
        s_have_dbg_ref = 0;
    else if (!s_have_dbg_ref)
        for (int32_t i = 0; i < CN_MAX; i++)
            for (int32_t j = 0; j < ROB_ITEM_EQUIP_ID_MAX; j++) {
                RobItemHavePart& part = s_have_dbg[i].part[j];
                part.item_no.clear();
                part.item_no.shrink_to_fit();
            }
}

// 0x140526230
void RobItem::init_have_sub_dbg() {
    if (s_have_sub_dbg_ref)
        s_have_sub_dbg_ref++;
    else {
        for (int32_t i = 0; i < CN_MAX; i++) {
            CHARA_NUM cn = (CHARA_NUM)i;
            const RobItemHeader* tbl = get_rob_item_header(cn);
            if (!tbl)
                continue;

            for (auto& j : tbl->table) {
                const RobItemTable* tbl = get_rob_item_table(cn, j.first);
                if (tbl && tbl->equip_sub_id != ROB_ITEM_EQUIP_SUB_ID_NONE)
                    s_have_sub_dbg[cn].part[tbl->equip_sub_id].item_no.push_back(j.first);
            }
        }
        s_have_sub_dbg_ref = 1;
    }
}

// 0x140522070
void RobItem::dest_have_sub_dbg() {
    if (--s_have_sub_dbg_ref < 0)
        s_have_sub_dbg_ref = 0;
    else if (!s_have_sub_dbg_ref)
        for (int32_t i = 0; i < CN_MAX; i++)
            for (int32_t j = 0; j < ROB_ITEM_EQUIP_ID_MAX; j++) {
                RobItemHavePart& part = s_have_sub_dbg[i].part[j];
                part.item_no.clear();
                part.item_no.shrink_to_fit();
            }
}

// Missing
RobItemHave* RobItem::get_have_dbg(CHARA_NUM cn) {
    return &s_have_dbg[cn];
}

// 0x140525830
RobItemHaveSub* RobItem::get_have_sub_dbg(CHARA_NUM cn) {
    return &s_have_sub_dbg[cn];
}

// 0x14052B0F0
void RobItem::s_req_obj(CHARA_NUM cn, uint32_t item_no, void* data, const object_database* obj_db) {
    if (!item_no)
        return;

    const std::vector<uint32_t>* item_objset = get_rob_item_table_objset(cn, item_no);
    if (!item_objset)
        return;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1)
            objset_info_storage_load_set(data, obj_db, i);
}

// 0x14052D4C0
bool RobItem::s_wait_obj(CHARA_NUM cn, uint32_t item_no) {
    if (!item_no)
        return false;

    const std::vector<uint32_t>* item_objset = get_rob_item_table_objset(cn, item_no);
    if (!item_objset)
        return true;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1 && objset_info_storage_load_obj_set_check_not_read(i))
            return true;
    return false;
}

// 0x140525560
void RobItem::s_free_obj(CHARA_NUM cn, uint32_t item_no) {
    if (!item_no)
        return;

    const std::vector<uint32_t>* item_objset = get_rob_item_table_objset(cn, item_no);
    if (!item_objset)
        return;

    for (uint32_t i : *item_objset)
        if (i != (uint32_t)-1)
            objset_info_storage_unload_set(i);
}

// 0x14052B160
void RobItem::s_req_obj_all(CHARA_NUM cn, const RobItemEquip* item_set, void* data, const object_database* obj_db) {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
        s_req_obj(cn, item_set->item_no[i], data, obj_db);
}

// 0x14052D550
bool RobItem::s_wait_obj_all(CHARA_NUM cn, const RobItemEquip* item_set) {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
        if (s_wait_obj(cn, item_set->item_no[i]))
            return true;
    return false;
}

// 0x1405255D0
void RobItem::s_free_obj_all(CHARA_NUM cn, const RobItemEquip* item_set) {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
        s_free_obj(cn, item_set->item_no[i]);
}

// 0x140521F70
void RobItem::s_delete_item(CHARA_NUM cn, uint32_t item_no, RobItem* rob_item) {
    rob_item->set_chara_num(cn);
    rob_item->delete_item(item_no);
}

// 0x14052CB40
void RobItem::s_regist_item_one(CHARA_NUM cn, uint32_t item_no, RobItem* rob_item) {
    rob_item->set_chara_num(cn);
    rob_item->regist_item_one(item_no);
}

// 0x14052CB10
void RobItem::s_regist_item_all(CHARA_NUM cn, const RobItemEquip* item_set, RobItem* rob_item) {
    rob_item->set_chara_num(cn);
    rob_item->regist_item_all(item_set);
}

RobJump::RobJump() : frame(), zvec(), next_land_frame(), land_time() {
    init();
}

void RobJump::init() {
    frame = 0.0f;
    zvec = 0.0f;
    next_land_frame = 0.0f;
    land_time = 0.0f;
}

RobAttack::RobAttack() : hit_timer(), flag(), ar_flag(), ar_mask(),
old_ar_flag(), old_ar_mask(), ar_flag_c(), ar_mask_c(), first_hit_result(),
cmb_first_ar_mask(), cmb_node_cnt(), next_attack_wait(), sideturn_stop_counter() {
    init();
}

void RobAttack::init() {
    hit_timer = 0;
    flag.u32 = 0;
    ar_flag.u32 = 0;
    ar_mask.u32 = 0;
    old_ar_flag.u32 = 0;
    old_ar_mask.u32 = 0;
    ar_flag_c.u32 = 0;
    ar_mask_c.u32 = 0;
    first_hit_result = false;
    cmb_first_ar_mask.u32 = 0;
    cmb_node_cnt = 0;
    next_attack_wait = false;
    sideturn_stop_counter = 0;
}

ActParam::ActParam() : flag(), type() {

}

void ActParam::init() {
    flag = 0;
    type = 0;
}

RobActType::RobActType() : name(), act_param(), motnum(), mirror() {

}

void RobActType::setup() {
    name = ROB_ACT_NONE;
    act_param.init();
    motnum = -1;
    mirror = false;
}

RobGuard::RobGuard() {
    init();
}

void RobGuard::init() {
    guard_kind = GUARD_KIND_NONE;
    guard_input.reset();
}

RobYarare::RobYarare() : get_damage(), get_power(), yarare_yang(), hit_ar_mask(), combo_count(),
combo_damage(), down_combo_count(), down_combo_damage(), down_combo_count2(), down_combo_damage2(),
down_combo_count3(), down_combo_count4(), nage_combo_count(), nage_combo_damage(), air_combo_count(),
air_combo_damage(), combo_flag(), combo_xang_offs(), max_nowall_combo_count(), max_nowall_combo_damage(),
max_wall_combo_count(), max_wall_combo_damage(), max_current_combo_count(), max_current_combo_damage(),
mot_get_damage(), real_get_damage(), orig_get_damage(), gs_resist_timer(), gs_resist_step() {
    init();
}

RobYarare::~RobYarare() {

}

// 0x140506C20
void RobYarare::init() {
    get_damage = 0;
    get_power = 0.0f;
    yarare_yang = 0;
    hit_ar_mask = {};
    efc_spd = 0.0f;
    get_spd = 0.0f;
    ringout_spd = 0.0f;
    combo_count = 0;
    combo_damage = 0;
    down_combo_count = 0;
    down_combo_damage = 0;
    down_combo_count2 = 0;
    down_combo_damage2 = 0;
    down_combo_count3 = 0;
    down_combo_count4 = 0;
    nage_combo_count = 0;
    nage_combo_damage = 0;
    air_combo_count = 0;
    air_combo_damage = 0;
    combo_flag = 0;
    combo_xang_offs = false;
    field_58.clear();
    max_nowall_combo_count = 0;
    max_nowall_combo_damage = 0;
    max_wall_combo_count = 0;
    max_wall_combo_damage = 0;
    max_current_combo_count = 0;
    max_current_combo_damage = 0;
    mot_get_damage = 0;
    real_get_damage = 0;
    orig_get_damage = 0;
    gs_resist_timer = 0.0f;
    gs_resist_step = 0.0f;
}

RobNage::RobNage() : disable_timer(), nagerare_cnt(),
kaeshi_damage(), enemy(), right_side(), tgt_timer() {
    init();
}

void RobNage::init() {
    disable_timer = 0.0f;
    nagerare_cnt = 0;
    kaeshi_damage = 0;
    enemy = 0;
    right_side = false;
    tgt_yang = 0;
    tgt_pos = 0.0f;
    tgt_timer = 0.0f;
}

RobRise::RobRise() : motnum(), hurry(), down_timer() {
    init();
}

void RobRise::init() {
    motnum = -1;
    hurry = 0.0f;
    down_timer = 0.0f;
}

RobUkemi::RobUkemi() : ukemi_kind(), land_elapsed() {
    init();
}

void RobUkemi::init() {
    ukemi_kind.u32 = 0;
    land_elapsed = 0.0f;
}

RobOffensiveMove::RobOffensiveMove() : offmv_timer(), revise_grd_emy_no(), target_flag() {
    init();
}

void RobOffensiveMove::init() {
    offmv_timer = 0.0f;
    revise_grd_emy_no = false;
    target_pos = { 0.0f, 0.0f, 999.0f };
    target_flag.u32 = 0;
}

RobSideTurn::RobSideTurn() : side_stop_counter(),
side_guard_disable(), side_turn_timer(), right_side_turn() {
    init();
}

void RobSideTurn::init() {
    side_stop_counter = 0.0f;
    side_guard_disable = 0.0f;
    side_turn_timer = 0.0f;
    right_side_turn = false;
}

RobAction::RobAction() : step(), field_B0(), field_B4(), kamae_type(), kamae_data(),
down_counter(), down_counter_ex(), old_down_counter(), mot_backup(),
gacha_count(), gacha(), drank_count(), drank_add_req(), wall_yoroke_timer() {

}

RobAction::~RobAction() {

}

// 0x140505C90
void RobAction::init() {
    step = 0;
    action.setup();
    old_action.setup();
    command.setup();
    bak_command.setup();
    sys_command.setup();
    jump.init();
    attack.init();
    guard.init();
    field_B0 = 0;
    field_B4 = false;
    yarare.init();
    nage.init();
    rise.init();
    ukemi.init();
    off_move.init();
    side_turn.init();
    kamae_type = 0;
    kamae_data = 0;
    down_counter = 0.0f;
    down_counter_ex = 0.0f;
    old_down_counter = 0.0f;
    mot_backup = -1;
    kaeshi_flag.reset();
    gacha_count = 0;
    gacha = false;
    drank_count = 0;
    drank_add_req = 0;
    wall_yoroke_timer = 0.0f;
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

void SubActExecAngry::init() {
    type = SUB_ACTION_EXECUTE_ANGRY;
    field_C = 0;
};

void SubActExecAngry::set(const SubActParam* param) {
    init();
};

void SubActExecAngry::Field_18(rob_chara* impl) {
    impl->rob_base.sub_action.field_10 = this;
};

void SubActExecAngry::Field_20(rob_chara* impl) {

};

SubActExecCountNum::SubActExecCountNum() : SubActExec(SUB_ACTION_EXECUTE_COUNT_NUM) {
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
};

SubActExecCountNum::~SubActExecCountNum() {

};

void SubActExecCountNum::init() {
    type = SUB_ACTION_EXECUTE_COUNT_NUM;
    field_C = 0;
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
};

void SubActExecCountNum::set(const SubActParam* param) {
    init();

    if (param) {
        const SubActParamCountNum* param_count_num = dynamic_cast<const SubActParamCountNum*>(param);
        if (param_count_num)
            field_10 = param_count_num->field_10;
    }
};

void SubActExecCountNum::Field_18(rob_chara* impl) {
    impl->rob_base.sub_action.field_10 = this;
};

void SubActExecCountNum::Field_20(rob_chara* impl) {

};

SubActExecCry::SubActExecCry() : SubActExec(SUB_ACTION_EXECUTE_CRY) {

};

SubActExecCry::~SubActExecCry() {

};

void SubActExecCry::init() {
    type = SUB_ACTION_EXECUTE_CRY;
    field_C = 0;
};

void SubActExecCry::set(const SubActParam* param) {
    init();
};

void SubActExecCry::Field_18(rob_chara* impl) {
    impl->rob_base.sub_action.field_10 = this;
};

void SubActExecCry::Field_20(rob_chara* impl) {

};

SubActExecEmbarrassed::SubActExecEmbarrassed() : SubActExec(SUB_ACTION_EXECUTE_EMBARRASSED) {
    field_10 = 0;
};

SubActExecEmbarrassed::~SubActExecEmbarrassed() {

};

void SubActExecEmbarrassed::init() {
    type = SUB_ACTION_EXECUTE_EMBARRASSED;
    field_C = 0;
    field_10 = 0;
};

void SubActExecEmbarrassed::set(const SubActParam* param) {
    init();
};

void SubActExecEmbarrassed::Field_18(rob_chara* impl) {
    impl->rob_base.sub_action.field_10 = this;
};

void SubActExecEmbarrassed::Field_20(rob_chara* impl) {

};

SubActExecLaugh::SubActExecLaugh() : SubActExec(SUB_ACTION_EXECUTE_LAUGH) {

};

SubActExecLaugh::~SubActExecLaugh() {

};

void SubActExecLaugh::init() {
    type = SUB_ACTION_EXECUTE_LAUGH;
    field_C = 0;
};

void SubActExecLaugh::set(const SubActParam* param) {
    init();
};

void SubActExecLaugh::Field_18(rob_chara* impl) {
    impl->rob_base.sub_action.field_10 = this;
};

void SubActExecLaugh::Field_20(rob_chara* impl) {

};

SubActExecShakeHand::SubActExecShakeHand() : SubActExec(SUB_ACTION_EXECUTE_SHAKE_HAND) {
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
};

SubActExecShakeHand::~SubActExecShakeHand() {

};

void SubActExecShakeHand::init() {
    type = SUB_ACTION_EXECUTE_SHAKE_HAND;
    field_C = 0;
    field_10 = 0;
    field_14 = 0;
    field_18 = 0;
};

void SubActExecShakeHand::set(const SubActParam* param) {
    init();

    if (param) {
        const SubActParamShakeHand* param_shake_hand = dynamic_cast<const SubActParamShakeHand*>(param);
        if (param_shake_hand)
            field_10 = param_shake_hand->field_10;
    }
};

void SubActExecShakeHand::Field_18(rob_chara* impl) {
    impl->rob_base.sub_action.field_10 = this;
};

void SubActExecShakeHand::Field_20(rob_chara* impl) {

};

RobSubAction::RobSubAction() : field_8(), field_10(), param(), field_20() {
    init();
}

RobSubAction::~RobSubAction() {
    dest();
}

void RobSubAction::dest() {
    field_8 = 0;
    field_10 = 0;

    if (param) {
        delete param;
        param = 0;
    }

    if (field_20) {
        delete field_20;
        field_20 = 0;
    }
}

void RobSubAction::init() {
    field_8 = 0;
    field_10 = 0;
    param = 0;
    field_20 = 0;
    cry.init();
    shake_hand.init();
    embarrassed.init();
    angry.init();
    laugh.init();
}

void RobSubAction::set(SubActParam* value) {
    if (!value)
        return;

    if (param) {
        delete param;
        param = 0;
    }
    param = value;
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
        if (data.loop_state >= ROB_PARTIAL_MOTION_LOOP_MAX && (data.frame_max - 1.0f) < data.frame)
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

    if (data.frame_ptr)
        data.frame = data.frame_ptr->f;

    if (data.step_ptr)
        data.step = data.step_ptr->f;
}

// 0x14053F300
void RobPartialMotion::Step() {
    if (!CheckPlaybackStateValid())
        return;

    if (CheckPlaybackStateForward()) {
        float_t frame = data.frame + data.step;
        float_t last_frame = data.frame_max - 1.0f;
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
        float_t last_frame = data.frame_max - 1.0f;
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
        if (data.frame_ptr)
            data.frame = data.frame_ptr->f;
        if (data.step_ptr)
            data.step = data.step_ptr->f;
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
curr_force(), curr_strength(), motnum(), set_frame(), transition_duration(), type(),
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
    motnum = -1;
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
enable_scale(), disable_x(), disable_y(), disable_z(), arm_length(), init_cnt() {
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
    init_cnt = 0;
}

void rob_chara_data_arm_adjust::reset() {
    enable = false;
    value = 0.0f;
    prev_value = 0.0f;
    next_value = 0.0f;
    duration = 0.0f;
    frame = 0.0f;
}

RobMotion::RobMotion() : frame(), step(), basic_step(), flag(), repeat_count(), same_mot_idx(), same_count(),
field_38(), field_B8(), mot_adjust_scale(), mot_xz_adjust_scale(), face_motnum(), parts_adjust(),
parts_adjust_prev(), adjust_global(), adjust_global_prev(), hand_adjust(), hand_adjust_prev(), arm_adjust() {
    num = -1;
    old_num = -1;
    osage_step = 1.0f;
}

RobMotion::~RobMotion() {

}

void RobMotion::reset() {
    num = -1;
    old_num = -1;
    frame.f = 0.0f;
    frame.old_f = 0.0f;
    frame.req_f = 0.0f;
    step.f = 0.0f;
    step.old_f = 0.0f;
    step.req_f = -1.0f;
    osage_step = -1.0f;
    basic_step = BSTEP_BEGIN;
    flag.u32 = 0;
    repeat_count = 0;
    shift_req.reset();
    same_mot_idx = 0;
    same_count = 0;

    for (int32_t& i : field_38)
        i = 0;
    for (int32_t& i : field_B8)
        i = 0;

    mot_adjust_scale = 1.0f;
    mot_xz_adjust_scale = 1.0f;
    mot_adjust_base_pos = 0.0f;
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

CtrlDMot::CtrlDMot() : mot_id(-1), mot_flag() {

}

RobMotId::RobMotId() {

}

void RobMotId::clear() {
    mot_id = -1;
    mot_flag = 0;
}

void RobMotId::set(uint32_t in_mot_id, uint32_t in_mot_flag) {
    mot_id = in_mot_id;
    mot_flag = in_mot_flag;
}

RobMotDAs::RobMotDAs() : count(), follow2(), nagerenai() {

}

void RobMotDAs::clear() {
    count = 0.0f;
    follow2 = -1.0f;
    nagerenai = -1.0f;
}

MhdData::MhdData() {
    mot_kind.mhk_kind[0] = 19;
    mot_kind.mhk_kind[1] = 0;
    mot_kind.mhk_kind[2] = 0;
    mot_kind.mhk_kind[3] = 0;
    start_style = 0x04;
    end_style = 0x04;
    mh_list = 0;
    pp_list = 0;
    cm_list = 0;
}

MhdFileHeader::MhdFileHeader() {
    file_id = -1;
    min_mot_id = 0;
    max_mot_id= 0;
    data_list = 0;
}

RobTarget::RobTarget() : target_flag(), target_end() {

}

MotLeafCtrl::MotLeafCtrl() : mode(), start_frame(), end_frame(), target_id(), limit() {

}

MotLeafCtrl::MotLeafCtrl(int16_t mode, float_t start_frame, float_t end_frame) : mode(mode),
start_frame(start_frame), end_frame(end_frame), target_id(), limit() {

}

RobMotData::RobMotData() : mot(), frame_max(), frame(), ex_damage_all(), start_style(), end_style(),
end_style_flag(), next_type(), next_limit(), loop_begin(), loop_end(), main_mot_frame(),
follow1_frame(), follow2_frame(), attack_flag(), attack_kind(), attack_unit(), attack_point(),
original_damage(), attack_damage(), dist_min_damage(), dc_type(), yarare_type(), efc_spd(),
attack_ball_multi(), down_attack_multi(), air_kind(), jump_frame(), stop_frame(), land_frame(), ev_land_frame(),
land_ypos(), gravity(), nagerare_mot(), nagerare_mot_mirror(), nage_ringout_frame(), nage_cancel_frame(),
nage_kabe_follow2(), nage_damage(), tag_jumph(), tag_xofs(), tag_zofs(), shift_num(), shift_adr(),
ukemi_start_ofs_frame(), ukemi_end_frame(), ukemi_exec_frame(), ukemi_flag(), kaesare_type(), kaeseru_type(),
kaesare_attrib(), gacha_flag(), gacha_times(), gacha_frame(), guard_type(), guard_frame(), frame_ctrl_type(),
frame_ctrl_frame(), frame_ctrl_tag_type(), frame_ctrl_tag_add(), frame_ctrl_min(), frame_ctrl_max(),
est_adr(), dturn_yang(), smooth_flag(), smooth_f_length(), smooth_r_length(), ride_on_ypos(), atk_snd_list(),
kabe_damage(), kabe_damage_type(), resist_low_limit(), resist_max_limit(), resist_percent(), resist_down_chk(),
resist_slow_frame(), resist_slow_step(), en_drink_off_type(), en_drink_off_dec(), trans_xofs(), trans_yofs(),
trans_zofs(), jump_zspd(), wall_haritsuki_frame(), init_cnt(), play_prog(), play_prog_org(), rob_cam_flag(),
colliball_flag(), colliball_ratio(), colliball_timer(), mot_adjust_start(), mot_adjust_end(), mot_adjust_flag(),
mot_adjust_type(), mot_adjust_scale(), pre_adjust_scale(), mot_adjust_base_ypos(), pre_adjust_base_ypos(),
atk2_start_frame(), atk2_end_frame(), atk2_set_flag(), mh_command(), mh_did() {
    clear();
}

RobMotData::~RobMotData() {
    clear();
}

void RobMotData::clear() {
    motkind_fix = {};
    motkind = {};
    mhd_smd_clear();
    mhd_pp_clear();
    mh_command = 0;
    mh_did = -1;
}

// 0x1405335C0
void RobMotData::mhd_pp_clear() {
    play_prog = 0;
    play_prog_org = 0;
    revise_flag.reset();
    rob_cam_flag = 0;

    target.target_flag = ROB_TARGET_NONE;
    target.target_end = 0.0f;
    target.target_pos = 0.0f;

    for (auto& i : colliball_flag)
        for (auto& j : i)
            j = false;

    for (auto& i : colliball_ratio)
        for (auto& j : i)
            j = 0.0f;

    for (auto& i : colliball_timer)
        for (auto& j : i)
            j = 0.0f;

    arm_adjust_next_value = 0.0f; // X
    arm_adjust_prev_value = 0.0f; // X
    arm_adjust_start_frame = -1;  // X
    arm_adjust_duration = -1.0f;  // X

    mot_adjust_start = -1.0f;
    mot_adjust_end = -1.0f;
    mot_adjust_flag = 0;
    mot_adjust_type = MA_TYPE_NONE;
    mot_adjust_scale = 1.0f;
    pre_adjust_scale = 1.0f;
    mot_adjust_base_ypos = 0.0f;
    pre_adjust_base_ypos = 0.0f;

    atk2_start_frame = -1.0f;
    atk2_end_frame = -1.0f;
    atk2_set_flag = false;

    for (MotLeafCtrl& i : leaf_ctrl)
        i = { -1, -1.0f, -1.0f };
}

// 0x1405338F0
void RobMotData::mhd_pp_exec(rob_chara* impl, float_t frame, const motion_database* mot_db) {
    atk2_set_flag = false;

    if (!play_prog)
        return;

    RobMhPp pp;
    pp.impl = impl;
    pp.base = &impl->rob_base;
    pp.base_emy = impl->enemy ? &impl->enemy->rob_base : 0;
    pp.smd_class.impl = impl;
    pp.smd_class.base = &impl->rob_base;
    pp.smd_class.motd = &impl->rob_base.motdata;

    int32_t frame_int = (int32_t)frame;
    while (play_prog->frame <= frame_int && play_prog->type >= 0) {
        pp.exec_func(play_prog->type, play_prog, frame_int, mot_db);
        play_prog++;
    }
}

// 0x140536DD0
void RobMotData::mhd_smd_atk_clear() {
    attack_flag = false;
    attack_kind.reset();
    attack_unit = AU_NO_ATTACK;
    attack_point = AP_NO_ATTACK;
    original_damage = 0;
    attack_damage = 0;
    dist_min_damage = -1;
    dc_type.reset();
    efc_yang = 0;
    yarare_type = E_PUNCH;
    efc_flag.reset();
    efc_spd = EST_NORMAL;
    attack_ball_multi = 1.2f;
    down_attack_multi = 1.0f;
    for (RobMotDAs& i : field_B8)
        i.clear();
    kaesare_type.reset();
    kaesare_attrib = KT_ATTRIB_NONE;
    est_adr = 0;
    atk_snd_list.clear();
    en_drink_off_type = MH_EDRK_DRINK_OFF_MAX;
    en_drink_off_dec = 0;
}

// 0x140537110
void RobMotData::mhd_smd_clear() {
    mot = -1;
    frame_max = 0.0f;
    frame = 0.0f;
    ex_damage_all = 0;
    mhd_smd_atk_clear();
    start_style = 0;
    end_style = 0;
    end_style_flag = 0;
    next_type = MH_NEXT_NONE;
    next.clear();
    next_end.clear();
    next_limit = -1;
    loop_begin = 0.0f;
    loop_end = -1.0f;
    main_mot_frame = 0.0f;
    follow1_frame = 0.0f;
    follow2_frame = 0.0f;
    air_kind.reset();
    jump_frame = 0.0f;
    stop_frame = 0.0f;
    land_frame = 0.0f;
    ev_land_frame = -1.0f;
    land_ypos = 0.0f;
    gravity = 1.0f;
    nagerare_mot.clear();
    nagerare_mot_mirror.clear();
    nage_ringout_frame = 0.0f;
    nage_cancel_frame = -1.0f;
    nage_kabe_follow2 = -1.0f;
    nage_damage = 0;
    nage_nuke_list.clear();
    tag_jumph = -1;
    tag_xofs = 0.0f;
    tag_zofs = 0.0f;
    shift_num = 0;
    shift_adr = 0;
    ukemi_start_ofs_frame = 0.0f;
    ukemi_end_frame = 0.0f;
    ukemi_exec_frame = -1.0f;
    ukemi_flag = 0;
    kaeseru_type.reset();
    mov_yang = 0;
    old_mov_yang = 0;
    gacha_flag.reset();
    gacha_mot.clear();
    gacha_times = 0;
    gacha_frame = 0.0f;
    guard_type = GK_TYPE_ALL;
    guard_flag.reset();
    guard_frame = 0.0f;
    frame_ctrl_type = 0;
    frame_ctrl_frame = 0.0f;
    frame_ctrl_tag_type = 0;
    frame_ctrl_tag_add = 0.0f;
    frame_ctrl_min = 0.0f;
    frame_ctrl_max = 0.0f;
    dturn_yang = 0;
    smooth_flag.reset();
    smooth_f_length = -1.0f;
    smooth_r_length = -1.0f;
    ride_on_ypos = 0.0f;
    kabe_damage = -1;
    kabe_damage_type = MH_KBD_NORMAL;
    nagereru_char.reset();
    resist_type.reset();
    resist_low_limit = -1;
    resist_max_limit = -1;
    resist_percent = 100;
    resist_down_chk = RESIST_CHK_DOWN;
    resist_slow_frame = 0.0f;
    resist_slow_step = 1.0f;
    trans_xofs = 0.0f;
    trans_yofs = 0.0f;
    trans_zofs = 0.0f;
    jump_zspd = 0.05f;
    wall_haritsuki_frame = -1.0f;
    init_cnt = -1;
}

// 0x140537380
void RobMotData::mhd_smd_exec(rob_chara* impl, MhdList* list) {
    if (!list || !list->data)
        return;

    RobMhSmd smd;
    smd.impl = impl;
    smd.base = &impl->rob_base;
    smd.motd = &impl->rob_base.motdata;

    while (list->data && list->type >= 0) {
        smd.exec_func(list->type, list);
        list++;
    }
}

RobPosition::RobPosition() : revise_type(), ground_pos(), ground_last() {
    reset();
}

void RobPosition::reset() {
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

RobAdjust::RobAdjust() : scale(), height_adjust(), pos_adjust_y(),
offset_x(), offset_y(), offset_z(), get_global_pos(), left_hand_scale(), right_hand_scale(),
left_hand_scale_default(), right_hand_scale_default(), item_scale() {
    reset();
}

void RobAdjust::reset() {
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

prj::Sphere3f::Sphere3f() : r() {

}

prj::BallCollision::BallCollision() : sink(), colli_ball_mask(), org_rad() {

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

RobColliWall::RobColliWall() : wdist(), wheight() {

}

void RobColliWall::setup() {
    wdist = 1000.0f;
    wang.set_rad(0.5f);
    wheight = -1.0f;
    wyang.set_rad(0.0f);
}

RobCollision::RobCollision() : touch_count(), wall_touch_cnt(), touch_mask(), attack_mask(), hit_mask(),
damage_mask(), check_wall(), wall_hit_num(), wall_hit_mask(), handrail_hit_num(), handrail_hit_mask(),
dummy_wall_hit_num(), dummy_wall_hit_mask(), dummy_ringout_num(), dummy_ringout_mask(), above_floor_num(),
above_floor_mask(), floor_hit_num(), floor_hit_mask(), ringout_num(), ringout_mask(), ringout_air_num(),
ringout_air_mask(), sink_floor(), floor_idx(), fld_on(), sink_wall(), wall_idx(), field_1AA0(),
field_1BE4(), field_1C68(), field_1C70(), field_1C78(), field_1C80(), field_1C88(), field_1CF4(),
field_1D60(), field_1DCC(), field_1DD0(), field_1DD4(), field_1DD8(), sink(), max_ypos(),
min_ypos(), max_idx(), min_idx(), top_ypos(), bot_ypos(), top_idx(), bot_idx(), wall_info(),
ringout_dist(), init_thrust_vec_req(), req_wall_break(), enable_dummy_collision(),
fall_ringout(), req_kabe_yoroke(), calc_rob_colli(), colli_attack_hit(), floor_ringout_disable() {

}

RobCollision::~RobCollision() {

}

// 0x140505FB0
void RobCollision::init() {
    touch_count = 0;
    wall_touch_cnt = 0;
    touch_mask = 0;
    attack_mask = 0;
    hit_mask = 0;
    damage_mask = 0;
    check_wall = 0;
    wall_hit_num = 0;
    wall_hit_mask = 0;
    handrail_hit_num = 0;
    handrail_hit_mask = 0;
    dummy_wall_hit_num = 0;
    dummy_wall_hit_mask = 0;
    dummy_ringout_num = 0;
    dummy_ringout_mask = 0;
    above_floor_num = 0;
    above_floor_mask = 0;
    floor_hit_num = 0;
    floor_hit_mask = 0;
    ringout_num = 0;
    ringout_mask = 0;
    ringout_air_num = 0;
    ringout_air_mask = 0;
    sink_floor = 0;
    floor_idx = -1;
    fld_on = 0;
    sink_wall = 0.0f;
    wall_idx = -1;
    wall_hit_pos = 0;

    for (mat4& i : mat)
        i = mat4_identity;

    for (mat4& i : push_mat)
        i = mat4_identity;

    for (prj::BallCollision& i : cb_hit)
        i = {};

    for (prj::BallCollision& i : cb_rob)
        i = {};

    for (prj::BallCollision& i : cb_stg)
        i = {};

    for (pos_scale& i : field_1AA0)
        i = {};

    for (float_t& i : field_1BE4)
        i = 0.0f;

    field_1C50.clear();
    field_1C50.resize(27);

    size_t v3 = 0;
    for (std::pair<int64_t, float_t>& i : field_1C50)
        i = { v3++, 0.0f };

    field_1C68 = 27;
    field_1C70 = 27;
    field_1C78 = 27;
    field_1C80 = 27;

    for (int32_t& i : field_1C88)
        i = 0;

    for (int32_t& i : field_1CF4)
        i = 0;

    for (int32_t& i : field_1D60)
        i = 0;

    field_1DCC = 0;
    field_1DD0 = 0;
    field_1DD4 = 0;
    field_1DD8 = 0;
    sink = 0.0f;
    max_ypos = 0.0f;
    min_ypos = 0.0f;
    max_idx = -1;
    min_idx = -1;
    top_ypos = 0.0f;
    bot_ypos = 0.0f;
    top_idx = -1;
    bot_idx = -1;

    moveF = 0.0f;
    moveR = 0.0f;
    moveRT = 0.0f;
    moveM = 0.0f;
    moveD = 0.0f;
    moveW[0] = 0.0f;
    moveW[1] = 0.0f;
    moveWd[0] = 0.0f;
    moveWd[1] = 0.0f;
    moveWr[0] = 0.0f;
    moveWr[1] = 0.0f;
    moveWT[0] = 0.0f;
    moveWT[1] = 0.0f;
    moveT[0] = 0.0f;
    moveT[1] = 0.0f;

    for (RobColliWall& i : wall_info)
        i.setup();

    for (float_t& i : ringout_dist)
        i = 0.0f;

    init_thrust_vec_req = true;
    thrust_vec = 0.0f;
    rep_hit_pos = 0.0f;
    req_wall_break = false;
    enable_dummy_collision = false;
    fall_ringout = false;
    req_kabe_yoroke = false;
    calc_rob_colli = false;
    colli_attack_hit = false;
}

RobInfo::RobInfo() : en_dist(), en_dist_3d(), en_flag(), old_en_flag(), flag(), profit(), my_profit() {

}

void RobInfo::init() {
    en_dist = 0;
    en_dist_3d = 0;
    en_dir_yang = 0;
    en_dir_yang2 = 0;
    revise_en_dir_yang = 0;
    en_flag.u32 = 0;
    old_en_flag.u32 = 0;
    flag.u32 = 0;
    en_nage_flag.reset();
    en_touch_nage_flag.reset();
    profit = 0.0f;
    my_profit = 0.0f;
}

RobRingOut::RobRingOut() : stop(), down(), stand(), fall(),
air(), air2down(), set_fix_hara(), replay_ringout() {

}

void RobRingOut::init() {
    stop = false;
    down = false;
    stand = false;
    fall = false;
    air = false;
    air2down = false;
    set_fix_hara = false;
    replay_ringout = false;
    ringout_pos = 0.0f;
    ringout_spd = 0.0f;
}

RobSound::RobSound() : blow_flag() {

}

void RobSound::init() {
    blow_flag = false;
}

RobBase::RobBase() : flag(), arm_adjust_scale() {

}

RobBase::~RobBase() {

}

void RobBase::init() {
    action.init();
    sub_action.init();
    robmot.reset();
    motdata.clear();
    position.reset();
    adjust.reset();
    collision.init();
    robinfo.init();
    ringout.init();
    sound.init();
    arm_adjust_scale = 1.0f;
}

rob_chara::rob_chara() : idnm(), type(), energy(), disp_pos_reset(),
disp_pos_reset_forbidden(), chara_num(), cos_id(), rob_data(), enemy() {
    frame_speed = 1.0f;
    bone_data = new rob_chara_bone_data();
    disp = new RobDisp();
}

rob_chara::~rob_chara() {
    delete bone_data;
    bone_data = 0;
    delete disp;
    disp = 0;
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

const void* OpdMaker::Data::GetOpdiFileData(object_info obj_info, uint32_t motnum) {
    auto elem = opdi_files.find({ obj_info, motnum });
    if (elem != opdi_files.end())
        return elem->second->get_data();
    return 0;
}

bool OpdMaker::Data::IsValidOpdiFile(rob_chara* rob_chr, uint32_t motnum) {
    if (!rob_chr)
        return false;

    RobDisp* disp = rob_chr->disp;
    for (int32_t i = RPK_KAMI; i < RPK_MAX; i++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)i);
        object_info obj_info = skin_disp->obj_uid;
        if (obj_info.not_null() && skin_disp->osage_nodes_count) {
            size_t data = (size_t)GetOpdiFileData(obj_info, motnum);
            if (!data
                || *((uint16_t*)(data + 0x04)) != (uint16_t)obj_info.id
                || *((uint16_t*)(data + 0x06)) != (uint16_t)obj_info.set_id
                || *((int32_t*)(data + 0x08)) != motnum
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

    RobDisp* disp = rob_chr->disp;
    for (int32_t i = RPK_KAMI; i < RPK_MAX; i++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)i);
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
    ROB_ID rob_id = (ROB_ID)rob_chr->idnm;

    opd_chara_data_array_open_opd_file(rob_id);

    do {
        if (GetEnd()) {
            SetWaiting(false);
            return;
        }

        uint32_t motnum = (*motion_ids)[motion_index];
        if (motnum == -1)
            break;

        rob_chr->replace_rob_motion(motnum, 0.0f, 0.0f,
            true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);

        float_t frame_max = (float_t)(int32_t)rob_chr->bone_data->get_frame_max() - 1.0f;
        int32_t init_cnt = 60;
        if (rob_chr->rob_base.motdata.init_cnt > 0)
            init_cnt = rob_chr->rob_base.motdata.init_cnt;

        float_t start_frame = 0.0f;
        bool no_opdi = true;
        if (data && data->IsValidOpdiFile(rob_chr, motnum)) {
            no_opdi = false;

            int32_t frame_max_int = (int32_t)frame_max;
            int32_t init = frame_max_int;
            start_frame = frame_max - 1.0f;
            while (init < init_cnt) {
                init += frame_max_int;
                start_frame = (float_t)(init - 1);
            }
        }

        rob_osage_mothead osg_mhd(rob_chr, 0, motnum, start_frame, aft_bone_data, aft_mot_db);
        if (no_opdi) {
            uint64_t frame = 0;
            int32_t init = 0;
            while (true) {
                osg_mhd.set_frame((float_t)(int64_t)frame);
                osg_mhd.ctrl();

                frame++;
                init++;
                if ((float_t)(int64_t)frame >= frame_max) {
                    if (init > init_cnt)
                        break;

                    frame = 0;
                    osg_mhd.init_data(aft_mot_db);
                }
            }
        }
        else {
            osg_mhd.ctrl();
            SetOsagePlayInitData(motnum);
        }

        osg_mhd.init_data(aft_mot_db);

        opd_chara_data_array_init_data(rob_id, motnum);

        uint64_t frame_int = 0;
        float_t frame = 0.0f;
        while (frame < frame_max) {
            osg_mhd.set_frame(frame);
            osg_mhd.ctrl();
            opd_chara_data_array_add_frame_data(rob_id);
            frame = (float_t)(int64_t)++frame_int;
        }

        opd_chara_data_array_encode_data(rob_id);
    } while (CheckForFinalMotion());

    opd_chara_data_array_write_file(rob_id);

    const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();

    std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, rob_id);

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

bool OpdMaker::SetOsagePlayInitData(uint32_t motnum) {
    RobDisp* disp = rob_chr->disp;
    for (int32_t i = RPK_KAMI; i < RPK_MAX; i++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)i);
        if (skin_disp->obj_uid.is_null() || !skin_disp->osage_nodes_count)
            continue;

        size_t data = (size_t)this->data->GetOpdiFileData(skin_disp->obj_uid, motnum);
        if (data)
            disp->set_osage_play_data_init((ROB_PARTS_KIND)i, (float_t*)(data + 0x10));
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

OpdMakeWorker::OpdMakeWorker(ROB_ID rob_id) : state(), items(), use_current_skp() {
    this->rob_id = rob_id;
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

    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    CHARA_NUM chara_num = rob_chr->chara_num;
    const RobData* rob_data = get_rob_data(chara_num);
    switch (state) {
    case 1: {
        opd_make_manager->chara_data.PopItems(chara_num, items);

        for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
            RobItem::s_regist_item_one(chara_num, items[i], &rob_chr->item);

        rob_chr->disp->reset_init_data(rob_chara_bone_data_get_node(
            RobManagement::rob_impl[rob_id].bone_data, BONE_NODE_N_HARA_CP));
        rob_chr->disp->set_shadow_group(rob_id);
        rob_chr->disp->shadow_flag = 0x05;
        state = 2;
    }
    case 2: {
        if (!task_rob_load_check_load_req_data()) {
            task_rob_load_append_load_req_data_obj(chara_num, rob_chr->item.get_equip());
            state = 3;
        }
    } break;
    case 3: {
        if (task_rob_load_check_load_req_data())
            break;

        rob_chr->item.equip(rob_id, aft_bone_data, aft_data, aft_obj_db);

        for (int32_t i = RPK_ITEM_BEGIN; i <= RPK_ITEM_END; i++) {
            const RobSkinDisp* skin_disp = rob_chr->disp->get_skin_work((ROB_PARTS_KIND)i);
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
        skin_param_manager_open(rob_id, osage_init);

        state = 7;
    } break;
    case 7: {
        if (skin_param_manager_check_alive(rob_id))
            break;
        state = 8;
    }
    case 8: {
        path_create_directory(get_ram_osage_play_data_tmp_dir());
        opd_maker_array[rob_id].InitThread(rob_chr, &opd_make_manager->motion_ids,
            opd_make_manager->use_opdi ? &data : 0);
        state = 9;
    } break;
    case 9: {
        if (opd_maker_array[rob_id].IsWaiting())
            break;

        data.Reset();

        const char* ram_osage_play_data_path = get_ram_osage_play_data_dir();
        const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();

        std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, rob_id);

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
        task_rob_load_append_free_req_data_obj(chara_num, rob_chr->item.get_equip());

        for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++)
            RobItem::s_delete_item(chara_num, items[i], &rob_chr->item);

        rob_chr->item.equip(rob_id, aft_bone_data, aft_data, aft_obj_db);
        skin_param_manager_reset(rob_id);
        if (opd_make_manager->chara_data.CheckNoItems(chara_num))
            return true;
        state = 1;
    } break;
    }
    return false;
}

bool OpdMakeWorker::dest() {
    if (opd_maker_array[rob_id].IsWaiting()) {
        opd_maker_array[rob_id].SetEnd();
        return false;
    }
    else {
        opd_maker_array[rob_id].Reset();
        return true;
    }
}

void OpdMakeWorker::disp() {

}

bool OpdMakeWorker::open(bool use_current_skp) {
    if (!task_rob_manager_check_chara_loaded(rob_id))
        return false;

    this->use_current_skp = use_current_skp;
    return app::Task::open("OPD_MAKE_WORKER");
}

bool OpdMakeWorker::close() {
    return app::Task::close();
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
        if (skin_param_manager_array_check_alive())
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
        const uint32_t motnum = i.first;

        for (int32_t j = 0; j < ROB_ID_MAX && i.second.size(); j++) {
            ROB_ID rob_id = (ROB_ID)j;
            rob_chara* rob_chr = rob_chara_array_get(rob_id);
            if (!rob_chr)
                continue;

            std::vector<pv_data_set_motion>& set_motion = this->set_motion[rob_id];

            if (set_motion.size()) {
                bool found_set_motion = false;
                for (pv_data_set_motion& j : set_motion)
                    if (j.motnum == motnum) {
                        found_set_motion = true;
                        break;
                    }

                if (!found_set_motion)
                    continue;
            }

            RobDisp* disp = rob_chr->disp;

            for (int32_t j = RPK_ITEM_BEGIN; j < RPK_ITEM_END && i.second.size(); j++) {
                const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)j);

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
                        obj_info[rob_id][j - RPK_ITEM_BEGIN] = skin_disp->obj_uid;
                        motion_ids[rob_id].push_back(motnum);
                    }

                    i.second.erase(elem);
                }
            }
        }
    }

    for (int32_t i = 0; i < ROB_ID_MAX; i++) {
        ROB_ID rob_id = (ROB_ID)i;
        std::vector<uint32_t>& _motion_ids = motion_ids[rob_id];
        std::sort(_motion_ids.begin(), _motion_ids.end());
        _motion_ids.erase(std::unique(_motion_ids.begin(), _motion_ids.end()), _motion_ids.end());
    }

    for (int32_t i = 0; i < ROB_ID_MAX; i++) {
        ROB_ID rob_id = (ROB_ID)i;
        rob_chara* rob_chr = rob_chara_array_get((ROB_ID)rob_id);
        if (!rob_chr)
            continue;

        for (const uint32_t motnum : motion_ids[rob_id]) {
            rob_chr->replace_rob_motion(motnum, 0.0f, 0.0f,
                true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
            //rob_chr->set_step_motion_step(1.0f);

            float_t frame_max = (float_t)(int32_t)rob_chr->bone_data->get_frame_max() - 1.0f;
            uint32_t init_cnt = 60;
            if (rob_chr->rob_base.motdata.init_cnt > 0)
                init_cnt = rob_chr->rob_base.motdata.init_cnt;

            bool has_opd = false;
            {
                const uint32_t* opd_motion_ids = get_opd_motion_ids();
                while (*opd_motion_ids != -1 && !has_opd) {
                    if (*opd_motion_ids == motnum)
                        has_opd = true;
                    opd_motion_ids++;
                }

                const uint32_t motion_set_id = aft_mot_db->get_motion_set_id_by_motion_id(motnum);
                const uint32_t* opd_motion_set_ids = get_opd_motion_set_ids();
                while (*opd_motion_set_ids != -1 && !has_opd) {
                    if (*opd_motion_set_ids == motion_set_id)
                        has_opd = true;
                    opd_motion_set_ids++;
                }
            }

            std::vector<pv_data_set_motion>& set_motion = this->set_motion[rob_id];

            bool has_osage_reset = false; // Added
            rob_osage_mothead osg_mhd(rob_chr, 0, motnum, 0.0f, false, aft_bone_data, aft_mot_db);
            if (has_opd) {
                rob_osage_mothead_data* osage_reset = new rob_osage_mothead_data(
                    MHP_OSAGE_RESET, motnum, aft_mot_db);
                MhpList* osage_reset_data = osage_reset->find_next_data();

                uint64_t frame = 0;
                uint64_t init = 0;
                while (true) {
                    osg_mhd.set_frame((float_t)(int64_t)frame);
                    osg_mhd.ctrl();

                    if (osage_reset_data) { // Added
                        if (osage_reset_data->frame == 0) {
                            uint32_t _motion_id = motnum;
                            float_t _frame = (float_t)(int64_t)frame;

                            RobDisp* disp = rob_chr->disp;
                            for (int32_t i = 0; i < 600; i++) // Should calm phys
                                osg_mhd.ctrl();

                            rob_chara_add_motion_reset_data(rob_chr, _motion_id, _frame, 0);
                            set_motion.push_back({ motnum, { (float_t)(int64_t)frame, 0  } });
                            has_osage_reset = true;
                        }

                        osage_reset_data = 0;
                    }

                    frame++;
                    init++;
                    if ((float_t)(int64_t)frame >= frame_max) {
                        if (init > init_cnt)
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

            opd_chara_data_array_init_data(rob_id, motnum);

            bool no_loop = has_osage_reset; // Added

            size_t set_motion_index = 0;
            uint64_t frame_int = 0;
            float_t frame = 0.0f;
            while (frame <= frame_max) {
                osg_mhd.set_frame(frame);

                while (set_motion_index < set_motion.size()) {
                    if (set_motion.data()[set_motion_index].motnum != motnum) {
                        set_motion_index++;
                        continue;
                    }

                    float_t _frame = (float_t)(int32_t)prj::roundf(
                        set_motion.data()[set_motion_index].frame_stage_index.first);
                    if (_frame > frame)
                        break;

                    rob_chr->set_motion_skin_param(motnum, _frame);
                    rob_chr->set_motion_reset_data(motnum, _frame);
                    set_motion_index++;
                }

                osg_mhd.ctrl();
                opd_chara_data_array_add_frame_data(rob_id);
                frame = (float_t)(int64_t)++frame_int;
            }

            opd_chara_data* opd_chr_data = opd_chara_data_array_get(rob_id);

            for (int32_t i = 0; i < RPK_ITEM_MAX; i++) {
                object_info obj_info = this->obj_info[rob_id][i];
                this->obj_info[rob_id][i] = {};
                if (obj_info.is_null())
                    continue;

                int32_t load_count = 1;
                auto elem = osage_play_data_manager->file_data.find({ obj_info, motnum });
                if (elem != osage_play_data_manager->file_data.end())
                    continue;

                std::vector<std::vector<opd_vec3_data_vec>>& opd_data = opd_chr_data->opd_data[i];

                size_t nodes_count = 0;
                for (const std::vector<opd_vec3_data_vec>& j : opd_data)
                    nodes_count += j.size();

                int64_t frame_count = (int64_t)frame_max;
                float_t* buf = (float_t*)malloc(sizeof(float_t) * 3ULL * frame_count * nodes_count);
                if (!buf)
                    continue;

                opd_file_data data;
                memset(&data.head, 0, sizeof(data.head));
                data.head.obj_info.first = obj_info.id;
                data.head.obj_info.second = obj_info.set_id;
                data.head.motnum = motnum;
                data.head.frame_count = (uint32_t)frame_count;
                data.head.nodes_count = (uint16_t)nodes_count;
                *(uint16_t*)((uint8_t*)&data.head + 0x12) = no_loop ? 0x01 : 0x00;
                data.data = buf;
                data.load_count = load_count;

                for (const std::vector<opd_vec3_data_vec>& j : opd_data)
                    for (const opd_vec3_data_vec& k : j) {
                        memcpy(buf, k.x.data(), frame_count * sizeof(float_t));
                        buf += frame_count;
                        memcpy(buf, k.y.data(), frame_count * sizeof(float_t));
                        buf += frame_count;
                        memcpy(buf, k.z.data(), frame_count * sizeof(float_t));
                        buf += frame_count;
                    }

                osage_play_data_manager->file_data.insert({ { obj_info, motnum }, data });
            }

            opd_chara_data_array_encode_data(rob_id);
            opd_chr_data->reset();
        }

        motion_ids[rob_id].clear();
        motion_ids[rob_id].shrink_to_fit();
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
    motnum = -1;
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

opd_chara_data::opd_chara_data() : rob_id(), init(), frame(), frame_max(), motnum(), field_18() {
    rob_id = (ROB_ID)opd_chara_data_counter++;
    reset();
}

opd_chara_data::~opd_chara_data() {

}

void opd_chara_data::add_frame_data() {
    if (!init || frame >= frame_max)
        return;

    RobDisp* disp = rob_chara_array_get(rob_id)->disp;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        std::vector<std::vector<opd_vec3_data_vec>>& opd_data = this->opd_data[i];

        size_t node_index = 0;
        for (const ExOsageBlock* j : skin_disp->osage_blk) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];

            const RobJointNode* k_begin = j->osage_work.joint_node_vec.data() + 1;
            const RobJointNode* k_end = j->osage_work.joint_node_vec.data() + j->osage_work.joint_node_vec.size();
            size_t l = 0;
            for (const RobJointNode* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.data()[frame] = k->reset_data.pos.x;
                opd_node_data.data()[l].y.data()[frame] = k->reset_data.pos.y;
                opd_node_data.data()[l].z.data()[frame] = k->reset_data.pos.z;
            }
        }

        for (const ExClothBlock* j : skin_disp->cloth) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];

            const CLOTH_VERTEX* k_begin = j->cloth_work.vtxarg.data() + j->cloth_work.width;
            const CLOTH_VERTEX* k_end = j->cloth_work.vtxarg.data() + j->cloth_work.vtxarg.size();
            size_t l = 0;
            for (const CLOTH_VERTEX* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.data()[frame] = k->reset_data.pos.x;
                opd_node_data.data()[l].y.data()[frame] = k->reset_data.pos.y;
                opd_node_data.data()[l].z.data()[frame] = k->reset_data.pos.z;
            }
        }
    }
    frame++;
}

void opd_chara_data::encode_data() {
    if (!init || frame != frame_max)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    const char* motion_name = aft_mot_db->get_motion_name(this->motnum);
    if (!motion_name)
        return;

    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    if (!rob_chr)
        return;

    RobDisp* disp = rob_chr->disp;

    std::string buf = sprintf_s_string("%s/%d", get_ram_osage_play_data_tmp_dir(), rob_id);

    p_farc_write* opd = this->opd;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++, opd++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
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

        size_t max_size = sizeof(osage_play_data_header) + (6ULL * (frame_max + 2ULL) * nodes_count);
        uint8_t* data = prj::MemoryManager::alloc<uint8_t>(prj::MemCTemp, max_size, "OPD_ENCORD_BUF");
        if (!data)
            continue;

        memset(data, 0, max_size);

        osage_play_data_header* opd_head = (osage_play_data_header*)data;
        opd_head->signature = 0;
        opd_head->frame_count = frame_max;
        opd_head->motnum = motnum;
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

void opd_chara_data::encode_init_data(uint32_t motnum) {
    if (!init || frame != frame_max)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    const char* motion_name = aft_mot_db->get_motion_name(this->motnum);
    if (!motion_name)
        return;

    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    if (!rob_chr)
        return;

    RobDisp* disp = rob_chr->disp;

    p_farc_write* opdi = this->opdi;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++, opdi++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        object_info obj_info = skin_disp->obj_uid;
        const char* object_name = objset_info_storage_get_obj_name(obj_info);
        if (!object_name)
            continue;

        size_t size = sizeof(osage_play_data_init_header) + sizeof(vec3) * 2 * skin_disp->osage_nodes_count;
        uint8_t* data = prj::MemoryManager::alloc<uint8_t>(prj::MemCTemp, size, "OPDI_WRITE_BUF");

        osage_play_data_init_header* opdi_head = (osage_play_data_init_header*)data;
        opdi_head->motnum = motnum;
        opdi_head->obj_info = { (uint16_t)obj_info.id, (uint16_t)obj_info.set_id };
        opdi_head->nodes_count = (uint16_t)skin_disp->osage_nodes_count;

        vec3* d = (vec3*)(data + sizeof(osage_play_data_init_header));

        for (const ExOsageBlock* j : skin_disp->osage_blk) {
            if (!(j->osage_work.joint_node_vec.size()))
                break;

            const RobJointNode* k_begin = j->osage_work.joint_node_vec.data() + 1;
            const RobJointNode* k_end = j->osage_work.joint_node_vec.data() + j->osage_work.joint_node_vec.size();
            for (const RobJointNode* k = k_begin; k != k_end; k++) {
                d[0] = k->pos;
                d[1] = k->vec;
                d += 2;
            }
        }

        for (const ExClothBlock* j : skin_disp->cloth) {
            if (!(j->cloth_work.vtxarg.size()))
                break;

            const CLOTH_VERTEX* k_begin = j->cloth_work.vtxarg.data() + j->cloth_work.width;
            const CLOTH_VERTEX* k_end = j->cloth_work.vtxarg.data() + j->cloth_work.vtxarg.size();
            size_t l = 0;
            for (const CLOTH_VERTEX* k = k_begin; k != k_end; k++, l++) {
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
    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    if (!rob_chr)
        return;

    RobDisp* disp = rob_chr->disp;

    std::string tmp_dir = sprintf_s_string("%s/%d", get_dev_ram_opdi_dir(), rob_chr->idnm);

    for (int32_t i = 0; i < RPK_ITEM_MAX; i++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
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

void opd_chara_data::init_data(uint32_t motnum) {
    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    if (!rob_chr)
        return;

    RobDisp* disp = rob_chr->disp;
    int32_t frame_max = (int32_t)rob_chr->rob_base.motdata.frame;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        std::vector<std::vector<opd_vec3_data_vec>>& opd_data = this->opd_data[i];
        opd_data.clear();
        opd_data.resize(skin_disp->cloth.size() + skin_disp->osage_blk.size());

        size_t node_index = 0;
        for (const ExOsageBlock* j : skin_disp->osage_blk) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];
            opd_node_data.resize(j->osage_work.joint_node_vec.size() - 1);

            const RobJointNode* k_begin = j->osage_work.joint_node_vec.data() + 1;
            const RobJointNode* k_end = j->osage_work.joint_node_vec.data() + j->osage_work.joint_node_vec.size();
            size_t l = 0;
            for (const RobJointNode* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.resize(frame_max);
                opd_node_data.data()[l].y.resize(frame_max);
                opd_node_data.data()[l].z.resize(frame_max);
            }
        }

        for (const ExClothBlock* j : skin_disp->cloth) {
            std::vector<opd_vec3_data_vec>& opd_node_data = opd_data.data()[node_index++];
            opd_node_data.resize(j->cloth_work.vtxarg.size() - j->cloth_work.width);

            const CLOTH_VERTEX* k_begin = j->cloth_work.vtxarg.data() + j->cloth_work.width;
            const CLOTH_VERTEX* k_end = j->cloth_work.vtxarg.data() + j->cloth_work.vtxarg.size();
            size_t l = 0;
            for (const CLOTH_VERTEX* k = k_begin; k != k_end; k++, l++) {
                opd_node_data.data()[l].x.resize(frame_max);
                opd_node_data.data()[l].y.resize(frame_max);
                opd_node_data.data()[l].z.resize(frame_max);
            }
        }
    }

    frame = 0;
    this->frame_max = frame_max;
    init = true;
    this->motnum = motnum;
}

void opd_chara_data::open_opd_file() {
    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    if (!rob_chr)
        return;

    RobDisp* disp = rob_chr->disp;

    p_farc_write* opd = this->opd;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++, opd++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();
        path_create_directory(ram_osage_play_data_tmp_path);

        std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, rob_id);
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
    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    if (!rob_chr)
        return;

    RobDisp* disp = rob_chr->disp;

    p_farc_write* opdi = this->opdi;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++, opdi++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i));
        if (!skin_disp->osage_nodes_count)
            continue;

        const char* ram_osage_play_data_tmp_path = get_ram_osage_play_data_tmp_dir();
        path_create_directory(ram_osage_play_data_tmp_path);

        std::string chara_dir = sprintf_s_string("%s/%d", ram_osage_play_data_tmp_path, rob_chr->idnm);
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
    motnum = -1;
    frame = 0;
    frame_max = 0;
    field_18 = 0;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++)
        opd_data[i].clear();
}

void opd_chara_data::write_file() {
    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    if (!rob_chr)
        return;

    RobDisp* disp = rob_chr->disp;
    for (int32_t i = 0; i < RPK_ITEM_MAX; i++)
        if (disp->get_skin_work((ROB_PARTS_KIND)(RPK_ITEM_BEGIN + i))->osage_nodes_count)
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

bool OsagePlayDataManager::open() {
    return app::Task::open("OSAGE_PLAY_DATA_MANAGER");
}

void OsagePlayDataManager::AppendCharaMotionId(rob_chara* rob_chr, const std::vector<uint32_t>& motion_ids) {
    if (CheckTaskReady() || !rob_chr)
        return;

    RobDisp* disp = rob_chr->disp;
    for (int32_t i = RPK_ITEM_BEGIN; i < RPK_ITEM_END; i++) {
        const RobSkinDisp* skin_disp = disp->get_skin_work((ROB_PARTS_KIND)i);
        if (!skin_disp || skin_disp->obj_uid.is_null()
            || (!skin_disp->osage_blk.size() && !skin_disp->cloth.size()))
            continue;

        for (const uint32_t j : motion_ids)
            req_data.push_back(skin_disp->obj_uid, j);
    }
}

bool OsagePlayDataManager::CheckTaskReady() {
    return check_alive();
}

void OsagePlayDataManager::GetOpdFileData(object_info obj_info,
    uint32_t motnum, const float_t*& data, uint32_t& count) {
    data = 0;
    count = 0;

    auto elem = file_data.find({ obj_info, motnum });
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
            data.head.obj_info.second), data.head.motnum }, data });
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

inline rob_osage_mothead_data::rob_osage_mothead_data(int32_t type,
    uint32_t motnum, const motion_database* mot_db) {
    pp_list = 0;
    this->type = type;
    pp_list = mothead_storage_get_mot_by_motion_id(motnum, mot_db)->pp_list;
    init = true;
}

inline rob_osage_mothead_data::~rob_osage_mothead_data() {

}

inline MhpList* rob_osage_mothead_data::find_next_data() {
    if (!pp_list)
        return 0;

    if (init)
        init = false;
    else
        pp_list++;

    MhpList* pp_list = this->pp_list;
    if (pp_list && pp_list->type >= 0) {
        int32_t type = pp_list->type;
        while (type != this->type) {
            pp_list++;
            type = pp_list->type;
            if (type < 0) {
                pp_list = 0;
                break;
            }
        }
    }
    else
        pp_list = 0;

    this->pp_list = pp_list;
    return pp_list;
}

rob_osage_mothead::rob_osage_mothead(rob_chara* rob_chr, int32_t stage_index, uint32_t motnum,
    float_t frame, const bone_database* bone_data, const motion_database* mot_db) : rob_chr(),
    motnum(), frame(), last_frame(),
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
        rob_chr->replace_rob_motion(motnum, frame, 0.0f,
            true, false, MOTION_BLEND_CROSS, bone_data, mot_db);
        this->frame = frame;
        this->motnum = motnum;
        last_frame = rob_chr->bone_data->get_frame_max() - 1.0f;
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
    uint32_t motnum, float_t frame, bool set_motion_reset_data,
    const bone_database* bone_data, const motion_database* mot_db) : rob_chr(),
    motnum(), frame(), last_frame(),
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
        rob_chr->replace_rob_motion(motnum, frame, 0.0f,
            true, set_motion_reset_data, MOTION_BLEND_CROSS, bone_data, mot_db);
        this->frame = frame;
        this->motnum = motnum;
        last_frame = rob_chr->bone_data->get_frame_max() - 1.0f;
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
    motnum = -1;
    frame = 0.0f;
    last_frame = 0.0f;
    reset_data();
}

// 0x14053D6C0
void rob_osage_mothead::init_data(const motion_database* mot_db) {
    reset_data();

    rob_parts_adjust = new rob_osage_mothead_data(MHP_ROB_PARTS_ADJUST, motnum, mot_db);
    rob_parts_adjust_data = rob_parts_adjust->find_next_data();

    rob_adjust_global = new rob_osage_mothead_data(MHP_ROB_ADJUST_GLOBAL, motnum, mot_db);
    rob_adjust_global_data = rob_adjust_global->find_next_data();

    sleeve_adjust = new rob_osage_mothead_data(MHP_SLEEVE_ADJUST, motnum, mot_db);
    sleeve_adjust_data = sleeve_adjust->find_next_data();

    disable_collision = new rob_osage_mothead_data(MHP_DISABLE_COLLISION, motnum, mot_db);
    disable_collision_data = disable_collision->find_next_data();

    rob_chara_coli_ring = new rob_osage_mothead_data(MHP_ROB_CHARA_COLI_RING, motnum, mot_db);
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
void rob_osage_mothead::set_coli_ring(MhpList* list) {
    const void* data = list->data;

    rob_chara_set_coli_ring(rob_chr, ((int8_t*)data)[0]);
}

// 0x14053E7B0
void rob_osage_mothead::set_frame(float_t value) {
    if (last_frame > value)
        frame = value;
}

// 0x14053E7C0
void rob_osage_mothead::set_disable_collision(MhpList* list) {
    const void* data = list->data;

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
void rob_osage_mothead::set_rob_adjust_global(MhpList* list) {
    const void* data = list->data;

    rob_chara_data_adjust v14;
    v14.reset();

    int8_t type = ((int8_t*)data)[4];
    if (type >= 0 && type < 6) {
        float_t set_frame = (float_t)list->frame;
        v14.enable = true;
        v14.frame = frame - set_frame;
        v14.transition_frame = frame - set_frame;
        v14.motnum = rob_chr->rob_base.robmot.num;
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
void rob_osage_mothead::set_rob_parts_adjust(MhpList* list) {
    const void* data = list->data;

    rob_chara_data_adjust v16;
    v16.reset();

    int8_t type = ((int8_t*)data)[5];
    if (type >= 0 && type < 6) {
        float_t set_frame = (float_t)list->frame;
        v16.enable = true;
        v16.frame = frame - set_frame;
        v16.transition_frame = frame - set_frame;
        v16.motnum = rob_chr->rob_base.robmot.num;
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
        rob_chr->disp->reset_nodes_ex_force((rob_osage_parts)((uint8_t*)data)[4]);

    rob_chara_set_parts_adjust_by_index(rob_chr, (rob_osage_parts)((uint8_t*)data)[4], &v16);
}

// 0x14053EBE0
void rob_osage_mothead::set_sleeve_adjust(MhpList* list) {
    const void* data = list->data;

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
        const RobItemEquip* item_set = get_default_costume_data(i.first, i.second);
        for (int32_t j = 0; j < ROB_ITEM_EQUIP_SUB_ID_MAX; j++) {
            uint32_t item_no = item_set->item_no[j];
            if (!item_no)
                continue;

            const RobItemTable* tbl = get_rob_item_table(i.first, item_no);
            if (!tbl)
                continue;

            if (!(tbl->attr & (ROB_ITEM_ATTR_COL | ROB_ITEM_ATTR_TEX)))
                chara_costumes[i.first].items[j].push_back(item_no);
            else if (tbl->org_itm)
                chara_costumes[i.first].items[j].push_back(tbl->org_itm);
        }
    }
}

void OpdMakeManager::CharaData::AddObjects(const std::vector<std::string>& customize_items) {
    for (const std::string& i : customize_items) {
        CHARA_NUM chara_num = CN_MAX;
        uint32_t item_no = 0;
        customize_item_table_handler_data_get_chara_item(i, chara_num, item_no);
        if (chara_num >= CN_MAX || !item_no)
            continue;

        const RobItemTable* tbl = get_rob_item_table(chara_num, item_no);
        if (!tbl)
            continue;

        if (!(tbl->attr & (ROB_ITEM_ATTR_COL | ROB_ITEM_ATTR_TEX)))
            chara_costumes[chara_num].items[tbl->equip_sub_id].push_back(item_no);
        else if (tbl->org_itm)
            chara_costumes[chara_num].items[tbl->equip_sub_id].push_back(tbl->org_itm);
    }
}

bool OpdMakeManager::CharaData::CheckNoItems(CHARA_NUM chara_num) {
    std::vector<uint32_t>* chara_items = chara_costumes[chara_num].items;
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++, chara_items++)
        if (chara_items->size())
            return false;
    return true;
}

void OpdMakeManager::CharaData::PopItems(CHARA_NUM chara_num, int32_t items[ROB_ITEM_EQUIP_SUB_ID_MAX]) {
    std::vector<uint32_t>* chara_items = chara_costumes[chara_num].items;
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++) {
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
        for (int32_t j = 0; j < ROB_ITEM_EQUIP_SUB_ID_MAX; j++)
            chara_costumes[i].items[j].clear();

    left = 0;
    count = 0;
}

void OpdMakeManager::CharaData::SortUnique() {
    for (int32_t i = 0; i < CN_MAX; i++)
        for (int32_t j = 0; j < ROB_ITEM_EQUIP_SUB_ID_MAX; j++) {
            prj::sort_unique(chara_costumes[i].items[j]);
            count += chara_costumes[i].items[j].size();
        }

    left = count;
}

OpdMakeManager::OpdMakeManager() : mode(), workers() {
    mode = 0;
    chara = CN_MIKU;

    int32_t rob_id = 0;
    for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
        workers[i] = new OpdMakeWorker((ROB_ID)rob_id++);

    motion_ids.clear();
    data.workers.resize(OPD_MAKE_COUNT);
    for (OpdMakeManagerData::Worker& i : data.workers) {
        i.items.resize(ROB_ITEM_EQUIP_SUB_ID_MAX);
        for (uint32_t& j : i.items)
            j = 0;
    }
    use_current_skp = false;
    use_opdi = false;
}

OpdMakeManager::~OpdMakeManager() {
    for (int32_t i = 0; i < OPD_MAKE_COUNT; i++) {
        delete workers[i];
        workers[i] = 0;
    }
}

bool OpdMakeManager::init() {
    mode = 1;

    rctx_ptr->render_manager->set_pass_sw(rndr::RND_PASSID_3D, false);

    if (!path_check_directory_exists(get_ram_osage_play_data_dir()))
        path_create_directory(get_ram_osage_play_data_dir());

    path_delete_directory(get_ram_osage_play_data_tmp_dir());

    if (use_current_skp || !task_rob_manager->check_alive()) {
        task_rob_manager_open();
        return true;
    }
    else {
        task_rob_manager_close();
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
                RobInit rob_init;
                rob_init.rob_type = ROB_TYPE_DATA_TEST;
                get_rob_management()->create_rob(chara, rob_init, 499, false);
            }
            mode = 6;
        }
        else
            mode = 11;
    } break;
    case 6: {
        bool wait = false;
        for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
            if (!task_rob_manager_check_chara_loaded((ROB_ID)i))
                wait = true;

        if (!wait) {
            task_rob_manager->suspend();
            mode = 7;
        }
    } break;
    case 7: {
        for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
            if (rob_chara_array_get((ROB_ID)i++))
                workers[i]->open(use_current_skp);
        mode = 8;
    } break;
    case 8: {
        bool wait = false;
        for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
            if (workers[i]->check_alive())
                wait = true;

        if (!wait)
            mode = use_current_skp ? 12 : 9;
    } break;
    case 9:
        for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
            get_rob_management()->dest_rob((ROB_ID)i);
        task_rob_manager->restart();
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
    for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
        if (workers[i]->check_alive())
            return false;

    if (!use_current_skp) {
        for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
            get_rob_management()->dest_rob((ROB_ID)i);

        for (int32_t i = 0; i < ROB_ID_MAX; i++)
            skin_param_manager_reset(i);

        for (uint32_t& i : motion_set_ids) {
            motion_set_unload_motion(i);
            motion_set_unload_mothead(i);
        }

        task_rob_manager_close();
    }

    rctx_ptr->render_manager->set_pass_sw(rndr::RND_PASSID_3D, true);

    if (path_check_directory_exists(get_ram_osage_play_data_tmp_dir()))
        path_delete_directory(get_ram_osage_play_data_tmp_dir());
    return true;
}

void OpdMakeManager::disp() {

}

void OpdMakeManager::open(const OpdMakeManagerArgs& args) {
    if (check_alive() || !args.motion_ids)
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

    app::Task::open("OPD_MAKE_MANAGER");
}

bool OpdMakeManager::close() {
    for (int32_t i = 0; i < OPD_MAKE_COUNT; i++)
        workers[i]->close();
    return app::Task::close();
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

ReqDataObj::ReqDataObj() : item_set() {

}

ReqDataObj::ReqDataObj(CHARA_NUM chara_num, int32_t count) : ReqData(chara_num, count), item_set() {

}

ReqDataObj::~ReqDataObj() {

}

void ReqDataObj::Reset() {
    ReqData::Reset();
    item_set = {};
}

// 0x1405333E0
void RobMhPp::exec_func(int32_t type, MhpList*& play_prog, int64_t frame, const motion_database* mot_db) {
    if (type < 0 || type >= MHP_MAX || !functbl[type].func)
        return;

    if (!(functbl[type].exec_flag & PPFEF_BF_SKIP) || frame <= 0)
        (this->*functbl[type].func)(play_prog->data, play_prog, (int32_t)frame, mot_db);
    else {
        frame -= play_prog->frame;
        if (frame <= 1)
            (this->*functbl[type].func)(play_prog->data, play_prog, (int32_t)frame, mot_db);
    }
}

rob_chara_age_age_data::rob_chara_age_age_data() : part(), field_14(),
rot_z(), field_1C(), rot_speed(), gravity(), alpha(), alive() {
    index = -1;
    scale = 1.0f;
    remaining = -1.0f;
}

void rob_chara_age_age_data::reset() {
    index = -1;
    part = 0;
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

void rob_chara_age_age_object::disp(render_context* rctx, size_t id,
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
    rctx->disp_manager->set_shadow_group(id ? SHADOW_GROUP_STAGE : SHADOW_GROUP_CHARA);
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
        if (j.part == 1 || j.part == 2) {
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
    if (data->part == 1)
        init_data = &rob_chara_age_age_init_data_left[data->index];
    else if (data->part == 2)
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
                data->rot_z = data->part == 1 ? -0.1f : 0.1f;
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
            if (j.part == 1 || j.part == 2) {
                if (frame >= 200.0f)
                    frame = 0.0f;

                ctrl_data(&j, mat);
            }
}

void rob_chara_age_age::disp(render_context* rctx, size_t id,
    bool npr, bool reflect, const vec3& a5, bool chara_color) {
    if (alpha >= 0.1f && visible)
        object.disp(rctx, id, npr || this->npr, reflect, a5, chara_color);
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
    motnum = -1;
}

osage_set_motion::~osage_set_motion() {

}

void osage_set_motion::init_frame(uint32_t motnum, float_t frame, int32_t stage_index) {
    frames.clear();
    this->motnum = motnum;
    frames.push_back(frame, stage_index);
}

skeleton_rotation_offset::skeleton_rotation_offset() : x(), y(), z() {

}

skeleton_rotation_offset::skeleton_rotation_offset(bool x, bool y, bool z, vec3 rotation) : x(x),
y(y), z(z), rotation(rotation) {

}

PvOsageManager::PvOsageManager() : state(), rob_id(), reset(), field_74(),
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
    uint32_t motnum, float_t frame, int32_t init_cnt) {
    if (!CheckResetFrameNotFound(motnum, frame))
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    rob_chara* rob_chr = rob_chara_array_get(rob_id);

    rob_osage_mothead osg_mhd(rob_chr, stage_index, motnum, frame, aft_bone_data, aft_mot_db);
    rob_chr->adjust_ctrl();
    rob_chr->rob_motion_modifier_ctrl();

    if (init_cnt < 0) {
        init_cnt = 60;
        if (rob_chr->rob_base.motdata.init_cnt > 0)
            init_cnt = rob_chr->rob_base.motdata.init_cnt;
    }

    rob_chara_add_motion_reset_data(rob_chr, motnum, frame, init_cnt);

    reset_frames_list.insert({ frame, true });
    rob_chr->reset_osage();

    for (int32_t i = 0; i < ROB_OSAGE_PARTS_MAX; i++)
        rob_chr->set_disable_collision((rob_osage_parts)i, false);
}

bool PvOsageManager::CheckResetFrameNotFound(uint32_t motnum, float_t frame) {
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

void PvOsageManager::SetPvId(int32_t pv_id, ROB_ID rob_id, bool reset) {
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
    this->rob_id = rob_id;

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
        if (i->motnum == i_next->motnum
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

    uint32_t motnum = -1;
    ::osage_set_motion* set_motion = 0;
    for (pv_data_set_motion& i : pv_set_motion) {
        if (motnum != i.motnum) {
            osage_set_motion.push_back({});
            set_motion = &osage_set_motion.back();
            set_motion->motnum = i.motnum;
        }

        if (set_motion)
            set_motion->frames.push_back(i.frame_stage_index);

        motnum = i.motnum;
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
        rob_chara* rob_chr = rob_chara_array_get(rob_id);
        rob_chr->replace_rob_motion(rob_chr->get_common_mot(MTP_NONE),
            0.0f, 0.0f, true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
        rob_chr->set_face_mottbl_motion(0, MTP_FACE_NULL, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, true, aft_mot_db);
        rob_chr->set_hand_l_mottbl_motion(0, MTP_HAND_NULL, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chr->set_hand_r_mottbl_motion(0, MTP_HAND_NULL, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chr->set_mouth_mottbl_motion(0, MTP_KUCHI_NULL, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chr->set_eyes_mottbl_motion(0, MTP_EYES_NULL, 0.0f, ROB_PARTIAL_MOTION_PLAYBACK_NONE,
            0.0f, 0.0f, 1.0f, ROB_PARTIAL_MOTION_LOOP_NONE, 0.0f, aft_mot_db);
        rob_chr->set_eyelid_mottbl_motion_from_face(0, 0.0f, -1.0f, 0.0f, aft_mot_db);
    }
    Reset();
}

void PvOsageManager::Reset() {
    state = 1;
    rob_id = ROB_ID_NULL;
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

    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    float_t last_frame = (float_t)(int32_t)rob_chr->bone_data->get_frame_max() - 1.0f;
    if (!a2->frames.size())
        return;

    uint32_t motnum = a2->motnum;

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
        if (CheckResetFrameNotFound(motnum, frame)) {
            v34.push_back(frame);
            reset_frames_list.insert({ frame, true });
        }
    }

    if (!v34.size() || v34.front() > last_frame)
        return;

    int32_t init_cnt = 60;
    if (rob_chr->rob_base.motdata.init_cnt > 0)
        init_cnt = rob_chr->rob_base.motdata.init_cnt;

    float_t frame = v34.front() - (float_t)init_cnt + 1.0f;
    while (frame < 0.0f)
        frame += last_frame;

    while (frame > last_frame)
        frame -= last_frame;

    float_t frame_1 = frame - 1.0f;
    if (frame_1 < 0.0f)
        frame_1 = last_frame - 1.0f;

    rob_osage_mothead osg_mhd(rob_chr, a2->frames.front().second, motnum, frame_1, aft_bone_data, aft_mot_db);
    float_t* i_begin = v34.data();
    float_t* i_end = v34.data() + v34.size();
    for (float_t* i = i_begin; i != i_end; ) {
        osg_mhd.set_frame(frame);
        osg_mhd.ctrl();

        float_t frame_1 = frame;
        frame = prj::floorf(frame) + 1.0f;

        if (init_cnt <= 1) {
            if (frame_1 == *i) {
                if (frame_1 == 0.0f && v32)
                    rob_chara_add_motion_reset_data(rob_chr, motnum, last_frame, 0);
                rob_chara_add_motion_reset_data(rob_chr, motnum, frame_1, 0);
                i++;
                continue;
            }
            if (frame_1 + 1.0f > *i)
                frame = *i;
        }
        else
            init_cnt--;

        if (frame >= last_frame) {
            frame = 0.0f;
            osg_mhd.init_data(aft_mot_db);
        }
    }

    rob_chr->reset_osage();
    for (int32_t i = ROB_OSAGE_PARTS_LEFT; i < ROB_OSAGE_PARTS_MAX; i++)
        rob_chr->set_disable_collision((rob_osage_parts)i, false);
}

void PvOsageManager::sub_1404F88A0(uint32_t stage_index, uint32_t motnum, float_t frame) {
    if (!CheckResetFrameNotFound(motnum, frame))
        return;

    ::osage_set_motion v7;
    v7.init_frame(motnum, frame, stage_index);
    sub_1404F83A0(&v7);
}

void PvOsageManager::sub_1404F8AA0() {
    if (!osage_set_motion.size())
        return;

    ::osage_set_motion& v4 = osage_set_motion.back();
    uint32_t motnum = v4.motnum;
    if (motnum == -1)
        return;

    data_struct* aft_data = &data_list[DATA_AFT];
    bone_database* aft_bone_data = &aft_data->data_ft.bone_data;
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    rob_chara* rob_chr = rob_chara_array_get(rob_id);
    reset_frames_list.clear();
    rob_chr->replace_rob_motion(motnum, 0.0f, 0.0f,
        true, false, MOTION_BLEND_CROSS, aft_bone_data, aft_mot_db);
    if (pv)
        for (const pv_db_pv_osage_init& i : pv->osage_init) {
            int32_t osage_init_motion_id = aft_mot_db->get_motion_id(i.motion.c_str());
            if (osage_init_motion_id != -1 && osage_init_motion_id == motnum) {
                if (rob_chr->rob_base.motdata.next_type == MH_NEXT_REPEAT)
                    sub_1404F88A0(i.stage, osage_init_motion_id, (float_t)i.frame);
                else
                    AddMotionFrameResetData(i.stage, osage_init_motion_id, (float_t)i.frame, -1);
            }
        }

    if (rob_chr->rob_base.motdata.next_type == MH_NEXT_REPEAT) {
        sub_1404F83A0(&v4);
    }
    else {
        bool v12 = true;
        MhpList* pp_list = mothead_storage_get_mot_by_motion_id(motnum, aft_mot_db)->pp_list;
        if (pp_list && pp_list->type >= 0) {
            int32_t type = pp_list->type;
            while (type != MHP_OSAGE_RESET) {
                pp_list++;
                type = pp_list->type;
                if (type < 0)
                    goto LABEL_1;
            }

            while (pp_list->frame) {
                pp_list++;
                int32_t type = pp_list->type;
                if (type < 0)
                    goto LABEL_1;

                while (type != MHP_OSAGE_RESET) {
                    pp_list++;
                    type = pp_list->type;
                    if (type < 0)
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
                AddMotionFrameResetData(i.second, motnum, i.first, -1);

        if (v12)
            AddMotionFrameResetData(0, motnum, 0.0f, -1);

        pp_list = mothead_storage_get_mot_by_motion_id(motnum, aft_mot_db)->pp_list;
        if (pp_list && pp_list->type >= 0) {
            int32_t type = pp_list->type;
            while (type != MHP_MOTION_SKIN_PARAM) {
                pp_list++;
                type = pp_list->type;
                if (type < 0)
                    return;
            }

            while (true) {
                int32_t init_cnt = 60;
                if (pp_list->data)
                    init_cnt = *(int32_t*)pp_list->data;

                AddMotionFrameResetData(0, motnum, (float_t)pp_list->frame, init_cnt);

                pp_list++;
                int32_t type = pp_list->type;
                if (type < 0)
                    break;

                while (type != MHP_MOTION_SKIN_PARAM) {
                    pp_list++;
                    type = pp_list->type;
                    if (type < 0)
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

void RobThreadParent::AppendRobCharaFunc(rob_chara* rob_chr, void(*func)(rob_chara*)) {
    std::unique_lock<std::mutex> u_lock(mtx);
    RobThread thrd;
    thrd.data = rob_chr;
    thrd.func = func;
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

void RobThreadHandler::AppendRobCharaFunc(ROB_ID rob_id,
    rob_chara* rob_chr, void(*func)(rob_chara*)) {
    arr[rob_id]->AppendRobCharaFunc(rob_chr, func);
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
    if (!rob_chr || !list || rob_chr->idnm >= ROB_ID_MAX || list->size() >= ROB_ID_MAX)
        return;

    bool found = false;
    for (rob_chara*& i : *list)
        if (i->idnm == rob_chr->idnm) {
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

void RobImplTask::FreeList(int8_t* idnm, std::list<rob_chara*>* list) {
    if (!idnm || !list || *idnm >= ROB_ID_MAX || !list->size())
        return;

    for (auto i = list->begin(); i != list->end();) {
        rob_chara* rob_chr = *i;
        if (rob_chr->idnm == *idnm)
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

void RobImplTask::FreeCtrlCharaList(int8_t* idnm) {
    FreeList(idnm, &ctrl_chara);
}

void RobImplTask::FreeFreeCharaList(int8_t* idnm) {
    FreeList(idnm, &free_chara);
}

void RobImplTask::FreeInitCharaList(int8_t* idnm) {
    FreeList(idnm, &init_chara);
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
            rob_thread_handler->AppendRobCharaFunc((ROB_ID)i->idnm,
                i, rob_base_rob_chara_ctrl_thread_main);

    rob_thread_handler->sub_14054E3F0();

    for (rob_chara*& i : ctrl_chara)
        if (i && !i->rob_base.flag.bit.no_ctrl)
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

bool TaskRobBase::check_type(RobType type) {
    return type >= ROB_TYPE_PLAYER && type <= ROB_TYPE_AUTH;
}

bool TaskRobBase::sync() {
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
            if (i && !i->rob_base.flag.bit.no_ctrl)
                sub_14050EA90(v13);*/
    }

    free_chara.clear();
    return false;
}

bool TaskRobCollision::dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobCollision::check_type(RobType type) {
    return type >= ROB_TYPE_PLAYER && type <= ROB_TYPE_AUTH;
}

bool TaskRobCollision::sync() {
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
        if (i && !i->rob_base.flag.bit.no_ctrl)
            rob_disp_rob_chara_ctrl(i);

    for (rob_chara*& i : ctrl_chara)
        if (i && !i->rob_base.flag.bit.no_ctrl)
            rob_thread_handler->AppendRobCharaFunc((ROB_ID)i->idnm, i,
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
        if (!i || pv_osage_manager_array_get_disp((ROB_ID)i->idnm))
            continue;

        if (i->get_disp_flag() && !i->rob_base.flag.bit.no_ctrl)
            rob_disp_rob_chara_disp(i);
    }
}

bool TaskRobDisp::check_type(RobType type) {
    return type >= 0 && type < ROB_TYPE_MAX;
}

bool TaskRobDisp::sync() {
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
        if (i && !i->rob_base.flag.bit.no_ctrl)
            i->rob_info_ctrl();

    free_chara.clear();
    return false;
}

bool TaskRobInfo::dest() {
    FreeCharaLists();
    return true;
}

bool TaskRobInfo::check_type(RobType type) {
    return type >= ROB_TYPE_PLAYER && type <= ROB_TYPE_AUTH;
}

bool TaskRobInfo::sync() {
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

bool TaskRobLoad::AppendFreeReqData(CHARA_NUM cn) {
    if (cn < CN_MIKU || cn > CN_TETO)
        return false;

    for (auto i = load_req_data.end(); i != load_req_data.begin(); ) {
        i--;
        if (i->chara_num == cn) {
            i = load_req_data.erase(i);
            return true;
        }
    }

    ReqData value;
    value.chara_num = cn;
    free_req_data.push_back(value);
    return true;
}

bool TaskRobLoad::AppendFreeReqDataObj(CHARA_NUM cn, const RobItemEquip* item_set) {
    if (cn < CN_MIKU || cn > CN_TETO)
        return false;

    for (auto i = load_req_data_obj.end(); i != load_req_data_obj.begin(); ) {
        i--;
        if (i->chara_num == cn
            && !memcmp(&i->item_set, item_set, sizeof(RobItemEquip))) {
            i = load_req_data_obj.erase(i);
            return true;
        }
    }

    ReqDataObj value;
    value.chara_num = cn;
    value.item_set = *item_set;
    free_req_data_obj.push_back(value);
    return true;
}

bool TaskRobLoad::AppendLoadReqData(CHARA_NUM cn) {
    if (cn < CN_MIKU || cn > CN_TETO)
        return false;

    for (auto i = free_req_data.end(); i != free_req_data.begin(); ) {
        i--;
        if (i->chara_num == cn) {
            i = free_req_data.erase(i);
            return true;
        }
    }

    ReqData value;
    value.chara_num = cn;
    value.count = 1;
    load_req_data.push_back(value);
    return true;
}

bool TaskRobLoad::AppendLoadReqDataObj(CHARA_NUM cn, const RobItemEquip* item_set) {
    if (cn < CN_MIKU || cn > CN_TETO)
        return false;

    for (auto i = free_req_data_obj.end(); i != free_req_data_obj.begin(); ) {
        i--;
        if (i->chara_num == cn
            && !memcmp(&i->item_set, item_set, sizeof(RobItemEquip))) {
            i = free_req_data_obj.erase(i);
            return true;
        }
    }

    ReqDataObj value;
    value.chara_num = cn;
    value.item_set = *item_set;
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
            && !memcmp(&i.item_set, &req_data_obj->item_set, sizeof(RobItemEquip))) {
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
            UnloadCharaItems(i.chara_num, &i.item_set);
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
            LoadCharaItems(i.chara_num, &i.item_set, aft_data, aft_obj_db);
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
            if (LoadCharaItemsCheckNotReadParent(i.chara_num, &i.item_set))
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
            && !memcmp(&i->item_set, &req_data_obj->item_set, sizeof(RobItemEquip))) {
            if (i->count > 0)
                i->count--;

            if (!i->count)
                i = loaded_req_data_obj.erase(i);
            return;
        }
}

bool TaskRobLoad::LoadCharaItemsCheckNotRead(CHARA_NUM cn, RobItemEquip* item_set) {
    for (uint32_t& i : item_set->item_no)
        if (RobItem::s_wait_obj(cn, i))
            return true;
    return false;
}

bool TaskRobLoad::LoadCharaItemsCheckNotReadParent(CHARA_NUM cn, RobItemEquip* item_set) {
    return TaskRobLoad::LoadCharaItemsCheckNotRead(cn, item_set)
        || item_set->item_no[ROB_ITEM_EQUIP_SUB_ID_KAMI] == 649
        && objset_info_storage_load_obj_set_check_not_read(3291);
}

void TaskRobLoad::LoadCharaItems(CHARA_NUM cn,
    RobItemEquip* item_set, void* data, const object_database* obj_db) {
    RobItem::s_req_obj_all(cn, item_set, data, obj_db);
    if (item_set->item_no[ROB_ITEM_EQUIP_SUB_ID_KAMI] == 649)
        objset_info_storage_load_set(data, obj_db, 3291);
}

void TaskRobLoad::LoadCharaObjSetMotionSet(CHARA_NUM cn,
    void* data, const object_database* obj_db, const motion_database* mot_db) {
    const RobData* rob_data = get_rob_data(cn);
    objset_info_storage_load_set(data, obj_db, rob_data->objset);
    motion_set_load_motion(cmn_set_id, "", mot_db);
    motion_set_load_mothead(cmn_set_id, "", mot_db);
    motion_set_load_motion(rob_data->motfile, "", mot_db);
}

bool TaskRobLoad::LoadCharaObjSetMotionSetCheck(CHARA_NUM cn) {
    const RobData* rob_data = get_rob_data(cn);
    if (objset_info_storage_load_obj_set_check_not_read(rob_data->objset)
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

void TaskRobLoad::UnloadCharaItems(CHARA_NUM cn, RobItemEquip* item_set) {
    if (item_set->item_no[ROB_ITEM_EQUIP_SUB_ID_KAMI] == 649)
        objset_info_storage_unload_set(3291);
    RobItem::s_free_obj_all(cn, item_set);
}

void TaskRobLoad::UnloadCharaObjSetMotionSet(CHARA_NUM cn) {
    const RobData* rob_data = get_rob_data(cn);
    objset_info_storage_unload_set(rob_data->objset);
    motion_set_unload_motion(cmn_set_id);
    motion_set_unload_mothead(cmn_set_id);
    motion_set_unload_motion(rob_data->motfile);
}

void TaskRobLoad::UnloadLoadedChara() {
    for (ReqDataObj& i : loaded_req_data_obj)
        for (int32_t j = i.count; j; j--)
            UnloadCharaItems(i.chara_num, &i.item_set);

    loaded_req_data_obj.clear();

    for (ReqData& i : loaded_req_data)
        for (int32_t j = i.count; j; j--)
            UnloadCharaObjSetMotionSet(i.chara_num);

    loaded_req_data.clear();
}

TaskRobManager::TaskRobManager() : ctrl_state(), dest_state() {
    reset_list();
}

TaskRobManager::~TaskRobManager() {

}

bool TaskRobManager::init() {
    task_rob_load_open();

    const RobTaskList* list_before = get_rob_manager_list_before(this);
    for (; list_before->task; list_before++) {
        RobImplTask* task = list_before->task;
        task->open(list_before->name);
        task->set_sync_pulse_mode(task->sync());
        task->FreeCharaLists();
    }

    const RobTaskList* list_after = get_rob_manager_list_after(this);
    for (; list_after->task; list_after++) {
        RobImplTask* task = list_after->task;
        task->open(list_after->name);
        task->set_sync_pulse_mode(task->sync());
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
                i->reset_rob(i->rob_init, aft_bone_data, aft_mot_db);
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
            task_rob_load_append_free_req_data_obj(i->chara_num, i->item.get_equip());
            FreeLoadedCharaList(&i->idnm);
        }

        for (rob_chara*& i : free_chara) {
            RobType type = i->type;
            const RobTaskList* list_before = get_rob_manager_list_before(this);
            for (; list_before->task; list_before++) {
                if (!list_before->task->check_type(type))
                    continue;

                list_before->task->FreeInitCharaList(&i->idnm);
                list_before->task->FreeCtrlCharaList(&i->idnm);
                list_before->task->AppendFreeCharaList(i);
            }

            const RobTaskList* list_after = get_rob_manager_list_after(this);
            for (; list_after->task; list_after++) {
                if (!list_after->task->check_type(type))
                    continue;

                list_after->task->FreeInitCharaList(&i->idnm);
                list_after->task->FreeCtrlCharaList(&i->idnm);
                list_after->task->AppendFreeCharaList(i);
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
            task_rob_load_append_load_req_data_obj(i->chara_num, i->item.get_equip());
        }
        init_chara.clear();
        ctrl_state = 0;
    }
    return false;
}

bool TaskRobManager::dest() {
    switch (dest_state) {
    case 0: {
        const  RobTaskList* list_before = get_rob_manager_list_before(this);
        for (; list_before->task; list_before++)
            list_before->task->close();

        const RobTaskList* list_after = get_rob_manager_list_after(this);
        for (; list_after->task; list_after++)
            list_after->task->close();

        init_chara.clear();
        load_chara.clear();
        free_chara.clear();
        loaded_chara.clear();
        dest_state = 1;
    }
    case 1:
        if (!task_rob_load_close())
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
    if (!rob_chr || rob_chr->idnm >= ROB_ID_MAX || free_chara.size() >= ROB_ID_MAX)
        return;

    int32_t idnm = rob_chr->idnm;
    for (auto i = init_chara.begin(); i != init_chara.end();)
        if ((*i)->idnm == idnm) {
            i = init_chara.erase(i);
            return;
        }
        else
            i++;

    bool loaded_chara_found = false;
    for (rob_chara*& i : loaded_chara)
        if (i->idnm == idnm) {
            loaded_chara_found = true;
            break;
        }

    bool load_chara_found = false;
    for (rob_chara*& i : load_chara)
        if (i->idnm == idnm) {
            load_chara_found = true;
            break;
        }

    if (!loaded_chara_found && !load_chara_found)
        return;

    bool free_chara_found = false;
    for (rob_chara*& i : free_chara)
        if (i->idnm == idnm) {
            free_chara_found = true;
            break;
        }

    if (!free_chara_found)
        free_chara.push_back(rob_chr);
}

void TaskRobManager::AppendInitCharaList(rob_chara* rob_chr) {
    if (!rob_chr || rob_chr->idnm >= ROB_ID_MAX || init_chara.size() >= ROB_ID_MAX)
        return;

    int32_t idnm = rob_chr->idnm;
    bool loaded_chara_found = false;
    for (rob_chara*& i : loaded_chara)
        if (i->idnm == idnm) {
            loaded_chara_found = true;
            break;
        }

    bool load_chara_found = false;
    for (rob_chara*& i : load_chara)
        if (i->idnm == idnm) {
            load_chara_found = true;
            break;
        }

    if (loaded_chara_found || load_chara_found)
        return;

    bool init_chara_found = false;
    for (rob_chara*& i : init_chara)
        if (i->idnm == idnm) {
            init_chara_found = true;
            break;
        }

    if (!init_chara_found)
        init_chara.push_back(rob_chr);
}

void TaskRobManager::AppendLoadedCharaList(rob_chara* rob_chr) {
    if (!rob_chr || rob_chr->idnm >= ROB_ID_MAX || loaded_chara.size() >= ROB_ID_MAX)
        return;

    int32_t idnm = rob_chr->idnm;
    bool loaded_chara_found = false;
    for (rob_chara*& i : loaded_chara)
        if (i->idnm == idnm) {
            loaded_chara_found = true;
            break;
        }

    if (!loaded_chara_found)
        loaded_chara.push_back(rob_chr);
}

bool TaskRobManager::CheckCharaLoaded(rob_chara* rob_chr) {
    if (!check_alive() || rob_chr->idnm < 0 || rob_chr->idnm >= ROB_ID_MAX)
        return false;

    int8_t idnm = rob_chr->idnm;
    for (rob_chara*& i : free_chara)
        if (i->idnm == idnm)
            return false;

    for (rob_chara*& i : loaded_chara)
        if (i->idnm == idnm)
            return true;
    return false;
}

bool TaskRobManager::CheckHasRobCharaLoad(rob_chara* rob_chr) {
    if (!rob_chr || rob_chr->idnm >= ROB_ID_MAX || !load_chara.size())
        return false;

    int32_t idnm = rob_chr->idnm;
    for (rob_chara*& i : load_chara)
        if (i->idnm == idnm)
            return true;

    return false;
}

void TaskRobManager::CheckTypeAppendInitCharaLists(std::list<rob_chara*>* rob_chr_list) {
    for (rob_chara*& i : *rob_chr_list) {
        RobType type = i->type;
        const RobTaskList* list_before = get_rob_manager_list_before(this);
        for (; list_before->task; list_before++) {
            RobImplTask* task = list_before->task;
            if (task->check_type(type))
                task->AppendInitCharaList(i);
        }

        const RobTaskList* list_after = get_rob_manager_list_after(this);
        for (; list_after->task; list_after++) {
            RobImplTask* task = list_after->task;
            if (task->check_type(type))
                task->AppendInitCharaList(i);
        }
    }
}

void TaskRobManager::FreeLoadedCharaList(int8_t* idnm) {
    if (!idnm || *idnm >= ROB_ID_MAX || !loaded_chara.size())
        return;

    for (auto i = loaded_chara.begin(); i != loaded_chara.end();)
        if ((*i)->idnm == *idnm) {
            i = loaded_chara.erase(i);
            return;
        }
        else
            i++;
}

bool TaskRobManager::GetFreeCharaListEmpty() {
    if (!check_alive())
        return false;

    if (ctrl_state == 1 && !init_chara.size())
        return !!free_chara.size();
    return true;
}

bool TaskRobManager::GetWait(rob_chara* rob_chr) {
    if (!check_alive())
        return false;

    int32_t idnm = rob_chr->idnm;
    if (idnm < 0 || idnm >= ROB_ID_MAX)
        return false;

    for (auto& i : init_chara)
        if (i->idnm == idnm)
            return true;

    for (auto& i : load_chara)
        if (i->idnm == idnm)
            return true;

    for (auto& i : loaded_chara)
        if (i->idnm == idnm)
            return true;

    for (auto& i : free_chara)
        if (i->idnm == idnm)
            return true;

    return false;
}

void TaskRobManager::reset_list() {
    init_chara.clear();
    load_chara.clear();
    free_chara.clear();
    loaded_chara.clear();
}

TaskRobMotionModifier::TaskRobMotionModifier() {

}

TaskRobMotionModifier::~TaskRobMotionModifier() {

}

bool TaskRobMotionModifier::init() {
    return true;
}

static void sub_1405484A0(rob_chara* rob_chr) {
    rob_chr->rob_base.robmot.step.old_f = rob_chr->rob_base.robmot.step.f;

    if (rob_chr->rob_base.robmot.step.req_f < 0.0f)
        rob_chr->rob_base.robmot.step.f = 1.0f;
    else
        rob_chr->rob_base.robmot.step.f = rob_chr->rob_base.robmot.step.req_f;

    rob_chara_bone_data_set_step(rob_chr->bone_data, rob_chr->rob_base.robmot.step.f);

    float_t osage_step = rob_chr->rob_base.robmot.osage_step;
    if (osage_step < 0.0f)
        osage_step = rob_chr->rob_base.robmot.step.f;
    rob_chr->disp->set_osage_step(osage_step);

    if (!rob_chr->rob_base.robmot.flag.bit.frame_ctrl) {
        if (rob_chr->rob_base.action.yarare.gs_resist_timer > 0.0f)
            rob_chr->rob_base.robmot.step.f = rob_chr->rob_base.action.yarare.gs_resist_step;
        return;
    }

    if (!rob_chr->rob_base.motdata.frame_ctrl_type)
        return;

    if (!rob_chr->enemy || rob_chr->rob_base.robmot.frame.f >= rob_chr->rob_base.motdata.frame_ctrl_frame) {
        rob_chr->rob_base.robmot.step.f = 1.0f;
        rob_chr->rob_base.robmot.flag.bit.frame_ctrl = false;
        return;
    }

    static const bool stru_140A2E410[][6] = {
        { 0, 0, 0, 0, 0, 0 },
        { 1, 1, 0, 1, 0, 0 },
        { 1, 0, 1, 1, 0, 1 },
        { 1, 0, 0, 0, 1, 1 },
        { 1, 0, 1, 1, 0, 1 },
        { 1, 0, 0, 0, 1, 1 },
        { 1, 0, 0, 1, 1, 1 },
        { 1, 1, 0, 1, 0, 1 },
        { 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 1, 1 },
        { 1, 1, 1, 1, 1, 1 },
    };

    float_t step = 1.0f;
    rob_chara* enemy = rob_chr->enemy;
#pragma warning(suppress: 6385)
    if (rob_chr->rob_base.motdata.frame_ctrl_type & 0x200 
        || (stru_140A2E410[enemy->rob_base.motdata.attack_point][rob_chr->rob_base.motdata.frame_ctrl_type & 0xFF]
            && (enemy->rob_base.motdata.attack_kind[ATK_HIT_OK]))) {
        float_t v11 = rob_chr->rob_base.motdata.frame_ctrl_tag_add;
        switch (rob_chr->rob_base.motdata.frame_ctrl_tag_type) {
        case 1:
            v11 += rob_chr->rob_base.motdata.frame;
            break;
        case 2:
            v11 += rob_chr->rob_base.motdata.main_mot_frame;
            break;
        case 3:
            v11 += rob_chr->rob_base.motdata.follow1_frame;
            break;
        case 4:
            v11 += rob_chr->rob_base.motdata.follow2_frame;
            break;
        }

        float_t v12 = v11 - enemy->rob_base.robmot.frame.f;
        if (v12 > 0.0f)
            step = (rob_chr->rob_base.motdata.frame_ctrl_frame - rob_chr->rob_base.robmot.frame.f) / v12;
    }

    rob_chr->rob_base.robmot.step.f = clamp_def(step,
        rob_chr->rob_base.motdata.frame_ctrl_min, rob_chr->rob_base.motdata.frame_ctrl_max);
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

bool TaskRobMotionModifier::check_type(RobType type) {
    return type >= ROB_TYPE_PLAYER && type <= ROB_TYPE_AUTH;
}

bool TaskRobMotionModifier::sync() {
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

bool TaskRobPrepareAction::check_type(RobType type) {
    return type >= ROB_TYPE_PLAYER && type <= ROB_TYPE_AUTH;
}

bool TaskRobPrepareAction::sync() {
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
    rob_chr->rob_base.robmot.frame.old_f = rob_chr->rob_base.robmot.frame.f;
    rob_chr->bone_data->motion_step();
    if (rob_chr->rob_base.robmot.flag.bit.ext_frame_req)
        rob_chr->bone_data->set_frame(rob_chr->rob_base.robmot.frame.req_f);
    rob_chr->rob_base.robmot.frame.f = rob_chr->bone_data->get_frame();
    rob_chr->rob_base.robmot.field_150.face.Step();
    rob_chr->rob_base.robmot.field_150.hand_l.Step();
    rob_chr->rob_base.robmot.field_150.hand_r.Step();
    rob_chr->rob_base.robmot.field_150.mouth.Step();
    rob_chr->rob_base.robmot.field_150.eyes.Step();
    rob_chr->rob_base.robmot.field_150.eyelid.Step();
    rob_chr->rob_base.robmot.field_3B0.face.Step();
    rob_chr->rob_base.robmot.field_3B0.hand_l.Step();
    rob_chr->rob_base.robmot.field_3B0.hand_r.Step();
    rob_chr->rob_base.robmot.field_3B0.mouth.Step();
    rob_chr->rob_base.robmot.field_3B0.eyes.Step();
    rob_chr->rob_base.robmot.field_3B0.eyelid.Step();
}

static void sub_140548460(rob_chara* rob_chr) {
    rob_chr->rob_base.flag.bit.old_not_normal = rob_chr->rob_base.flag.bit.not_normal;
    if (rob_chr->rob_base.motdata.motkind[MK_CHANGE])
        rob_chr->rob_base.flag.bit.not_normal = 1;
    else
        rob_chr->rob_base.flag.bit.not_normal = 0;
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

bool TaskRobPrepareControl::check_type(RobType type) {
    return type >= ROB_TYPE_PLAYER && type <= ROB_TYPE_AUTH;
}

bool TaskRobPrepareControl::sync() {
    return true;
}
