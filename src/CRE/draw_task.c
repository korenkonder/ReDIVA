/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_task.h"
#include "camera.h"
#include "fbo.h"
#include "post_process.h"
#include "shader_ft.h"

static void draw_task_add(render_context* rctx, draw_object_type type, draw_task* task);
static void draw_task_object_init(draw_task* task, object_data* object_data, mat4* mat,
    float_t bounding_radius, object_sub_mesh* sub_mesh, object_mesh* mesh, object_material_data* material,
    uint32_t* textures, int32_t mat_count, mat4u* mats, GLuint array_buffer,
    GLuint element_array_buffer, vec4* blend_color, int32_t morph_array_buffer,
    int32_t instances_count, mat4* instances_mat, void(*draw_object_func)(draw_object*));
static void draw_task_object_translucent_init(draw_task* task, mat4* mat,
    draw_task_object_translucent* object);
static int32_t draw_task_translucent_sort_count_layers(render_context* rctx,
    int32_t* alpha_array, draw_object_type opaque,
    draw_object_type transparent, draw_object_type translucent);
static void draw_task_translucent_sort_has_objects(render_context* rctx, bool* arr, draw_object_type type);

void draw_task_draw_objects_by_type(render_context* rctx, draw_object_type type,
    int32_t a2, int32_t show_vector, bool a4, int32_t alpha) {
    if (draw_task_get_count(rctx, type) < 1)
        return;

    int32_t alpha_test = 0;
    float_t min_alpha = 1.0f;
    bool reflect = uniform_value[U_REFLECT] == 1;
    void(*draw_object_func)(render_context* rctx, draw_object* draw) = draw_object_draw_default;

    for (int32_t i = 0; i < 6; i++)
        gl_state_active_bind_texture_2d(i, 0);
    gl_state_active_texture(0);
    gl_state_set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    uniform_value_reset();
    gl_state_get();

    switch (type) {
    case DRAW_OBJECT_TRANSLUCENT:
    case DRAW_OBJECT_TRANSLUCENT_NO_SHADOW:
    case DRAW_OBJECT_TRANSLUCENT_TYPE_22:
    case DRAW_OBJECT_TRANSLUCENT_TYPE_25:
    case DRAW_OBJECT_TRANSLUCENT_TYPE_28:
        if (a2)
            draw_object_func = draw_object_draw_translucent;
        else
            gl_state_set_depth_mask(GL_FALSE);
        min_alpha = 0.0;
        break;
    case DRAW_OBJECT_TRANSPARENT:
    case DRAW_OBJECT_TRANSPARENT_TYPE_21:
    case DRAW_OBJECT_TRANSPARENT_TYPE_24:
    case DRAW_OBJECT_TRANSPARENT_TYPE_27:
        alpha_test = 1;
        min_alpha = 0.1f;
        break;
    case DRAW_OBJECT_SHADOW_CHARA:
    case DRAW_OBJECT_SHADOW_STAGE:
    case DRAW_OBJECT_SHADOW_OBJECT_CHARA:
    case DRAW_OBJECT_SHADOW_OBJECT_STAGE:
        draw_object_func = draw_object_draw_shadow;
        min_alpha = 0.5f;
        break;
    case DRAW_OBJECT_TYPE_6:
        draw_object_func = draw_object_draw_translucent;
        gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        rctx->draw_state.shader_index = SHADER_FT_SIL;
        break;
    case DRAW_OBJECT_TYPE_7:
        draw_object_func = draw_object_draw_translucent;
        gl_state_set_color_mask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        rctx->draw_state.shader_index = SHADER_FT_SIL;
        alpha_test = 1;
        min_alpha = 0.0f;
        break;
    case DRAW_OBJECT_REFLECT_CHARA_OPAQUE:
        gl_state_set_cull_face_mode(GL_FRONT);
        if (reflect)
            draw_object_func = draw_object_draw_reflect;
        else if (rctx->draw_pass.reflect_type == 2)
            draw_object_func = draw_object_draw_reflect_type_2;
        break;
    case DRAW_OBJECT_REFLECT_CHARA_TRANSLUCENT:
        gl_state_set_cull_face_mode(GL_FRONT);
        if (reflect)
            draw_object_func = draw_object_draw_reflect;
        else if (rctx->draw_pass.reflect_type == 2)
            draw_object_func = draw_object_draw_reflect_type_2;
        min_alpha = 0.0f;
        break;
    case DRAW_OBJECT_REFLECT_CHARA_TRANSPARENT:
        gl_state_set_cull_face_mode(GL_FRONT);
        alpha_test = 1;
        min_alpha = 0.1f;
        if (reflect)
            draw_object_func = draw_object_draw_reflect;
        else {
            if (rctx->draw_pass.reflect_type == 2)
                draw_object_func = draw_object_draw_reflect_type_2;
        }
        break;
    case DRAW_OBJECT_REFLECT_OPAQUE:
        alpha_test = 1;
        if (!a4)
            draw_object_func = draw_object_draw_reflect_type_2;
        break;
    case DRAW_OBJECT_REFLECT_TRANSLUCENT:
    case DRAW_OBJECT_REFRACT_TRANSLUCENT:
        gl_state_set_depth_mask(GL_FALSE);
        min_alpha = 0.0f;
        break;
    case DRAW_OBJECT_REFLECT_TRANSPARENT:
    case DRAW_OBJECT_REFRACT_TRANSPARENT:
        alpha_test = 1;
        min_alpha = 0.1f;
        break;
    case DRAW_OBJECT_SSS:
        draw_object_func = draw_object_draw_sss;
        break;
    case DRAW_OBJECT_RIPPLE:
        draw_object_func = draw_object_draw_translucent;
        break;
    default:
        break;
    }
    shader_env_frag_set(&shaders_ft, 21, 0.0f, 0.0f, 0.0f, min_alpha);
    uniform_value[U_ALPHA_TEST] = alpha_test;

    vector_ptr_draw_task* vec = &rctx->object_data.draw_task_array[type];
    if (alpha < 0)
        for (draw_task** i = vec->begin; i != vec->end; i++) {
            draw_task* task = *i;
            switch (task->type) {
            case DRAW_TASK_TYPE_OBJECT: {
                mat4 mat;
                mat4u_to_mat4(&task->mat, &mat);
                draw_object_draw(rctx, &task->data.object,
                    &mat, draw_object_func, show_vector);
            } break;
            case DRAW_TASK_TYPE_PRIMITIVE: {
                mat4 mat;
                mat4u_to_mat4(&task->mat, &mat);
                draw_object_model_mat_load(rctx, &mat);
                //draw_primitive_draw(&task->data.primitive);
            } break;
            case DRAW_TASK_TYPE_PREPROCESS: {
                mat4 mat;
                mat4u_to_mat4(&task->mat, &mat);
                draw_object_model_mat_load(rctx, &mat);
                task->data.preprocess.func(rctx, task->data.preprocess.data);
            } break;
            case DRAW_TASK_TYPE_OBJECT_TRANSLUCENT: {
                mat4 mat;
                mat4u_to_mat4(&task->mat, &mat);
                for (int32_t j = 0; j < task->data.object_translucent.count; j++)
                    draw_object_draw(rctx, task->data.object_translucent.objects[j],
                        &mat, draw_object_func, show_vector);
            } break;
            }
        }
    else
        for (draw_task** i = vec->begin; i != vec->end; i++) {
            draw_task* task = *i;
            switch (task->type) {
            case DRAW_TASK_TYPE_OBJECT: {
                int32_t a = (int32_t)(task->data.object.blend_color.w * 255.0f);
                a = clamp(a, 0, 255);
                if (a == alpha) {
                    mat4 mat;
                    mat4u_to_mat4(&task->mat, &mat);
                    draw_object_draw(rctx, &task->data.object,
                        &mat, draw_object_func, show_vector);
                }
            } break;
            case DRAW_TASK_TYPE_OBJECT_TRANSLUCENT: {
                mat4 mat;
                mat4u_to_mat4(&task->mat, &mat);
                for (int32_t j = 0; j < task->data.object_translucent.count; j++) {
                    draw_object* object = task->data.object_translucent.objects[j];
                    int32_t a = (int32_t)(object->blend_color.w * 255.0f);
                    a = clamp(a, 0, 255);
                    if (a == alpha)
                        draw_object_draw(rctx, object,
                            &mat, draw_object_func, show_vector);
                }
            } break;
            }
        }

    switch (type) {
    case DRAW_OBJECT_TRANSLUCENT:
    case DRAW_OBJECT_TRANSLUCENT_NO_SHADOW:
    case DRAW_OBJECT_REFLECT_TRANSLUCENT:
    case DRAW_OBJECT_REFRACT_TRANSLUCENT:
    case DRAW_OBJECT_TRANSLUCENT_TYPE_22:
    case DRAW_OBJECT_TRANSLUCENT_TYPE_25:
    case DRAW_OBJECT_TRANSLUCENT_TYPE_28:
        if (!a2)
            gl_state_set_depth_mask(GL_TRUE);
        break;
    case DRAW_OBJECT_TYPE_6:
    case DRAW_OBJECT_TYPE_7:
        rctx->draw_state.shader_index = -1;
        gl_state_set_color_mask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        break;
    case DRAW_OBJECT_REFLECT_CHARA_OPAQUE:
    case DRAW_OBJECT_REFLECT_CHARA_TRANSLUCENT:
    case DRAW_OBJECT_REFLECT_CHARA_TRANSPARENT:
    case DRAW_OBJECT_TRANSPARENT_TYPE_21:
    case DRAW_OBJECT_TRANSPARENT_TYPE_24:
    case DRAW_OBJECT_TRANSPARENT_TYPE_27:
        gl_state_set_cull_face_mode(GL_BACK);
        break;
    }

    uniform_value_reset();
    shader_unbind();
    gl_state_set_blend_func(GL_ONE, GL_ZERO);
}

