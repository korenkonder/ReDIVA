/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light_param.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/str_utils.hpp"
#include "render_context.hpp"

static void light_param_data_get_stage_name_string(std::string* str, int32_t stage_index);
static int32_t light_param_data_load_file(light_param_data* a1, p_file_handler* a2);
static void light_param_data_load_file_pv_cut(std::map<int32_t, light_param_data>* tree,
    farc* f, light_param_data* default_light_param, int32_t pv_id);
static void light_param_data_load_pv_cut_file_names(light_param_data* light_param, int32_t cut_id, std::string* name);
static void light_param_data_load_stage_file_names(light_param_data* light_param, std::string* name);
static void light_param_data_set(light_param_data* a1, light_param_data_storage* storage);
static void light_param_storage_free_file_handlers(light_param_data_storage* a1);
static void light_param_storage_load_stages(light_param_data_storage* a1, std::vector<int32_t>* stage_indices);
static void light_param_storage_load_stages(light_param_data_storage* a1,
    std::vector<uint32_t>* stage_hashes, stage_database* stage_data);
static int32_t light_param_storage_load_file(light_param_data_storage* a1, bool read_now);
static void light_param_storage_reset(light_param_data_storage* a1);
static void light_param_storage_set_default_light_param(light_param_data_storage* a1, int32_t stage_index);
static void light_param_storage_set_pv_cut(light_param_data_storage* a1,
    std::map<int32_t, light_param_data>::iterator* elem, int32_t cut_id);
static void light_param_storage_set_stage(light_param_data_storage* a1,
    std::map<int32_t, light_param_data>::iterator* elem, int32_t stage_index);

light_param_data_storage* light_param_data_storage_data;

extern render_context* rctx_ptr;

light_param_data::light_param_data() : pv() {

}

light_param_data::~light_param_data() {

}

int32_t light_param_data_load_file() {
    return light_param_storage_load_file(light_param_data_storage_data, false);
}

light_param_data_storage::light_param_data_storage() :  textures(),
state(), stage_index(), default_stage_node(), pv_id() {

}

light_param_data_storage::~light_param_data_storage() {

}

void light_param_data_storage::load(data_struct* data) {
    glGenTextures(5, light_param_data_storage_data->textures);
    light_param_storage_data_load_stage(0);
    while (light_param_storage_load_file(light_param_data_storage_data, true));
    light_param_storage_set_default_light_param(light_param_data_storage_data, 0);
}

void light_param_data_storage::unload() {
    glDeleteTextures(5, light_param_data_storage_data->textures);
}

void light_param_storage_data_init() {
    light_param_data_storage_data = new light_param_data_storage;
}

void light_param_storage_data_free_file_handlers() {
    light_param_storage_free_file_handlers(light_param_data_storage_data);
}

int32_t light_param_storage_data_load_file() {
    return light_param_storage_load_file(light_param_data_storage_data, false);
}

void light_param_storage_data_load_stage(int32_t stage_index) {
    std::vector<int32_t> stage_indices = { stage_index };
    light_param_storage_load_stages(light_param_data_storage_data, &stage_indices);
}

void light_param_storage_data_load_stage(
    uint32_t stage_hash, stage_database* stage_data) {
    std::vector<uint32_t> stage_hashes = { stage_hash };
    light_param_storage_load_stages(light_param_data_storage_data, &stage_hashes, stage_data);
}

void light_param_storage_data_load_stages(std::vector<int32_t>* stage_indices) {
    light_param_storage_load_stages(light_param_data_storage_data, stage_indices);
}

void light_param_storage_data_load_stages(
    std::vector<uint32_t>* stage_hashes, stage_database* stage_data) {
    light_param_storage_load_stages(light_param_data_storage_data, stage_hashes, stage_data);
}

void light_param_storage_data_reset() {
    light_param_storage_reset(light_param_data_storage_data);
}

void light_param_storage_data_set_default_light_param() {
    light_param_data_set(&light_param_data_storage_data->default_light_param, light_param_data_storage_data);
}

void light_param_storage_data_set_pv_id(int32_t pv_id) {
    light_param_data_storage_data->pv_id = pv_id;
}

