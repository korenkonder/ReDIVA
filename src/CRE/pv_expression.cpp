/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include <string>
#include <vector>
#include "pv_expression.hpp"
#include "../KKdLib/hash.hpp"
#include "rob/rob.hpp"
#include "render_context.hpp"

struct pv_expression {
    rob_chara* rob_chr;
    int32_t motion_id;
    pv_exp_data* face_data;
    pv_exp_data* face_data_curr;
    pv_exp_data* face_data_prev;
    pv_exp_data* face_cl_data;
    pv_exp_data* face_cl_data_curr;
    pv_exp_data* face_cl_data_prev;
    float_t frame_speed;
    bool face_data_update;
    bool face_cl_data_update;

    pv_expression();
    ~pv_expression();

    bool ctrl();
    void face_cl_set_data(float_t frame);
    void face_cl_set_frame(float_t frame);
    void face_set_data(float_t frame);
    void face_set_frame(float_t frame);
    void reset();
    void reset_data();
    bool set_motion(const char* file, int32_t motion_id);
    bool set_motion(uint32_t, int32_t motion_id);
};

pv_expression pv_expression_array[ROB_CHARA_COUNT];

extern render_context* rctx_ptr;

std::vector<pv_expression_file*> pv_expression_file_storage;

pv_expression_file::pv_expression_file() : data(), load_count() {
    hash = hash_murmurhash_empty;
}

pv_expression_file::~pv_expression_file() {

}

void pv_expression_file::load_file(pv_expression_file* exp_file, const void* data, size_t size) {
    exp_file->data->read(data, size, exp_file->data->modern);
}

void pv_expression_array_ctrl(void* rob_chr) {
    for (pv_expression& i : pv_expression_array)
        if (i.rob_chr == rob_chr) {
            i.ctrl();
            break;
        }
}

void pv_expression_array_reset() {
    for (pv_expression& i : pv_expression_array)
        i.reset();
}

bool pv_expression_array_reset_data(size_t chara_id, void* rob_chr, float_t frame_speed) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return false;

    pv_expression& exp = pv_expression_array[chara_id];
    exp.rob_chr = (rob_chara*)rob_chr;
    exp.frame_speed = frame_speed;
    exp.reset_data();
    return true;
}

void pv_expression_array_reset_motion(size_t chara_id) {
    if (chara_id < 0 || chara_id >= ROB_CHARA_COUNT)
        return;

    pv_expression& exp = pv_expression_array[chara_id];
    if (exp.rob_chr)
        exp.rob_chr->autoblink_enable();
    exp.reset();
}

bool pv_expression_array_set_motion(const char* file, size_t chara_id, int32_t motion_id) {
    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
        return pv_expression_array[chara_id].set_motion(file, motion_id);
    return false;
}

bool pv_expression_array_set_motion(uint32_t hash, size_t chara_id, int32_t motion_id) {
    if (chara_id >= 0 && chara_id < ROB_CHARA_COUNT)
        return pv_expression_array[chara_id].set_motion(hash, motion_id);
    return false;
}


bool pv_expression_file_check_not_ready(const char* file) {
    uint32_t hash = hash_utf8_murmurhash(file);
    for (pv_expression_file*& i : pv_expression_file_storage)
        if (i && i->hash == hash)
            return i->file_handler.check_not_ready();
    return true;
}

bool pv_expression_file_check_not_ready(uint32_t hash) {
    for (pv_expression_file*& i : pv_expression_file_storage)
        if (i && i->hash == hash)
            return i->file_handler.check_not_ready();
    return true;
}

void pv_expression_file_load(void* data, const char* path, const char* file) {
    uint32_t hash = hash_utf8_murmurhash(file);
    for (pv_expression_file*& i : pv_expression_file_storage)
        if (i && i->hash == hash) {
            i->load_count++;
            return;
        }

    pv_expression_file* pv_exp_file = new pv_expression_file;
    if (pv_exp_file->file_handler.read_file(data, path, file)) {
        pv_exp_file->data = new pv_exp;
        pv_exp_file->data->modern = false;

        pv_exp_file->file_handler.set_callback_data(0,
            (PFNFILEHANDLERCALLBACK*)pv_expression_file::load_file, pv_exp_file);
        pv_exp_file->load_count = 1;
    }
    else
        pv_exp_file->load_count = 0;
    pv_exp_file->hash = hash;
    pv_expression_file_storage.push_back(pv_exp_file);
}

void pv_expression_file_load(void* data, const char* path, uint32_t hash) {
    for (pv_expression_file*& i : pv_expression_file_storage)
        if (i && i->hash == hash) {
            i->load_count++;
            return;
        }

    pv_expression_file* pv_exp_file = new pv_expression_file;
    if (pv_exp_file->file_handler.read_file(data, path, hash, ".dex")) {
        pv_exp_file->data = new pv_exp;
        pv_exp_file->data->modern = true;

        pv_exp_file->file_handler.set_callback_data(0,
            (PFNFILEHANDLERCALLBACK*)pv_expression_file::load_file, pv_exp_file);
        pv_exp_file->load_count = 1;
    }
    else
        pv_exp_file->load_count = 0;
    pv_exp_file->hash = hash;
    pv_expression_file_storage.push_back(pv_exp_file);
}