void draw_task_draw_objects_by_type_translucent(render_context* rctx, bool opaque_enable,
    bool transparent_enable, bool translucent_enable, draw_object_type opaque,
    draw_object_type transparent, draw_object_type translucent) {
    if (draw_task_get_count(rctx, opaque) < 1)
        return;
    else if (draw_task_get_count(rctx, transparent) < 1)
        return;
    else if (draw_task_get_count(rctx, translucent) < 1)
        return;

    int32_t alpha_array[256];
    int32_t count = draw_task_translucent_sort_count_layers(rctx,
        alpha_array, opaque, transparent, translucent);
    for (int32_t i = 0; i < count; i++) {
        int32_t alpha = alpha_array[i];
        fbo_blit(rctx->post_process.render_texture.fbos[0],
            rctx->post_process.alpha_layer_texture.fbos[0],
            0, 0, rctx->post_process.render_width, rctx->post_process.render_height,
            0, 0, rctx->post_process.render_width, rctx->post_process.render_height,
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_LINEAR);

        if (opaque_enable && draw_task_get_count(rctx, opaque))
            draw_task_draw_objects_by_type(rctx, opaque, 0, 0, 1, alpha);
        if (transparent_enable && draw_task_get_count(rctx, transparent))
            draw_task_draw_objects_by_type(rctx, transparent, 0, 0, 1, alpha);
        if (translucent_enable && draw_task_get_count(rctx, translucent)) {
            gl_state_enable_blend();
            draw_task_draw_objects_by_type(rctx, translucent, 0, 0, 1, alpha);
            gl_state_disable_blend();
        }

        render_texture_bind(&rctx->post_process.buf_texture, 0);
        render_texture_shader_set_glsl(&rctx->post_process.alpha_layer_shader);
        gl_state_active_bind_texture_2d(0, rctx->post_process.alpha_layer_texture.color_texture->texture);
        gl_state_active_bind_texture_2d(1, rctx->post_process.render_texture.color_texture->texture);
        glUniform1f(0, (float_t)(alpha * (1.0 / 255.0)));
        render_texture_draw_custom_glsl();

        fbo_blit(rctx->post_process.buf_texture.fbos[0],
            rctx->post_process.render_texture.fbos[0],
            0, 0, rctx->post_process.render_width, rctx->post_process.render_height,
            0, 0, rctx->post_process.render_width, rctx->post_process.render_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }
}

inline int32_t draw_task_get_count(render_context* rctx, draw_object_type type) {
    return (int32_t)(rctx->object_data.draw_task_array[type].end
        - rctx->object_data.draw_task_array[type].begin);
}

bool draw_task_add_draw_object(render_context* rctx, object* obj,
    object_mesh_vertex_buffer* obj_vertex_buf, object_mesh_index_buffer* obj_index_buf, mat4* mat,
    uint32_t* textures, vec4* blend_color, mat4* bone_mat, object* object_morph,
    object_mesh_vertex_buffer* obj_morph_vertex_buf, int32_t instances_count,
    mat4* instances_mat, void(*draw_object_func)(draw_object*), bool enable_bone_mat) {
    object_data* object_data = &rctx->object_data;
    draw_task_flags draw_task_flags = object_data->draw_task_flags;

    if (object_data->object_culling && !instances_count && !bone_mat && (!obj
        || !object_bounding_sphere_check_visibility(&obj->bounding_sphere, object_data, rctx->camera, mat))) {
        object_data->culled.objects++;
        return false;
    }
    object_data->passed.objects++;

    for (int32_t i = 0; i < obj->meshes_count; i++) {
        object_mesh* mesh = &obj->meshes[i];
        object_mesh* mesh_morph = 0;
        if (obj_vertex_buf && obj_morph_vertex_buf) {
            if (object_mesh_vertex_buffer_get_size(&obj_vertex_buf[i])
                != object_mesh_vertex_buffer_get_size(&obj_morph_vertex_buf[i]))
                continue;

            if (object_morph && i < object_morph->meshes_count)
                mesh_morph = &object_morph->meshes[i];
        }

        if (object_data->object_culling && !instances_count && !bone_mat
            && !object_bounding_sphere_check_visibility(&mesh->bounding_sphere, object_data, rctx->camera, mat)
            && (!mesh_morph || !object_bounding_sphere_check_visibility(
                &mesh_morph->bounding_sphere, object_data, rctx->camera, mat))) {
            object_data->culled.meshes++;
            continue;
        }
        object_data->passed.meshes++;

        draw_object* translucent_priority[40];
        int32_t translucent_priority_count = 0;

        for (int32_t j = 0; j < mesh->sub_meshes_count; j++) {
            object_sub_mesh* sub_mesh = &mesh->sub_meshes[j];
            object_sub_mesh* sub_mesh_morph = 0;
            if (sub_mesh->flags & OBJECT_SUB_MESH_FLAG_8)
                continue;

            if (object_data->object_culling && !instances_count && !bone_mat) {
                int32_t v32 = object_bounding_sphere_check_visibility(
                    &sub_mesh->bounding_sphere, object_data, rctx->camera, mat);
                if (v32 != 2 || (~mesh->flags & OBJECT_MESH_BILLBOARD &&
                    ~mesh->flags & OBJECT_MESH_BILLBOARD_Y_AXIS)) {
                    if (v32 == 2) {
                        if (object_data->object_bounding_sphere_check_func)
                            v32 = 1;
                        else
                            v32 = object_axis_aligned_bounding_box_check_visibility(
                                &sub_mesh->axis_aligned_bounding_box, rctx->camera, mat);
                    }

                    if (!v32) {
                        if (!mesh_morph || j >= mesh_morph->sub_meshes_count) {
                            object_data->culled.sub_meshes++;
                            continue;
                        }

                        sub_mesh_morph = &mesh_morph->sub_meshes[j];
                        if (!sub_mesh_morph) {
                            object_data->culled.sub_meshes++;
                            continue;
                        }

                        v32 = object_bounding_sphere_check_visibility(
                            &sub_mesh_morph->bounding_sphere, object_data, rctx->camera, mat);
                        if (v32 == 2) {
                            if (object_data->object_bounding_sphere_check_func)
                                v32 = 1;
                            else
                                v32 = object_axis_aligned_bounding_box_check_visibility(
                                    &sub_mesh_morph->axis_aligned_bounding_box, rctx->camera, mat);
                        }

                        if (!v32) {
                            object_data->culled.sub_meshes++;
                            continue;
                        }
                    }
                }
            }
            object_data->passed.sub_meshes++;

            int32_t bone_indices_count = sub_mesh->bone_indices_count;
            uint16_t* bone_indices = sub_mesh->bone_indices;

            mat4u* mats;
            if (bone_indices_count && enable_bone_mat) {
                mats = object_data_buffer_add_mat4(&rctx->object_data.buffer, bone_indices_count);
                if (bone_mat)
                    for (int32_t k = 0; k < bone_indices_count; k++)
                        mat4_to_mat4u(&bone_mat[*bone_indices++], &mats[k]);
                else
                    for (int32_t k = 0; k < bone_indices_count; k++)
                        mat4_to_mat4u((mat4*)&mat4_identity, &mats[k]);
            }
            else {
                mats = 0;
                bone_indices_count = 0;
            }

            object_material_data* material = &obj->materials[sub_mesh->material_index];
            draw_task* task = object_data_buffer_add_draw_task(&rctx->object_data.buffer, DRAW_TASK_TYPE_OBJECT);
            if (!task)
                continue;

            GLuint morph_array_buffer = 0;
            if (obj_morph_vertex_buf)
                morph_array_buffer = object_mesh_vertex_buffer_get_buffer(&obj_morph_vertex_buf[i]);

            GLuint element_array_buffer = 0;
            if (obj_index_buf)
                element_array_buffer = obj_index_buf[i];

            GLuint array_buffer = 0;
            if (obj_vertex_buf)
                array_buffer = object_mesh_vertex_buffer_get_buffer(&obj_vertex_buf[i]);

            draw_task_object_init(
                task,
                object_data,
                mat,
                obj->bounding_sphere.radius,
                sub_mesh,
                mesh,
                material,
                textures,
                bone_indices_count,
                mats,
                array_buffer,
                element_array_buffer,
                blend_color,
                morph_array_buffer,
                instances_count,
                instances_mat,
                draw_object_func);

            if (draw_task_flags & DRAW_TASK_SHADOW_OBJECT) {
                draw_task_add(rctx, DRAW_OBJECT_SHADOW_OBJECT_CHARA + object_data->shadow_type, task);
                if (draw_task_flags & DRAW_TASK_RIPPLE)
                    draw_task_add(rctx, DRAW_OBJECT_RIPPLE, task);
                continue;
            }

            object_material_blend_flags blend_flags = material->material.blend_flags;
            if (draw_task_flags & (DRAW_TASK_10000 | DRAW_TASK_20000 | DRAW_TASK_40000)
                && task->data.object.blend_color.w < 1.0f) {
                if (~draw_task_flags | DRAW_TASK_NO_TRANSLUCENCY) {
                    if (blend_flags.flag_28 || (blend_flags.punch_through
                        || !(blend_flags.alpha_texture | blend_flags.alpha_material))
                        && ~sub_mesh->flags & OBJECT_SUB_MESH_TRANSPARENT) {
                        if (!blend_flags.punch_through) {
                            if (draw_task_flags & DRAW_TASK_10000)
                                draw_task_add(rctx, DRAW_OBJECT_OPAQUE_TYPE_20, task);
                            else if (draw_task_flags & DRAW_TASK_20000)
                                draw_task_add(rctx, DRAW_OBJECT_OPAQUE_TYPE_23, task);
                            else
                                draw_task_add(rctx, DRAW_OBJECT_OPAQUE_TYPE_26, task);
                        }
                        else {
                            if (draw_task_flags & DRAW_TASK_10000)
                                draw_task_add(rctx, DRAW_OBJECT_TRANSPARENT_TYPE_21, task);
                            else if (draw_task_flags & DRAW_TASK_20000)
                                draw_task_add(rctx, DRAW_OBJECT_TRANSPARENT_TYPE_24, task);
                            else
                                draw_task_add(rctx, DRAW_OBJECT_TRANSPARENT_TYPE_27, task);
                        }

                        if (draw_task_flags & DRAW_TASK_SSS)
                            draw_task_add(rctx, DRAW_OBJECT_SSS, task);
                    }

                    if (draw_task_flags & DRAW_TASK_10000)
                        draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_TYPE_22, task);
                    else if (draw_task_flags & DRAW_TASK_20000)
                        draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_TYPE_25, task);
                    else
                        draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_TYPE_28, task);
                }
            }
            else {
                if (blend_flags.flag_28 || task->data.object.blend_color.w >= 1.0f
                    && (blend_flags.punch_through || !(blend_flags.alpha_texture | blend_flags.alpha_material))
                    && ~sub_mesh->flags & OBJECT_SUB_MESH_TRANSPARENT) {
                    if (draw_task_flags & DRAW_TASK_SHADOW)
                        draw_task_add(rctx, DRAW_OBJECT_SHADOW_CHARA + object_data->shadow_type, task);

                    if (draw_task_flags & DRAW_TASK_SSS)
                        draw_task_add(rctx, DRAW_OBJECT_SSS, task);

                    if (material->material.blend_flags.punch_through) {
                        if (~draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY)
                            draw_task_add(rctx, DRAW_OBJECT_TRANSPARENT, task);

                        if (draw_task_flags & DRAW_TASK_CHARA_REFLECT)
                            draw_task_add(rctx, DRAW_OBJECT_REFLECT_CHARA_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_REFLECT)
                            draw_task_add(rctx, DRAW_OBJECT_REFLECT_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_REFRACT)
                            draw_task_add(rctx, DRAW_OBJECT_REFRACT_TRANSPARENT, task);
                    }
                    else {
                        if (~draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY)
                            draw_task_add(rctx, DRAW_OBJECT_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_20)
                            draw_task_add(rctx, DRAW_OBJECT_TYPE_6, task);

                        if (draw_task_flags & DRAW_TASK_CHARA_REFLECT)
                            draw_task_add(rctx, DRAW_OBJECT_REFLECT_CHARA_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_REFLECT)
                            draw_task_add(rctx, DRAW_OBJECT_REFLECT_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_REFRACT)
                            draw_task_add(rctx, DRAW_OBJECT_REFRACT_OPAQUE, task);
                    }

                    if (draw_task_flags & DRAW_TASK_RIPPLE)
                        draw_task_add(rctx, DRAW_OBJECT_RIPPLE, task);
                    continue;
                }
                else if (~draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY) {
                    if (!blend_flags.translucent_priority)
                        if (mesh->flags & OBJECT_MESH_TRANSLUCENT_NO_SHADOW
                            || draw_task_flags & DRAW_TASK_TRANSLUCENT_NO_SHADOW)
                            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_NO_SHADOW, task);
                        else
                            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT, task);
                    else if (translucent_priority_count < 40)
                        translucent_priority[translucent_priority_count++] = &task->data.object;
                }
            }

            if (draw_task_flags & DRAW_TASK_SHADOW)
                draw_task_add(rctx, DRAW_OBJECT_SHADOW_CHARA + object_data->shadow_type, task);
            if (draw_task_flags & DRAW_TASK_40)
                draw_task_add(rctx, DRAW_OBJECT_TYPE_7, task);
            if (draw_task_flags & DRAW_TASK_CHARA_REFLECT)
                draw_task_add(rctx, DRAW_OBJECT_REFLECT_CHARA_OPAQUE, task);
            if (draw_task_flags & DRAW_TASK_REFLECT) {
                if (rctx->draw_pass.reflect_type != 2)
                    draw_task_add(rctx, DRAW_OBJECT_REFLECT_OPAQUE, task);
                else
                    draw_task_add(rctx, DRAW_OBJECT_REFLECT_TRANSLUCENT, task);
            }
            if (draw_task_flags & DRAW_TASK_REFRACT)
                draw_task_add(rctx, DRAW_OBJECT_REFRACT_TRANSLUCENT, task);
            if (draw_task_flags & DRAW_TASK_RIPPLE)
                draw_task_add(rctx, DRAW_OBJECT_RIPPLE, task);
        }

        if (!translucent_priority_count)
            continue;

        draw_task_object_translucent object_translucent;
        object_translucent.count = 0;
        for (int32_t j = 62; j; j--)
            for (int32_t k = 0; k < translucent_priority_count; k++) {
                draw_object* object = translucent_priority[k];
                if (object->material->material.blend_flags.translucent_priority != j)
                    continue;

                object_translucent.objects[object_translucent.count] = object;
                object_translucent.count++;
            }

        draw_task* task = object_data_buffer_add_draw_task(&rctx->object_data.buffer,
            DRAW_TASK_TYPE_OBJECT_TRANSLUCENT);
        if (!task)
            continue;

        draw_task_object_translucent_init(task, mat, &object_translucent);
        if (draw_task_flags & DRAW_TASK_10000)
            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_TYPE_22, task);
        else if (draw_task_flags & DRAW_TASK_20000)
            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_TYPE_25, task);
        else if (draw_task_flags & DRAW_TASK_40000)
            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_TYPE_28, task);
        else
            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT, task);
    }
    return true;
}

