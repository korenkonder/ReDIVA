/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "glitter.hpp"
#include "../../KKdLib/io/file_stream.hpp"
#include "../../KKdLib/io/json.hpp"
#include "../../KKdLib/io/path.hpp"
#include "../../KKdLib/farc.hpp"
#include "../../KKdLib/msgpack.hpp"
#include "../data.hpp"
#include "../object.hpp"

namespace Glitter {
    static void effect_group_msgpack_read(const char* path, const char* file, EffectGroup* eff_group);

    FileReader::FileReader(GLT)
        : file_handler(), farc(), effect_group(), load_count(), state(), init_scene(), obj_db(), tex_db() {
        emission = -1.0f;
        type = GLT_VAL;
        hash = GLT_VAL != Glitter::FT ? hash_murmurhash_empty : hash_fnv1a64m_empty;
    }

    FileReader::FileReader(GLT, const char* path, const char* file, float_t emission)
        : file_handler(), farc(), effect_group(), load_count(), state(), init_scene(), obj_db(), tex_db() {
        this->path = path ? path
            : (GLT_VAL != Glitter::FT ? "root+/particle/" : "rom/particle/");
        this->file = file;
        this->emission = emission;
        this->type = GLT_VAL;
        this->hash = GLT_VAL != Glitter::FT
            ? hash_utf8_murmurhash(file)
            : hash_utf8_fnv1a64m(file);
    }

    FileReader::FileReader(GLT, const wchar_t* path, const wchar_t* file, float_t emission)
        : file_handler(), farc(), effect_group(), load_count(), state(), init_scene(), obj_db(), tex_db() {
        char* path_temp = utf16_to_utf8(path);
        char* file_temp = utf16_to_utf8(file);
        this->path = path_temp ? path_temp
            : (GLT_VAL != Glitter::FT ? "root+/particle/" : "rom/particle/");
        this->file = file_temp;
        this->emission = emission;
        this->type = GLT_VAL;
        this->hash = GLT_VAL != Glitter::FT
            ? hash_utf8_murmurhash(file_temp)
            : hash_utf8_fnv1a64m(file_temp);
        free_def(path_temp);
        free_def(file_temp);
    }

    FileReader::~FileReader() {
        delete farc;
        delete file_handler;
    }

    bool FileReader::CheckInit(GPM) {
        if (!effect_group)
            return true;

        bool ret = false;
        if (!effect_group->scene_init) {
            float_t v20 = Glitter::glt_particle_manager->field_D4;
            if (Glitter::glt_particle_manager->field_D4 > 0.0f) {
                if (effect_group->scene) {
                    if (effect_group->scene->ResetCheckInit(GPM_VAL, &v20))
                        ret = true;
                    else
                        effect_group->scene_init = true;
                    glt_particle_manager->sub_1403A53E0(v20);
                }
                else
                    effect_group->scene_init = true;
            }
            else
                ret = true;
        }

#if defined(CRE_DEV)
        if (type == Glitter::X)
            for (Mesh& i : effect_group->meshes)
                if (!i.ready && i.object_set_hash != hash_murmurhash_empty && i.object_set_hash != -1)
                    if (object_storage_load_obj_set_check_not_read(i.object_set_hash, obj_db, tex_db))
                        ret = true;
                    else
                        i.ready = true;
#endif

        if (!ret) {
            effect_group = 0;
            return true;
        }
        return false;
    }

    bool FileReader::LoadFarc(void* data, const char* path, const char* file,
        uint64_t hash, object_database* obj_db, texture_database* tex_db) {
        this->obj_db = obj_db;
        this->tex_db = tex_db;
        if (type == Glitter::FT && this->hash != hash_fnv1a64m_empty
            || type != Glitter::FT && this->hash != hash_murmurhash_empty)
            return false;

        this->path = path;
        this->file = file;

        std::string file_temp = this->file + ".farc";

        file_handler = new p_file_handler;
        farc = new ::farc;
        if (file_handler->read_file(data, path, file_temp.c_str())) {
            this->hash = hash;
            load_count = 1;
            return true;
        }
        return false;
    }

    void FileReader::ParseAnimation(f2_struct* st, Animation* anim) {
        if (!st || !st->header.data_size
            || st->header.signature != reverse_endianness_uint32_t('ANIM'))
            return;

        for (f2_struct& i : st->sub_structs)
            ParseCurve(&i, anim);
    }

    void FileReader::ParseCurve(f2_struct* st, Animation* anim) {
        if (!st || !st->header.data_size
            || st->header.signature != reverse_endianness_uint32_t('CURV'))
            return;

        Curve* c = new Curve(type);
        if (!c)
            return;

        c->version = st->header.version;
        size_t d = (size_t)st->data.data();
        uint32_t keys_count;
        if (type == Glitter::X) {
            if (st->header.version == 1) {
                if (st->header.use_big_endian) {
                    c->type = (CurveType)load_reverse_endianness_uint32_t((void*)d);
                    c->repeat = load_reverse_endianness_uint32_t((void*)(d + 4)) != 0;
                    c->flags = (CurveFlag)load_reverse_endianness_uint32_t((void*)(d + 8));
                    c->random_range = load_reverse_endianness_float_t((void*)(d + 12));
                    keys_count = (uint32_t)load_reverse_endianness_int16_t((void*)(d + 16));
                    c->start_time = load_reverse_endianness_int16_t((void*)(d + 18));
                    c->end_time = load_reverse_endianness_int16_t((void*)(d + 20));
                }
                else {
                    c->type = (CurveType) * (uint32_t*)d;
                    c->repeat = *(uint32_t*)(d + 4) != 0;
                    c->flags = (CurveFlag) * (uint32_t*)(d + 8);
                    c->random_range = *(float_t*)(d + 12);
                    keys_count = *(uint16_t*)(d + 16);
                    c->start_time = *(uint16_t*)(d + 18);
                    c->end_time = *(uint16_t*)(d + 20);
                }
            }
            else {
                if (st->header.use_big_endian) {
                    c->type = (CurveType)load_reverse_endianness_uint32_t((void*)d);
                    c->repeat = load_reverse_endianness_uint32_t((void*)(d + 4)) != 0;
                    c->flags = (CurveFlag)load_reverse_endianness_uint32_t((void*)(d + 8));
                    c->random_range = load_reverse_endianness_float_t((void*)(d + 12));
                    keys_count = (uint32_t)load_reverse_endianness_uint16_t((void*)(d + 28));
                    c->start_time = load_reverse_endianness_uint16_t((void*)(d + 30));
                    c->end_time = load_reverse_endianness_uint16_t((void*)(d + 32));
                }
                else {
                    c->type = (CurveType) * (uint32_t*)d;
                    c->repeat = *(uint32_t*)(d + 4) != 0;
                    c->flags = (CurveFlag) * (uint32_t*)(d + 8);
                    c->random_range = *(float_t*)(d + 12);
                    keys_count = *(uint16_t*)(d + 28);
                    c->start_time = *(uint16_t*)(d + 30);
                    c->end_time = *(uint16_t*)(d + 32);
                }
            }
        }
        else {
            if (st->header.use_big_endian) {
                c->type = (CurveType)load_reverse_endianness_uint32_t((void*)d);
                c->repeat = load_reverse_endianness_uint32_t((void*)(d + 4)) != 0;
                c->flags = (CurveFlag)load_reverse_endianness_uint32_t((void*)(d + 8));
                c->random_range = load_reverse_endianness_float_t((void*)(d + 12));
                keys_count = load_reverse_endianness_uint16_t((void*)(d + 16));
                c->start_time = load_reverse_endianness_uint16_t((void*)(d + 18));
                c->end_time = load_reverse_endianness_uint16_t((void*)(d + 20));
            }
            else {
                c->type = (CurveType) * (uint32_t*)d;
                c->repeat = *(uint32_t*)(d + 4) != 0;
                c->flags = (CurveFlag) * (uint32_t*)(d + 8);
                c->random_range = *(float_t*)(d + 12);
                keys_count = *(uint16_t*)(d + 16);
                c->start_time = *(uint16_t*)(d + 18);
                c->end_time = *(uint16_t*)(d + 20);
            }

            if (c->version == 0)
                switch (c->type) {
                case CURVE_ROTATION_X:
                case CURVE_ROTATION_Y:
                case CURVE_ROTATION_Z:
                    c->random_range = 0.0f;
                    break;
                }
        }

        for (f2_struct& i : st->sub_structs)
            if (i.header.data_size && i.header.signature == reverse_endianness_uint32_t('KEYS')) {
                UnpackCurve(i.data.data(), anim, c,
                    keys_count, i.header.version, i.header.use_big_endian);
                break;
            }
        anim->curves.push_back(c);
    }

    bool FileReader::ParseDivaEffect(GPM, f2_struct* st) {
        if (!st || !st->header.data_size
            || st->header.signature != reverse_endianness_uint32_t('DVEF'))
            return false;

        uint32_t dvef_version = st->header.version;
        EffectGroup* eff_group = new EffectGroup(type);
        if (!eff_group)
            return false;

        eff_group->scene = 0;
        if (!ParseEffectGroup(st, &eff_group->effects, eff_group)) {
            eff_group->not_loaded = true;
            if (GPM_VAL->AppendEffectGroup(hash, eff_group, this))
                return true;
        }
        else if (GPM_VAL->AppendEffectGroup(hash, eff_group, this)) {
#if defined(CRE_DEV)
            if (type == Glitter::X)
                for (Mesh& i : eff_group->meshes)
                    if (i.object_set_hash != hash_murmurhash_empty) {
                        object_storage_load_set_hash(file_handler->ptr->ds, i.object_set_hash);
                        i.load = true;
                    }
#endif

            if (!init_scene)
                return true;

            effect_group = eff_group;
            int32_t id = 1;
            for (Effect*& i : eff_group->effects) {
                if (i->data.start_time <= 0.0f) {
                    id++;
                    continue;
                }

                if (!effect_group->scene)
                    effect_group->scene = new Scene(0, type == Glitter::FT
                        ? hash_fnv1a64m_empty : hash_murmurhash_empty, eff_group, true);

                if (effect_group->scene)
                    effect_group->scene->InitEffect(GPM_VAL, i, id, true);
                id++;
            }
            return true;
        }

        delete eff_group;
        return false;
    }

    bool FileReader::ParseDivaList(f2_struct* st, EffectGroup* eff_group) {
        if (!st || !st->header.data_size
            || st->header.signature != reverse_endianness_uint32_t('LIST'))
            return false;

        for (f2_struct& i : st->sub_structs)
            if (UnpackDivaList(&i, eff_group))
                break;
        return true;
    }

    bool FileReader::ParseDivaResource(GPM, f2_struct* st, EffectGroup* eff_group) {
        if (!st || !st->header.data_size)
            return false;

        for (f2_struct& i : st->sub_structs)
            if (UnpackDivaResource(GPM_VAL, &i, eff_group))
                break;
        return true;
    }

    bool FileReader::ParseEffect(f2_struct* st, EffectGroup* eff_group) {
        if (!st || !st->header.data_size
            || st->header.signature != reverse_endianness_uint32_t('EFCT'))
            return false;

        Effect* eff = new Effect(type);
        if (!UnpackEffect(st->data.data(), eff,
            st->header.version, st->header.use_big_endian)) {
            delete eff;
            return false;
        }

        ParseAnimation(st->sub_structs.size() ? st->sub_structs.data() : 0, &eff->animation);

        for (f2_struct& i : st->sub_structs)
            ParseEmitter(&i, eff, eff_group);
        eff_group->effects.push_back(eff);
        return true;
    }

