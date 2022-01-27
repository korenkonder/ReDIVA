/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "stage.h"
#include "../KKdLib/str_utils.h"
#include "draw_task.h"

void stage_init(stage* s) {
    memset(s, 0, sizeof(stage));
}

void stage_load(stage* s, data_struct* data, auth_3d_database* auth_3d_db, object_database* obj_db,
    texture_database* tex_db, stage_database* stage_data, char* name, render_context* rctx) {
    if (!s || !stage_data || !name)
        return;

    size_t name_len = utf8_length(name);

    s->modern = stage_data->modern;
    s->display = true;
    s->ring = true;
    s->ground = true;
    s->sky = true;
    s->effects = true;

    string auth_3d_category = string_empty;
    uint32_t auth_3d_count = 0;
    uint32_t* auth_3d_ids = 0;
    uint32_t render_texture = -1;
    uint32_t movie_texture = -1;
    s->light_param_name = 0;
    if (!s->modern) {
        s->stage = 0;
        vector_stage_info* stage = &stage_data->stage;
        for (stage_info* i = stage->begin; i != stage->end; i++)
            if (!str_utils_compare_length(name, name_len + 1,
                string_data(&i->name), i->name.length + 1)) {
                string_copy(&i->auth_3d_name, &auth_3d_category);
                s->stage = i;
                auth_3d_count = i->auth_3d_count;
                auth_3d_ids = i->auth_3d_ids;
                render_texture = i->render_texture;
                movie_texture = i->movie_texture;
                s->light_param_name = light_param_get_string(string_data(&i->name));
                break;
            }

        if (!s->stage)
            return;
    }
    else {
        s->stage_modern = 0;
        vector_stage_info_modern* stage = &stage_data->stage_modern;
        for (stage_info_modern* i = stage->begin; i != stage->end; i++)
            if (!str_utils_compare_length(name, name_len + 1,
                string_data(&i->name), i->name.length + 1)) {
                string_copy(&i->auth_3d_name, &auth_3d_category);
                s->stage_modern = i;
                auth_3d_count = i->auth_3d_count;
                auth_3d_ids = i->auth_3d_ids;
                render_texture = i->render_texture;
                movie_texture = i->movie_texture;
                s->light_param_name = light_param_get_string(string_data(&i->name));
                break;
            }

        if (!s->stage_modern)
            return;
    }

    light_param_storage_load_light_param_data(data, s->light_param_name);

    auth_3d_database_uid* uids = auth_3d_db->uid.begin;
    if (auth_3d_category.length) {
        for (auth_3d_database_category* i = auth_3d_db->category.begin; i != auth_3d_db->category.end; i++) {
            if (str_utils_compare_length(string_data(&i->name), i->name.length,
                string_data(&auth_3d_category), auth_3d_category.length))
                continue;
            else if (i->name.length > auth_3d_category.length
                && string_data(&i->name)[auth_3d_category.length] != 'S')
                continue;

            string auth_3d_farc;
            string_copy(&i->name, &auth_3d_farc);
            string_add_length(&auth_3d_farc, ".farc", 5);

            farc f;
            farc_init(&f);
            data_struct_load_file(data, &f, "rom/auth_3d/", string_data(&auth_3d_farc), farc_load_file);
            string_free(&auth_3d_farc);

            if (vector_length(f.files) < 1) {
                farc_free(&f);
                continue;
            }

            for (int32_t* j = i->uid.begin; j != i->uid.end; j++)
                for (uint32_t k = 0; k < auth_3d_count; k++) {
                    if (uids[*j].enabled && uids[*j].org_uid != auth_3d_ids[k])
                        continue;

                    string auth_3d_file;
                    string_copy(&uids[*j].category, &auth_3d_file);
                    string_add_length(&auth_3d_file, ".a3da", 5);

                    farc_file* ff = farc_read_file(&f, string_data(&auth_3d_file));

                    if (!ff || !ff->data || !ff->size) {
                        string_free(&auth_3d_file);
                        continue;
                    }
                    
                    a3da a3da_file;
                    a3da_init(&a3da_file);
                    a3da_mread(&a3da_file, ff->data, ff->size);
                    if (!a3da_file.ready) {
                        a3da_free(&a3da_file);
                        string_free(&auth_3d_file);
                        continue;
                    }

                    int32_t id = auth_3d_data_load_uid(uids[*j].org_uid, auth_3d_db);
                    auth_3d* auth = auth_3d_data_get_auth_3d(id);
                    if (auth) {
                        auth_3d_load(auth, &a3da_file, obj_db, tex_db);
                        vector_int32_t_push_back(&s->auth_3d_ids, &id);
                    }
                    a3da_free(&a3da_file);

                    string_free(&auth_3d_file);
                }

            farc_free(&f);
            break;
        }
    }
    string_free(&auth_3d_category);

    char set_name[11];
    set_name[0] = 0;
    if (!str_utils_compare_length(name, name_len, "STGPV", 5)) {
        memcpy(set_name, name, 8);
        set_name[8] = 0;

    }
    else if(!str_utils_compare_length(name, name_len, "STGD2PV", 7)) {
        memcpy(set_name, name, 10);
        set_name[10] = 0;
    }

    object_set_info* set_info;
    s->set_id = -1;
    if (set_name[0] && object_database_get_object_set_info(obj_db, set_name, &set_info)) {
        s->set_id = set_info->id;
        object_set_load_by_db_index(&set_info, data, obj_db, set_info->id);
    }

    s->stage_set_id = -1;
    if (object_database_get_object_set_info(obj_db, name, &set_info)) {
        s->stage_set_id = set_info->id;
        object_set_load_by_db_index(&set_info, data, obj_db, set_info->id);
    }
    
    if (render_texture != -1)
        post_process_render_texture_set(&rctx->post_process,
            texture_storage_get_texture(render_texture), 0);

    if (movie_texture != -1)
        post_process_movie_texture_set(&rctx->post_process,
            texture_storage_get_texture(movie_texture));
}