bool draw_task_add_draw_object_by_object_info(render_context* rctx, mat4* mat,
    object_info obj_info, vec4* blend_color, mat4* bone_mat, int32_t instances_count,
    mat4* instances_mat, void(*draw_object_func)(draw_object*), bool enable_bone_mat) {
    if (obj_info.id == -1 && obj_info.set_id == -1)
        return false;

    object_data* object_data = &rctx->object_data;

    object* obj = object_storage_get_object(obj_info);
    if (!obj)
        return false;

    uint32_t* textures = object_storage_get_textures(obj_info.set_id);
    object_mesh_vertex_buffer* obj_vertex_buffer = object_storage_get_object_mesh_vertex_buffer(obj_info);
    object_mesh_index_buffer* obj_index_buffer = object_storage_get_object_mesh_index_buffer(obj_info);

    object* obj_morph = 0;
    object_mesh_vertex_buffer* obj_morph_vertex_buffer = 0;
    if (object_data->morph.object.set_id != -1) {
        obj_morph = object_storage_get_object(object_data->morph.object);
        obj_morph_vertex_buffer = object_storage_get_object_mesh_vertex_buffer(object_data->morph.object);
    }

    return draw_task_add_draw_object(rctx,
        obj,
        obj_vertex_buffer,
        obj_index_buffer,
        mat,
        textures,
        blend_color,
        bone_mat,
        obj_morph,
        obj_morph_vertex_buffer,
        instances_count,
        instances_mat,
        draw_object_func,
        enable_bone_mat);
}

