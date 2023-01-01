/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "draw_task.hpp"
#include "../KKdLib/hash.hpp"
#include "../KKdLib/sort.hpp"
#include "camera.hpp"
#include "fbo.hpp"
#include "post_process.hpp"
#include "shader_ft.hpp"

static void draw_task_add(render_context* rctx, draw_object_type type, draw_task* task);
static void draw_task_preprocess_init(draw_task* task, const mat4* mat,
    void(*func)(render_context* rctx, void* data), void* data);
static void draw_task_object_init(draw_task* task, object_data* object_data, const mat4* mat,
    float_t bounding_radius, obj_sub_mesh* sub_mesh, obj_mesh* mesh, obj_material_data* material,
    std::vector<GLuint>* textures, int32_t mat_count, mat4* mats, GLuint array_buffer,
    GLuint element_array_buffer, vec4* blend_color, vec4* emission, int32_t morph_array_buffer,
    int32_t instances_count, mat4* instances_mat, void(*draw_object_func)(draw_object*));
static void draw_task_object_translucent_init(draw_task* task, const mat4* mat,
    draw_task_object_translucent* object);
static int draw_task_sort_quicksort_compare0(void const* src1, void const* src2);
static int draw_task_sort_quicksort_compare1(void const* src1, void const* src2);
static int draw_task_sort_quicksort_compare2(void const* src1, void const* src2);
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
    gl_state_set_primitive_restart_index(0xFFFF);
    uniform_value_reset();
    gl_state_get();

    switch (type) {
    case DRAW_OBJECT_TRANSLUCENT:
    case DRAW_OBJECT_TRANSLUCENT_NO_SHADOW:
    case DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_1:
    case DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2:
    case DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_3:
    case DRAW_OBJECT_TRANSLUCENT_LOCAL:
    case DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2_LOCAL:
        if (a2)
            draw_object_func = draw_object_draw_translucent;
        else
            gl_state_set_depth_mask(GL_FALSE);
        min_alpha = 0.0;
        break;
    case DRAW_OBJECT_TRANSPARENT:
    case DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_1:
    case DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2:
    case DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_3:
    case DRAW_OBJECT_TRANSPARENT_LOCAL:
    case DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2_LOCAL:
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
        else if (rctx->draw_pass.reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
            draw_object_func = draw_object_draw_reflect_reflect_map;
        break;
    case DRAW_OBJECT_REFLECT_CHARA_TRANSLUCENT:
        gl_state_set_cull_face_mode(GL_FRONT);
        if (reflect)
            draw_object_func = draw_object_draw_reflect;
        else if (rctx->draw_pass.reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
            draw_object_func = draw_object_draw_reflect_reflect_map;
        min_alpha = 0.0f;
        break;
    case DRAW_OBJECT_REFLECT_CHARA_TRANSPARENT:
        gl_state_set_cull_face_mode(GL_FRONT);
        alpha_test = 1;
        min_alpha = 0.1f;
        if (reflect)
            draw_object_func = draw_object_draw_reflect;
        else {
            if (rctx->draw_pass.reflect_type == STAGE_DATA_REFLECT_REFLECT_MAP)
                draw_object_func = draw_object_draw_reflect_reflect_map;
        }
        break;
    case DRAW_OBJECT_REFLECT_OPAQUE:
        alpha_test = 1;
        if (!a4)
            draw_object_func = draw_object_draw_reflect_reflect_map;
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
    case DRAW_OBJECT_PREPROCESS:
        draw_object_func = draw_object_draw_translucent;
        break;
    default:
        break;
    }
    shaders_ft.env_frag_set(21, 0.0f, 0.0f, 0.0f, min_alpha);
    uniform_value[U_ALPHA_TEST] = alpha_test;

    std::vector<draw_task*>& vec = rctx->object_data.draw_task_array[type];
    if (alpha < 0)
        for (draw_task*& i : vec) {
            draw_task* task = i;
            switch (task->type) {
            case DRAW_TASK_OBJECT: {
                draw_object_draw(rctx, &task->data.object,
                    &task->mat, draw_object_func, show_vector);
            } break;
            case DRAW_TASK_OBJECT_PRIMITIVE: {
                draw_object_model_mat_load(rctx, &task->mat);
                //draw_primitive_draw(&task->data.primitive);
            } break;
            case DRAW_TASK_OBJECT_PREPROCESS: {
                draw_object_model_mat_load(rctx, &task->mat);
                task->data.preprocess.func(rctx, task->data.preprocess.data);
            } break;
            case DRAW_TASK_OBJECT_TRANSLUCENT: {
                for (uint32_t j = 0; j < task->data.object_translucent.count; j++)
                    draw_object_draw(rctx, task->data.object_translucent.objects[j],
                        &task->mat, draw_object_func, show_vector);
            } break;
            }
        }
    else
        for (draw_task*& i : vec) {
            draw_task* task = i;
            switch (task->type) {
            case DRAW_TASK_OBJECT: {
                int32_t a = (int32_t)(task->data.object.blend_color.w * 255.0f);
                a = clamp_def(a, 0, 255);
                if (a == alpha) {
                    draw_object_draw(rctx, &task->data.object,
                        &task->mat, draw_object_func, show_vector);
                }
            } break;
            case DRAW_TASK_OBJECT_TRANSLUCENT: {
                for (uint32_t j = 0; j < task->data.object_translucent.count; j++) {
                    draw_object* object = task->data.object_translucent.objects[j];
                    int32_t a = (int32_t)(object->blend_color.w * 255.0f);
                    a = clamp_def(a, 0, 255);
                    if (a == alpha)
                        draw_object_draw(rctx, object,
                            &task->mat, draw_object_func, show_vector);
                }
            } break;
            }
        }

    switch (type) {
    case DRAW_OBJECT_TRANSLUCENT:
    case DRAW_OBJECT_TRANSLUCENT_NO_SHADOW:
    case DRAW_OBJECT_REFLECT_TRANSLUCENT:
    case DRAW_OBJECT_REFRACT_TRANSLUCENT:
    case DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_1:
    case DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2:
    case DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_3:
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
    case DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_1:
    case DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2:
    case DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_3:
        gl_state_set_cull_face_mode(GL_BACK);
        break;
    }

    uniform_value_reset();
    shader::unbind();
    gl_state_set_blend_func(GL_ONE, GL_ZERO);
}

void draw_task_draw_objects_by_type_translucent(render_context* rctx, bool opaque_enable,
    bool transparent_enable, bool translucent_enable, draw_object_type opaque,
    draw_object_type transparent, draw_object_type translucent) {
    if (draw_task_get_count(rctx, opaque) < 1
        && draw_task_get_count(rctx, transparent) < 1
        && draw_task_get_count(rctx, translucent) < 1)
        return;

    post_process* pp = &rctx->post_process;

    int32_t alpha_array[256];
    int32_t count = draw_task_translucent_sort_count_layers(rctx,
        alpha_array, opaque, transparent, translucent);
    for (int32_t i = 0; i < count; i++) {
        int32_t alpha = alpha_array[i];
        fbo::blit(pp->rend_texture.fbos[0], pp->alpha_layer_texture.fbos[0],
            0, 0, pp->render_width, pp->render_height,
            0, 0, pp->render_width, pp->render_height,
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        if (opaque_enable && draw_task_get_count(rctx, opaque))
            draw_task_draw_objects_by_type(rctx, opaque, 0, 0, true, alpha);
        if (transparent_enable && draw_task_get_count(rctx, transparent))
            draw_task_draw_objects_by_type(rctx, transparent, 0, 0, true, alpha);
        if (translucent_enable && draw_task_get_count(rctx, translucent)) {
            gl_state_enable_blend();
            draw_task_draw_objects_by_type(rctx, translucent, 0, 0, true, alpha);
            gl_state_disable_blend();
        }

        pp->buf_texture.bind();
        pp->alpha_layer_shader.use();
        gl_state_active_bind_texture_2d(0, pp->alpha_layer_texture.color_texture->tex);
        gl_state_active_bind_texture_2d(1, pp->rend_texture.color_texture->tex);
        glUniform1f(0, (float_t)(alpha * (1.0 / 255.0)));
        render_texture::draw_custom_glsl();

        fbo::blit(pp->buf_texture.fbos[0], pp->rend_texture.fbos[0],
            0, 0, pp->render_width, pp->render_height,
            0, 0, pp->render_width, pp->render_height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}

inline int32_t draw_task_get_count(render_context* rctx, draw_object_type type) {
    return (int32_t)rctx->object_data.draw_task_array[type].size();
}

bool draw_task_add_draw_object(render_context* rctx, obj* object,
    obj_mesh_vertex_buffer* obj_vertex_buf, obj_mesh_index_buffer* obj_index_buf, const mat4* mat,
    std::vector<GLuint>* textures, vec4* blend_color, mat4* bone_mat, obj* object_morph,
    obj_mesh_vertex_buffer* obj_morph_vertex_buf, int32_t instances_count,
    mat4* instances_mat, void(*draw_object_func)(draw_object*), bool enable_bone_mat, bool local) {
    object_data* object_data = &rctx->object_data;
    draw_task_flags draw_task_flags = object_data->draw_task_flags;

    rctx->camera->update_data();

    if (!local && object_data->object_culling && !instances_count && !bone_mat && (!object
        || !object_bounding_sphere_check_visibility(&object->bounding_sphere, object_data, rctx->camera, mat))) {
        object_data->culled.objects++;
        return false;
    }
    object_data->passed.objects++;

    for (uint32_t i = 0; i < object->num_mesh; i++) {
        obj_mesh* mesh = &object->mesh_array[i];
        obj_mesh* mesh_morph = 0;
        if (obj_vertex_buf && obj_morph_vertex_buf) {
            if (obj_vertex_buf[i].get_size() != obj_morph_vertex_buf[i].get_size())
                continue;

            if (object_morph && i < object_morph->num_mesh)
                mesh_morph = &object_morph->mesh_array[i];
        }

        if (!local && object_data->object_culling && !instances_count && !bone_mat
            && !object_bounding_sphere_check_visibility(&mesh->bounding_sphere, object_data, rctx->camera, mat)
            && (!mesh_morph || !object_bounding_sphere_check_visibility(
                &mesh_morph->bounding_sphere, object_data, rctx->camera, mat))) {
            object_data->culled.meshes++;
            continue;
        }
        object_data->passed.meshes++;

        draw_object* translucent_priority[40];
        int32_t translucent_priority_count = 0;

        for (uint32_t j = 0; j < mesh->num_submesh; j++) {
            obj_sub_mesh* sub_mesh = &mesh->submesh_array[j];
            obj_sub_mesh* sub_mesh_morph = 0;
            if (sub_mesh->attrib.m.cloth)
                continue;

            if (!local && object_data->object_culling && !instances_count && !bone_mat) {
                int32_t v32 = object_bounding_sphere_check_visibility(
                    &sub_mesh->bounding_sphere, object_data, rctx->camera, mat);
                if (v32 != 2 || (!mesh->attrib.m.billboard && !mesh->attrib.m.billboard_y_axis)) {
                    if (v32 == 2) {
                        if (object_data->object_bounding_sphere_check_func)
                            v32 = 1;
                        else
                            v32 = obj_axis_aligned_bounding_box_check_visibility(
                                &sub_mesh->axis_aligned_bounding_box, rctx->camera, mat);
                    }

                    if (!v32) {
                        if (!mesh_morph || j >= mesh_morph->num_submesh) {
                            object_data->culled.submesh_array++;
                            continue;
                        }

                        sub_mesh_morph = &mesh_morph->submesh_array[j];
                        if (!sub_mesh_morph) {
                            object_data->culled.submesh_array++;
                            continue;
                        }

                        v32 = object_bounding_sphere_check_visibility(
                            &sub_mesh_morph->bounding_sphere, object_data, rctx->camera, mat);
                        if (v32 == 2) {
                            if (object_data->object_bounding_sphere_check_func)
                                v32 = 1;
                            else
                                v32 = obj_axis_aligned_bounding_box_check_visibility(
                                    &sub_mesh_morph->axis_aligned_bounding_box, rctx->camera, mat);
                        }

                        if (!v32) {
                            object_data->culled.submesh_array++;
                            continue;
                        }
                    }
                }
            }
            object_data->passed.submesh_array++;

            int32_t num_bone_index = sub_mesh->num_bone_index;
            uint16_t* bone_index = sub_mesh->bone_index_array;

            mat4* mats;
            if (num_bone_index && enable_bone_mat) {
                mats = object_data->buffer.add_mat4(num_bone_index);
                if (bone_mat)
                    for (int32_t k = 0; k < num_bone_index; k++, bone_index++)
                        mats[k] = bone_mat[*bone_index];
                else
                    for (int32_t k = 0; k < num_bone_index; k++)
                        mats[k] = mat4_identity;
            }
            else {
                mats = 0;
                num_bone_index = 0;
            }

            obj_material_data* material = &object->material_array[sub_mesh->material_index];
            draw_task* task = object_data->buffer.add_draw_task(DRAW_TASK_OBJECT);
            if (!task)
                continue;

            GLuint morph_array_buffer = 0;
            if (obj_morph_vertex_buf)
                morph_array_buffer = obj_morph_vertex_buf[i].get_buffer();

            GLuint element_array_buffer = 0;
            if (obj_index_buf)
                element_array_buffer = obj_index_buf[i].buffer;

            GLuint array_buffer = 0;
            if (obj_vertex_buf)
                array_buffer = obj_vertex_buf[i].get_buffer();

            uint32_t material_hash = hash_utf8_murmurhash(material->material.name);

            material_list_struct* mat_list = 0;
            for (int32_t k = 0; k < object_data->material_list_count; k++)
                if (object_data->material_list_array[k].hash == material_hash) {
                    mat_list = &object_data->material_list_array[k];
                    break;
                }

            vec4 _blend_color = 1.0f;
            vec4 _emission = 0.0f;

            if (mat_list) {
                bool has_blend_color = false;
                if (mat_list->has_blend_color.x) {
                    _blend_color.x = mat_list->blend_color.x;
                    has_blend_color = true;
                }

                if (mat_list->has_blend_color.y) {
                    _blend_color.y = mat_list->blend_color.y;
                    has_blend_color = true;
                }

                if (mat_list->has_blend_color.z) {
                    _blend_color.z = mat_list->blend_color.z;
                    has_blend_color = true;
                }

                if (mat_list->has_blend_color.w) {
                    _blend_color.w = mat_list->blend_color.w;
                    has_blend_color = true;
                }

                if (blend_color) {
                    if (!has_blend_color)
                        _blend_color = *blend_color;
                    else
                        _blend_color *= *blend_color;
                }

                bool has_emission = false;
                if (mat_list->has_emission.x) {
                    _emission.x = mat_list->emission.x;
                    has_emission = true;
                }

                if (mat_list->has_emission.y) {
                    _emission.y = mat_list->emission.y;
                    has_emission = true;
                }

                if (mat_list->has_emission.z) {
                    _emission.z = mat_list->emission.z;
                    has_emission = true;
                }

                if (mat_list->has_emission.w) {
                    _emission.w = mat_list->emission.w;
                    has_emission = true;
                }

                if (!has_emission)
                    _emission = material->material.color.emission;
            }
            else {
                if (blend_color)
                    _blend_color = *blend_color;
                _emission = material->material.color.emission;
            }

            draw_task_object_init(task, object_data, mat, object->bounding_sphere.radius, sub_mesh,
                mesh, material, textures, num_bone_index, mats, array_buffer, element_array_buffer,
                &_blend_color, &_emission, morph_array_buffer, instances_count, instances_mat, draw_object_func);

            if (draw_task_flags & DRAW_TASK_SHADOW_OBJECT) {
                draw_task_add(rctx, (draw_object_type)(DRAW_OBJECT_SHADOW_OBJECT_CHARA
                    + object_data->shadow_type), task);
                if (draw_task_flags & DRAW_TASK_PREPROCESS)
                    draw_task_add(rctx, DRAW_OBJECT_PREPROCESS, task);
                continue;
            }

            obj_material_attrib_member attrib = material->material.attrib.m;
            if (draw_task_flags & (DRAW_TASK_ALPHA_ORDER_1 | DRAW_TASK_ALPHA_ORDER_2 | DRAW_TASK_ALPHA_ORDER_3)
                && task->data.object.blend_color.w < 1.0f) {
                if (!(draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY)) {
                    if (attrib.flag_28 || (attrib.punch_through
                        || !(attrib.alpha_texture | attrib.alpha_material))
                        && !sub_mesh->attrib.m.transparent) {
                        if (!attrib.punch_through) {
                            if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_1)
                                draw_task_add(rctx, DRAW_OBJECT_OPAQUE_ALPHA_ORDER_1, task);
                            else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_2)
                                draw_task_add(rctx, local ? DRAW_OBJECT_OPAQUE_ALPHA_ORDER_2_LOCAL
                                    : DRAW_OBJECT_OPAQUE_ALPHA_ORDER_2, task);
                            else
                                draw_task_add(rctx, DRAW_OBJECT_OPAQUE_ALPHA_ORDER_3, task);
                        }
                        else {
                            if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_1)
                                draw_task_add(rctx, DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_1, task);
                            else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_2)
                                draw_task_add(rctx, local ? DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2_LOCAL
                                    : DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_2, task);
                            else
                                draw_task_add(rctx, DRAW_OBJECT_TRANSPARENT_ALPHA_ORDER_3, task);
                        }

                        if (draw_task_flags & DRAW_TASK_SSS)
                            draw_task_add(rctx, DRAW_OBJECT_SSS, task);
                    }

                    if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_1)
                        draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_1, task);
                    else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_2)
                        draw_task_add(rctx, local ? DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2_LOCAL
                            : DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2, task);
                    else
                        draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_3, task);
                }
            }
            else {
                if (attrib.flag_28 || task->data.object.blend_color.w >= 1.0f
                    && (attrib.punch_through || !(attrib.alpha_texture | attrib.alpha_material))
                    && !sub_mesh->attrib.m.transparent) {
                    if (draw_task_flags & DRAW_TASK_SHADOW)
                        draw_task_add(rctx, (draw_object_type)(DRAW_OBJECT_SHADOW_CHARA
                            + object_data->shadow_type), task);

                    if (draw_task_flags & DRAW_TASK_SSS)
                        draw_task_add(rctx, DRAW_OBJECT_SSS, task);

                    if (attrib.punch_through) {
                        if (!(draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY))
                            draw_task_add(rctx, local ? DRAW_OBJECT_TRANSPARENT_LOCAL
                                : DRAW_OBJECT_TRANSPARENT, task);

                        if (draw_task_flags & DRAW_TASK_CHARA_REFLECT)
                            draw_task_add(rctx, DRAW_OBJECT_REFLECT_CHARA_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_REFLECT)
                            draw_task_add(rctx, DRAW_OBJECT_REFLECT_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_REFRACT)
                            draw_task_add(rctx, DRAW_OBJECT_REFRACT_TRANSPARENT, task);
                    }
                    else {
                        if (!(draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY))
                            draw_task_add(rctx, local ? DRAW_OBJECT_OPAQUE_LOCAL
                                : DRAW_OBJECT_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_20)
                            draw_task_add(rctx, DRAW_OBJECT_TYPE_6, task);

                        if (draw_task_flags & DRAW_TASK_CHARA_REFLECT)
                            draw_task_add(rctx, DRAW_OBJECT_REFLECT_CHARA_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_REFLECT)
                            draw_task_add(rctx, DRAW_OBJECT_REFLECT_OPAQUE, task);

                        if (draw_task_flags & DRAW_TASK_REFRACT)
                            draw_task_add(rctx, DRAW_OBJECT_REFRACT_OPAQUE, task);
                    }

                    if (draw_task_flags & DRAW_TASK_PREPROCESS)
                        draw_task_add(rctx, DRAW_OBJECT_PREPROCESS, task);
                    continue;
                }
                else if (!(draw_task_flags & DRAW_TASK_NO_TRANSLUCENCY)) {
                    if (!attrib.translucent_priority)
                        if (local)
                            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_LOCAL, task);
                        else if (mesh->attrib.m.translucent_no_shadow
                            || draw_task_flags & DRAW_TASK_TRANSLUCENT_NO_SHADOW)
                            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_NO_SHADOW, task);
                        else
                            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT, task);
                    else if (translucent_priority_count < 40)
                        translucent_priority[translucent_priority_count++] = &task->data.object;
                }
            }

            if (draw_task_flags & DRAW_TASK_SHADOW)
                draw_task_add(rctx, (draw_object_type)(DRAW_OBJECT_SHADOW_CHARA
                    + object_data->shadow_type), task);
            if (draw_task_flags & DRAW_TASK_40)
                draw_task_add(rctx, DRAW_OBJECT_TYPE_7, task);
            if (draw_task_flags & DRAW_TASK_CHARA_REFLECT)
                draw_task_add(rctx, DRAW_OBJECT_REFLECT_CHARA_OPAQUE, task);
            if (draw_task_flags & DRAW_TASK_REFLECT) {
                if (rctx->draw_pass.reflect_type != STAGE_DATA_REFLECT_REFLECT_MAP)
                    draw_task_add(rctx, DRAW_OBJECT_REFLECT_OPAQUE, task);
                else
                    draw_task_add(rctx, DRAW_OBJECT_REFLECT_TRANSLUCENT, task);
            }
            if (draw_task_flags & DRAW_TASK_REFRACT)
                draw_task_add(rctx, DRAW_OBJECT_REFRACT_TRANSLUCENT, task);
            if (draw_task_flags & DRAW_TASK_PREPROCESS)
                draw_task_add(rctx, DRAW_OBJECT_PREPROCESS, task);
        }

        if (!translucent_priority_count)
            continue;

        draw_task_object_translucent object_translucent;
        object_translucent.count = 0;
        for (int32_t j = 62; j; j--)
            for (int32_t k = 0; k < translucent_priority_count; k++) {
                draw_object* object = translucent_priority[k];
                if (object->material->material.attrib.m.translucent_priority != j)
                    continue;

                object_translucent.objects[object_translucent.count] = object;
                object_translucent.count++;
            }

        draw_task* task = rctx->object_data.buffer.add_draw_task(DRAW_TASK_OBJECT_TRANSLUCENT);
        if (!task)
            continue;

        draw_task_object_translucent_init(task, mat, &object_translucent);
        if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_1)
            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_1, task);
        else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_2)
            draw_task_add(rctx, local ? DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2_LOCAL
                : DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_2, task);
        else if (draw_task_flags & DRAW_TASK_ALPHA_ORDER_3)
            draw_task_add(rctx, DRAW_OBJECT_TRANSLUCENT_ALPHA_ORDER_3, task);
        else
            draw_task_add(rctx, local ? DRAW_OBJECT_TRANSLUCENT_LOCAL : DRAW_OBJECT_TRANSLUCENT, task);
    }
    return true;
}