    bool FileReader::ParseEffectGroup(f2_struct* st, std::vector<Effect*>* vec, EffectGroup* eff_group) {
        if (!st || !st->header.data_size
            || st->header.signature != reverse_endianness_uint32_t('DVEF'))
            return false;

        for (f2_struct& i : st->sub_structs) {
            if (!i.header.data_size)
                continue;

            if (i.header.signature == reverse_endianness_uint32_t('EFCT')) {
                if (!ParseEffect(&i, eff_group))
                    return false;
            }
            else if (i.header.signature == reverse_endianness_uint32_t('DVRS')
                && !UnpackDivaResourceHashes(&i, eff_group))
                return false;
        }
        return true;
    }

    bool FileReader::ParseEmitter(f2_struct* st, Effect* eff, EffectGroup* eff_group) {
        if (!st || !st->header.data_size || st->header.signature != reverse_endianness_uint32_t('EMIT'))
            return false;

        Emitter* emit = new Emitter(type);
        if (!UnpackEmitter(st->data.data(), emit,
            st->header.version, st->header.use_big_endian)) {
            delete emit;
            return false;
        }

        ParseAnimation(st->sub_structs.size() ? st->sub_structs.data() : 0, &emit->animation);

        for (f2_struct& i : st->sub_structs)
            ParseParticle(&i, emit, eff, eff_group);
        eff->emitters.push_back(emit);
        return true;
    }

    bool FileReader::ParseParticle(f2_struct* st, Emitter* emit, Effect* eff, EffectGroup* eff_group) {
        if (!st || !st->header.data_size || st->header.signature != reverse_endianness_uint32_t('PTCL'))
            return false;

        Particle* ptcl = new Particle(type);
        if (!UnpackParticle(st->data.data(), ptcl,
            st->header.version, eff, st->header.use_big_endian, eff_group)) {
            delete ptcl;
            return false;
        }

        ParseAnimation(st->sub_structs.size() ? st->sub_structs.data() : 0, &ptcl->animation);
        emit->particles.push_back(ptcl);
        return true;
    }

    bool FileReader::Read(GPM) {
        std::string dve_file = file + ".dve";
        farc_file* dve_ff = farc->read_file(dve_file.c_str());
        if (!dve_ff)
            return false;

        f2_struct st;
        st.read(dve_ff->data, dve_ff->size);
        if (st.header.signature != reverse_endianness_uint32_t('DVEF'))
            return false;

        if (!ParseDivaEffect(GPM_VAL, &st)) {
            EffectGroup* eff_group = new EffectGroup(type);
            if (eff_group) {
                eff_group->not_loaded = true;
                if (!GPM_VAL->AppendEffectGroup(hash, eff_group, this))
                    delete eff_group;
            }
            return false;
        }

        EffectGroup* eff_group = GPM_VAL->GetEffectGroup(hash);
        if (!eff_group)
            return false;

        std::string drs_file = file + ".drs";
        farc_file* drs_ff = farc->read_file(drs_file.c_str());
        if (drs_ff) {
            f2_struct st;
            st.read(drs_ff->data, drs_ff->size);

            if (st.header.signature == reverse_endianness_uint32_t('DVRS'))
                ParseDivaResource(GPM_VAL, &st, eff_group);
        }

        std::string lst_file = file + ".lst";
        farc_file* lst_ff = farc->read_file(lst_file.c_str());
        if (lst_ff) {
            f2_struct st;
            st.read(lst_ff->data, lst_ff->size);
            if (st.header.signature == reverse_endianness_uint32_t('LIST'))
                ParseDivaList(&st, eff_group);
        }

        effect_group_msgpack_read("patch\\AFT\\particle", file.c_str(), eff_group);
        return true;
    }

    bool FileReader::ReadFarc(GPM) {
        if (state) {
            if (state == 1)
                if (CheckInit(GPM_VAL))
                    state = 2;
                else
                    return false;
            return true;
        }

        if (file_handler && !file_handler->check_not_ready()) {
            farc->read(file_handler->get_data(), file_handler->get_size(), true);
            if (Read(GPM_VAL) && init_scene)
                state = 1;
            else
                return true;
        }
        return false;
    }

    void FileReader::UnpackCurve(void* data, Animation* anim,
        Curve* c, uint32_t count, uint32_t keys_version, bool big_endian) {
        if (!data || !c || count < 1)
            return;

        c->keys_version = keys_version;
        size_t d = (size_t)data;

        Curve::Key key;
        key.type = KEY_CONSTANT;
        key.frame = 0;
        key.value = 0.0f;
        key.tangent1 = 0.0f;
        key.tangent2 = 0.0f;
        key.random_range = 0.0f;

        std::vector<Curve::Key>& keys = c->keys;
        keys.reserve(count);
        if (type == Glitter::X) {
            if (c->keys_version == 2) {
                if (big_endian)
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType)load_reverse_endianness_int16_t((void*)d);
                            key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4));
                                key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8));
                                key.random_range = load_reverse_endianness_float_t((void*)(d + 12));
                                key.value = load_reverse_endianness_float_t((void*)(d + 16));
                                keys.push_back(key);
                                d += 20;
                            }
                            else {
                                key.random_range = load_reverse_endianness_float_t((void*)(d + 4));
                                key.value = load_reverse_endianness_float_t((void*)(d + 8));
                                keys.push_back(key);
                                d += 12;
                            }
                        }
                    else
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType)load_reverse_endianness_int16_t((void*)d);
                            key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4));
                                key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8));
                                key.value = load_reverse_endianness_float_t((void*)(d + 12));
                                keys.push_back(key);
                                d += 16;
                            }
                            else {
                                key.value = load_reverse_endianness_float_t((void*)(d + 4));
                                keys.push_back(key);
                                d += 8;
                            }
                        }
                else
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType) * (int16_t*)d;
                            key.frame = *(int16_t*)(d + 2);
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = *(float_t*)(d + 4);
                                key.tangent2 = *(float_t*)(d + 8);
                                key.random_range = *(float_t*)(d + 12);
                                key.value = *(float_t*)(d + 16);
                                keys.push_back(key);
                                d += 20;
                            }
                            else {
                                key.random_range = *(float_t*)(d + 4);
                                key.value = *(float_t*)(d + 8);
                                keys.push_back(key);
                                d += 12;
                            }
                        }
                    else
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType) * (int16_t*)d;
                            key.frame = *(int16_t*)(d + 2);
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = *(float_t*)(d + 4);
                                key.tangent2 = *(float_t*)(d + 8);
                                key.value = *(float_t*)(d + 12);
                                keys.push_back(key);
                                d += 16;
                            }
                            else {
                                key.value = *(float_t*)(d + 4);
                                keys.push_back(key);
                                d += 8;
                            }
                        }
            }
            else {
                if (big_endian)
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType)load_reverse_endianness_int16_t((void*)d);
                            key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = load_reverse_endianness_float_t((void*)(d + 16));
                                key.tangent2 = load_reverse_endianness_float_t((void*)(d + 20));
                                key.random_range = load_reverse_endianness_float_t((void*)(d + 24));
                                key.value = load_reverse_endianness_float_t((void*)(d + 28));
                                keys.push_back(key);
                                d += 32;
                            }
                            else {
                                key.random_range = load_reverse_endianness_float_t((void*)(d + 8));
                                key.value = load_reverse_endianness_float_t((void*)(d + 12));
                                keys.push_back(key);
                                d += 16;
                            }
                        }
                    else
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType)load_reverse_endianness_int16_t((void*)d);
                            key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4));
                                key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8));
                                key.value = load_reverse_endianness_float_t((void*)(d + 12));
                                keys.push_back(key);
                                d += 16;
                            }
                            else {
                                key.value = load_reverse_endianness_float_t((void*)(d + 12));
                                keys.push_back(key);
                                d += 16;
                            }
                        }
                else
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType) * (int16_t*)d;
                            key.frame = *(int16_t*)(d + 2);
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = *(float_t*)(d + 16);
                                key.tangent2 = *(float_t*)(d + 20);
                                key.random_range = *(float_t*)(d + 24);
                                key.value = *(float_t*)(d + 28);
                                keys.push_back(key);
                                d += 32;
                            }
                            else {
                                key.random_range = *(float_t*)(d + 8);
                                key.value = *(float_t*)(d + 12);
                                keys.push_back(key);
                                d += 16;
                            }
                        }
                    else
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType) * (int16_t*)d;
                            key.frame = *(int16_t*)(d + 2);
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = *(float_t*)(d + 4);
                                key.tangent2 = *(float_t*)(d + 8);
                                key.value = *(float_t*)(d + 12);
                                keys.push_back(key);
                                d += 16;
                            }
                            else {
                                key.value = *(float_t*)(d + 12);
                                keys.push_back(key);
                                d += 16;
                            }
                        }
            }
        }
        else {
            float_t scale = 1.0f;
            if (c->keys_version == 0)
                switch (c->type) {
                case CURVE_ROTATION_X:
                case CURVE_ROTATION_Y:
                case CURVE_ROTATION_Z:
                    scale = DEG_TO_RAD_FLOAT;
                    break;
                case CURVE_COLOR_R:
                case CURVE_COLOR_G:
                case CURVE_COLOR_B:
                case CURVE_COLOR_A:
                    scale = (float_t)(1.0 / 255.0);
                    break;
                }

            if (big_endian)
                if (scale == 1.0f)
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType)load_reverse_endianness_int16_t((void*)d);
                            key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4));
                                key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8));
                                key.random_range = load_reverse_endianness_float_t((void*)(d + 12));
                                key.value = load_reverse_endianness_float_t((void*)(d + 16));
                                keys.push_back(key);
                                d += 20;
                            }
                            else {
                                key.random_range = load_reverse_endianness_float_t((void*)(d + 4));
                                key.value = load_reverse_endianness_float_t((void*)(d + 8));
                                keys.push_back(key);
                                d += 12;
                            }
                        }
                    else
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType)load_reverse_endianness_int16_t((void*)d);
                            key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4));
                                key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8));
                                key.value = load_reverse_endianness_float_t((void*)(d + 12));
                                keys.push_back(key);
                                d += 16;
                            }
                            else {
                                key.value = load_reverse_endianness_float_t((void*)(d + 4));
                                keys.push_back(key);
                                d += 8;
                            }
                        }
                else
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType)load_reverse_endianness_int16_t((void*)d);
                            key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4)) * scale;
                                key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8)) * scale;
                                key.random_range = load_reverse_endianness_float_t((void*)(d + 12)) * scale;
                                key.value = load_reverse_endianness_float_t((void*)(d + 16)) * scale;
                                keys.push_back(key);
                                d += 20;
                            }
                            else {
                                key.random_range = load_reverse_endianness_float_t((void*)(d + 4)) * scale;
                                key.value = load_reverse_endianness_float_t((void*)(d + 8)) * scale;
                                keys.push_back(key);
                                d += 12;
                            }
                        }
                    else
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType)load_reverse_endianness_int16_t((void*)d);
                            key.frame = load_reverse_endianness_int16_t((void*)(d + 2));
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = load_reverse_endianness_float_t((void*)(d + 4)) * scale;
                                key.tangent2 = load_reverse_endianness_float_t((void*)(d + 8)) * scale;
                                key.value = load_reverse_endianness_float_t((void*)(d + 12)) * scale;
                                keys.push_back(key);
                                d += 16;
                            }
                            else {
                                key.value = load_reverse_endianness_float_t((void*)(d + 4)) * scale;
                                keys.push_back(key);
                                d += 8;
                            }
                        }
            else
                if (scale == 1.0f)
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType) * (int16_t*)d;
                            key.frame = *(int16_t*)(d + 2);
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = *(float_t*)(d + 4);
                                key.tangent2 = *(float_t*)(d + 8);
                                key.random_range = *(float_t*)(d + 12);
                                key.value = *(float_t*)(d + 16);
                                keys.push_back(key);
                                d += 20;
                            }
                            else {
                                key.random_range = *(float_t*)(d + 4);
                                key.value = *(float_t*)(d + 8);
                                keys.push_back(key);
                                d += 12;
                            }
                        }
                    else
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType) * (int16_t*)d;
                            key.frame = *(int16_t*)(d + 2);
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = *(float_t*)(d + 4);
                                key.tangent2 = *(float_t*)(d + 8);
                                key.value = *(float_t*)(d + 12);
                                keys.push_back(key);
                                d += 16;
                            }
                            else {
                                key.value = *(float_t*)(d + 4);
                                keys.push_back(key);
                                d += 8;
                            }
                        }
                else
                    if (c->flags & CURVE_KEY_RANDOM_RANGE)
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType) * (int16_t*)d;
                            key.frame = *(int16_t*)(d + 2);
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = *(float_t*)(d + 4) * scale;
                                key.tangent2 = *(float_t*)(d + 8) * scale;
                                key.random_range = *(float_t*)(d + 12) * scale;
                                key.value = *(float_t*)(d + 16) * scale;
                                keys.push_back(key);
                                d += 20;
                            }
                            else {
                                key.random_range = *(float_t*)(d + 4) * scale;
                                key.value = *(float_t*)(d + 8) * scale;
                                keys.push_back(key);
                                d += 12;
                            }
                        }
                    else
                        for (size_t i = count; i; i--) {
                            key.type = (KeyType) * (int16_t*)d;
                            key.frame = *(int16_t*)(d + 2);
                            if (key.type == KEY_HERMITE) {
                                key.tangent1 = *(float_t*)(d + 4) * scale;
                                key.tangent2 = *(float_t*)(d + 8) * scale;
                                key.value = *(float_t*)(d + 12) * scale;
                                keys.push_back(key);
                                d += 16;
                            }
                            else {
                                key.value = *(float_t*)(d + 4) * scale;
                                keys.push_back(key);
                                d += 8;
                            }
                        }
        }