inline bool draw_task_add_draw_object_by_object_info_alpha(render_context* rctx, mat4* mat,
    object_info obj_info, float_t alpha) {
    vec4 blend_color = vec4_identity;
    blend_color.w = alpha;
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, &blend_color, 0, 0, 0, 0, false);
}

inline bool draw_task_add_draw_object_by_object_info_alpha_bone_mat(render_context* rctx, mat4* mat,
    object_info obj_info, float_t alpha, mat4* bone_mat) {
    vec4 blend_color = vec4_identity;
    blend_color.w = alpha;
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true);
}

inline bool draw_task_add_draw_object_by_object_info_bone_mat(render_context* rctx, mat4* mat,
    object_info obj_info, mat4* bone_mat) {
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, 0, bone_mat, 0, 0, 0, true);
}

inline bool draw_task_add_draw_object_by_object_info_color(render_context* rctx, mat4* mat,
    object_info obj_info, float_t r, float_t g, float_t b, float_t a) {
    vec4 blend_color;
    blend_color.x = r;
    blend_color.y = g;
    blend_color.z = b;
    blend_color.w = a;
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, &blend_color, 0, 0, 0, 0, false);
}

inline bool draw_task_add_draw_object_by_object_info_color_vec4(render_context* rctx, mat4* mat,
    object_info obj_info, vec4* blend_color) {
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, blend_color, 0, 0, 0, 0, false);
}

