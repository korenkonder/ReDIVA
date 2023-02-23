/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "sprite.hpp"
#include "../KKdLib/prj/shared_ptr.hpp"
#include "../KKdLib/prj/stack_allocator.hpp"
#include "data.hpp"
#include "file_handler.hpp"
#include "resolution_mode.hpp"

struct SpriteHeader {

};

struct SpriteHeaderFile {

};

struct SprSet {
    int32_t flag;
    int32_t index;
    SpriteHeaderFile* header_file;
    SpriteHeader* header;
    texture** textures;
    int32_t load_count;
    p_file_handler file_handler;
    bool ready;
    prj::shared_ptr<prj::stack_allocator> alloc_handler;
    std::string file;

    SprSet(int32_t index);
    ~SprSet();

    void Unload();
    bool UnloadTexture();
};

namespace spr {
    struct SpriteManager {
        std::map<int32_t, SprSet> sets;
        std::list<SprArgs> reqlist[4][64];
        float_t aspect[2];
        std::pair<resolution_mode, std::pair<vec2, vec2>> field_1018;
        std::pair<resolution_mode, std::pair<vec2, vec2>> field_102C;
        int32_t index;
        mat4 projection;
        mat4 mat;
        resolution_mode resolution_mode;

        SpriteManager();
        ~SpriteManager();

        void Clear();
        SprSet* GetSet(int32_t index);
        void ResetIndex();
        void ResetResData();
    };
    
    SprSet* spr::SpriteManager::GetSet(int32_t index) {
        auto elem = sets.find(index);
        if (elem != sets.end())
            return &elem->second;
        return 0;
    }
}

spr::SpriteManager sprite_manager;

namespace spr {
    void calc_sprite_vertex(spr::SprArgs* args, vec3* vtx, mat4* mat, bool font) {
        vtx[0] = 0.0f;
        vtx[1].x = 0.0f;
        vtx[1].y = args->sprite_size.y;
        vtx[1].z = 0.0f;
        vtx[2].x = args->sprite_size.x;
        vtx[2].y = args->sprite_size.y;
        vtx[2].z = 0.0f;
        vtx[3].x = args->sprite_size.x;
        vtx[3].y = 0.0f;
        vtx[3].z = 0.0f;

        mat4 m;
        SprAttr attr = args->attr;
        if (!(attr & SPR_ATTR_MATRIX)) {
            if (attr & SPR_ATTR_CTR) {
                if (attr & SPR_ATTR_CTR_LT)
                    args->center = 0.0f;
                else if (attr & SPR_ATTR_CTR_LC) {
                    args->center.x = 0.0f;
                    args->center.y = args->sprite_size.y * 0.5f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_LB) {
                    args->center.x = 0.0f;
                    args->center.y = args->sprite_size.y;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_CT) {
                    args->center.x = args->sprite_size.x * 0.5f;
                    args->center.y = 0.0f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_CC) {
                    args->center.x = args->sprite_size.x * 0.5f;
                    args->center.y = args->sprite_size.y * 0.5f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_CB) {
                    args->center.x = args->sprite_size.x * 0.5f;
                    args->center.y = args->sprite_size.y;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_RT) {
                    args->center.x = args->sprite_size.x;
                    args->center.y = 0.0f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_RC) {
                    args->center.x = args->sprite_size.x;
                    args->center.y = args->sprite_size.y * 0.5f;
                    args->center.z = 0.0f;
                }
                else if (attr & SPR_ATTR_CTR_RB) {
                    args->center.x = args->sprite_size.x;
                    args->center.y = args->sprite_size.y;
                    args->center.z = 0.0f;
                }
            }

            if (font) {
                if (attr & SPR_ATTR_FLIP_H) {
                    args->scale.x = -args->scale.x;
                    args->rot.z = -args->rot.z;
                }
                if (attr & SPR_ATTR_FLIP_V) {
                    args->scale.y = -args->scale.y;
                    args->rot.z = -args->rot.z;
                }
            }

            if (fabsf(args->skew_angle.x) > 0.000001f) {
                float_t skew_width = tanf(args->skew_angle.x) * args->sprite_size.y * 0.5f;
                vtx[0].x = vtx[0].x + skew_width;
                vtx[1].x = vtx[1].x - skew_width;
                vtx[2].x = vtx[2].x - skew_width;
                vtx[3].x = vtx[3].x + skew_width;
            }

            if (fabsf(args->skew_angle.y) > 0.000001f) {
                float_t skew_height = tanf(args->skew_angle.y) * args->sprite_size.x * 0.5f;
                vtx[0].y = vtx[0].y - skew_height;
                vtx[1].y = vtx[1].y - skew_height;
                vtx[2].y = vtx[2].y + skew_height;
                vtx[3].y = vtx[3].y + skew_height;
            }

            mat4_set_translation(&m, &args->trans);
            if (fabsf(args->rot.x) > 0.000001f)
                mat4_rotate_x_mult(&m, args->rot.x, &m);
            if (fabsf(args->rot.y) > 0.000001f)
                mat4_rotate_y_mult(&m, args->rot.y, &m);
            if (fabsf(args->rot.z) > 0.000001f)
                mat4_rotate_z_mult(&m, args->rot.z, &m);
            mat4_scale_rot(&m, &args->scale, &m);
            const vec3 center = -args->center;
            mat4_translate_mult(&m, &center, &m);
        }
        else {
            mat4_set_translation(&m, &args->trans);
            mat4_scale_rot(&m, &args->scale, &m);
        }

        mat4_mult(&args->mat, &m, &m);

        mat4_mult_vec3_trans(&m, &vtx[0], &vtx[0]);
        mat4_mult_vec3_trans(&m, &vtx[1], &vtx[1]);
        mat4_mult_vec3_trans(&m, &vtx[2], &vtx[2]);
        mat4_mult_vec3_trans(&m, &vtx[3], &vtx[3]);

        if (mat)
            *mat = m;
    }
}