inline void draw_task_add_draw_object_by_obj(render_context* rctx, const mat4* mat,
    obj* obj, std::vector<GLuint>* textures, obj_mesh_vertex_buffer* obj_vert_buf,
    obj_mesh_index_buffer* obj_index_buf, mat4* bone_mat, float_t alpha) {
    if (!obj)
        return;

    vec4 blend_color = { 1.0f, 1.0f, 1.0f, alpha };
    vec4* blend_color_ptr = alpha < 1.0f ? &blend_color : 0;

    draw_task_add_draw_object(rctx, obj, obj_vert_buf, obj_index_buf, mat, textures, blend_color_ptr,
        bone_mat, 0, 0, 0, 0, 0, !!bone_mat);
}

inline bool draw_task_add_draw_object_by_object_info(render_context* rctx, const mat4* mat,
    object_info obj_info, mat4* bone_mat) {
    vec4 blend_color = 1.0f;
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true);
}

bool draw_task_add_draw_object_by_object_info(render_context* rctx, const mat4* mat,
    object_info obj_info, vec4* blend_color, mat4* bone_mat, int32_t instances_count,
    mat4* instances_mat, void(*draw_object_func)(draw_object*), bool enable_bone_mat, bool local) {
    if (obj_info.id == -1 && obj_info.set_id == -1)
        return false;

    obj* object = object_storage_get_obj(obj_info);
    if (!object)
        return false;

    std::vector<GLuint>* textures = object_storage_get_obj_set_textures(obj_info.set_id);
    obj_mesh_vertex_buffer* obj_vertex_buffer = object_storage_get_obj_mesh_vertex_buffer(obj_info);
    obj_mesh_index_buffer* obj_index_buffer = object_storage_get_obj_mesh_index_buffer(obj_info);

    obj* obj_morph = 0;
    obj_mesh_vertex_buffer* obj_morph_vertex_buffer = 0;
    object_data* object_data = &rctx->object_data;
    if (object_data->morph.object.set_id != -1) {
        obj_morph = object_storage_get_obj(object_data->morph.object);
        obj_morph_vertex_buffer = object_storage_get_obj_mesh_vertex_buffer(object_data->morph.object);
    }

    return draw_task_add_draw_object(rctx, object, obj_vertex_buffer, obj_index_buffer,
        mat, textures, blend_color, bone_mat, obj_morph, obj_morph_vertex_buffer,
        instances_count, instances_mat, draw_object_func, enable_bone_mat, local);
}