void draw_task_add_draw_object_by_object_info_object_skin(render_context* rctx, object_info obj_info,
    vector_texture_pattern_struct* texture_pattern, texture_data_struct* texture_data, float_t alpha,
    mat4* matrices, mat4* ex_data_matrices, mat4* mat, mat4* global_mat) {
    object_skin* skin = object_storage_get_object_skin(obj_info);
    if (!skin)
        return;

    object_skin_set_matrix_buffer(skin, matrices, ex_data_matrices, rctx->matrix_buffer, mat, global_mat);

    vec4 texture_color_coeff;
    vec4 texture_color_offset;
    vec4 texture_specular_coeff;
    vec4 texture_specular_offset;
    object_data* object_data = &rctx->object_data;
    if (texture_data && !texture_data->field_0) {
        vec4 value;
        object_data_get_texture_color_coeff(object_data, &texture_color_coeff);
        vec3_mult(texture_data->texture_color_coeff, *(vec3*)&texture_color_coeff, *(vec3*)&value);
        value.w = 0.0f;
        object_data_set_texture_color_coeff(object_data, &value);

        object_data_get_texture_color_offset(object_data, &texture_color_offset);
        *(vec3*)&value = texture_data->texture_color_offset;
        value.w = 0.0f;
        object_data_set_texture_color_offset(object_data, &value);

        object_data_get_texture_specular_coeff(object_data, &texture_specular_coeff);
        vec3_mult(texture_data->texture_specular_coeff, *(vec3*)&texture_specular_coeff, *(vec3*)&value);
        value.w = 0.0f;
        object_data_set_texture_specular_coeff(object_data, &value);

        object_data_get_texture_specular_offset(object_data, &texture_specular_offset);
        *(vec3*)&value = texture_data->texture_specular_offset;
        value.w = 0.0f;
        object_data_set_texture_specular_offset(object_data, &value);
    }

    size_t texture_pattern_count = texture_pattern ? vector_length(*texture_pattern) : 0;
    if (texture_pattern && texture_pattern_count)
        object_data_set_texture_pattern(object_data, (int32_t)texture_pattern_count, texture_pattern->begin);

    if (fabsf(alpha - 1.0f) > 0.000001f)
        draw_task_add_draw_object_by_object_info_alpha_bone_mat(rctx, global_mat, obj_info, alpha, rctx->matrix_buffer);
    else
        draw_task_add_draw_object_by_object_info_bone_mat(rctx, global_mat, obj_info, rctx->matrix_buffer);

    if (texture_pattern && texture_pattern_count)
        object_data_set_texture_pattern(object_data, 0, 0);

    if (texture_data && !texture_data->field_0) {
        object_data_set_texture_color_coeff(object_data, &texture_color_coeff);
        object_data_set_texture_color_offset(object_data, &texture_color_offset);
        object_data_set_texture_specular_coeff(object_data, &texture_specular_coeff);
        object_data_set_texture_specular_offset(object_data, &texture_specular_offset);
    }
}