void light_param_storage_data_set_pv_cut(int32_t cut_id) {
    std::map<int32_t, light_param_data>::iterator elem;
    light_param_storage_set_pv_cut(light_param_data_storage_data, &elem, cut_id);
}

void light_param_storage_data_set_stage(int32_t stage_id) {
    std::map<int32_t, light_param_data>::iterator elem;
    light_param_storage_set_stage(light_param_data_storage_data, &elem, stage_id);
}

void light_param_storage_data_free() {
    delete light_param_data_storage_data;
}

static int32_t light_param_storage_load_file(light_param_data_storage* a1, bool read_now) {
    if (a1->state == 1) {
        light_param_data* light_param = &a1->stage_light_param_iterator->second;
        std::string* paths = light_param->paths;
        std::string* files = light_param->files;
        for (int32_t i = 0; i < 6; i++)
            a1->file_handlers[i].read_file(rctx_ptr->data, paths[i].c_str(), files[i].c_str());
        a1->state = 2;
        return 1;
    }
    else if (a1->state == 2) {
        for (int32_t i = 0; i < 6; ++i)
            if (read_now)
                a1->file_handlers[i].read_now();
            else if (a1->file_handlers[i].check_not_ready())
                return 1;

        int32_t load_error = light_param_data_load_file(&a1->stage_light_param_iterator->second, a1->file_handlers);

        for (int32_t i = 0; i < 6; i++)
            a1->file_handlers[i].free_data();

        a1->stage_light_param_iterator++;

        if (a1->stage_light_param_iterator == a1->stage.end())
            a1->state = 3;
        else
            a1->state = 1;

        if (load_error) {
            for (int32_t i = 0; i < 6; i++)
                a1->file_handlers[i].call_free_func_free_data();
            a1->state = 0;
            return 0;
        }
        return 1;
    }
    else if (a1->state == 3) {
        char buf[0x100];
        sprintf_s(buf, sizeof(buf), "light_pv%03d.farc", a1->pv_id);
        if (a1->farc_file_handler.read_file(rctx_ptr->data, "rom/light_param/", buf))
            a1->state = 4;
        else {
            a1->farc_file_handler.free_data();
            a1->state = 0;
            return 0;
        }
        return 1;
    }
    else if (a1->state == 4) {
        if (read_now)
            a1->farc_file_handler.read_now();
        else if (a1->farc_file_handler.check_not_ready())
            return 1;

        farc a1a;
        a1a.read(a1->farc_file_handler.get_data(), a1->farc_file_handler.get_size(), true);
        if (a1a.files.size())
            light_param_data_load_file_pv_cut(&a1->pv_cut, &a1a, &a1->default_light_param, a1->pv_id);
        a1->state = 0;
        return 0;
    }
    return 0;
}

static void light_param_data_get_stage_name_string(std::string* str, int32_t stage_index) {
    str->clear();
    str->shrink_to_fit();
    stage_database* data = &rctx_ptr->data->data_ft.stage_data;
    const char* stage_name = stage_index < data->stage_data.size()
        ? data->stage_data[stage_index].name.c_str() : 0;
    if (!stage_name)
        return;

    size_t length = utf8_length(stage_name);
    if (length <= 3)
        return;

    stage_name += 3;
    length -= 3;

    char* s = force_malloc_s(char, length + 1);
    for (size_t i = 0; i < length; i++) {
        char c = stage_name[i];
        if (c == '_') {
            length = i;
            break;
        }

        if (c > 0x40 && c < 0x5B)
            c += 0x20;
        s[i] = c;
    }
    s[length] = '\0';

    *str = std::string(s, length);
    free(s);
}

static void light_param_data_get_stage_name_string(std::string* str,
    uint32_t stage_hash, stage_database* stage_data) {
    str->clear();
    str->shrink_to_fit();
    const char* stage_name = 0;
    for (stage_data_modern& i : stage_data->stage_modern) {
        if (i.hash != stage_hash)
            continue;

        stage_name = i.name.c_str();
        break;
    }

    if (!stage_name)
        return;

    size_t length = utf8_length(stage_name);
    if (length <= 3)
        return;

    stage_name += 3;
    length -= 3;

    char* s = force_malloc_s(char, length + 1);
    for (size_t i = 0; i < length; i++) {
        char c = stage_name[i];
        if (c == '_') {
            length = i;
            break;
        }

        if (c > 0x40 && c < 0x5B)
            c += 0x20;
        s[i] = c;
    }
    s[length] = '\0';

    *str = std::string(s, length);
    free(s);
}