inline bool draw_task_add_draw_object_by_object_info(render_context* rctx, const mat4* mat,
    object_info obj_info, float_t alpha, mat4* bone_mat) {
    vec4 blend_color = 1.0f;
    blend_color.w = alpha;
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true);
}

inline bool draw_task_add_draw_object_by_object_info(render_context* rctx, const mat4* mat,
    object_info obj_info, float_t r, float_t g, float_t b, float_t a, mat4* bone_mat, bool local) {
    vec4 blend_color = { r, g, b, a };
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, &blend_color, bone_mat, 0, 0, 0, true, local);
}

inline bool draw_task_add_draw_object_by_object_info(render_context* rctx, const mat4* mat,
    object_info obj_info, vec4* blend_color, mat4* bone_mat, bool local) {
    return draw_task_add_draw_object_by_object_info(rctx,
        mat, obj_info, blend_color, 0, 0, 0, 0, false, local);
}

void draw_task_add_draw_object_by_object_info_object_skin(render_context* rctx, object_info obj_info,
    std::vector<texture_pattern_struct>* texture_pattern, texture_data_struct* texture_data, float_t alpha,
    mat4* matrices, mat4* ex_data_matrices, const mat4* mat, mat4* global_mat) {
    obj_skin* skin = object_storage_get_obj_skin(obj_info);
    if (!skin)
        return;

    obj_skin_set_matrix_buffer(skin, matrices, ex_data_matrices, rctx->matrix_buffer, mat, global_mat);

    vec4 texture_color_coeff;
    vec4 texture_color_offset;
    vec4 texture_specular_coeff;
    vec4 texture_specular_offset;
    object_data* object_data = &rctx->object_data;
    if (texture_data && !texture_data->field_0) {
        vec4 value;
        object_data->get_texture_color_coeff(texture_color_coeff);
        *(vec3*)&value = texture_data->texture_color_coeff * *(vec3*)&texture_color_coeff;
        value.w = 0.0f;
        object_data->set_texture_color_coeff(value);

        object_data->get_texture_color_offset(texture_color_offset);
        *(vec3*)&value = texture_data->texture_color_offset;
        value.w = 0.0f;
        object_data->set_texture_color_offset(value);

        object_data->get_texture_specular_coeff(texture_specular_coeff);
        *(vec3*)&value = texture_data->texture_specular_coeff * *(vec3*)&texture_specular_coeff;
        value.w = 0.0f;
        object_data->set_texture_specular_coeff(value);

        object_data->get_texture_specular_offset(texture_specular_offset);
        *(vec3*)&value = texture_data->texture_specular_offset;
        value.w = 0.0f;
        object_data->set_texture_specular_offset(value);
    }

    size_t texture_pattern_count = texture_pattern ? texture_pattern->size() : 0;
    if (texture_pattern && texture_pattern_count)
        object_data->set_texture_pattern((int32_t)texture_pattern_count, texture_pattern->data());

    if (fabsf(alpha - 1.0f) > 0.000001f)
        draw_task_add_draw_object_by_object_info(rctx,
            global_mat, obj_info, alpha, rctx->matrix_buffer);
    else
        draw_task_add_draw_object_by_object_info(rctx,
            global_mat, obj_info, rctx->matrix_buffer);

    if (texture_pattern && texture_pattern_count)
        object_data->set_texture_pattern();

    if (texture_data && !texture_data->field_0) {
        object_data->set_texture_color_coeff(texture_color_coeff);
        object_data->set_texture_color_offset(texture_color_offset);
        object_data->set_texture_specular_coeff(texture_specular_coeff);
        object_data->set_texture_specular_offset(texture_specular_offset);
    }
}