#if defined(CRE_DEV)
        extern bool glitter_editor_enable;
        if (glitter_editor_enable)
            c->FitKeysIntoCurve(type);
#endif
    }

    bool FileReader::UnpackDivaList(f2_struct* st, EffectGroup* eff_group) {
        if (!st || !st->header.data_size
            || st->header.signature != reverse_endianness_uint32_t('GEFF'))
            return false;

        size_t d = (size_t)st->data.data();
        uint32_t length;
        if (st->header.use_big_endian)
            length = load_reverse_endianness_uint32_t((void*)d);
        else
            length = *(uint32_t*)d;

        if (length != eff_group->effects.size()) {
            for (Effect*& i : eff_group->effects)
                if (i)
                    i->name.clear();
            return false;
        }

        if (length) {
            d += 0x04;
            for (Effect*& i : eff_group->effects) {
                if (i) {
                    char buf[0x80];
                    memcpy(buf, (void*)d, 0x80);
                    buf[0x7F] = 0;

                    i->name.assign(buf);
                }
                d += 0x80;
            }
        }
        return true;
    }

    bool FileReader::UnpackDivaResource(GPM, f2_struct* st, EffectGroup* eff_group) {
        if (!st || !st->header.data_size
            || st->header.signature != reverse_endianness_uint32_t('TXPC'))
            return false;

        eff_group->resources_tex.unpack_file(st->data.data(), st->header.use_big_endian);
        if (!eff_group->resources_count)
            return false;

        size_t count = eff_group->resources_tex.textures.size();

        if (count < 1 || eff_group->resources_count < 1
            || eff_group->resources_count != count)
            return false;

        texture_id* ids = force_malloc<texture_id>(count);
        for (size_t i = 0; i < count; i++) {
            ids[i] = texture_id(0x2A, GPM_VAL->texture_counter);
            GPM_VAL->texture_counter++;
        }

        if (!texture_txp_set_load(&eff_group->resources_tex, &eff_group->resources, ids)) {
            free_def(ids);
            return false;
        }
        free_def(ids);

        for (Effect*& i : eff_group->effects) {
            if (!i)
                continue;

            for (Emitter*& j : i->emitters) {
                if (!j)
                    continue;

                for (Particle*& k : j->particles) {
                    if (!k)
                        continue;

                    k->data.texture = 0;
                    k->data.mask_texture = 0;

                    if (k->data.type == PARTICLE_LINE
                        || k->data.type == PARTICLE_MESH)
                        continue;

                    for (size_t l = 0; l < eff_group->resources_count; l++) {
                        if (k->data.tex_hash == eff_group->resource_hashes[l])
                            k->data.texture = eff_group->resources[l]->tex;
                        if (k->data.mask_tex_hash == eff_group->resource_hashes[l])
                            k->data.mask_texture = eff_group->resources[l]->tex;
                    }
                }
            }
        }
        return true;
    }

    bool FileReader::UnpackDivaResourceHashes(f2_struct* st, EffectGroup* eff_group) {
        if (eff_group->resources_count && eff_group->resource_hashes.size() > 0)
            return true;
        else if (!st || !st->header.data_size
            || st->header.signature != reverse_endianness_uint32_t('DVRS'))
            return false;

        size_t d = (size_t)st->data.data();
        if (!d)
            return false;

        uint32_t count;
        if (st->header.use_big_endian)
            count = load_reverse_endianness_uint32_t((void*)d);
        else
            count = *(int32_t*)d;
        d += 8;

        eff_group->resources_count = count;
        if (count) {
            eff_group->resource_hashes = std::vector<uint64_t>(count);
            uint64_t* resource_hashes = eff_group->resource_hashes.data();
            if (!resource_hashes)
                return false;

            if (st->header.use_big_endian)
                for (size_t i = count; i; i--, resource_hashes++, d += sizeof(uint64_t))
                    *resource_hashes = load_reverse_endianness_uint64_t((void*)d);
            else
                memcpy(resource_hashes, (void*)d, sizeof(uint64_t) * count);
        }
        return true;
    }

    bool FileReader::UnpackEffect(void* data, Effect* eff, int32_t efct_version, bool big_endian) {
        eff->version = efct_version;
        if (type == Glitter::X) {
            eff->scale = 1.0f;
            eff->data.start_time = 0;
            eff->data.ext_anim_x = 0;
            eff->data.flags = (EffectFlag)0;
            eff->data.name_hash = hash_murmurhash_empty;
            eff->data.seed = 0;

            if (eff->version < 8 || eff->version > 12)
                return false;

            size_t d = (size_t)data;
            EffectFileFlag flags;
            if (big_endian) {
                eff->data.name_hash = load_reverse_endianness_uint64_t((void*)d);
                eff->data.appear_time = load_reverse_endianness_int32_t((void*)(d + 8));
                eff->data.life_time = load_reverse_endianness_int32_t((void*)(d + 12));
                eff->data.start_time = load_reverse_endianness_int32_t((void*)(d + 16));
                eff->data.color.b = *(uint8_t*)(d + 20);
                eff->data.color.g = *(uint8_t*)(d + 21);
                eff->data.color.r = *(uint8_t*)(d + 22);
                eff->data.color.a = *(uint8_t*)(d + 23);
                if (load_reverse_endianness_int32_t((void*)(d + 24)))
                    enum_or(eff->data.flags, EFFECT_LOOP);
                eff->translation.x = load_reverse_endianness_float_t((void*)(d + 28));
                eff->translation.y = load_reverse_endianness_float_t((void*)(d + 32));
                eff->translation.z = load_reverse_endianness_float_t((void*)(d + 36));
                eff->rotation.x = load_reverse_endianness_float_t((void*)(d + 40));
                eff->rotation.y = load_reverse_endianness_float_t((void*)(d + 44));
                eff->rotation.z = load_reverse_endianness_float_t((void*)(d + 48));
                flags = (EffectFileFlag)load_reverse_endianness_int32_t((void*)(d + 52));
            }
            else {
                eff->data.name_hash = *(uint64_t*)data;
                eff->data.appear_time = *(int32_t*)(d + 8);
                eff->data.life_time = *(int32_t*)(d + 12);
                eff->data.start_time = *(int32_t*)(d + 16);
                eff->data.color.b = *(uint8_t*)(d + 20);
                eff->data.color.g = *(uint8_t*)(d + 21);
                eff->data.color.r = *(uint8_t*)(d + 22);
                eff->data.color.a = *(uint8_t*)(d + 23);
                if (*(int32_t*)(d + 24))
                    enum_or(eff->data.flags, EFFECT_LOOP);
                eff->translation = *(vec3*)(d + 28);
                eff->rotation = *(vec3*)(d + 40);
                flags = (EffectFileFlag) * (int32_t*)(d + 52);
            }
            d += 56;

            if (flags & EFFECT_FILE_ALPHA)
                enum_or(eff->data.flags, EFFECT_ALPHA);

            if (flags & EFFECT_FILE_FOG)
                enum_or(eff->data.flags, EFFECT_FOG);
            else if (flags & EFFECT_FILE_FOG_HEIGHT)
                enum_or(eff->data.flags, EFFECT_FOG_HEIGHT);

            if (flags & EFFECT_FILE_EMISSION)
                enum_or(eff->data.flags, EFFECT_EMISSION);

            if (flags & EFFECT_FILE_USE_SEED)
                enum_or(eff->data.flags, EFFECT_USE_SEED);

            if (eff->version != 8 && flags & 0x20)
                enum_or(eff->data.flags, 0x80);

            if (big_endian) {
                eff->data.emission = load_reverse_endianness_float_t((void*)d);
                eff->data.seed = load_reverse_endianness_int32_t((void*)(d + 4));
                eff->data.unk = load_reverse_endianness_float_t((void*)(d + 8));
            }
            else {
                eff->data.emission = *(float_t*)d;
                eff->data.seed = *(int32_t*)(d + 4);
                eff->data.unk = *(float_t*)(d + 8);
            }
            d += 12;

            if (eff->version != 8)
                d += 8;
            else
                d += 12;

            int32_t type;
            if (big_endian)
                type = load_reverse_endianness_int32_t((void*)d);
            else
                type = *(int32_t*)d;

            if (eff->version != 8)
                d += 4;
            else
                d += 8;

            if (type == 1)
                enum_or(eff->data.flags, EFFECT_LOCAL);
            else if (type == 2) {
                Effect::ExtAnimX* ext_anim_x = force_malloc<Effect::ExtAnimX>();
                eff->data.ext_anim_x = ext_anim_x;
                if (ext_anim_x) {
                    if (big_endian) {
                        ext_anim_x->chara_index = load_reverse_endianness_int32_t((void*)d);
                        ext_anim_x->flags = (EffectExtAnimFlag)
                            load_reverse_endianness_int32_t((void*)(d + 4));
                        ext_anim_x->node_index = (EffectExtAnimCharaNode)
                            load_reverse_endianness_int32_t((void*)(d + 8));
                    }
                    else {
                        ext_anim_x->chara_index = *(int32_t*)d;
                        ext_anim_x->flags = (EffectExtAnimFlag) * (int32_t*)(d + 4);
                        ext_anim_x->node_index = (EffectExtAnimCharaNode) * (int32_t*)(d + 8);
                    }

                    enum_or(ext_anim_x->flags, EFFECT_EXT_ANIM_CHARA_ANIM);
                }
            }
            else if (type == 3) {
                if (eff->version == 8) {
                    Effect::ExtAnimX* ext_anim_x = force_malloc<Effect::ExtAnimX>();
                    eff->data.ext_anim_x = ext_anim_x;
                    if (ext_anim_x) {
                        if (big_endian) {
                            ext_anim_x->object_hash = (uint32_t)
                                load_reverse_endianness_uint64_t((void*)d);
                            ext_anim_x->flags = (EffectExtAnimFlag)
                                load_reverse_endianness_int32_t((void*)(d + 8));
                        }
                        else {
                            ext_anim_x->object_hash = (uint32_t) * (uint64_t*)d;
                            ext_anim_x->flags = (EffectExtAnimFlag) * (int32_t*)(d + 8);
                        }

                        ext_anim_x->instance_id = 0;
                        ext_anim_x->file_name_hash = hash_murmurhash_empty;
                        if (*(char*)(d + 12)) {
                            strncpy_s(ext_anim_x->mesh_name, 0x80, (char*)(d + 12), 0x7F);
                            ext_anim_x->mesh_name[0x7F] = 0;
                        }
                        else
                            ext_anim_x->mesh_name[0] = 0;
                    }
                }
                else if (eff->version == 10) {
                    Effect::ExtAnimX* ext_anim_x = force_malloc<Effect::ExtAnimX>();
                    eff->data.ext_anim_x = ext_anim_x;
                    if (ext_anim_x) {
                        if (big_endian) {
                            ext_anim_x->object_hash = (uint32_t)
                                load_reverse_endianness_uint64_t((void*)d);
                            ext_anim_x->flags = (EffectExtAnimFlag)
                                load_reverse_endianness_int32_t((void*)(d + 8));
                        }
                        else {
                            ext_anim_x->object_hash = (uint32_t) * (uint64_t*)d;
                            ext_anim_x->flags = (EffectExtAnimFlag) * (int32_t*)(d + 8);
                        }

                        ext_anim_x->instance_id = 0;
                        ext_anim_x->file_name_hash = hash_murmurhash_empty;
                        if (*(char*)(d + 16)) {
                            strncpy_s(ext_anim_x->mesh_name, 0x80, (char*)(d + 16), 0x7F);
                            ext_anim_x->mesh_name[0x7F] = 0;
                        }
                        else
                            ext_anim_x->mesh_name[0] = 0;
                    }
                }
                else {
                    Effect::ExtAnimX* ext_anim_x = force_malloc<Effect::ExtAnimX>();
                    eff->data.ext_anim_x = ext_anim_x;
                    if (ext_anim_x) {
                        if (big_endian) {
                            ext_anim_x->object_hash = (uint32_t)
                                load_reverse_endianness_uint64_t((void*)d);
                            ext_anim_x->flags = (EffectExtAnimFlag)
                                load_reverse_endianness_int32_t((void*)(d + 8));
                            ext_anim_x->instance_id = load_reverse_endianness_int32_t((void*)(d + 12));
                            ext_anim_x->file_name_hash = (uint32_t)
                                load_reverse_endianness_uint64_t((void*)(d + 16));
                        }
                        else {
                            ext_anim_x->object_hash = (uint32_t) * (uint64_t*)d;
                            ext_anim_x->flags = (EffectExtAnimFlag) * (int32_t*)(d + 8);
                            ext_anim_x->instance_id = *(int32_t*)(d + 12);
                            ext_anim_x->file_name_hash = (uint32_t) * (uint64_t*)(d + 16);
                        }

                        if (*(char*)(d + 32)) {
                            strncpy_s(ext_anim_x->mesh_name, 0x80, (char*)(d + 32), 0x7F);
                            ext_anim_x->mesh_name[0x7F] = 0;
                        }
                        else
                            ext_anim_x->mesh_name[0] = 0;
                    }
                }
            }
        }
        else {
            eff->scale = 1.0f;
            eff->data.start_time = 0;
            eff->data.ext_anim = 0;
            eff->data.flags = (EffectFlag)0;
            eff->data.name_hash = type != Glitter::FT ? hash_murmurhash_empty : hash_fnv1a64m_empty;
            eff->data.seed = 0;

            if (eff->version < 6 || eff->version > 7)
                return false;

            size_t d = (size_t)data;
            EffectFileFlag flags;
            if (big_endian) {
                eff->data.name_hash = load_reverse_endianness_uint64_t((void*)d);
                eff->data.appear_time = load_reverse_endianness_int32_t((void*)(d + 8));
                eff->data.life_time = load_reverse_endianness_int32_t((void*)(d + 12));
                eff->data.start_time = load_reverse_endianness_int32_t((void*)(d + 16));
                eff->data.color.b = *(uint8_t*)(d + 20);
                eff->data.color.g = *(uint8_t*)(d + 21);
                eff->data.color.r = *(uint8_t*)(d + 22);
                eff->data.color.a = *(uint8_t*)(d + 23);
                if (load_reverse_endianness_int32_t((void*)(d + 24)))
                    enum_or(eff->data.flags, EFFECT_LOOP);
                eff->translation.x = load_reverse_endianness_float_t((void*)(d + 28));
                eff->translation.y = load_reverse_endianness_float_t((void*)(d + 32));
                eff->translation.z = load_reverse_endianness_float_t((void*)(d + 36));
                eff->rotation.x = load_reverse_endianness_float_t((void*)(d + 40));
                eff->rotation.y = load_reverse_endianness_float_t((void*)(d + 44));
                eff->rotation.z = load_reverse_endianness_float_t((void*)(d + 48));
                flags = (EffectFileFlag)load_reverse_endianness_int32_t((void*)(d + 52));
            }
            else {
                eff->data.name_hash = *(uint64_t*)data;
                eff->data.appear_time = *(int32_t*)(d + 8);
                eff->data.life_time = *(int32_t*)(d + 12);
                eff->data.start_time = *(int32_t*)(d + 16);
                eff->data.color.b = *(uint8_t*)(d + 20);
                eff->data.color.g = *(uint8_t*)(d + 21);
                eff->data.color.r = *(uint8_t*)(d + 22);
                eff->data.color.a = *(uint8_t*)(d + 23);
                if (*(int32_t*)(d + 24))
                    enum_or(eff->data.flags, EFFECT_LOOP);
                eff->translation = *(vec3*)(d + 28);
                eff->rotation = *(vec3*)(d + 40);
                flags = (EffectFileFlag) * (int32_t*)(d + 52);
            }
            d += 56;

            if (flags & EFFECT_FILE_ALPHA)
                enum_or(eff->data.flags, EFFECT_ALPHA);

            if (flags & EFFECT_FILE_FOG)
                enum_or(eff->data.flags, EFFECT_FOG);
            else if (flags & EFFECT_FILE_FOG_HEIGHT)
                enum_or(eff->data.flags, EFFECT_FOG_HEIGHT);

            if (flags & EFFECT_FILE_EMISSION)
                enum_or(eff->data.flags, EFFECT_EMISSION);

            if (eff->version == 7) {
                if (big_endian)
                    eff->data.emission = load_reverse_endianness_float_t((void*)d);
                else
                    eff->data.emission = *(float_t*)d;
                d += 4;
            }

            int32_t type;
            if (big_endian)
                type = load_reverse_endianness_int32_t((void*)d);
            else
                type = *(int32_t*)d;
            d += 4;

            if (type == 1)
                enum_or(eff->data.flags, EFFECT_LOCAL);
            else if (type == 2) {
                Effect::ExtAnim* ext_anim = force_malloc<Effect::ExtAnim>();
                eff->data.ext_anim = ext_anim;
                if (ext_anim) {
                    if (big_endian) {
                        ext_anim->chara_index = load_reverse_endianness_int32_t((void*)d);
                        ext_anim->flags = (EffectExtAnimFlag)
                            load_reverse_endianness_int32_t((void*)(d + 4));
                        ext_anim->node_index = (EffectExtAnimCharaNode)
                            load_reverse_endianness_int32_t((void*)(d + 8));
                    }
                    else {
                        ext_anim->chara_index = *(int32_t*)d;
                        ext_anim->flags = (EffectExtAnimFlag) * (int32_t*)(d + 4);
                        ext_anim->node_index = (EffectExtAnimCharaNode) * (int32_t*)(d + 8);
                    }

                    enum_or(ext_anim->flags, EFFECT_EXT_ANIM_CHARA_ANIM);
                }
            }
            else if (type == 3) {
                Effect::ExtAnim* ext_anim = force_malloc<Effect::ExtAnim>();
                eff->data.ext_anim = ext_anim;
                if (ext_anim) {
                    if (big_endian) {
                        ext_anim->object_hash = load_reverse_endianness_uint64_t((void*)d);
                        ext_anim->flags = (EffectExtAnimFlag)
                            load_reverse_endianness_int32_t((void*)(d + 8));
                    }
                    else {
                        ext_anim->object_hash = *(uint64_t*)d;
                        ext_anim->flags = (EffectExtAnimFlag) * (int32_t*)(d + 8);
                    }

                    ext_anim->object = obj_db->get_object_info_by_fnv1a64m_hash_upper(ext_anim->object_hash);
                    ext_anim->node_index = EFFECT_EXT_ANIM_CHARA_MAX;
                    if (*(char*)(d + 12)) {
                        strncpy_s(ext_anim->mesh_name, 0x80, (char*)(d + 12), 0x7F);
                        ext_anim->mesh_name[0x7F] = 0;
                    }
                    else
                        ext_anim->mesh_name[0x00] = 0;
                }
            }
        }
        return true;
    }

    bool FileReader::UnpackEmitter(void* data, Emitter* emit, uint32_t emit_version, bool big_endian) {
        emit->version = emit_version;
        if (type == Glitter::X) {
            size_t d = (size_t)data;
            if (big_endian) {
                emit->data.start_time = load_reverse_endianness_int32_t((void*)d);
                emit->data.life_time = load_reverse_endianness_int32_t((void*)(d + 4));
                emit->data.loop_start_time = load_reverse_endianness_int32_t((void*)(d + 8));
                emit->data.loop_end_time = load_reverse_endianness_int32_t((void*)(d + 12));
                emit->data.flags = (EmitterFlag)
                    load_reverse_endianness_int32_t((void*)(d + 16));
            }
            else {
                emit->data.start_time = *(int32_t*)d;
                emit->data.life_time = *(int32_t*)(d + 4);
                emit->data.loop_start_time = *(int32_t*)(d + 8);
                emit->data.loop_end_time = *(int32_t*)(d + 12);
                emit->data.flags = (EmitterFlag) * (int32_t*)(d + 16);
            }
            d += 20;

            if (emit->version < 3 || emit->version > 4)
                return false;

            if (big_endian) {
                emit->data.type = (EmitterType)
                    load_reverse_endianness_int16_t((void*)d);
                emit->data.direction = (Direction)
                    load_reverse_endianness_int16_t((void*)(d + 2));
                emit->data.emission_interval = load_reverse_endianness_float_t((void*)(d + 4));
                emit->data.particles_per_emission = load_reverse_endianness_float_t((void*)(d + 8));
                emit->data.timer = (EmitterTimerType)
                    load_reverse_endianness_int16_t((void*)(d + 12));
                emit->data.seed = load_reverse_endianness_int32_t((void*)(d + 16));
            }
            else {
                emit->data.type = (EmitterType) * (int16_t*)d;
                emit->data.direction = (Direction) * (int16_t*)(d + 2);
                emit->data.emission_interval = *(float_t*)(d + 4);
                emit->data.particles_per_emission = *(float_t*)(d + 8);
                emit->data.timer = (EmitterTimerType) * (int16_t*)(d + 12);
                emit->data.seed = *(int32_t*)(d + 16);
            }
            d += emit->version == 3 ? 36 : 32;

            if (big_endian) {
                emit->translation.x = load_reverse_endianness_float_t((void*)d);
                emit->translation.y = load_reverse_endianness_float_t((void*)(d + 4));
                emit->translation.z = load_reverse_endianness_float_t((void*)(d + 8));
                emit->rotation.x = load_reverse_endianness_float_t((void*)(d + 12));
                emit->rotation.y = load_reverse_endianness_float_t((void*)(d + 16));
                emit->rotation.z = load_reverse_endianness_float_t((void*)(d + 20));
                emit->scale.x = load_reverse_endianness_float_t((void*)(d + 24));
                emit->scale.y = load_reverse_endianness_float_t((void*)(d + 28));
                emit->scale.z = load_reverse_endianness_float_t((void*)(d + 32));
                emit->data.rotation_add.x = load_reverse_endianness_float_t((void*)(d + 36));
                emit->data.rotation_add.y = load_reverse_endianness_float_t((void*)(d + 40));
                emit->data.rotation_add.z = load_reverse_endianness_float_t((void*)(d + 44));
                emit->data.rotation_add_random.x = load_reverse_endianness_float_t((void*)(d + 48));
                emit->data.rotation_add_random.y = load_reverse_endianness_float_t((void*)(d + 52));
                emit->data.rotation_add_random.z = load_reverse_endianness_float_t((void*)(d + 56));
            }
            else {
                emit->translation = *(vec3*)d;
                emit->rotation = *(vec3*)(d + 12);
                emit->scale = *(vec3*)(d + 24);
                emit->data.rotation_add = *(vec3*)(d + 36);
                emit->data.rotation_add_random = *(vec3*)(d + 48);
            }
            d += 60;

            if (big_endian)
                switch (emit->data.type) {
                case EMITTER_BOX:
                    emit->data.box.size.x = load_reverse_endianness_float_t((void*)d);
                    emit->data.box.size.y = load_reverse_endianness_float_t((void*)(d + 4));
                    emit->data.box.size.z = load_reverse_endianness_float_t((void*)(d + 8));
                    return true;
                case EMITTER_CYLINDER:
                    emit->data.cylinder.radius = load_reverse_endianness_float_t((void*)d);
                    emit->data.cylinder.height = load_reverse_endianness_float_t((void*)(d + 4));
                    emit->data.cylinder.start_angle = load_reverse_endianness_float_t((void*)(d + 8));
                    emit->data.cylinder.end_angle = load_reverse_endianness_float_t((void*)(d + 12));
                    emit->data.cylinder.on_edge = load_reverse_endianness_int32_t((void*)(d + 16)) & 1 ? true : false;
                    emit->data.cylinder.direction = (EmitterEmissionDirection)
                        (load_reverse_endianness_int32_t((void*)(d + 16)) >> 1);
                    return true;
                case EMITTER_SPHERE:
                    emit->data.sphere.radius = load_reverse_endianness_float_t((void*)d);
                    emit->data.sphere.latitude = load_reverse_endianness_float_t((void*)(d + 4));
                    emit->data.sphere.longitude = load_reverse_endianness_float_t((void*)(d + 8));
                    emit->data.sphere.on_edge = load_reverse_endianness_int32_t((void*)(d + 12)) & 1 ? true : false;
                    emit->data.sphere.direction = (EmitterEmissionDirection)
                        (load_reverse_endianness_int32_t((void*)(d + 12)) >> 1);
                    return true;
                case EMITTER_POLYGON:
                    emit->data.polygon.size = load_reverse_endianness_float_t((void*)d);
                    emit->data.polygon.count = load_reverse_endianness_int32_t((void*)(d + 4));
                    emit->data.polygon.direction = (EmitterEmissionDirection)
                        (load_reverse_endianness_int32_t((void*)(d + 8)) >> 1);
                    return true;
                }
            else
                switch (emit->data.type) {
                case EMITTER_BOX:
                    emit->data.box.size = *(vec3*)d;
                    return true;
                case EMITTER_CYLINDER:
                    emit->data.cylinder.radius = *(float_t*)d;
                    emit->data.cylinder.height = *(float_t*)(d + 4);
                    emit->data.cylinder.start_angle = *(float_t*)(d + 8);
                    emit->data.cylinder.end_angle = *(float_t*)(d + 12);
                    emit->data.cylinder.on_edge = *(int32_t*)(d + 16) & 1 ? true : false;
                    emit->data.cylinder.direction = (EmitterEmissionDirection)
                        (*(int32_t*)(d + 16) >> 1);
                    return true;
                case EMITTER_SPHERE:
                    emit->data.sphere.radius = *(float_t*)d;
                    emit->data.sphere.latitude = *(float_t*)(d + 4);
                    emit->data.sphere.longitude = *(float_t*)(d + 8);
                    emit->data.sphere.on_edge = *(int32_t*)(d + 12) & 1 ? true : false;
                    emit->data.sphere.direction = (EmitterEmissionDirection)
                        (*(int32_t*)(d + 12) >> 1);
                    return true;
                case EMITTER_POLYGON:
                    emit->data.polygon.size = *(float_t*)d;
                    emit->data.polygon.count = *(int32_t*)(d + 4);
                    emit->data.polygon.direction = (EmitterEmissionDirection)
                        (*(int32_t*)(d + 8) >> 1);
                    return true;
                }
            return false;
        }
        else {
            size_t d = (size_t)data;
            if (big_endian) {
                emit->data.start_time = load_reverse_endianness_int32_t((void*)d);
                emit->data.life_time = load_reverse_endianness_int32_t((void*)(d + 4));
                emit->data.loop_start_time = load_reverse_endianness_int32_t((void*)(d + 8));
                emit->data.loop_end_time = load_reverse_endianness_int32_t((void*)(d + 12));
                emit->data.flags = (EmitterFlag)
                    load_reverse_endianness_int32_t((void*)(d + 16));
            }
            else {
                emit->data.start_time = *(int32_t*)d;
                emit->data.life_time = *(int32_t*)(d + 4);
                emit->data.loop_start_time = *(int32_t*)(d + 8);
                emit->data.loop_end_time = *(int32_t*)(d + 12);
                emit->data.flags = (EmitterFlag) * (int32_t*)(d + 16);
            }
            d += 20;

            emit->data.timer = EMITTER_TIMER_BY_TIME;
            emit->data.seed = 0;
            if (emit->version < 1 || emit->version > 2)
                return false;

            if (big_endian) {
                emit->data.type = (EmitterType)
                    load_reverse_endianness_int16_t((void*)d);
                emit->data.direction = (Direction)
                    load_reverse_endianness_int16_t((void*)(d + 2));
                emit->data.emission_interval = load_reverse_endianness_float_t((void*)(d + 4));
                emit->data.particles_per_emission = load_reverse_endianness_float_t((void*)(d + 8));
                emit->translation.x = load_reverse_endianness_float_t((void*)(d + 16));
                emit->translation.y = load_reverse_endianness_float_t((void*)(d + 20));
                emit->translation.z = load_reverse_endianness_float_t((void*)(d + 24));
                emit->rotation.x = load_reverse_endianness_float_t((void*)(d + 28));
                emit->rotation.y = load_reverse_endianness_float_t((void*)(d + 32));
                emit->rotation.z = load_reverse_endianness_float_t((void*)(d + 36));
                emit->scale.x = load_reverse_endianness_float_t((void*)(d + 40));
                emit->scale.y = load_reverse_endianness_float_t((void*)(d + 44));
                emit->scale.z = load_reverse_endianness_float_t((void*)(d + 48));
                emit->data.rotation_add.x = load_reverse_endianness_float_t((void*)(d + 52));
                emit->data.rotation_add.y = load_reverse_endianness_float_t((void*)(d + 56));
                emit->data.rotation_add.z = load_reverse_endianness_float_t((void*)(d + 60));
                emit->data.rotation_add_random.x = load_reverse_endianness_float_t((void*)(d + 64));
                emit->data.rotation_add_random.y = load_reverse_endianness_float_t((void*)(d + 68));
                emit->data.rotation_add_random.z = load_reverse_endianness_float_t((void*)(d + 72));
            }
            else {
                emit->data.type = (EmitterType) * (int16_t*)d;
                emit->data.direction = (Direction) * (int16_t*)(d + 2);
                emit->data.emission_interval = *(float_t*)(d + 4);
                emit->data.particles_per_emission = *(float_t*)(d + 8);
                emit->translation = *(vec3*)(d + 16);
                emit->rotation = *(vec3*)(d + 28);
                emit->scale = *(vec3*)(d + 40);
                emit->data.rotation_add = *(vec3*)(d + 52);
                emit->data.rotation_add_random = *(vec3*)(d + 64);
            }
            d += 76;

            if (big_endian)
                switch (emit->data.type) {
                case EMITTER_BOX:
                    emit->data.box.size.x = load_reverse_endianness_float_t((void*)d);
                    emit->data.box.size.y = load_reverse_endianness_float_t((void*)(d + 4));
                    emit->data.box.size.z = load_reverse_endianness_float_t((void*)(d + 8));
                    return true;
                case EMITTER_CYLINDER:
                    emit->data.cylinder.radius = load_reverse_endianness_float_t((void*)d);
                    emit->data.cylinder.height = load_reverse_endianness_float_t((void*)(d + 4));
                    emit->data.cylinder.start_angle = load_reverse_endianness_float_t((void*)(d + 8));
                    emit->data.cylinder.end_angle = load_reverse_endianness_float_t((void*)(d + 12));
                    emit->data.cylinder.on_edge = load_reverse_endianness_int32_t((void*)(d + 16)) & 1 ? true : false;
                    emit->data.cylinder.direction = (EmitterEmissionDirection)
                        (load_reverse_endianness_int32_t((void*)(d + 16)) >> 1);
                    return true;
                case EMITTER_SPHERE:
                    emit->data.sphere.radius = load_reverse_endianness_float_t((void*)d);
                    emit->data.sphere.latitude = load_reverse_endianness_float_t((void*)(d + 4));
                    emit->data.sphere.longitude = load_reverse_endianness_float_t((void*)(d + 8));
                    emit->data.sphere.on_edge = load_reverse_endianness_int32_t((void*)(d + 12)) & 1 ? true : false;
                    emit->data.sphere.direction = (EmitterEmissionDirection)
                        (load_reverse_endianness_int32_t((void*)(d + 12)) >> 1);
                    return true;
                case EMITTER_POLYGON:
                    emit->data.polygon.size = load_reverse_endianness_float_t((void*)d);
                    emit->data.polygon.count = load_reverse_endianness_int32_t((void*)(d + 4));
                    return true;
                }
            else
                switch (emit->data.type) {
                case EMITTER_BOX:
                    emit->data.box.size = *(vec3*)d;
                    return true;
                case EMITTER_CYLINDER:
                    emit->data.cylinder.radius = *(float_t*)d;
                    emit->data.cylinder.height = *(float_t*)(d + 4);
                    emit->data.cylinder.start_angle = *(float_t*)(d + 8);
                    emit->data.cylinder.end_angle = *(float_t*)(d + 12);
                    emit->data.cylinder.on_edge = *(int32_t*)(d + 16) & 1 ? true : false;
                    emit->data.cylinder.direction = (EmitterEmissionDirection)(*(int32_t*)(d + 16) >> 1);
                    return true;
                case EMITTER_SPHERE:
                    emit->data.sphere.radius = *(float_t*)d;
                    emit->data.sphere.latitude = *(float_t*)(d + 4);
                    emit->data.sphere.longitude = *(float_t*)(d + 8);
                    emit->data.sphere.on_edge = *(int32_t*)(d + 12) & 1 ? true : false;
                    emit->data.sphere.direction = (EmitterEmissionDirection)(*(int32_t*)(d + 12) >> 1);
                    return true;
                case EMITTER_POLYGON:
                    emit->data.polygon.size = *(float_t*)d;
                    emit->data.polygon.count = *(int32_t*)(d + 4);
                    return true;
                }
            return false;
        }
    }

    bool FileReader::UnpackParticle(void* data, Particle* ptcl,
        uint32_t ptcl_version, Effect* eff, bool big_endian, EffectGroup* eff_group) {
        ptcl->version = ptcl_version;

        uint8_t r;
        uint8_t b;
        uint8_t g;
        uint8_t a;
        ParticleBlend blend_mode;
        ParticleBlend mask_blend_mode;
        uint64_t tex_hash;
        uint64_t mask_tex_hash;
        int32_t frame_step_uv;
        int32_t uv_index_start;
        int32_t uv_index;
        int32_t uv_index_end;
        UVIndexType uv_index_type;
        uint8_t split_u;
        uint8_t split_v;
        int32_t unk2;
        int32_t unk3;
        int32_t unk4;

        if (type == Glitter::X) {
            ptcl->data.mesh.object_set_name_hash = hash_murmurhash_empty;
            ptcl->data.mesh.object_name_hash = hash_murmurhash_empty;
            //ptcl->data.mesh.mesh_name[0] = 0;
            //ptcl->data.mesh.sub_mesh_hash = hash_murmurhash_empty;

            size_t d = (size_t)data;
            if (big_endian) {
                ptcl->data.life_time = load_reverse_endianness_int32_t((void*)d);
                ptcl->data.life_time_random = load_reverse_endianness_int32_t((void*)(d + 4));
                ptcl->data.fade_in = load_reverse_endianness_int32_t((void*)(d + 8));
                ptcl->data.fade_in_random = load_reverse_endianness_int32_t((void*)(d + 12));
                ptcl->data.fade_out = load_reverse_endianness_int32_t((void*)(d + 16));
                ptcl->data.fade_out_random = load_reverse_endianness_int32_t((void*)(d + 20));
                ptcl->data.type = (ParticleType)load_reverse_endianness_int32_t((void*)(d + 24));
                ptcl->data.draw_type = (Direction)load_reverse_endianness_int32_t((void*)(d + 28));
            }
            else {
                ptcl->data.life_time = *(int32_t*)d;
                ptcl->data.life_time_random = *(int32_t*)(d + 4);
                ptcl->data.fade_in = *(int32_t*)(d + 8);
                ptcl->data.fade_in_random = *(int32_t*)(d + 12);
                ptcl->data.fade_out = *(int32_t*)(d + 16);
                ptcl->data.fade_out_random = *(int32_t*)(d + 20);
                ptcl->data.type = (ParticleType) * (int32_t*)(d + 24);
                ptcl->data.draw_type = (Direction) * (int32_t*)(d + 28);
            }

            ptcl->data.unk0 = 0;
            ptcl->data.unk1 = -1.0f;

            if (ptcl->version < 4 || ptcl->version > 5)
                return false;

            if (big_endian) {
                ptcl->data.rotation.x = load_reverse_endianness_float_t((void*)(d + 32));
                ptcl->data.rotation.y = load_reverse_endianness_float_t((void*)(d + 36));
                ptcl->data.rotation.z = load_reverse_endianness_float_t((void*)(d + 40));
                ptcl->data.rotation_random.x = load_reverse_endianness_float_t((void*)(d + 44));
                ptcl->data.rotation_random.y = load_reverse_endianness_float_t((void*)(d + 48));
                ptcl->data.rotation_random.z = load_reverse_endianness_float_t((void*)(d + 52));
                ptcl->data.rotation_add.x = load_reverse_endianness_float_t((void*)(d + 56));
                ptcl->data.rotation_add.y = load_reverse_endianness_float_t((void*)(d + 60));
                ptcl->data.rotation_add.z = load_reverse_endianness_float_t((void*)(d + 64));
                ptcl->data.rotation_add_random.x = load_reverse_endianness_float_t((void*)(d + 68));
                ptcl->data.rotation_add_random.y = load_reverse_endianness_float_t((void*)(d + 72));
                ptcl->data.rotation_add_random.z = load_reverse_endianness_float_t((void*)(d + 76));
                ptcl->data.scale.x = load_reverse_endianness_float_t((void*)(d + 80));
                ptcl->data.scale.y = load_reverse_endianness_float_t((void*)(d + 84));
                ptcl->data.scale.z = load_reverse_endianness_float_t((void*)(d + 88));
                ptcl->data.scale_random.x = load_reverse_endianness_float_t((void*)(d + 92));
                ptcl->data.scale_random.y = load_reverse_endianness_float_t((void*)(d + 96));
                ptcl->data.scale_random.z = load_reverse_endianness_float_t((void*)(d + 100));
                ptcl->data.z_offset = load_reverse_endianness_float_t((void*)(d + 104));
                ptcl->data.pivot = (Pivot)load_reverse_endianness_int32_t((void*)(d + 108));
                ptcl->data.flags = (ParticleFlag)load_reverse_endianness_int32_t((void*)(d + 112));
                ptcl->data.unk0 = load_reverse_endianness_int32_t((void*)(d + 116));
            }
            else {
                ptcl->data.rotation = *(vec3*)(d + 32);
                ptcl->data.rotation_random = *(vec3*)(d + 44);
                ptcl->data.rotation_add = *(vec3*)(d + 56);
                ptcl->data.rotation_add_random = *(vec3*)(d + 68);
                ptcl->data.scale = *(vec3*)(d + 80);
                ptcl->data.scale_random = *(vec3*)(d + 92);
                ptcl->data.z_offset = *(float_t*)(d + 104);
                ptcl->data.pivot = (Pivot) * (int32_t*)(d + 108);
                ptcl->data.flags = (ParticleFlag) * (int32_t*)(d + 112);
                ptcl->data.unk0 = *(int32_t*)(d + 116);
            }

            if (eff->data.flags & EFFECT_LOCAL)
                enum_or(ptcl->data.flags, PARTICLE_LOCAL);
            if (eff->data.flags & EFFECT_EMISSION)
                enum_or(ptcl->data.flags, PARTICLE_EMISSION);

            if (big_endian) {
                ptcl->data.uv_scroll_2nd_add.x = load_reverse_endianness_float_t((void*)(d + 120));
                ptcl->data.uv_scroll_2nd_add.y = load_reverse_endianness_float_t((void*)(d + 124));
                ptcl->data.uv_scroll_2nd_add_scale = load_reverse_endianness_float_t((void*)(d + 128));
                ptcl->data.speed = load_reverse_endianness_float_t((void*)(d + 136));
                ptcl->data.speed_random = load_reverse_endianness_float_t((void*)(d + 140));
                ptcl->data.deceleration = load_reverse_endianness_float_t((void*)(d + 144));
                ptcl->data.deceleration_random = load_reverse_endianness_float_t((void*)(d + 148));
                ptcl->data.direction.x = load_reverse_endianness_float_t((void*)(d + 152));
                ptcl->data.direction.y = load_reverse_endianness_float_t((void*)(d + 156));
                ptcl->data.direction.z = load_reverse_endianness_float_t((void*)(d + 160));
                ptcl->data.direction_random.x = load_reverse_endianness_float_t((void*)(d + 164));
                ptcl->data.direction_random.y = load_reverse_endianness_float_t((void*)(d + 168));
                ptcl->data.direction_random.z = load_reverse_endianness_float_t((void*)(d + 172));
                ptcl->data.gravity.x = load_reverse_endianness_float_t((void*)(d + 176));
                ptcl->data.gravity.y = load_reverse_endianness_float_t((void*)(d + 180));
                ptcl->data.gravity.z = load_reverse_endianness_float_t((void*)(d + 184));
                ptcl->data.acceleration.x = load_reverse_endianness_float_t((void*)(d + 188));
                ptcl->data.acceleration.y = load_reverse_endianness_float_t((void*)(d + 192));
                ptcl->data.acceleration.z = load_reverse_endianness_float_t((void*)(d + 196));
                ptcl->data.acceleration_random.x = load_reverse_endianness_float_t((void*)(d + 200));
                ptcl->data.acceleration_random.y = load_reverse_endianness_float_t((void*)(d + 204));
                ptcl->data.acceleration_random.z = load_reverse_endianness_float_t((void*)(d + 208));
                ptcl->data.reflection_coeff = load_reverse_endianness_float_t((void*)(d + 212));
                ptcl->data.reflection_coeff_random = load_reverse_endianness_float_t((void*)(d + 216));
                ptcl->data.rebound_plane_y = load_reverse_endianness_float_t((void*)(d + 220));
                ptcl->data.uv_scroll_add.x = load_reverse_endianness_float_t((void*)(d + 224));
                ptcl->data.uv_scroll_add.y = load_reverse_endianness_float_t((void*)(d + 228));
                ptcl->data.uv_scroll_add_scale = load_reverse_endianness_float_t((void*)(d + 232));
                ptcl->data.sub_flags = (ParticleSubFlag)
                    load_reverse_endianness_int32_t((void*)(d + 236));
                ptcl->data.count = load_reverse_endianness_int32_t((void*)(d + 240));
                ptcl->data.draw_flags = (ParticleDrawFlag)
                    load_reverse_endianness_int32_t((void*)(d + 244));
                ptcl->data.unk1 = load_reverse_endianness_float_t((void*)(d + 248));
                ptcl->data.emission = load_reverse_endianness_float_t((void*)(d + 252));
            }
            else {
                ptcl->data.uv_scroll_2nd_add = *(vec2*)(d + 120);
                ptcl->data.uv_scroll_2nd_add_scale = *(float_t*)(d + 128);
                ptcl->data.speed = *(float_t*)(d + 136);
                ptcl->data.speed_random = *(float_t*)(d + 140);
                ptcl->data.deceleration = *(float_t*)(d + 144);
                ptcl->data.deceleration_random = *(float_t*)(d + 148);
                ptcl->data.direction = *(vec3*)(d + 152);
                ptcl->data.direction_random = *(vec3*)(d + 164);
                ptcl->data.gravity = *(vec3*)(d + 176);
                ptcl->data.acceleration = *(vec3*)(d + 188);
                ptcl->data.acceleration_random = *(vec3*)(d + 200);
                ptcl->data.reflection_coeff = *(float_t*)(d + 212);
                ptcl->data.reflection_coeff_random = *(float_t*)(d + 216);
                ptcl->data.rebound_plane_y = *(float_t*)(d + 220);
                ptcl->data.uv_scroll_add = *(vec2*)(d + 224);
                ptcl->data.uv_scroll_add_scale = *(float_t*)(d + 232);
                ptcl->data.sub_flags = (ParticleSubFlag) * (int32_t*)(d + 236);
                ptcl->data.count = *(int32_t*)(d + 240);
                ptcl->data.draw_flags = (ParticleDrawFlag) * (int32_t*)(d + 244);
                ptcl->data.unk1 = *(float_t*)(d + 248);
                ptcl->data.emission = *(float_t*)(d + 252);
            }
            d += 256;

            if (ptcl->version == 4) {
                ptcl->data.direction *= 10.0f;
                ptcl->data.direction_random *= 10.0f;
            }

            if (ptcl->data.emission >= min_emission)
                enum_or(ptcl->data.flags, PARTICLE_EMISSION);

            ptcl->data.locus_history_size = 0;
            ptcl->data.locus_history_size_random = 0;

            switch (ptcl->data.type) {
            case 3:
                ptcl->data.type = PARTICLE_LINE;
                break;
            case 1:
                ptcl->data.type = PARTICLE_MESH;
                break;
            }

            bool has_tex;
            if (ptcl->data.type == PARTICLE_QUAD)
                has_tex = true;
            else if (ptcl->data.type == PARTICLE_LINE)
                has_tex = false;
            else if (ptcl->data.type == PARTICLE_LOCUS) {
                if (big_endian) {
                    ptcl->data.locus_history_size = load_reverse_endianness_uint16_t((void*)(d + 0));
                    ptcl->data.locus_history_size_random = load_reverse_endianness_uint16_t((void*)(d + 2));
                }
                else {
                    ptcl->data.locus_history_size = *(uint16_t*)(d + 0);
                    ptcl->data.locus_history_size_random = *(uint16_t*)(d + 2);
                }
                d += 4;
                has_tex = true;
            }
            else if (ptcl->data.type == PARTICLE_MESH) {
                if (big_endian) {
                    ptcl->data.mesh.object_name_hash = load_reverse_endianness_uint64_t((void*)d);
                    ptcl->data.mesh.object_set_name_hash = load_reverse_endianness_uint64_t((void*)(d + 8));
                }
                else {
                    ptcl->data.mesh.object_name_hash = *(uint64_t*)d;
                    ptcl->data.mesh.object_set_name_hash = *(uint64_t*)(d + 8);
                }
                //if (*(char*)(d + 10)) {
                //    strncpy_s(ptcl->data.mesh.mesh_name, 0x40, (char*)(d + 16), 0x3F);
                //    ptcl->data.mesh.mesh_name[0x3F] = 0;
                //}
                //else
                //    ptcl->data.mesh.mesh_name[0] = 0;
                //if (big_endian)
                //    ptcl->data.mesh.sub_mesh_hash = load_reverse_endianness_uint64_t((void*)(d + 80));
                //else
                //    ptcl->data.mesh.sub_mesh_hash = *(uint64_t*)(d + 80);
                d += 88;
                has_tex = false;

                eff_group->meshes.push_back((uint32_t)ptcl->data.mesh.object_set_name_hash);
            }
            else
                return false;

            ptcl->data.texture = 0;
            ptcl->data.mask_texture = 0;

            tex_hash = hash_murmurhash_empty;
            mask_tex_hash = hash_murmurhash_empty;
            unk2 = 0;
            unk3 = 0;

            if (big_endian) {
                if (has_tex)
                    tex_hash = load_reverse_endianness_uint64_t((void*)d);
                r = *(uint8_t*)(d + 8);
                g = *(uint8_t*)(d + 9);
                b = *(uint8_t*)(d + 10);
                a = *(uint8_t*)(d + 11);
                blend_mode = (ParticleBlend)load_reverse_endianness_int32_t((void*)(d + 12));
                unk2 = load_reverse_endianness_int32_t((void*)(d + 16));
                split_u = (uint8_t)load_reverse_endianness_int32_t((void*)(d + 20));
                split_v = (uint8_t)load_reverse_endianness_int32_t((void*)(d + 24));
                uv_index_type = (UVIndexType)load_reverse_endianness_int32_t((void*)(d + 28));
                uv_index = load_reverse_endianness_int16_t((void*)(d + 32));
                frame_step_uv = load_reverse_endianness_int16_t((void*)(d + 34));
                uv_index_start = load_reverse_endianness_int32_t((void*)(d + 36));
                uv_index_end = load_reverse_endianness_int32_t((void*)(d + 40));
                unk3 = load_reverse_endianness_int32_t((void*)(d + 44));
            }
            else {
                if (has_tex)
                    tex_hash = *(uint64_t*)d;
                r = *(uint8_t*)(d + 8);
                g = *(uint8_t*)(d + 9);
                b = *(uint8_t*)(d + 10);
                a = *(uint8_t*)(d + 11);
                blend_mode = (ParticleBlend) * (int32_t*)(d + 12);
                unk2 = *(int32_t*)(d + 16);
                split_u = (uint8_t) * (int32_t*)(d + 20);
                split_v = (uint8_t) * (int32_t*)(d + 24);
                uv_index_type = (UVIndexType) * (int32_t*)(d + 28);
                uv_index = *(int16_t*)(d + 32);
                frame_step_uv = *(int16_t*)(d + 34);
                uv_index_start = *(int32_t*)(d + 36);
                uv_index_end = *(int32_t*)(d + 40);
                unk3 = *(int32_t*)(d + 44);
            }
            d += 48;

            if (ptcl->data.flags & PARTICLE_TEXTURE_MASK)
                if (big_endian) {
                    if (has_tex)
                        mask_tex_hash = load_reverse_endianness_uint64_t((void*)d);
                    mask_blend_mode = (ParticleBlend)
                        load_reverse_endianness_int32_t((void*)(d + 8));
                    unk4 = load_reverse_endianness_int32_t((void*)(d + 12));
                }
                else {
                    if (has_tex)
                        mask_tex_hash = *(uint64_t*)d;
                    mask_blend_mode = (ParticleBlend) * (int32_t*)(d + 8);
                    unk4 = *(int32_t*)(d + 12);
                }
            else {
                mask_blend_mode = PARTICLE_BLEND_TYPICAL;
                unk4 = 0xFF;
            }
        }
        else {
            size_t d = (size_t)data;
            if (big_endian) {
                ptcl->data.life_time = load_reverse_endianness_int32_t((void*)d);
                ptcl->data.type = (ParticleType)load_reverse_endianness_int32_t((void*)(d + 4));
                ptcl->data.draw_type = (Direction)load_reverse_endianness_int32_t((void*)(d + 8));
            }
            else {
                ptcl->data.life_time = *(int32_t*)d;
                ptcl->data.type = (ParticleType) * (int32_t*)(d + 4);
                ptcl->data.draw_type = (Direction) * (int32_t*)(d + 8);
            }

            ptcl->data.unk0 = 0;
            ptcl->data.unk1 = -1.0f;

            if (ptcl->version < 2 || ptcl->version > 3)
                return false;

            if (big_endian) {
                ptcl->data.rotation.x = load_reverse_endianness_float_t((void*)(d + 12));
                ptcl->data.rotation.y = load_reverse_endianness_float_t((void*)(d + 16));
                ptcl->data.rotation.z = load_reverse_endianness_float_t((void*)(d + 20));
                ptcl->data.rotation_random.x = load_reverse_endianness_float_t((void*)(d + 24));
                ptcl->data.rotation_random.y = load_reverse_endianness_float_t((void*)(d + 28));
                ptcl->data.rotation_random.z = load_reverse_endianness_float_t((void*)(d + 32));
                ptcl->data.rotation_add.x = load_reverse_endianness_float_t((void*)(d + 36));
                ptcl->data.rotation_add.y = load_reverse_endianness_float_t((void*)(d + 40));
                ptcl->data.rotation_add.z = load_reverse_endianness_float_t((void*)(d + 44));
                ptcl->data.rotation_add_random.x = load_reverse_endianness_float_t((void*)(d + 48));
                ptcl->data.rotation_add_random.y = load_reverse_endianness_float_t((void*)(d + 52));
                ptcl->data.rotation_add_random.z = load_reverse_endianness_float_t((void*)(d + 56));
                ptcl->data.scale.x = load_reverse_endianness_float_t((void*)(d + 60));
                ptcl->data.scale.y = load_reverse_endianness_float_t((void*)(d + 64));
                ptcl->data.scale.z = load_reverse_endianness_float_t((void*)(d + 68));
                ptcl->data.scale_random.x = load_reverse_endianness_float_t((void*)(d + 72));
                ptcl->data.scale_random.y = load_reverse_endianness_float_t((void*)(d + 76));
                ptcl->data.scale_random.z = load_reverse_endianness_float_t((void*)(d + 80));
                ptcl->data.z_offset = load_reverse_endianness_float_t((void*)(d + 84));
                ptcl->data.pivot = (Pivot)load_reverse_endianness_int32_t((void*)(d + 88));
                ptcl->data.flags = (ParticleFlag)load_reverse_endianness_int32_t((void*)(d + 92));
            }
            else {
                ptcl->data.rotation = *(vec3*)(d + 12);
                ptcl->data.rotation_random = *(vec3*)(d + 24);
                ptcl->data.rotation_add = *(vec3*)(d + 36);
                ptcl->data.rotation_add_random = *(vec3*)(d + 48);
                ptcl->data.scale = *(vec3*)(d + 60);
                ptcl->data.scale_random = *(vec3*)(d + 72);
                ptcl->data.z_offset = *(float_t*)(d + 84);
                ptcl->data.pivot = (Pivot) * (int32_t*)(d + 88);
                ptcl->data.flags = (ParticleFlag) * (int32_t*)(d + 92);
            }

            if (eff->data.flags & EFFECT_LOCAL)
                enum_or(ptcl->data.flags, PARTICLE_LOCAL);
            if (eff->data.flags & EFFECT_EMISSION)
                enum_or(ptcl->data.flags, PARTICLE_EMISSION);

            if (big_endian) {
                ptcl->data.speed = load_reverse_endianness_float_t((void*)(d + 96));
                ptcl->data.speed_random = load_reverse_endianness_float_t((void*)(d + 100));
                ptcl->data.deceleration = load_reverse_endianness_float_t((void*)(d + 104));
                ptcl->data.deceleration_random = load_reverse_endianness_float_t((void*)(d + 108));
                ptcl->data.direction.x = load_reverse_endianness_float_t((void*)(d + 112));
                ptcl->data.direction.y = load_reverse_endianness_float_t((void*)(d + 116));
                ptcl->data.direction.z = load_reverse_endianness_float_t((void*)(d + 120));
                ptcl->data.direction_random.x = load_reverse_endianness_float_t((void*)(d + 124));
                ptcl->data.direction_random.y = load_reverse_endianness_float_t((void*)(d + 128));
                ptcl->data.direction_random.z = load_reverse_endianness_float_t((void*)(d + 132));
                ptcl->data.gravity.x = load_reverse_endianness_float_t((void*)(d + 136));
                ptcl->data.gravity.y = load_reverse_endianness_float_t((void*)(d + 140));
                ptcl->data.gravity.z = load_reverse_endianness_float_t((void*)(d + 144));
                ptcl->data.acceleration.x = load_reverse_endianness_float_t((void*)(d + 148));
                ptcl->data.acceleration.y = load_reverse_endianness_float_t((void*)(d + 152));
                ptcl->data.acceleration.z = load_reverse_endianness_float_t((void*)(d + 156));
                ptcl->data.acceleration_random.x = load_reverse_endianness_float_t((void*)(d + 160));
                ptcl->data.acceleration_random.y = load_reverse_endianness_float_t((void*)(d + 164));
                ptcl->data.acceleration_random.z = load_reverse_endianness_float_t((void*)(d + 168));
                ptcl->data.reflection_coeff = load_reverse_endianness_float_t((void*)(d + 172));
                ptcl->data.reflection_coeff_random = load_reverse_endianness_float_t((void*)(d + 176));
                ptcl->data.rebound_plane_y = load_reverse_endianness_float_t((void*)(d + 180));
                ptcl->data.uv_scroll_add.x = load_reverse_endianness_float_t((void*)(d + 184));
                ptcl->data.uv_scroll_add.y = load_reverse_endianness_float_t((void*)(d + 188));
                ptcl->data.uv_scroll_add_scale = load_reverse_endianness_float_t((void*)(d + 192));
                ptcl->data.sub_flags = (ParticleSubFlag)
                    load_reverse_endianness_int32_t((void*)(d + 196));
                ptcl->data.count = load_reverse_endianness_int32_t((void*)(d + 200));
            }
            else {
                ptcl->data.speed = *(float_t*)(d + 96);
                ptcl->data.speed_random = *(float_t*)(d + 100);
                ptcl->data.deceleration = *(float_t*)(d + 104);
                ptcl->data.deceleration_random = *(float_t*)(d + 108);
                ptcl->data.direction = *(vec3*)(d + 112);
                ptcl->data.direction_random = *(vec3*)(d + 124);
                ptcl->data.gravity = *(vec3*)(d + 136);
                ptcl->data.acceleration = *(vec3*)(d + 148);
                ptcl->data.acceleration_random = *(vec3*)(d + 160);
                ptcl->data.reflection_coeff = *(float_t*)(d + 172);
                ptcl->data.reflection_coeff_random = *(float_t*)(d + 176);
                ptcl->data.rebound_plane_y = *(float_t*)(d + 180);
                ptcl->data.uv_scroll_add = *(vec2*)(d + 184);
                ptcl->data.uv_scroll_add_scale = *(float_t*)(d + 192);
                ptcl->data.sub_flags = (ParticleSubFlag) * (int32_t*)(d + 196);
                ptcl->data.count = *(int32_t*)(d + 200);
            }
            d += 204;

            ptcl->data.locus_history_size = 0;
            ptcl->data.locus_history_size_random = 0;
            ptcl->data.draw_flags = (ParticleDrawFlag)0;
            ptcl->data.emission = 0.0f;

            if (ptcl->version == 3) {
                if (big_endian) {
                    ptcl->data.unk1 = load_reverse_endianness_float_t((void*)d);
                    ptcl->data.emission = load_reverse_endianness_float_t((void*)(d + 4));
                }
                else {
                    ptcl->data.unk1 = *(float_t*)d;
                    ptcl->data.emission = *(float_t*)(d + 4);
                }

                if (ptcl->data.emission >= min_emission)
                    enum_or(ptcl->data.flags, PARTICLE_EMISSION);
                d += 8;
            }

            if (ptcl->data.type == PARTICLE_LOCUS || ptcl->data.type == PARTICLE_MESH) {
                if (big_endian) {
                    ptcl->data.locus_history_size = load_reverse_endianness_uint16_t((void*)d);
                    ptcl->data.locus_history_size_random = load_reverse_endianness_uint16_t((void*)(d + 2));
                }
                else {
                    ptcl->data.locus_history_size = *(uint16_t*)d;
                    ptcl->data.locus_history_size_random = *(uint16_t*)(d + 2);
                }
                d += 4;
            }

            ptcl->data.texture = 0;
            ptcl->data.mask_texture = 0;

            unk2 = 0;
            unk3 = 0;

            if (big_endian) {
                tex_hash = load_reverse_endianness_uint64_t((void*)d);
                r = *(uint8_t*)(d + 8);
                g = *(uint8_t*)(d + 9);
                b = *(uint8_t*)(d + 10);
                a = *(uint8_t*)(d + 11);
                blend_mode = (ParticleBlend)load_reverse_endianness_int32_t((void*)(d + 12));
                unk2 = load_reverse_endianness_int32_t((void*)(d + 16));
                split_u = (uint8_t)load_reverse_endianness_int32_t((void*)(d + 20));
                split_v = (uint8_t)load_reverse_endianness_int32_t((void*)(d + 24));
                uv_index_type = (UVIndexType)load_reverse_endianness_int32_t((void*)(d + 28));
                uv_index = load_reverse_endianness_int16_t((void*)(d + 32));
                frame_step_uv = load_reverse_endianness_int16_t((void*)(d + 34));
                uv_index_start = load_reverse_endianness_int32_t((void*)(d + 36));
                uv_index_end = load_reverse_endianness_int32_t((void*)(d + 40));
            }
            else {
                tex_hash = *(uint64_t*)d;
                r = *(uint8_t*)(d + 8);
                g = *(uint8_t*)(d + 9);
                b = *(uint8_t*)(d + 10);
                a = *(uint8_t*)(d + 11);
                blend_mode = (ParticleBlend) * (int32_t*)(d + 12);
                unk2 = *(int32_t*)(d + 16);
                split_u = (uint8_t) * (int32_t*)(d + 20);
                split_v = (uint8_t) * (int32_t*)(d + 24);
                uv_index_type = (UVIndexType) * (int32_t*)(d + 28);
                uv_index = *(int16_t*)(d + 32);
                frame_step_uv = *(int16_t*)(d + 34);
                uv_index_start = *(int32_t*)(d + 36);
                uv_index_end = *(int32_t*)(d + 40);
            }
            d += 44;

            if (eff_group->version >= 7) {
                if (big_endian)
                    unk3 = load_reverse_endianness_int32_t((void*)d);
                else
                    unk3 = *(int32_t*)d;
                d += 4;
            }

            if (ptcl->data.flags & PARTICLE_TEXTURE_MASK)
                if (big_endian) {
                    mask_tex_hash = load_reverse_endianness_uint64_t((void*)d);
                    mask_blend_mode = (ParticleBlend)
                        load_reverse_endianness_int32_t((void*)(d + 8));
                    unk4 = load_reverse_endianness_int32_t((void*)(d + 12));
                }
                else {
                    mask_tex_hash = *(uint64_t*)d;
                    mask_blend_mode = (ParticleBlend) * (int32_t*)(d + 8);
                    unk4 = *(int32_t*)(d + 12);
                }
            else {
                mask_tex_hash = type != Glitter::FT
                    ? hash_murmurhash_empty : hash_fnv1a64m_empty;
                mask_blend_mode = PARTICLE_BLEND_TYPICAL;
                unk4 = 0xFF;
            }
        }

        ptcl->data.tex_hash = tex_hash;
        ptcl->data.mask_tex_hash = mask_tex_hash;
        ptcl->data.color.x = (float_t)r;
        ptcl->data.color.y = (float_t)g;
        ptcl->data.color.z = (float_t)b;
        ptcl->data.color.w = (float_t)a;
        ptcl->data.blend_mode = blend_mode;
        ptcl->data.mask_blend_mode = mask_blend_mode;
        ptcl->data.split_u = split_u;
        ptcl->data.split_v = split_v;
        ptcl->data.uv_index_type = uv_index_type;
        ptcl->data.uv_index = max_def(uv_index, 0);
        ptcl->data.frame_step_uv = frame_step_uv;
        ptcl->data.uv_index_start = max_def(uv_index_start, 0);
        ptcl->data.uv_index_end = uv_index_end;
        ptcl->data.split_uv.x = (float_t)split_u;
        ptcl->data.split_uv.y = (float_t)split_v;
        ptcl->data.unk2 = unk2;
        ptcl->data.unk3 = unk3;
        ptcl->data.unk4 = unk4;

        ptcl->data.split_uv = vec2::rcp(ptcl->data.split_uv);
        ptcl->data.color *= (float_t)(1.0 / 255.0);

        int32_t uv_max_count = (int32_t)(split_u * split_v);
        if (uv_max_count)
            uv_max_count--;

        switch (uv_index_type) {
        case UV_INDEX_FIXED:
        case UV_INDEX_RANDOM:
        case UV_INDEX_FORWARD:
        case UV_INDEX_REVERSE:
        case UV_INDEX_USER:
            ptcl->data.uv_index &= uv_max_count;
            break;
        case UV_INDEX_INITIAL_RANDOM_FIXED:
        case UV_INDEX_INITIAL_RANDOM_FORWARD:
        case UV_INDEX_INITIAL_RANDOM_REVERSE:
            if (uv_index < uv_index_start)
                ptcl->data.uv_index = uv_index_start;
            else if (uv_index_end >= 0 && uv_index > uv_index_end)
                ptcl->data.uv_index = uv_index_end;

            ptcl->data.uv_index = min_def(ptcl->data.uv_index, uv_max_count);
            break;
        }

        if (uv_index_end >= 0)
            ptcl->data.uv_index_count = uv_index_end - uv_index_start + 1;
        else
            ptcl->data.uv_index_count = uv_max_count - uv_index_start;

        if (ptcl->data.uv_index_count < 0)
            ptcl->data.uv_index_count = 0;
        return true;
    }

    static void effect_group_msgpack_read(const char* path, const char* file, EffectGroup* eff_group) {
        if (!eff_group || eff_group->type != Glitter::X || !path_check_directory_exists(path))
            return;

        char file_buf[0x80];
        for (const char* i = file; *i && *i != '.'; i++) {
            char c = *i;
            file_buf[i - file] = c;
            file_buf[i - file + 1] = 0;
        }

        char buf[0x200];
        sprintf_s(buf, sizeof(buf), "%s\\%s.json", path, file_buf);
        if (!path_check_file_exists(buf))
            return;

        msgpack msg;

        file_stream s;
        s.open(buf, "rb");
        io_json_read(s, &msg);
        s.close();

        if (msg.type != MSGPACK_MAP)
            return;

        msgpack* effects = msg.read_array("effect");
        if (effects) {
            msgpack_array* ptr = effects->data.arr;
            for (msgpack& i : *ptr) {
                msgpack& effect = i;

                std::string name = effect.read_string("name");
                uint32_t name_hash = hash_string_murmurhash(name);

                for (Effect*& j : eff_group->effects) {
                    if (!j || name_hash != j->data.name_hash)
                        continue;

                    Effect* eff = j;

                    if (effect.read_bool("use_emission")) {
                        enum_or(eff->data.flags, EFFECT_EMISSION);

                        for (Emitter*& k : eff->emitters)
                            if (k)
                                for (Particle*& l : k->particles)
                                    if (l)
                                        enum_or(l->data.flags, PARTICLE_EMISSION);
                    }

                    bool effect_emission = !!(eff->data.flags & EFFECT_EMISSION);

                    msgpack* emission = effect.read("emission");
                    if (emission)
                        eff->data.emission = emission->read_float_t();

                    msgpack* emitters = effect.read_array("emitter");
                    if (emitters) {
                        msgpack_array* ptr = emitters->data.arr;
                        for (msgpack& k : *ptr) {
                            msgpack& emitter = k;

                            int32_t index = emitter.read_int32_t("index");
                            if (index < 0 || index >= eff->emitters.size())
                                continue;

                            Emitter* emit = eff->emitters.data()[index];
                            if (!emit)
                                continue;

                            msgpack* particles = emitter.read_array("particle");
                            if (particles) {
                                msgpack_array* ptr = particles->data.arr;
                                for (msgpack& l : *ptr) {
                                    msgpack& particle = l;

                                    int32_t index = particle.read_int32_t("index");
                                    if (index < 0 || index >= emit->particles.size())
                                        continue;

                                    Particle* ptcl = emit->particles.data()[index];
                                    if (!ptcl || ptcl->version < 3)
                                        continue;

                                    msgpack* emission = particle.read("emission");
                                    if (emission)
                                        ptcl->data.emission = emission->read_float_t();

                                    if (effect_emission || ptcl->data.emission >= min_emission)
                                        enum_or(ptcl->data.flags, PARTICLE_EMISSION);
                                    else
                                        enum_and(ptcl->data.flags, ~PARTICLE_EMISSION);
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}
