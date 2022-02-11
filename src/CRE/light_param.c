/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "light_param.h"
#include "../KKdLib/str_utils.h"

vector_func(light_param_data)

typedef struct light_param_storage {
    bool pv;
    union {
        uint64_t hash;
        uint32_t pv_id;
    };
    int32_t count;
    union {
        light_param_data data;
        light_param_set set;
    };
} light_param_storage;

vector(light_param_storage)

static void light_param_set_load_farc_file(light_param_set* set, farc_file* ff);

vector_func(light_param_storage)

vector_light_param_storage light_param_storage_data;

void light_param_data_init(light_param_data* light_param) {
    memset(light_param, 0, sizeof(light_param_data));
    light_param_light_init(&light_param->light);
    light_param_fog_init(&light_param->fog);
    light_param_glow_init(&light_param->glow);
    light_param_ibl_init(&light_param->ibl);
    light_param_wind_init(&light_param->wind);
    light_param_face_init(&light_param->face);
}

void light_param_data_load(light_param_data* light_param, data_struct* data, char* name) {
    char buf[_MAX_PATH];

    light_param->id = 0;

    sprintf_s(buf, sizeof(buf), "light_%s.txt", name);
    data_struct_load_file(data, &light_param->light, "rom/light_param/", buf, light_param_light_load_file);

    sprintf_s(buf, sizeof(buf), "fog_%s.txt", name);
    data_struct_load_file(data, &light_param->fog, "rom/light_param/", buf, light_param_fog_load_file);

    sprintf_s(buf, sizeof(buf), "glow_%s.txt", name);
    data_struct_load_file(data, &light_param->glow, "rom/light_param/", buf, light_param_glow_load_file);

    sprintf_s(buf, sizeof(buf), "%s.ibl", name);
    data_struct_load_file(data, &light_param->ibl, "rom/ibl/", buf, light_param_ibl_load_file);

    sprintf_s(buf, sizeof(buf), "wind_%s.txt", name);
    data_struct_load_file(data, &light_param->wind, "rom/light_param/", buf, light_param_wind_load_file);

    sprintf_s(buf, sizeof(buf), "face_%s.txt", name);
    data_struct_load_file(data, &light_param->face, "rom/light_param/", buf, light_param_face_load_file);
}

void light_param_data_free(light_param_data* light_param) {
    light_param_light_free(&light_param->light);
    light_param_fog_free(&light_param->fog);
    light_param_glow_free(&light_param->glow);
    light_param_ibl_free(&light_param->ibl);
    light_param_wind_free(&light_param->wind);
    light_param_face_free(&light_param->face);
}

void light_param_set_init(light_param_set* set) {
    memset(set, 0, sizeof(light_param_set));
}

void light_param_set_load(light_param_set* set, data_struct* data, int32_t pv_id) {
    char buf[_MAX_PATH];
    sprintf_s(buf, sizeof(buf), "light_pv%03d.farc", pv_id);

    farc f;
    farc_init(&f);
    if (!data_struct_load_file(data, &f, "rom/light_param/", buf, farc_load_file))
        return;

    for (farc_file* i = f.files.begin; i != f.files.end; i++)
        light_param_set_load_farc_file(set, i);
    farc_free(&f);
}

void light_param_set_free(light_param_set* set) {
    vector_light_param_data_free(set, light_param_data_free);
}

char* light_param_get_string(char* name) {
    size_t length = utf8_length(name);
    if (str_utils_compare_length(name, length, "STG", 3))
        return 0;

    name += 3;
    length -= 3;

    char* str = force_malloc_s(char, length + 1);
    for (size_t i = 0; i < length; i++) {
        char c = name[i];
        if (c > 0x40 && c < 0x5B)
            c += 0x20;
        str[i] = c;
    }
    str[length] = 0;
    return str;
}

inline void light_param_storage_init() {
    light_param_storage_data = vector_empty(light_param_storage);
}

