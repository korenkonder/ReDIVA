/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "rob_item.hpp"
#include "rob.hpp"

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

static const ROB_ITEM_EQUIP_SUB_ID equip_sub_id_phase2[] = {
    ROB_ITEM_EQUIP_SUB_ID_ZUJO, ROB_ITEM_EQUIP_SUB_ID_KAMI, ROB_ITEM_EQUIP_SUB_ID_HITAI,
    ROB_ITEM_EQUIP_SUB_ID_ME, ROB_ITEM_EQUIP_SUB_ID_MEGANE, ROB_ITEM_EQUIP_SUB_ID_MIMI,
    ROB_ITEM_EQUIP_SUB_ID_KUCHI, ROB_ITEM_EQUIP_SUB_ID_MAKI, ROB_ITEM_EQUIP_SUB_ID_KUBI,
    ROB_ITEM_EQUIP_SUB_ID_INNER, ROB_ITEM_EQUIP_SUB_ID_KATA, ROB_ITEM_EQUIP_SUB_ID_U_UDE,
    ROB_ITEM_EQUIP_SUB_ID_L_UDE, ROB_ITEM_EQUIP_SUB_ID_JOHA_MAE, ROB_ITEM_EQUIP_SUB_ID_JOHA_USHIRO,
    ROB_ITEM_EQUIP_SUB_ID_BELT, ROB_ITEM_EQUIP_SUB_ID_KOSI, ROB_ITEM_EQUIP_SUB_ID_SUNE,
    ROB_ITEM_EQUIP_SUB_ID_KUTSU, ROB_ITEM_EQUIP_SUB_ID_HEAD, ROB_ITEM_EQUIP_SUB_ID_MAX,
};

static const ROB_ITEM_EQUIP_SUB_ID equip_sub_id_phase3[] = {
    ROB_ITEM_EQUIP_SUB_ID_TE, ROB_ITEM_EQUIP_SUB_ID_ASI, ROB_ITEM_EQUIP_SUB_ID_MAX,
};

static const ROB_ITEM_EQUIP_SUB_ID equip_sub_id_phase4[] = {
    ROB_ITEM_EQUIP_SUB_ID_PANTS, ROB_ITEM_EQUIP_SUB_ID_OUTER, ROB_ITEM_EQUIP_SUB_ID_MAX,
};

static const ROB_PARTS_KIND rpk_item[] = {
    RPK_ZUJO, RPK_KAMI, RPK_NONE, RPK_NONE,
    RPK_MEGANE, RPK_NONE, RPK_KUCHI, RPK_NONE,
    RPK_KUBI, RPK_NONE, RPK_OUTER, RPK_NONE,
    RPK_NONE, RPK_NONE, RPK_NONE, RPK_NONE,
    RPK_JOHA_USHIRO, RPK_NONE, RPK_NONE, RPK_PANTS,
    RPK_NONE, RPK_NONE, RPK_NONE, RPK_NONE,
    RPK_NONE,
};

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

// 0x14052B8F0
bool RobItem::s_check_equip_exclusion(CHARA_NUM cn, RobItemEquip* item_set) {
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++) {
        if (!item_set->item_no[i])
            continue;

        uint32_t exclusion = RobItem::s_get_exclusion(cn, i);
        if (!exclusion)
            continue;

        uint32_t* item_no = item_set->item_no;
        for (int32_t j = 0; j < ROB_ITEM_EQUIP_SUB_ID_MAX; j++, item_no++)
            if ((exclusion & (1u << j)) && *item_no)
                *item_no = 0;
    }
    return 1;
}

// 0x14052BA00
bool RobItem::s_check_equip_sub(CHARA_NUM cn, int32_t cos, const RobItemHeader* header, RobItemEquip* item_set) {
    s_get_ng_item();

    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++) {
        uint32_t item_no = item_set->item_no[i];
        if ((!s_check_equip_sub_phase0(item_no, header)
            || !s_check_equip_sub_phase1(cn, item_no)
            || !s_check_equip_sub_phase2(cn, item_no, i)
            || !s_check_equip_sub_phase3(cn, item_no, i)
            || !s_check_equip_sub_phase4(cn, cos, item_no))
            && !s_repair_equip(cn, cos, i, item_set))
            return false;
    }
    return true;
}

// 0x14052BAE0
bool RobItem::s_check_equip_sub_phase0(uint32_t item_no, const RobItemHeader* header) {
    return header->check_item(item_no);
}

// 0x14052BB10
bool RobItem::s_check_equip_sub_phase1(CHARA_NUM cn, uint32_t item_no) {
    return !s_check_ng_item(cn, item_no);
}

// 0x14052BB30
bool RobItem::s_check_equip_sub_phase2(CHARA_NUM cn, uint32_t item_no, int32_t id) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl) {
        if (id == ROB_ITEM_EQUIP_SUB_ID_HADA || tbl->type == ROB_ITEM_TYPE_REM)
            return true;
        return get_rob_item_table_objset(cn, item_no);
    }
    return false;
}

// 0x14052BBA0
bool RobItem::s_check_equip_sub_phase3(CHARA_NUM cn, uint32_t item_no, int32_t id) {
    const RobItemTable* tbl = get_rob_item_table(cn, item_no);
    if (tbl)
        return id == tbl->equip_sub_id;
    return false;
}

// 0x14052BBF0
bool RobItem::s_check_equip_sub_phase4(CHARA_NUM cn, int32_t cos_id, uint32_t item_no) {
    return true;
}