void pv_expression_file_unload(const char* file) {
    uint32_t hash = hash_utf8_murmurhash(file);
    for (pv_expression_file*& i : pv_expression_file_storage) {
        if (!i || i->hash != hash)
            continue;

        if (--i->load_count < 0)
            i->load_count = 0;
        else if (!i->load_count) {
            if (i->data) {
                delete i->data;
                i->data = 0;
            }

            i->file_handler.free_data();
        }
        delete i;

        pv_expression_file_storage.erase(pv_expression_file_storage.begin()
            + (&i - pv_expression_file_storage.data()));
        return;
    }
}

void pv_expression_file_unload(uint32_t hash) {
    for (pv_expression_file*& i : pv_expression_file_storage) {
        if (!i || i->hash != hash)
            continue;

        if (--i->load_count < 0)
            i->load_count = 0;
        else if (!i->load_count) {
            if (i->data) {
                delete i->data;
                i->data = 0;
            }

            i->file_handler.free_data();
        }

        pv_expression_file_storage.erase(pv_expression_file_storage.begin()
            + (&i - pv_expression_file_storage.data()));
        return;
    }
}

void pv_expression_file_storage_init() {
    pv_expression_file_storage = {};
}

void pv_expression_file_storage_free() {
    for (pv_expression_file*& i : pv_expression_file_storage)
        delete i;
    pv_expression_file_storage.clear();
    pv_expression_file_storage.shrink_to_fit();
}

pv_expression::pv_expression() : rob_chr(), face_data(), face_data_curr(), face_data_prev(),
face_cl_data(), face_cl_data_curr(), face_cl_data_prev(), face_data_update(), face_cl_data_update() {
    motion_id = -1;
    frame_speed = 1.0f;
}

pv_expression::~pv_expression() {

}

bool pv_expression::ctrl() {
    if (!rob_chr || rob_chr->data.motion.motion_id != motion_id)
        return false;

    float_t frame = rob_chr->get_frame();
    if (face_data_curr) {
        face_set_frame(frame);
        face_set_data(frame);
    }

    if (face_cl_data_curr) {
        face_cl_set_frame(frame);
        face_cl_set_data(frame);
    }
    return true;
}

void pv_expression::face_cl_set_data(float_t frame) {
    pv_exp_data* v2 = face_cl_data_curr;
    pv_exp_data* v3 = face_cl_data_prev;

    int32_t id = 4;
    float_t duration = 0.0f;
    float_t value = 0.0f;
    float_t offset = 0.0f;

    if (face_cl_data_update) {
        face_cl_data_update = false;

        if (v2->type == 1) {
            value = v2->value;
            id = v2->id;
        }

        if (v2->type == 1 && frame - v2->frame >= v2->trans)
            v3 = v2++;
        else if (v3->type == 1) {
            id = v3->id;
            duration = 0.0f;
            value = v3->value;
            offset = 0.0f;
        }
    }
    else {
        float_t v9 = 1.0f;
        if (v3->trans + v3->frame + 1.0f < v2->frame || v2->trans <= 0.0f)
            v9 = 0.0f;

        if (v2->frame + v9 > frame || v2->type != 1) {
            face_cl_data_curr = v2;
            face_cl_data_prev = v3;
            return;
        }

        value = v2->value;
        id = v2->id;

        float_t v10 = frame - v2->frame;
        float_t v11 = v2->trans - v10;
        if (v11 < 0.0f) {
            v11 = 0.0f;
            v10 = 0.0f;
        }

        duration = (1.0f / frame_speed) * v11;
        if (duration > 0.0f)
            offset = (1.0f / frame_speed) * v10;
        v3 = v2++;
    }

    if (id != 4 && rob_chr) {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        rob_chr->set_eyelid_mottbl_motion_from_face(id, duration, value, offset, aft_mot_db);
    }

    face_cl_data_curr = v2;
    face_cl_data_prev = v3;
}

void pv_expression::face_cl_set_frame(float_t frame) {
    pv_exp_data* v2 = face_cl_data_prev;
    pv_exp_data* v3 = face_cl_data_curr;

    if (frame < v2->frame) {
        v2 = face_cl_data;
        v3 = face_cl_data;
    }

    bool update = false;
    if (frame > v3->frame && v3->type != -1) {
        pv_exp_data* v5 = v3 + 1;
        while (v5->type != -1 && frame > v5->frame) {
            v2 = v3;
            v3 = v5++;
            update = true;
        }
    }

    face_cl_data_curr = v3;
    face_cl_data_prev = v2;
    face_cl_data_update = update;
}