void sprite_manager_init() {
    sprite_manager = {};
}

void sprite_manager_load_set(int32_t set_id, std::string* mdata_dir) {
    data_struct* aft_data = &data_list[DATA_AFT];
    sprite_database* aft_spr_db = &aft_data->data_ft.spr_db;

    const spr_db_spr_set* spr_set = aft_spr_db->get_spr_set_by_id(set_id);
    //sprite_manager.ReadFile(spr_set->index, spr_set->file_name.c_str(), mdata_dir);
}

void sprite_manager_reset_res_data() {
    sprite_manager.ResetResData();
}

void sprite_manager_free() {
    sprite_manager.Clear();
}

namespace spr {
    SpriteManager::SpriteManager() : aspect(), index() {
        ResetIndex();
        resolution_mode = RESOLUTION_MODE_MAX;

        resolution_struct res = resolution_struct(RESOLUTION_MODE_HD);

        aspect[0] = (float_t)res.aspect;
        field_1018.first = res.resolution_mode;
        field_1018.second = { 0.0f, { (float_t)res.width, (float_t)res.height } };

        aspect[1] = (float_t)res.aspect;
        field_102C.first = res.resolution_mode;
        field_102C.second = { 0.0f, { (float_t)res.width, (float_t)res.height } };

        mat = mat4_identity;
    }

    SpriteManager::~SpriteManager() {
        for (int32_t i = 0; i < 4; i++)
            for (int32_t j = 0; j < 64; j++)
                reqlist[i][j].clear();
        sets.clear();
    }

    void SpriteManager::Clear() {
        for (int32_t i = 0; i < 4; i++)
            for (int32_t j = 0; j < 64; j++)
                reqlist[i][j].clear();

        for (auto i : sets)
            i.second.Unload();
        sets.clear();
    }

    void SpriteManager::ResetIndex() {
        index = 0;
    }

    void SpriteManager::ResetResData() {
        ResetIndex();
        resolution_mode = RESOLUTION_MODE_MAX;

        resolution_struct* res = res_window_get();

        aspect[0] = (float_t)res->aspect;
        field_1018.first = res->resolution_mode;
        field_1018.second = { 0.0f, { (float_t)res->width, (float_t)res->height } };

        aspect[1] = (float_t)res->aspect;
        field_102C.first = res->resolution_mode;
        field_102C.second = { 0.0f, { (float_t)res->width, (float_t)res->height } };

        mat = mat4_identity;
    }
}

SprSet::SprSet(int32_t index) : flag(),  header_file(), header(), textures(), load_count(), ready() {
    this->index = index;
}

SprSet::~SprSet() {

}

void SprSet::Unload() {
    if (load_count > 1) {
        load_count--;
        return;
    }

    file_handler.reset();
    UnloadTexture();
    alloc_handler.reset();

    free_def(header_file);
    this->header = 0;
    file.clear();
    ready = false;
    load_count = 0;
}

bool SprSet::UnloadTexture() {
    if (textures) {
        texture_array_free(textures);
        textures = 0;
        return true;
    }
    return false;
}
