/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "effect_group.h"
#include "../../KKdLib/database/object.h"
#include "../../KKdLib/database/texture.h"
#include "../../KKdLib/io/path.h"
#include "../../KKdLib/obj.h"
#include "../../KKdLib/farc.h"
#include "../../KKdLib/obj.h"
#include "../../KKdLib/str_utils.h"
#include "../data.h"
#include "../object.h"
#include "../texture.h"
#include "effect.h"
#include "scene.h"
#include "texture.h"

glitter_effect_group* glitter_effect_group_init(GLT) {
    glitter_effect_group* eg = force_malloc(sizeof(glitter_effect_group));
    eg->emission = 1.0f;
    eg->version = GLT_VAL == GLITTER_X ? 0x0C : 0x09;
#if defined(CRE_DEV)
    eg->object_set_ids = vector_empty(uint32_t);
#endif
    return eg;
}

bool glitter_effect_group_parse_file(glitter_effect_group* eg, f2_struct* st) {
    for (f2_struct* i = st->sub_structs.begin; i != st->sub_structs.end; i++) {
        if (!i->header.data_size)
            continue;

        if (i->header.signature == reverse_endianness_uint32_t('EFCT')
            && !glitter_effect_parse_file(eg, i, &eg->effects))
            return false;
        else if (i->header.signature == reverse_endianness_uint32_t('DVRS')
            && !glitter_texture_hashes_unpack_file(eg, i))
            return false;
    }
    return true;
}

bool glitter_effect_group_unparse_file(GLT, glitter_effect_group* eg, f2_struct* st) {
    memset(st, 0, sizeof(f2_struct));

    for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
        if (!*i)
            continue;

        f2_struct s;
        memset(&s, 0, sizeof(f2_struct));
        if (glitter_effect_unparse_file(GLT_VAL, eg, &s, *i))
            vector_f2_struct_push_back(&st->sub_structs, &s);
    }

    f2_struct s;
    memset(&s, 0, sizeof(f2_struct));
    if (glitter_texture_hashes_pack_file(eg, &s))
        vector_f2_struct_push_back(&st->sub_structs, &s);

    st->header.signature = reverse_endianness_uint32_t('DVEF');
    st->header.length = 0x20;
    st->header.use_big_endian = false;
    st->header.use_section_size = true;
    st->header.version = eg->version;
    return true;
}

#if defined(CRE_DEV)
bool glitter_effect_group_check_model(glitter_effect_group* eg) {
    if (eg->type != GLITTER_X)
        return false;

    bool has_model = false;
    vector_uint32_t* object_set_ids = &eg->object_set_ids;
    for (glitter_effect** i = eg->effects.begin; i != eg->effects.end; i++) {
        if (!*i)
            continue;

        glitter_effect* efct = *i;
        for (glitter_emitter** j = efct->emitters.begin; j != efct->emitters.end; j++) {
            if (!*j)
                continue;

            glitter_emitter* emit = *j;
            for (glitter_particle** k = emit->particles.begin; k != emit->particles.end; k++) {
                if (!*k)
                    continue;

                glitter_particle* ptcl = *k;
                if (ptcl->data.type != GLITTER_PARTICLE_MESH)
                    continue;

                has_model = true;
                uint32_t object_set_hash = (uint32_t)ptcl->data.mesh.object_set_name_hash;
                bool found = false;
                for (uint32_t* l = object_set_ids->begin; l != object_set_ids->end; l++)
                    if (*l == object_set_hash) {
                        found = true;
                        break;
                    }

                if (!found)
                    vector_uint32_t_push_back(object_set_ids, &object_set_hash);
            }
        }
    }
    return has_model;
}

void glitter_effect_group_load_model(glitter_effect_group* eg, void* ds) {
    if (eg->type != GLITTER_X)
        return;

    vector_uint32_t* object_set_ids = &eg->object_set_ids;
    if (vector_length(*object_set_ids) < 1)
        return;

    for (uint32_t* i = object_set_ids->begin; i != object_set_ids->end; i++) {
        object_database obj_db;
        texture_database tex_db;
        object_database_init(&obj_db);
        texture_database_init(&tex_db);
        object_set_load_by_hash(ds, &obj_db, &tex_db, *i);
        object_database_free(&obj_db);
        texture_database_free(&tex_db);
    }
}

void glitter_effect_group_free_model(glitter_effect_group* eg) {
    for (uint32_t* i = eg->object_set_ids.begin; i != eg->object_set_ids.end; i++)
        object_storage_delete_object_set(*i);
    vector_uint32_t_free(&eg->object_set_ids, 0);
}
#endif

void glitter_effect_group_dispose(glitter_effect_group* eg) {
    vector_ptr_glitter_effect_free(&eg->effects, glitter_effect_dispose);
    vector_uint64_t_free(&eg->resource_hashes, 0);
    glitter_texture_unload(eg);
    txp_set_free(&eg->resources_tex);
#if defined(CRE_DEV)
    glitter_effect_group_free_model(eg);
#endif
    free(eg);
}