inline bool draw_task_add_draw_object_by_object_info_opaque(render_context* rctx, mat4* mat,
    object_info obj_info) {
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, (vec4*)&vec4_identity, 0, 0, 0, 0, false);
}

void draw_task_sort(render_context* rctx, draw_object_type type, int32_t compare_func) {
    vector_ptr_draw_task* vec = &rctx->object_data.draw_task_array[type];
    if (vector_length(*vec) < 1)
        return;

    mat4 mat;
    vec3 center;
    object_data* object_data = &rctx->object_data;
    for (draw_task** i = vec->begin; i != vec->end; i++) {
        draw_task* task = *i;
        mat4u_to_mat4(&task->mat, &mat);
        mat4_get_translation(&mat, &center);
        if (task->type == DRAW_TASK_TYPE_OBJECT) {
            obj_mesh_flags mesh_flags = task->data.object.mesh->flags;
            if (mesh_flags & OBJECT_MESH_BILLBOARD) {
                model_mat_face_camera_view(&rctx->camera->view, &mat, &mat);}
            else if (mesh_flags & OBJECT_MESH_BILLBOARD_Y_AXIS)
                model_mat_face_camera_position(&rctx->camera->view, &mat, &mat);
            else {
                object_sub_mesh* sub_mesh = task->data.object.sub_mesh;
                if (task->data.object.mat_count < 1 || !sub_mesh->bone_indices_count)
                    mat4_mult_vec3_trans(&mat, &sub_mesh->bounding_sphere.center, &center);
                else {
                    vec3 center_sum = vec3_null;
                    for (int32_t j = 0; j < sub_mesh->bone_indices_count; j++) {
                        center = sub_mesh->bounding_sphere.center;
                        mat4u_to_mat4(&task->data.object.mats[j], &mat);
                        mat4_mult_vec3_trans(&mat, &center, &center);
                        vec3_add(center_sum, center, center_sum);
                    }
                    vec3_mult_scalar(center_sum, 1.0f / (float_t)sub_mesh->bone_indices_count, center);
                }
            }
            task->bounding_radius = task->data.object.mesh->bounding_sphere.radius;
        }

        //mat4_mult_vec3_trans(&rctx->camera->view, &center, &center);
        //task->depth = center.z;
        task->depth = rctx->camera->view_point.z - center.z;
    }

    switch (compare_func) {
    case 0:
        for (draw_task** i = vec->begin; i != &vec->end[-1]; i++)
            for (draw_task** j = i + 1; j != vec->end; j++)
                if ((*i)->depth > (*j)->depth) {
                    draw_task* temp = *i;
                    *i = *j;
                    *j = temp;
                }
        break;
    case 1:
        for (draw_task** i = vec->begin; i != &vec->end[-1]; i++)
            for (draw_task** j = i + 1; j != vec->end; j++)
                if ((*i)->depth < (*j)->depth) {
                    draw_task* temp = *i;
                    *i = *j;
                    *j = temp;
                }
        break;
    case 2:
        for (draw_task** i = vec->begin; i != &vec->end[-1]; i++)
            for (draw_task** j = i + 1; j != vec->end; j++)
                if ((*i)->bounding_radius < (*j)->bounding_radius) {
                    draw_task* temp = *i;
                    *i = *j;
                    *j = temp;
                }
        break;
    }
}