inline void light_param_storage_append_light_param_data(char* name) {
    uint64_t hash = hash_utf8_fnv1a64m(name, false);
    for (light_param_storage* i = light_param_storage_data.begin;
        i != light_param_storage_data.end; i++)
        if (!i->pv && i->hash == hash) {
            i->count++;
            return;
        }
}

inline void light_param_storage_append_light_param_set(int32_t pv_id) {
    for (light_param_storage* i = light_param_storage_data.begin;
        i != light_param_storage_data.end; i++)
        if (i->pv && i->pv_id == pv_id) {
            i->count++;
            return;
        }
}

inline void light_param_storage_load_light_param_data(data_struct* data, char* name) {
    light_param_data light_param;
    light_param_data_init(&light_param);
    light_param_data_load(&light_param, data, name);
    light_param_storage_insert_light_param_data(&light_param, name);
}

inline void light_param_storage_load_light_param_set(data_struct* data, int32_t pv_id) {
    light_param_set light_param;
    light_param_set_init(&light_param);
    light_param_set_load(&light_param, data, pv_id);
    light_param_storage_insert_light_param_set(&light_param, pv_id);
}

inline void light_param_storage_insert_light_param_data(light_param_data* data, char* name) {
    uint64_t hash = hash_utf8_fnv1a64m(name, false);
    for (light_param_storage* i = light_param_storage_data.begin;
        i != light_param_storage_data.end; i++)
        if (!i->pv && i->hash == hash) {
            light_param_data_free(&i->data);
            i->count++;
            i->data = *data;
            return;
        }

    light_param_storage* lit_param_set_storage =
        vector_light_param_storage_reserve_back(&light_param_storage_data);
    lit_param_set_storage->pv = false;
    lit_param_set_storage->hash = hash;
    lit_param_set_storage->count = 1;
    lit_param_set_storage->data = *data;
}

inline void light_param_storage_insert_light_param_set(light_param_set* set, int32_t pv_id) {
    for (light_param_storage* i = light_param_storage_data.begin;
        i != light_param_storage_data.end; i++)
        if (i->pv && i->pv_id == pv_id) {
            light_param_set_free(&i->set);
            i->count++;
            i->set = *set;
            return;
        }

    light_param_storage* lit_param_set_storage = vector_light_param_storage_reserve_back(&light_param_storage_data);
    lit_param_set_storage->pv = true;
    lit_param_set_storage->pv_id = pv_id;
    lit_param_set_storage->count = 1;
    lit_param_set_storage->set = *set;
}

inline light_param_data* light_param_storage_get_light_param_data(char* name) {
    uint64_t hash = hash_utf8_fnv1a64m(name, false);
    for (light_param_storage* i = light_param_storage_data.begin;
        i != light_param_storage_data.end; i++)
        if (!i->pv && i->hash == hash)
            return &i->data;
    return 0;
}

inline light_param_set* light_param_storage_get_light_param_set(int32_t pv_id) {
    for (light_param_storage* i = light_param_storage_data.begin;
        i != light_param_storage_data.end; i++)
        if (i->pv && i->pv_id == pv_id)
            return &i->set;
    return 0;
}

inline void light_param_storage_delete_light_param_data(char* name) {
    uint64_t hash = hash_utf8_fnv1a64m(name, false);
    for (light_param_storage* i = light_param_storage_data.begin;
        i != light_param_storage_data.end; i++)
        if (!i->pv && i->hash == hash) {
            i->count--;
            if (i->count > 0)
                break;

            light_param_set_free(&i->set);

            vector_light_param_storage_erase(&light_param_storage_data,
                i - light_param_storage_data.begin, 0);
            break;
        }
}

inline void light_param_storage_delete_light_param_set(int32_t pv_id) {
    for (light_param_storage* i = light_param_storage_data.begin;
        i != light_param_storage_data.end; i++)
        if (i->pv && i->pv_id == pv_id) {
            i->count--;
            if (i->count > 0)
                break;

            light_param_set_free(&i->set);

            vector_light_param_storage_erase(&light_param_storage_data,
                i - light_param_storage_data.begin, 0);
            break;
        }
}