void pv_expression::face_set_data(float_t frame) {
    pv_exp_data* v2 = face_data_curr;
    pv_exp_data* v3 = face_data_prev;

    int32_t expression_id = 78;
    float_t value = 0.0f;
    float_t duration = 0.0f;
    float_t offset = 0.0f;

    if (face_data_update) {
        face_data_update = false;
        if (!v2->type) {
            expression_id = v2->id;
            value = v2->value;
        }

        if (!v2->type && frame - v2->frame >= v2->trans || v3->type)
            v3 = v2++;
        else {
            expression_id = v3->id;
            value = v3->value;
            duration = 0.0f;
            offset = 0.0f;
        }
    }
    else {
        float_t v10 = 1.0f;
        if (v3->trans + v3->frame + 1.0f < v2->frame || v2->trans <= 0.0f)
            v10 = 0.0f;

        if (v2->frame + v10 > frame || v2->type) {
            face_data_prev = v3;
            face_data_curr = v2;
            return;
        }

        expression_id = v2->id;
        value = v2->value;

        float_t v11 = frame - v2->frame;
        float_t v12 = v2->trans - v11;
        if (v12 < 0.0f) {
            v12 = 0.0f;
            v11 = 0.0f;
        }

        duration = (1.0f / frame_speed) * v12;
        if (duration > 0.0f)
            offset = (1.0f / frame_speed) * v11;
        v3 = v2++;
    }

    if (expression_id != 78 && rob_chr) {
        data_struct* aft_data = &data_list[DATA_AFT];
        motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

        int32_t mottbl_index = expression_id_to_mottbl_index(expression_id);
        rob_chr->set_face_mottbl_motion(0, mottbl_index, value, mottbl_index >= 214 && mottbl_index <= 223 ? 1 : 0,
            duration, 0.0f, 1.0f, -1, offset, false, aft_mot_db);
    }

    face_data_prev = v3;
    face_data_curr = v2;
}

void pv_expression::face_set_frame(float_t frame) {
    pv_exp_data* v2 = face_data_prev;
    pv_exp_data* v3 = face_data_curr;

    if (frame < v2->frame) {
        v2 = face_data;
        v3 = face_data;
    }

    bool update = false;
    if (frame > v3->frame && v3->type != -1) {
        pv_exp_data* v5 = v3 + 1;
        while (v5->type != -1 && frame > v5->frame) {
            v2 = v3;
            v3 = v5++;
            update = true;
        }
    }

    face_data_curr = v3;
    face_data_prev = v2;
    face_data_update = update;
}

void pv_expression::reset() {
    rob_chr = 0;
    motion_id = -1;
    face_data = 0;
    face_data_curr = 0;
    face_data_prev = 0;
    face_cl_data = 0;
    face_cl_data_curr = 0;
    face_cl_data_prev = 0;
    frame_speed = 1.0f;
    face_data_update = false;
    face_cl_data_update = false;
}

void pv_expression::reset_data() {
    motion_id = -1;
    face_data = 0;
    face_data_curr = 0;
    face_data_prev = 0;
    face_cl_data = 0;
    face_cl_data_curr = 0;
    face_cl_data_prev = 0;
    face_data_update = false;
    face_cl_data_update = false;
}

bool pv_expression::set_motion(const char* file, int32_t motion_id) {
    return set_motion(hash_utf8_murmurhash(file), motion_id);
}

bool pv_expression::set_motion(uint32_t hash, int32_t motion_id) {
    pv_expression_file* pv_exp_file = 0;
    for (pv_expression_file*& i : pv_expression_file_storage)
        if (i && i->hash == hash) {
            pv_exp_file = i;
            break;
        }

    if (!pv_exp_file || !pv_exp_file->data)
        return false;

    pv_exp* exp = pv_exp_file->data;
    if (!exp->motion_data.size()) {
        reset_data();
        return false;
    }

    data_struct* aft_data = &data_list[DATA_AFT];
    motion_database* aft_mot_db = &aft_data->data_ft.mot_db;

    const char* motion_name = aft_mot_db->get_motion_name(motion_id);

    size_t motion_index = -1;
    for (pv_exp_mot& i : exp->motion_data)
        if (!i.name.compare(motion_name)) {
            motion_index = (&i - exp->motion_data.data());
            break;
        }

    if (motion_index == -1) {
        reset_data();
        return false;
    }

    this->motion_id = motion_id;

    pv_exp_data* face_data = exp->motion_data[motion_index].face_data;
    if (this->face_data != face_data) {
        this->face_data = face_data;
        this->face_data_prev = face_data;
        this->face_data_curr = face_data;
    }

    pv_exp_data* face_cl_data = exp->motion_data[motion_index].face_cl_data;
    if (this->face_cl_data != face_cl_data) {
        this->face_cl_data = face_cl_data;
        this->face_cl_data_prev = face_cl_data;
        this->face_cl_data_curr = face_cl_data;
        if (rob_chr)
            rob_chr->autoblink_disable();
    }
    return true;
}