void draw_task_add_draw_preprocess(render_context* rctx, const mat4* mat,
    void(*func)(render_context* rctx, void* data), void* data, draw_object_type type) {
    object_data* object_data = &rctx->object_data;
    draw_task* task = object_data->buffer.add_draw_task(DRAW_TASK_OBJECT_PREPROCESS);
    if (task) {
        draw_task_preprocess_init(task, mat, func, data);
        draw_task_add(rctx, type, task);
    }
}

void draw_task_sort(render_context* rctx, draw_object_type type, int32_t compare_func) {
    std::vector<draw_task*>& vec = rctx->object_data.draw_task_array[type];
    if (vec.size() < 1)
        return;

    object_data* object_data = &rctx->object_data;
    for (draw_task*& i : vec) {
        draw_task* task = i;
        vec3 center;
        mat4_get_translation(&task->mat, &center);
        if (task->type == DRAW_TASK_OBJECT) {
            mat4 mat = task->mat;
            if (task->data.object.mesh->attrib.m.billboard)
                model_mat_face_camera_view(&rctx->camera->view, &mat, &mat);
            else if (task->data.object.mesh->attrib.m.billboard_y_axis)
                model_mat_face_camera_position(&rctx->camera->view, &mat, &mat);

            obj_sub_mesh* sub_mesh = task->data.object.sub_mesh;
            if (task->data.object.mat_count < 1 || !sub_mesh->num_bone_index)
                mat4_mult_vec3_trans(&mat, &sub_mesh->bounding_sphere.center, &center);
            else {
                vec3 center_sum = 0.0f;
                for (uint32_t j = 0; j < sub_mesh->num_bone_index; j++) {
                    center = sub_mesh->bounding_sphere.center;
                    mat4_mult_vec3_trans(&task->data.object.mats[j], &center, &center);
                    center_sum += center;
                }
                center_sum *= 1.0f / (float_t)sub_mesh->num_bone_index;
            }
            task->bounding_radius = task->data.object.mesh->bounding_sphere.radius;
        }

        mat4_mult_vec3_trans(&rctx->camera->view, &center, &center);
        task->depth = center.z;
    }

    switch (compare_func) {
    case 0:
        quicksort_custom(vec.data(), vec.size(),
            sizeof(draw_task*), draw_task_sort_quicksort_compare0);
        break;
    case 1:
        quicksort_custom(vec.data(), vec.size(),
            sizeof(draw_task*), draw_task_sort_quicksort_compare1);
        break;
    case 2:
        quicksort_custom(vec.data(), vec.size(),
            sizeof(draw_task*), draw_task_sort_quicksort_compare2);
        break;
    }
}