static int32_t light_param_data_load_file(light_param_data* a1, p_file_handler* a2) {
    if (a2[0].ptr) {
        a1->ibl = light_param_ibl();
        a1->ibl.read(a2[0].get_data(), a2[0].get_size());
    }

    if (a2[1].ptr) {
        a1->light = light_param_light();
        a1->light.read(a2[1].get_data(), a2[1].get_size());
    }

    if (a2[2].ptr) {
        a1->fog = light_param_fog();
        a1->fog.read(a2[2].get_data(), a2[2].get_size());
    }

    if (a2[3].ptr) {
        a1->glow = light_param_glow();
        a1->glow.read(a2[3].get_data(), a2[3].get_size());
    }

    if (a2[4].ptr) {
        a1->wind = light_param_wind();
        a1->wind.read(a2[4].get_data(), a2[4].get_size());
    }

    if (a2[5].ptr) {
        a1->face = light_param_face();
        a1->face.read(a2[5].get_data(), a2[5].get_size());
    }

    return !(a1->ibl.ready | a1->light.ready | a1->fog.ready
        | a1->glow.ready | a1->wind.ready | a1->face.ready) ? 1 : 0;
}

static void light_param_data_load_file_pv_cut(std::map<int32_t, light_param_data>* tree,
    farc* f, light_param_data* default_light_param, int32_t pv_id) {
    char buf[0x100];

    sprintf_s(buf, sizeof(buf), "pv%03d", pv_id);

    std::string pv_str = buf;

    for (farc_file& i : f->files) {
        const char* name = i.name.c_str();
        size_t name_len = i.name.size();

        int32_t type = 0;
        if (!str_utils_compare_length(name, name_len, "light", 5) && name_len > 6 && name[5] == '_') {
            type = 1;
            name += 6;
            name_len -= 6;
        }
        else if (!str_utils_compare_length(name, name_len, "fog", 3) && name_len > 4 && name[3] == '_') {
            type = 2;
            name += 4;
            name_len -= 4;
        }
        else if (!str_utils_compare_length(name, name_len, "glow", 4) && name_len > 5 && name[4] == '_') {
            type = 3;
            name += 5;
            name_len -= 5;
        }
        else if (!str_utils_compare_length(name, name_len, "wind", 4) && name_len > 5 && name[4] == '_') {
            type = 4;
            name += 5;
            name_len -= 5;
        }
        else if (!str_utils_compare_length(name, name_len, "face", 4) && name_len > 5 && name[4] == '_') {
            type = 5;
            name += 5;
            name_len -= 5;
        }
        else
            continue;

        char* b = buf;
        const char* n = name;
        size_t s = 0;
        bool ret = false;
        do {
            char c = *n++;
            if (c == '_' || c == '.')
                break;
            else if (!c) {
                ret = true;
                break;
            }

            *b++ = c;
            s++;
        } while (*n);

        if (ret)
            continue;

        *b++ = 0;

        b = buf;
        if (s < 5 || b[0] != 'p' || b[1] != 'v' || b[2] < '0' && b[2] > '9'
            || b[3] < '0' && b[3] > '9' || b[4] < '0' && b[4] > '9')
            continue;

        int32_t pv_id = (b[2] - '0') * 100;
        pv_id += (b[3] - '0') * 10;
        pv_id += b[4] - '0';

        s = 0;
        ret = false;
        do {
            char c = *n++;
            if (c == '_' || c == '.')
                break;
            else if (!c) {
                ret = true;
                break;
            }

            *b++ = c;
            s++;
        } while (*n);

        if (ret)
            continue;

        *b++ = 0;

        b = buf;
        if (s != 4 || b[0] != 'c' || b[1] < '0' && b[1] > '9'
            || b[2] < '0' && b[2] > '9' || b[3] < '0' && b[3] > '9')
            continue;

        int32_t cut_id = (b[1] - '0') * 100;
        cut_id += (b[2] - '0') * 10;
        cut_id += b[3] - '0';

        std::map<int32_t, light_param_data>::iterator elem = tree->find(cut_id);
        light_param_data* light_param;
        if (elem != tree->end())
            light_param = &elem->second;
        else {
            light_param = &tree->insert({ cut_id, *default_light_param }).first->second;
            light_param->ibl.ready = false;
            light_param->light.ready = false;
            light_param->fog.ready = false;
            light_param->glow.ready = false;
            light_param->wind.ready = false;
            light_param->face.ready = false;
            light_param->pv = true;
            light_param_data_load_pv_cut_file_names(light_param, cut_id, &pv_str);
        }

        if (type == 1) {
            light_param->light = light_param_light();
            light_param->light.read(i.data, i.size);
        }
        else if (type == 2) {
            light_param->fog = light_param_fog();
            light_param->fog.read(i.data, i.size);
        }
        else if (type == 3) {
            light_param->glow = light_param_glow();
            light_param->glow.read(i.data, i.size);
        }
        else if (type == 4) {
            light_param->wind = light_param_wind();
            light_param->wind.read(i.data, i.size);
        }
        else if (type == 5) {
            light_param->face = light_param_face();
            light_param->face.read(i.data, i.size);
        }
    }
}

