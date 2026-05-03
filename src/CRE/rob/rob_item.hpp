/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../../KKdLib/default.hpp"
#include "../mdl/disp_manager.hpp"
#include "../item_table.hpp"
#include "../texture.hpp"

struct RobItemTXHD {
    texture* src;
    texture* dst;
    bool dst_copy;

    RobItemTXHD();
    ~RobItemTXHD();
};

struct RobItemHavePart {
    std::vector<uint32_t> item_no;

    RobItemHavePart() = default;
    ~RobItemHavePart() = default;
};

struct RobItemHave {
    RobItemHavePart part[ROB_ITEM_EQUIP_ID_MAX];

    RobItemHave() = default;
    ~RobItemHave() = default;
};

struct RobItemHaveSub {
    RobItemHavePart part[ROB_ITEM_EQUIP_SUB_ID_MAX];

    RobItemHaveSub() = default;
    ~RobItemHaveSub() = default;
};

typedef prj::vector_pair_combine<std::string, RobItemEquip> RobItemDbgSet;

struct bone_database;
struct render_context;

class RobDisp;

class RobItem {
private:
    CHARA_NUM m_cn;
    CHARA_NUM m_cn_load;
    RobItemEquip m_equip;
    RobItemEquip m_equip_load;
    std::map<uint32_t, std::vector<RobItemTXHD>> m_txhd_map;
    std::map<ROB_PARTS_KIND, std::vector<uint32_t>> m_nude_attr_map;
    std::map<object_info, ROB_PARTS_KIND> m_rpk_map;
    std::map<int32_t, ROB_PARTS_KIND> m_rpk_sp_map;
    std::vector<TexChange> m_texchg_list[RPK_MAX];
    std::map<int32_t, object_info> m_head_map;

private:
    void equip_phase0(RobItemEquip* item_set);
    void equip_phase1(RobDisp* rdp,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void equip_phase2(RobDisp* rdp, RobItemEquip* item_set,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void equip_phase3(RobDisp* rdp, RobItemEquip* item_set,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void equip_phase4(RobDisp* rdp, RobItemEquip* item_set,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void equip_phase5(RobDisp* rdp, RobItemEquip* item_set);
    void equip_phase6(RobDisp* rdp, RobItemEquip* item_set);
    void equip_phase6(RobDisp* rdp, RobItemEquip* item_set, uint32_t item_no);
    void equip_phase7();
    ROB_PARTS_KIND get_rpk_item(ROB_ITEM_EQUIP_SUB_ID id) const;
    void hide_rpk_sp(RobDisp* rdp, int32_t rpk_sp,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void make_chg_tex(uint32_t item_no, const RobItemTable* tbl);
    void free_copy_texture_all();
    void init_nude_attr_map();
    void make_nude_attr_map(uint32_t item_no, const RobItemTable* tbl);
    void set_obj_phase2(RobDisp* rdp, uint32_t item_no, const RobItemTable* tbl, ROB_ITEM_EQUIP_SUB_ID id,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_obj_phase3(RobDisp* rdp, uint32_t item_no, const RobItemTable* tbl,
        const bone_database* bone_data, void* data, const object_database* obj_db);
    void set_texture(RobDisp* rdp,
        const std::vector<uint32_t>& item_nos, ROB_PARTS_KIND rpk, bool is_hyoutan = false);
    void set_texture(RobDisp* rdp, uint32_t item_no, ROB_PARTS_KIND rpk, bool is_hyoutan = false);
    void reset_texture(RobDisp* rdp, ROB_PARTS_KIND rpk, bool is_hyoutan = false);
    void clear_texchg_list();
    void disp_obj_internal(object_info obj_uid, mat4& mat, uint32_t item_no, render_context* rctx);
    static bool s_check_equip_exclusion(CHARA_NUM cn, RobItemEquip* item_set);
    static bool s_check_equip_sub(CHARA_NUM cn, int32_t cos, const RobItemHeader* header, RobItemEquip* item_set);
    static bool s_check_equip_sub_phase0(uint32_t, const RobItemHeader* header);
    static bool s_check_equip_sub_phase1(CHARA_NUM cn, uint32_t item_no);
    static bool s_check_equip_sub_phase2(CHARA_NUM cn, uint32_t item_no, int32_t id);
    static bool s_check_equip_sub_phase3(CHARA_NUM cn, uint32_t, int32_t);
    static bool s_check_equip_sub_phase4(CHARA_NUM cn, int32_t, uint32_t);
    static bool s_check_equip_chara(CHARA_NUM cn, int32_t cos, RobItemEquip* item_set);
    static bool s_check_equip_point(CHARA_NUM cn, const RobItemHeader* header, const RobItemEquip* item_set);
    static bool s_repair_equip(CHARA_NUM cn, int32_t cos, int32_t id, RobItemEquip* in_item_set);

    static int32_t s_have_dbg_ref;
    static RobItemHave s_have_dbg[];
    static int32_t s_have_sub_dbg_ref;
    static RobItemHaveSub s_have_sub_dbg[];

public:
    RobItem();
    ~RobItem();
    void set_chara_num(CHARA_NUM cn);
    void req_obj(uint32_t item_no, void* data, const object_database* obj_db);
    bool wait_obj(uint32_t item_no);
    void free_obj(uint32_t item_no);
    void req_obj_all(void* data, const object_database* obj_db);
    bool wait_obj_all();
    void free_obj_all();
    void free_obj_diff();

private:
    void regist_item(ROB_ITEM_EQUIP_SUB_ID id, uint32_t item_no);

public:
    void regist_item_one(uint32_t item_no);
    void regist_item_all(const RobItemEquip* item_set);
    void delete_item(uint32_t item_no);
    void equip(int32_t rc, const bone_database* bone_data,
        void* data, const object_database* obj_db);
    ROB_ITEM_EQUIP_ID get_equip_id(uint32_t item_no) const;
    ROB_ITEM_EQUIP_SUB_ID get_equip_sub_id(uint32_t item_no) const;
    uint32_t check_exclusive_item(uint32_t item_no) const;
    RobItemEquip* get_equip();
    const RobItemEquip* get_equip() const;
    bool is_equipped_item(uint32_t item_no) const;
    const char* get_name(uint32_t item_no) const;
    //const char* get_name_asc(uint32_t item_no) const;
    void init_disp_obj(uint32_t item_no);
    void disp_obj(uint32_t item_no, render_context* rctx, const mat4& mat);
    void get_item_texchange_list(uint32_t item_no, std::vector<TexChange>& tex_chg_list);
    RobItemEquip* get_equip_load();
    const RobItemEquip* get_equip_load() const;

    void regist_item_all(const struct RobInitItem* item_set);
    void free_copy_texture(uint32_t item_no);
    void free_copy_texture(std::map<uint32_t, std::vector<RobItemTXHD>>::iterator elem);

    bool check_npr_flag() const;
    object_info get_head_object_replace(int32_t head_object_id) const;
    float_t get_face_depth() const;

    static const char* s_get_equip_sub_id_str(ROB_ITEM_EQUIP_SUB_ID id);
    static bool s_check_equip(CHARA_NUM cn, int32_t cos, RobItemEquip* item_set);
    //static void s_get_objset(CHARA_NUM cn, uint32_t, int32_t*, size_t);
    static const char* s_get_name(const CHARA_NUM& cn, uint32_t item_no);
    static ROB_ITEM_EQUIP_ID s_get_equip_id(CHARA_NUM cn, uint32_t item_no);
    static ROB_ITEM_EQUIP_SUB_ID s_get_equip_sub_id(CHARA_NUM cn, uint32_t item_no);
    static void s_get_equip_sub_group(int32_t id, std::vector<int32_t>* sub_group);
    static bool s_is_replace_part(CHARA_NUM cn, int32_t id);
    static bool s_equip_ok(CHARA_NUM cn, uint32_t item_no, int32_t id);
    static bool s_have_costume(CHARA_NUM cn, int32_t id);
    static uint32_t s_get_exclusion(CHARA_NUM cn, uint32_t item_no);
    //static uint32_t s_get_set_item_no(CHARA_NUM cn, uint32_t item_no);
    static uint32_t s_get_point(CHARA_NUM cn, uint32_t item_no);
    static void s_get_offset_list(CHARA_NUM cn, uint32_t item_no, std::vector<RobItemDataOfs>& ofs_list, bool clear);
    static ROB_ITEM_TYPE s_get_type(CHARA_NUM cn, uint32_t item_no);
    static bool s_is_texorg(CHARA_NUM cn, uint32_t item_no);
    static bool s_is_objorg(CHARA_NUM cn, uint32_t item_no);
    static void s_get_ng_item();
    static void s_get_ng_item_name();
    static void s_get_ng_item_point();
    static bool s_check_ng_item(CHARA_NUM cn, uint32_t item_no);
    static std::string s_check_ng_item_name(CHARA_NUM cn, uint32_t item_no);
    static int32_t s_check_ng_item_point(CHARA_NUM cn, uint32_t item_no);
    static uint32_t get_dbgset_num(CHARA_NUM cn);
    static const RobItemDbgSet* get_dbgset(CHARA_NUM cn);
    static void init_have_dbg();
    static void dest_have_dbg();
    static void init_have_sub_dbg();
    static void dest_have_sub_dbg();
    static RobItemHave* get_have_dbg(CHARA_NUM cn);
    static RobItemHaveSub* get_have_sub_dbg(CHARA_NUM cn);

    static void s_req_obj(CHARA_NUM cn, uint32_t item_no, void* data, const object_database* obj_db);
    static bool s_wait_obj(CHARA_NUM cn, uint32_t item_no);
    static void s_free_obj(CHARA_NUM cn, uint32_t item_no);
    static void s_req_obj_all(CHARA_NUM cn, const RobItemEquip* item_set, void* data, const object_database* obj_db);
    static bool s_wait_obj_all(CHARA_NUM cn, const RobItemEquip* item_set);
    static void s_free_obj_all(CHARA_NUM cn, const RobItemEquip* item_set);

    static void s_regist_item_one(CHARA_NUM cn, uint32_t item_no, RobItem* rob_item);
    static void s_regist_item_all(CHARA_NUM cn, const RobItemEquip* item_set, RobItem* rob_item);
    static void s_delete_item(CHARA_NUM cn, uint32_t item_no, RobItem* rob_item);
};