int32_t obj_axis_aligned_bounding_box_check_visibility(
    obj_axis_aligned_bounding_box* aabb, camera* cam, const mat4* mat) {
    vec3 points[8];
    points[0] = aabb->center + (aabb->size ^ vec3( 0.0f,  0.0f,  0.0f));
    points[1] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f, -0.0f));
    points[2] = aabb->center + (aabb->size ^ vec3(-0.0f,  0.0f,  0.0f));
    points[3] = aabb->center + (aabb->size ^ vec3( 0.0f, -0.0f, -0.0f));
    points[4] = aabb->center + (aabb->size ^ vec3( 0.0f, -0.0f,  0.0f));
    points[5] = aabb->center + (aabb->size ^ vec3(-0.0f,  0.0f, -0.0f));
    points[6] = aabb->center + (aabb->size ^ vec3( 0.0f,  0.0f, -0.0f));
    points[7] = aabb->center + (aabb->size ^ vec3(-0.0f, -0.0f,  0.0f));

    mat4 view_mat;
    mat4_mult(mat, &cam->view, &view_mat);
    for (int32_t i = 0; i < 8; i++)
        mat4_mult_vec3_trans(&view_mat, &points[i], &points[i]);

    vec4 v2[6];
    *(vec3*)&v2[0] = { 0.0f, 0.0f, -1.0f };
    v2[0].w = (float_t)-cam->min_distance;
    *(vec3*)&v2[1] = cam->field_1E4;
    v2[1].w = 0.0f;
    *(vec3*)&v2[2] = cam->field_1F0;
    v2[2].w = 0.0f;
    *(vec3*)&v2[3] = cam->field_1FC;
    v2[3].w = 0.0f;
    *(vec3*)&v2[4] = cam->field_208;
    v2[4].w = 0.0f;
    *(vec3*)&v2[5] = { 0.0f, 0.0f, 1.0f };
    v2[5].w = (float_t)cam->max_distance;

    for (int32_t i = 0; i < 6; i++)
        for (int32_t j = 0; j < 8; j++) {
            float_t v34 = vec3::dot(*(vec3*)&v2[i], points[j]) + v2[i].w;
            if (v34 > 0.0f)
                break;

            if (j == 7)
                return 0;
        }
    return 1;
}