int32_t object_axis_aligned_bounding_box_check_visibility(
    object_axis_aligned_bounding_box* aabb, camera* cam, mat4* mat) {
    vec3 size;
    vec3 points[8];
    vec3_xor(aabb->size, ((vec3){ 0.0f, 0.0f, 0.0f }), size);
    vec3_add(aabb->center, size, points[0]);
    vec3_xor(aabb->size, ((vec3){ -0.0f, -0.0f, -0.0f }), size);
    vec3_add(aabb->center, size, points[1]);
    vec3_xor(aabb->size, ((vec3){ -0.0f, 0.0f, 0.0f }), size);
    vec3_add(aabb->center, size, points[2]);
    vec3_xor(aabb->size, ((vec3){ 0.0f, -0.0f, -0.0f }), size);
    vec3_add(aabb->center, size, points[3]);
    vec3_xor(aabb->size, ((vec3){ 0.0f, -0.0f, 0.0f }), size);
    vec3_add(aabb->center, size, points[4]);
    vec3_xor(aabb->size, ((vec3){ -0.0f, 0.0f, -0.0f }), size);
    vec3_add(aabb->center, size, points[5]);
    vec3_xor(aabb->size, ((vec3){ 0.0f, 0.0f, -0.0f }), size);
    vec3_add(aabb->center, size, points[6]);
    vec3_xor(aabb->size, ((vec3){ -0.0f, -0.0f, 0.0f }), size);
    vec3_add(aabb->center, size, points[7]);

    mat4 view_mat;
    mat4_mult(mat, &cam->view, &view_mat);
    for (int32_t i = 0; i < 8; i++)
        mat4_mult_vec3_trans(&view_mat, &points[i], &points[i]);

    vec4 v2[6];
    *(vec3*)&v2[0] = (vec3){ 0.0f, 0.0f, -1.0f };
    v2[0].w = (float_t)-cam->min_distance;
    *(vec3*)&v2[1] = cam->field_1E4;
    v2[1].w = 0.0f;
    *(vec3*)&v2[2] = cam->field_1F0;
    v2[2].w = 0.0f;
    *(vec3*)&v2[3] = cam->field_1FC;
    v2[3].w = 0.0f;
    *(vec3*)&v2[4] = cam->field_208;
    v2[4].w = 0.0f;
    *(vec3*)&v2[5] = (vec3){ 0.0f, 0.0f, 1.0f };
    v2[5].w = (float_t)cam->max_distance;

    for (int32_t i = 0; i < 6; i++)
        for (int32_t j = 0; j < 8; j++) {
            float_t v34 = 0.0f;
            vec3_dot(*(vec3*)&v2[i], points[j], v34);
            v34 += v2[i].w;
            if (v34 > 0.0f)
                break;

            if (j == 7)
                return 0;
        }
    return 1;
}