void stage_set(stage* s, render_context* rctx) {
    if (!s || !rctx || s == rctx->stage)
        return;
    
    rctx->stage = s;
    render_context_set_light_param(rctx,
        light_param_storage_get_light_param_data(s->light_param_name));
}

void stage_update(stage* s, render_context* rctx) {
    if (!s || s->modern || !s->stage)
        return;

    stage_info* info = s->stage;
    
    if (s->effects) {
        for (int32_t* i = s->auth_3d_ids.begin; i != s->auth_3d_ids.end; i++) {
            auth_3d* auth = auth_3d_data_get_auth_3d(*i);
            if (auth)
                auth_3d_get_value(auth, (mat4*)&mat4_identity, get_delta_frame());
        }
    }

    if (!s->display)
        return;

    if (s->ground)
        draw_task_add_draw_object_by_object_info_opaque(rctx,
            (mat4*)&mat4_identity, info->object_ground);

    if (s->sky)
        draw_task_add_draw_object_by_object_info_opaque(rctx,
            (mat4*)&mat4_identity, info->object_sky);

    if (s->effects) {
        for (int32_t* i = s->auth_3d_ids.begin; i != s->auth_3d_ids.end; i++) {
            auth_3d* auth = auth_3d_data_get_auth_3d(*i);
            if (auth)
                auth_3d_data_set(auth, (mat4*)&mat4_identity, rctx);
        }
    }
}

void stage_update_modern(stage* s, render_context* rctx) {
    if (!s || !s->modern || !s->stage_modern)
        return;

    stage_info_modern* info = s->stage_modern;

    if (s->effects) {
        for (int32_t* i = s->auth_3d_ids.begin; i != s->auth_3d_ids.end; i++) {
            auth_3d* auth = auth_3d_data_get_auth_3d(*i);
            if (auth)
                auth_3d_get_value(auth, (mat4*)&mat4_identity, get_delta_frame());
        }
    }

    if (!s->display)
        return;

    if (s->ground)
        draw_task_add_draw_object_by_object_info_opaque(rctx,
            (mat4*)&mat4_identity, info->object_ground);

    if (s->sky)
        draw_task_add_draw_object_by_object_info_opaque(rctx,
            (mat4*)&mat4_identity, info->object_sky);

    if (s->effects) {
        for (int32_t* i = s->auth_3d_ids.begin; i != s->auth_3d_ids.end; i++) {
            auth_3d* auth = auth_3d_data_get_auth_3d(*i);
            if (auth)
                auth_3d_data_set(auth, (mat4*)&mat4_identity, rctx);
        }
    }
}

void stage_free(stage* s, render_context* rctx) {
    uint32_t render_texture = -1;
    uint32_t movie_texture = -1;
    if (!s->modern && s->stage) {
        stage_info* info = s->stage;
        render_texture = info->render_texture;
        movie_texture = info->movie_texture;

        for (int32_t* i = s->auth_3d_ids.begin; i != s->auth_3d_ids.end; i++)
            auth_3d_data_unload_id(*i, rctx);
        vector_int32_t_free(&s->auth_3d_ids, 0);
    }
    else if (s->modern && s->stage_modern) {
        stage_info_modern* info = s->stage_modern;
        render_texture = info->render_texture;
        movie_texture = info->movie_texture;

        for (int32_t* i = s->auth_3d_ids.begin; i != s->auth_3d_ids.end; i++)
            auth_3d_data_unload_id(*i, rctx);
        vector_int32_t_free(&s->auth_3d_ids, 0);
    }

    if (render_texture != -1)
        post_process_render_texture_free(&rctx->post_process,
            texture_storage_get_texture(render_texture), false);

    if (movie_texture != -1)
        post_process_movie_texture_free(&rctx->post_process,
            texture_storage_get_texture(movie_texture));

    object_storage_delete_object_set(s->set_id);
    object_storage_delete_object_set(s->stage_set_id);
    free(s->light_param_name);

    rctx->stage = 0;
}