static void light_param_data_load_pv_cut_file_names(light_param_data* light_param, int32_t cut_id, std::string* name) {
    char buf[0x100];

    sprintf_s(buf, sizeof(buf), "%s_c%03d", name->c_str(), cut_id);
    light_param->name = buf;

    sprintf_s(buf, sizeof(buf), "%s.ibl", light_param->name.c_str());
    light_param->paths[0] = "rom/ibl/";
    light_param->files[0] = buf;

    sprintf_s(buf, sizeof(buf), "light_%s.txt", light_param->name.c_str());
    light_param->paths[1] = "rom/light_param/";
    light_param->files[1] = buf;

    sprintf_s(buf, sizeof(buf), "fog_%s.txt", light_param->name.c_str());
    light_param->paths[2] = "rom/light_param/";
    light_param->files[2] = buf;

    sprintf_s(buf, sizeof(buf), "glow_%s.txt", light_param->name.c_str());
    light_param->paths[3] = "rom/light_param/";
    light_param->files[3] = buf;

    sprintf_s(buf, sizeof(buf), "wind_%s.txt", light_param->name.c_str());
    light_param->paths[4] = "rom/light_param/";
    light_param->files[4] = buf;

    sprintf_s(buf, sizeof(buf), "face_%s.txt", light_param->name.c_str());
    light_param->paths[5] = "rom/light_param/";
    light_param->files[5] = buf;
}

void light_param_data_load_stage_file_names(light_param_data* light_param, std::string* name) {
    char buf[0x100];

    light_param->name = *name;

    sprintf_s(buf, sizeof(buf), "%s.ibl", light_param->name.c_str());
    light_param->paths[0] = "rom/ibl/";
    light_param->files[0] = buf;

    sprintf_s(buf, sizeof(buf), "light_%s.txt", light_param->name.c_str());
    light_param->paths[1] = "rom/light_param/";
    light_param->files[1] = buf;

    sprintf_s(buf, sizeof(buf), "fog_%s.txt", light_param->name.c_str());
    light_param->paths[2] = "rom/light_param/";
    light_param->files[2] = buf;

    sprintf_s(buf, sizeof(buf), "glow_%s.txt", light_param->name.c_str());
    light_param->paths[3] = "rom/light_param/";
    light_param->files[3] = buf;

    sprintf_s(buf, sizeof(buf), "wind_%s.txt", light_param->name.c_str());
    light_param->paths[4] = "rom/light_param/";
    light_param->files[4] = buf;

    sprintf_s(buf, sizeof(buf), "face_%s.txt", light_param->name.c_str());
    light_param->paths[5] = "rom/light_param/";
    light_param->files[5] = buf;
}

static void light_param_storage_free_file_handlers(light_param_data_storage* a1) {
    for (int32_t i = 0; i < 6; ++i)
        a1->file_handlers[i].free_data();
    a1->farc_file_handler.free_data();
}