int32_t object_bounding_sphere_check_visibility(object_bounding_sphere* sphere,
    object_data* object_data, camera* cam, mat4* mat) {
    if (object_data->object_bounding_sphere_check_func)
        return object_data->object_bounding_sphere_check_func(sphere);

    vec3 center;
    mat4_mult_vec3_trans(mat, &sphere->center, &center);
    mat4_mult_vec3_trans(&cam->view, &center, &center);
    float_t radius = mat4_get_max_scale(mat) * sphere->radius;

    double_t min_depth = (double_t)center.z - (double_t)radius;
    double_t max_depth = (double_t)center.z + (double_t)radius;
    if (-cam->min_distance < min_depth || -cam->max_distance > max_depth)
        return 0;

    float_t v5;
    vec3_dot(cam->field_1E4, center, v5);
    if (v5 < -radius)
        return 0;

    float_t v6;
    vec3_dot(cam->field_1F0, center, v6);
    if (v6 < -radius)
        return 0;

    float_t v7;
    vec3_dot(cam->field_1FC, center, v7);
    if (v7 < -radius)
        return 0;

    float_t v8;
    vec3_dot(cam->field_208, center, v8);
    if (v8 < -radius)
        return 0;

    if (-cam->min_distance >= max_depth && -cam->max_distance <= min_depth
        && v5 >= radius && v6 >= radius && v7 >= radius && v8 >= radius)
        return 1;
    return 2;
}

inline static void draw_task_add(render_context* rctx, draw_object_type type, draw_task* task) {
    vector_ptr_draw_task_push_back(&rctx->object_data.draw_task_array[type], &task);
}

inline static void draw_task_object_init(draw_task* task, object_data* object_data, mat4* mat,
    float_t bounding_radius, object_sub_mesh* sub_mesh, object_mesh* mesh, object_material_data* material,
    uint32_t* textures, int32_t mat_count, mat4u* mats, GLuint array_buffer,
    GLuint element_array_buffer, vec4* blend_color, int32_t morph_array_buffer,
    int32_t instances_count, mat4* instances_mat, void(*draw_object_func)(draw_object*)) {
    task->type = DRAW_TASK_TYPE_OBJECT;
    mat4_to_mat4u(mat, &task->mat);
    task->bounding_radius = bounding_radius;
    task->data.object.mesh = mesh;
    task->data.object.morph_value = object_data->morph.value;
    task->data.object.material = material;
    task->data.object.sub_mesh = sub_mesh;
    task->data.object.textures = textures;
    task->data.object.mat_count = mat_count;
    task->data.object.mats = mats;
    task->data.object.array_buffer = array_buffer;
    task->data.object.element_array_buffer = element_array_buffer;
    task->data.object.morph_array_buffer = morph_array_buffer;

    draw_object* draw = &task->data.object;
    draw->texture_pattern_count = object_data->texture_pattern_count;
    for (int32_t i = 0; i < object_data->texture_pattern_count && i < TEXTURE_PATTERN_COUNT; i++)
        draw->texture_pattern_array[i] = object_data->texture_pattern_array[i];

    draw->texture_transform_count = object_data->texture_transform_count;
    for (int32_t i = 0; i < object_data->texture_transform_count && i < TEXTURE_TRANSFORM_COUNT; i++)
        draw->texture_transform_array[i] = object_data->texture_transform_array[i];

    if (blend_color) {
        draw->set_blend_color = true;
        vec4_to_vec4u(*blend_color, draw->blend_color);
    }
    else {
        draw->set_blend_color = false;
        draw->blend_color = vec4u_identity;
    }

    draw->chara_color = object_data->chara_color;
    draw->self_shadow = object_data->draw_task_flags & (DRAW_TASK_8 | DRAW_TASK_4) ? 1 : 0;
    draw->shadow = object_data->shadow_type;
    vec4_to_vec4u(object_data->texture_color_coeff, draw->texture_color_coeff);
    draw->texture_color_coeff.w = object_data->wet_param;
    vec4_to_vec4u(object_data->texture_color_offset, draw->texture_color_offset);
    vec4_to_vec4u(object_data->texture_specular_coeff, draw->texture_specular_coeff);
    vec4_to_vec4u(object_data->texture_specular_offset, draw->texture_specular_offset);
    draw->instances_count = instances_count;
    draw->instances_mat = instances_mat;
    draw->draw_object_func = draw_object_func;
}

inline static void draw_task_object_translucent_init(draw_task* task, mat4* mat,
    draw_task_object_translucent* object) {
    task->type = DRAW_TASK_TYPE_OBJECT_TRANSLUCENT;
    mat4_to_mat4u(mat, &task->mat);
    task->data.object_translucent = *object;
}

inline static int32_t draw_task_translucent_sort_count_layers(render_context* rctx,
    int32_t* alpha_array, draw_object_type opaque,
    draw_object_type transparent, draw_object_type translucent) {
    bool arr[256] = { 0 };

    draw_task_translucent_sort_has_objects(rctx, arr, opaque);
    draw_task_translucent_sort_has_objects(rctx, arr, transparent);
    draw_task_translucent_sort_has_objects(rctx, arr, translucent);

    int32_t count = 0;
    for (int32_t i = 0xFF; i >= 1; i--)
        if (arr[i]) {
            count++;
            *alpha_array++ = i;
        }
    return count;
}

inline static void draw_task_translucent_sort_has_objects(render_context* rctx, bool* arr, draw_object_type type) {
    vector_ptr_draw_task* vec = &rctx->object_data.draw_task_array[type];
    for (draw_task** i = vec->begin; i != vec->end; i++) {
        draw_task* task = *i;
        if (task->type != DRAW_TASK_TYPE_OBJECT)
            continue;

        int32_t alpha = (int32_t)(task->data.object.blend_color.w * 255.0f);
        alpha = clamp(alpha, 0, 255);
        arr[alpha] = true;
    }
}