int32_t object_bounding_sphere_check_visibility(obj_bounding_sphere* sphere,
    object_data* object_data, camera* cam, const mat4* mat) {
    if (object_data->object_bounding_sphere_check_func)
        return object_data->object_bounding_sphere_check_func(sphere, cam);

    vec3 center;
    mat4_mult_vec3_trans(mat, &sphere->center, &center);
    mat4_mult_vec3_trans(&cam->view, &center, &center);
    float_t radius = mat4_get_max_scale(mat) * sphere->radius;

    double_t min_depth = (double_t)center.z - (double_t)radius;
    double_t max_depth = (double_t)center.z + (double_t)radius;
    if (-cam->min_distance < min_depth || -cam->max_distance > max_depth)
        return 0;

    float_t v5 = vec3::dot(cam->field_1E4, center);
    if (v5 < -radius)
        return 0;

    float_t v6 = vec3::dot(cam->field_1F0, center);
    if (v6 < -radius)
        return 0;

    float_t v7 = vec3::dot(cam->field_1FC, center);
    if (v7 < -radius)
        return 0;

    float_t v8 = vec3::dot(cam->field_208, center);
    if (v8 < -radius)
        return 0;

    if (-cam->min_distance >= max_depth && -cam->max_distance <= min_depth
        && v5 >= radius && v6 >= radius && v7 >= radius && v8 >= radius)
        return 1;
    return 2;
}