static void light_param_storage_load_stages(light_param_data_storage* a1, std::vector<int32_t>* stage_indices) {
    if (a1->state)
        for (int32_t i = 0; i < 6; ++i)
            if (a1->file_handlers[i].check_not_ready())
                a1->file_handlers[i].call_free_func_free_data();

    a1->stage_index = -1;
    a1->stage.clear();

    for (int32_t& i : *stage_indices) {
        std::string name;
        light_param_data_get_stage_name_string(&name, i);
        if (!name.size())
            continue;

        std::map<int32_t, light_param_data>::iterator elem = a1->stage.find(i);
        light_param_data* light_param;
        if (elem != a1->stage.end())
            light_param = &elem->second;
        else
            light_param = &a1->stage.insert({ i, a1->default_light_param }).first->second;

        light_param_data_load_stage_file_names(light_param, &name);
        light_param->pv = false;
    }
    a1->stage_light_param_iterator = a1->stage.begin();
    a1->state = a1->stage.size() ? 1 : 0;
}

static void light_param_storage_load_stages(light_param_data_storage* a1,
    std::vector<uint32_t>* stage_hashes, stage_database* stage_data) {
    if (a1->state)
        for (int32_t i = 0; i < 6; ++i)
            if (a1->file_handlers[i].check_not_ready())
                a1->file_handlers[i].call_free_func_free_data();

    a1->stage_index = -1;
    a1->stage.clear();

    for (uint32_t& i : *stage_hashes) {
        std::string name;
        light_param_data_get_stage_name_string(&name, i, stage_data);
        if (!name.size())
            continue;

        if (!str_utils_compare_length(name.c_str(), name.size(), "pv0", 3))
            name[2] = '8';

        std::map<int32_t, light_param_data>::iterator elem = a1->stage.find(i);
        light_param_data* light_param;
        if (elem != a1->stage.end())
            light_param = &elem->second;
        else
            light_param = &a1->stage.insert({ i, a1->default_light_param }).first->second;

        light_param_data_load_stage_file_names(light_param, &name);
        light_param->pv = false;
    }
    a1->stage_light_param_iterator = a1->stage.begin();
    a1->state = a1->stage.size() ? 1 : 0;
}

static void light_param_storage_reset(light_param_data_storage* a1) {
    a1->stage.clear();
    a1->pv_cut.clear();
    a1->pv_id = 0;
    for (p_file_handler& i : a1->file_handlers)
        i.free_data();
    a1->farc_file_handler.free_data();
}

static void light_param_storage_set_default_light_param(light_param_data_storage* a1, int32_t stage_index) {
    std::map<int32_t, light_param_data>::iterator elem;
    light_param_storage_set_stage(a1, &elem, stage_index);
    if (elem != a1->stage.end())
        a1->default_light_param = elem->second;
}

static void light_param_data_set(light_param_data* a1, light_param_data_storage* storage) {
    if (!a1->pv || a1->light.ready)
        rctx_ptr->light_param_data_light_set(&a1->light);
    if (!a1->pv || a1->fog.ready)
        rctx_ptr->light_param_data_fog_set(&a1->fog);
    if (!a1->pv || a1->glow.ready)
        rctx_ptr->light_param_data_glow_set(&a1->glow);
    if (!a1->pv || a1->ibl.ready)
        rctx_ptr->light_param_data_ibl_set(&a1->ibl, storage);
    if (!a1->pv) {
        rctx_ptr->light_param_data_wind_set(&a1->wind);
        if (a1->face.ready)
            rctx_ptr->light_param_data_face_set(&a1->face);
    }
}

static void light_param_storage_set_pv_cut(light_param_data_storage* a1,
    std::map<int32_t, light_param_data>::iterator* elem, int32_t cut_id) {
    *elem = a1->pv_cut.find(cut_id);
    if (*elem != a1->pv_cut.end())
        light_param_data_set(&(*elem)->second, a1);
}

static void light_param_storage_set_stage(light_param_data_storage* a1,
    std::map<int32_t, light_param_data>::iterator* elem, int32_t stage_index) {
    *elem = a1->stage.find(stage_index);
    if (*elem != a1->stage.end()) {
        a1->stage_index = stage_index;
        a1->name = (*elem)->second.name;
        light_param_data_set(&(*elem)->second, a1);
    }
}