// 0x14052B580
bool RobItem::s_check_equip_chara(CHARA_NUM cn, int32_t cos, RobItemEquip* item_set) {
    std::vector<int32_t> ids;
    const RobItemTable* tbl = get_rob_item_table(cn, item_set->item_no[ROB_ITEM_EQUIP_SUB_ID_INNER]);
    if (tbl && tbl->flag.m.is_dummy && !item_set->item_no[ROB_ITEM_EQUIP_SUB_ID_OUTER])
        ids.push_back(ROB_ITEM_EQUIP_SUB_ID_OUTER);

    if (!item_set->item_no[ROB_ITEM_EQUIP_SUB_ID_KAMI])
        ids.push_back(ROB_ITEM_EQUIP_SUB_ID_KAMI);

    if (!item_set->item_no[ROB_ITEM_EQUIP_SUB_ID_PANTS])
        ids.push_back(ROB_ITEM_EQUIP_SUB_ID_PANTS);

    for (uint32_t id : ids)
        if (!s_repair_equip(cn, cos, id, item_set))
            return false;
    return true;
}

// 0x14052B970
bool RobItem::s_check_equip_point(CHARA_NUM cn, const RobItemHeader* header, const RobItemEquip* item_set) {
    s_get_ng_item_point();

    bool v6 = true;
    uint32_t v7 = 0;
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++) {
        uint32_t item_no = item_set->item_no[i];
        if (item_no) {
            if (header->check_item(item_no))
                v7 += s_get_point(cn, item_no);
            else {
                v6 = false;
                break;
            }
        }
    }

    return v7 <= 10 ? v6 : false;
}

// 0x14052C430
bool RobItem::s_repair_equip(CHARA_NUM cn, int32_t cos, int32_t id, RobItemEquip* in_item_set) {
    const RobItemEquip* item_set = get_default_costume_data(cn, cos);
    if (item_set) {
        in_item_set->item_no[id] = item_set->item_no[id];
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

// 0x140525A40
const char* RobItem::get_name(uint32_t item_no) const {
    const RobItemTable* tbl = get_rob_item_table(this->m_cn, item_no);
    if (tbl)
        return tbl->name.c_str();
    return 0;
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

// 0x140521700
bool RobItem::check_npr_flag() const {
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

// 0x1405257A0
float_t RobItem::get_face_depth() const {
    float_t face_depth = 0.0f;
    for (int32_t i = 0; i < ROB_ITEM_EQUIP_SUB_ID_MAX; i++) {
        const RobItemTable* tbl = get_rob_item_table(m_cn, m_equip.item_no[i]);
        if (tbl)
            face_depth = max_def(tbl->face_depth, face_depth);
    }
    return face_depth;
}

// 0x14052BF20
const char* RobItem::s_get_equip_sub_id_str(ROB_ITEM_EQUIP_SUB_ID id) {
    if (id >= 0 && id < ROB_ITEM_EQUIP_SUB_ID_MAX)
        return rob_item_equip_sub_id_str[id];
    return 0;
}

// 0x14052B4F0
bool RobItem::s_check_equip(CHARA_NUM cn, int32_t cos, RobItemEquip* item_set) {
    const RobItemHeader* header = get_rob_item_header(cn);
    if (!header)
        return false;
    return s_check_equip_exclusion(cn, item_set) && s_check_equip_sub(cn, cos, header, item_set)
        && s_check_equip_chara(cn, cos, item_set) && s_check_equip_point(cn, header, item_set);
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
void RobItem::regist_item_all(const RobInitItem* item_set) {
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

// Missing
bool RobItem::s_is_replace_part(CHARA_NUM cn, int32_t id) {
    if (cn < 0 || cn >= CN_MAX)
        return true;

    const RobItemHeader* header = get_rob_item_header(cn);
    if (!header)
        return true;
    return false;
}

// 0x14052BBA0
bool RobItem::s_equip_ok(CHARA_NUM cn, uint32_t item_no, int32_t id) {
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

// 0x14052C0A0
void RobItem::s_get_ng_item() {
    // NOT IMPL!
}

// 0x14052C0F0
void RobItem::s_get_ng_item_name() {
    // NOT IMPL!
}

// 0x14052C1C0
void RobItem::s_get_ng_item_point() {
    // NOT IMPL!
}

// 0x14052BC00
bool RobItem::s_check_ng_item(CHARA_NUM cn, uint32_t item_no) {
    // NOT IMPL!
    return false;
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
    const RobItemHeader* header = get_rob_item_header(cn);
    if (header)
        return (uint32_t)header->dbgset.size();
    return 0;
}

// 0x1405256A0
const RobItemDbgSet* RobItem::get_dbgset(CHARA_NUM cn) {
    const RobItemHeader* header = get_rob_item_header(cn);
    if (header)
        return &header->dbgset;
    return 0;
}

// 0x140525FC0
void RobItem::init_have_dbg() {
    if (s_have_dbg_ref)
        s_have_dbg_ref++;
    else {
        for (int32_t i = 0; i < CN_MAX; i++) {
            CHARA_NUM cn = (CHARA_NUM)i;
            const RobItemHeader* header = get_rob_item_header(cn);
            if (!header)
                continue;

            bool found = false;
            for (int32_t j = 0; j < ROB_ITEM_EQUIP_ID_MAX; j++)
                if (s_have_dbg[i].part[j].item_no.size()) {
                    found = true;
                    break;
                }

            if (found)
                break;

            for (auto& j : header->table) {
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
            const RobItemHeader* header = get_rob_item_header(cn);
            if (!header)
                continue;

            for (auto& j : header->table) {
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