inline static void draw_task_add(render_context* rctx, draw_object_type type, draw_task* task) {
    rctx->object_data.draw_task_array[type].push_back(task);
}

inline static void draw_task_preprocess_init(draw_task* task, const mat4* mat,
    void(*func)(render_context* rctx, void* data), void* data) {
    task->type = DRAW_TASK_OBJECT_PREPROCESS;
    task->mat = *mat;
    task->data.preprocess.func = func;
    task->data.preprocess.data = data;
}

inline static void draw_task_object_init(draw_task* task, object_data* object_data, const mat4* mat,
    float_t bounding_radius, obj_sub_mesh* sub_mesh, obj_mesh* mesh, obj_material_data* material,
    std::vector<GLuint>* textures, int32_t mat_count, mat4* mats, GLuint array_buffer,
    GLuint element_array_buffer, vec4* blend_color, vec4* emission, int32_t morph_array_buffer,
    int32_t instances_count, mat4* instances_mat, void(*draw_object_func)(draw_object*)) {
    task->type = DRAW_TASK_OBJECT;
    task->mat = *mat;
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

    if (blend_color && *blend_color != 1.0f) {
        draw->set_blend_color = true;
        draw->blend_color = *blend_color;
    }
    else {
        draw->set_blend_color = false;
        draw->blend_color = 1.0f;
    }

    draw->emission = *emission;

    draw->chara_color = object_data->chara_color;
    draw->self_shadow = object_data->draw_task_flags & (DRAW_TASK_8 | DRAW_TASK_4) ? 1 : 0;
    draw->shadow = object_data->shadow_type;
    draw->texture_color_coeff = object_data->texture_color_coeff;
    draw->texture_color_coeff.w = object_data->wet_param;
    draw->texture_color_offset = object_data->texture_color_offset;
    draw->texture_specular_coeff = object_data->texture_specular_coeff;
    draw->texture_specular_offset = object_data->texture_specular_offset;
    draw->instances_count = instances_count;
    draw->instances_mat = instances_mat;
    draw->draw_object_func = draw_object_func;

    object_data->add_vertex_array(&task->data.object);
}