inline void light_param_storage_free() {
    for (light_param_storage* i = light_param_storage_data.begin;
        i != light_param_storage_data.end; i++)
        if (!i->pv)
            light_param_data_free(&i->data);
        else
            light_param_set_free(&i->set);
    vector_light_param_storage_free(&light_param_storage_data, 0);
}

static void light_param_set_load_farc_file(light_param_set* set, farc_file* ff) {
    char* name = string_data(&ff->name);
    size_t name_len = utf8_length(name);

    int32_t type = 0;
    if (!str_utils_compare_length(name, name_len, "light", 5) && name[5] == '_') {
        type = 1;
        name += 6;
        name_len -= 6;
    }
    else if (!str_utils_compare_length(name, name_len, "fog", 3) && name[3] == '_') {
        type = 2;
        name += 4;
        name_len -= 4;
    }
    else if (!str_utils_compare_length(name, name_len, "glow", 4) && name[4] == '_') {
        type = 3;
        name += 5;
        name_len -= 5;
    }
    else if (!str_utils_compare_length(name, name_len, "wind", 4) && name[4] == '_') {
        type = 4;
        name += 5;
        name_len -= 5;
    }
    else if (!str_utils_compare_length(name, name_len, "face", 4) && name[4] == '_') {
        type = 5;
        name += 5;
        name_len -= 5;
    }
    else
        return;

    char buf[0x100];
    char* b = buf;
    char* n = name;
    size_t s = 0;
    do {
        char c = *n++;
        if (c == '_' || c == '.')
            break;
        else if (!c)
            return;

        *b++ = c;
        s++;
    } while (*n);
    *b++ = 0;

    b = buf;
    if (s < 5 || b[0] != 'p' || b[1] != 'v' || b[2] < '0' && b[2] > '9'
        || b[3] < '0' && b[3] > '9' || b[4] < '0' && b[4] > '9')
        return;

    int32_t pv_id = (b[2] - '0') * 100;
    pv_id += (b[3] - '0') * 10;
    pv_id += b[4] - '0';

    s = 0;
    do {
        char c = *n++;
        if (c == '_' || c == '.')
            break;
        else if (!c)
            return;

        *b++ = c;
        s++;
    } while (*n);
    *b++ = 0;

    b = buf;
    if (s != 4 || b[0] != 'c' || b[1] < '0' && b[1] > '9'
        || b[2] < '0' && b[2] > '9' || b[3] < '0' && b[3] > '9')
        return;

    int32_t cut = (b[1] - '0') * 100;
    cut += (b[2] - '0') * 10;
    cut += b[3] - '0';

    light_param_data* data = 0;
    for (data = set->begin; data != set->end; data++)
        if (data->id == cut)
            break;

    if (data == set->end) {
        data = vector_light_param_data_reserve_back(set);
        data->id = cut;
    }

    if (type == 1) {
        if (data->light.ready)
            light_param_light_free(&data->light);
        light_param_light_init(&data->light);
        light_param_light_mread(&data->light, ff->data, ff->size);
    }
    else if (type == 2) {
        if (data->fog.ready)
            light_param_fog_free(&data->fog);
        light_param_fog_init(&data->fog);
        light_param_fog_mread(&data->fog, ff->data, ff->size);
    }
    else if (type == 3) {
        if (data->glow.ready)
            light_param_glow_free(&data->glow);
        light_param_glow_init(&data->glow);
        light_param_glow_mread(&data->glow, ff->data, ff->size);
    }
    else if (type == 4) {
        if (data->wind.ready)
            light_param_wind_free(&data->wind);
        light_param_wind_init(&data->wind);
        light_param_wind_mread(&data->wind, ff->data, ff->size);
    }
    else if (type == 5) {
        if (data->face.ready)
            light_param_face_free(&data->face);
        light_param_face_init(&data->face);
        light_param_face_mread(&data->face, ff->data, ff->size);
    }
}