inline static void draw_task_object_translucent_init(draw_task* task, const mat4* mat,
    draw_task_object_translucent* object) {
    task->type = DRAW_TASK_OBJECT_TRANSLUCENT;
    task->mat = *mat;
    task->data.object_translucent = *object;
}

static int draw_task_sort_quicksort_compare0(void const* src1, void const* src2) {
    float_t d1 = (*(draw_task**)src1)->depth;
    float_t d2 = (*(draw_task**)src2)->depth;
    return d1 > d2 ? -1 : (d1 < d2 ? 1 : 0);
}

static int draw_task_sort_quicksort_compare1(void const* src1, void const* src2) {
    float_t d1 = (*(draw_task**)src1)->depth;
    float_t d2 = (*(draw_task**)src2)->depth;
    return d1 < d2 ? -1 : (d1 > d2 ? 1 : 0);
}

static int draw_task_sort_quicksort_compare2(void const* src1, void const* src2) {
    float_t r1 = (*(draw_task**)src1)->bounding_radius;
    float_t r2 = (*(draw_task**)src2)->bounding_radius;
    return r1 > r2 ? -1 : (r1 < r2 ? 1 : 0);
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
    std::vector<draw_task*>& vec = rctx->object_data.draw_task_array[type];
    for (draw_task*& i : vec) {
        draw_task* task = i;
        if (task->type != DRAW_TASK_OBJECT)
            continue;

        int32_t alpha = (int32_t)(task->data.object.blend_color.w * 255.0f);
        alpha = clamp_def(alpha, 0, 255);
        arr[alpha] = true;
    }
}
