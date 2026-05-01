/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#include "debug_print.hpp"
#include "render_context.hpp"
#include "screen_param.hpp"
#include "sprite.hpp"

static vec4 primitive_color;

extern render_context* rctx_ptr;

void debug_put(const prj::Line3f& line) {
    debug_put_line(line.p0, line.p1);
}

void debug_put(const prj::Capsule3f& capsule) {
    debug_put_capsule(capsule);
}

void debug_put(const prj::Cylinder3f& cylinder) {
    debug_put_cylinder(cylinder.c0, cylinder.c1, cylinder.r);
}

void debug_put(const prj::Plane3f& plane) {
    debug_put_plane(plane);
}

void debug_put(const prj::Sphere3f& sphere) {
    debug_put_sphere(sphere.c.x, sphere.c.y, sphere.c.z, sphere.r);
}

void debug_put(const prj::Triangle3f& triangle) {
    debug_put_triangle(triangle);
}

void debug_put_capsule(const vec3& c0, const vec3& c1, float_t r) {
    abort();
}

void debug_put_capsule(const prj::Capsule3f& capsule) {
    debug_put_capsule(capsule.c0, capsule.c1, capsule.r);
}

void debug_put_capsule(float_t x, float_t r) {
    vec3 c0;
    vec3 c1;
    c1.x = x;
    debug_put_capsule(c0, c1, r);
}

void debug_put_color(const vec4& color) {
    primitive_color = color;
}

void debug_put_color(color4u8 color) {
    primitive_color = color;
}

void debug_put_cylinder(const vec3& c0, const vec3& c1, float_t r) {
    abort();
}

void debug_put_cylinder(const prj::Cylinder3f& cylinder) {
    debug_put_cylinder(cylinder.c0, cylinder.c1, cylinder.r);
}

void debug_put_ellipse(const vec3& c0, const vec3& c1, float_t r) {
    abort();
}

void debug_put_line(const prj::Line3f& line) {
    debug_put_line(line.p0, line.p1);
}

void debug_put_line(const vec3& p0, const vec3& p1) {
    vec2 sc_p0 = project_screen(p0, true);
    vec2 sc_p1 = project_screen(p1, true);
    spr::putLine(sc_p0, sc_p1, SCREEN_MODE_MAX, spr::SPR_PRIO_DW, primitive_color);
}

void debug_put_line(float_t p0x, float_t p0y, float_t p0z, float_t p1x, float_t p1y, float_t p1z) {
    debug_put_line(vec3(p0x, p0y, p0z), vec3(p1x, p1y, p1z));
}

void debug_put_line_axis(const mat4& mat) {
    debug_put_line_axis(mat, color_red, color_green, color_blue);
}

void debug_put_line_axis(const mat4& mat, color4u8 color_x, color4u8 color_y, color4u8 color_z) {
    vec3 center = { 0.0f, 0.0f, 0.0f };
    mat4_transform_point(&mat, &center, &center);

    vec3 x_axis = { 1.0f, 0.0f, 0.0f };
    mat4_transform_point(&mat, &x_axis, &x_axis);
    debug_put_color(color_x);
    debug_put_line(center, x_axis);

    vec3 y_axis = { 0.0f, 1.0f, 0.0f };
    mat4_transform_point(&mat, &y_axis, &y_axis);
    debug_put_color(color_y);
    debug_put_line(center, y_axis);

    vec3 z_axis = { 0.0f, 0.0f, 1.0f };
    mat4_transform_point(&mat, &z_axis, &z_axis);
    debug_put_color(color_z);
    debug_put_line(center, z_axis);
}

void debug_put_plane(const vec3& p, const vec3& n, const vec3& s) {
    float_t zang = atan2f(n.y, n.x);
    float_t yang = atan2f(-n.z, sqrtf(n.x * n.x + n.y * n.y));

    mat4 mat;
    mat4_translate(&p, &mat);

    mat4_mul_rotate_z(&mat, zang, &mat);
    mat4_mul_rotate_y(&mat, yang, &mat);
    mat4_mul_rotate_z(&mat, 90.0f * DEG_TO_RAD_FLOAT, &mat);
    mat4_scale_rot(&mat, &s, &mat);

    rctx_ptr->disp_manager->entry_obj_by_object_info(mat, object_info(0x02, 0x02), &primitive_color);
}

void debug_put_plane(const prj::Plane3f& plane) {
    debug_put_plane(plane.point, plane.normal);
}

void debug_put_plane_zx(const vec3& p, float_t w, float_t h) {
    mat4 mat;
    mat4_translate(&p, &mat);

    mdl::EtcObj etc(mdl::ETC_OBJ_PLANE);
    etc.color = primitive_color;
    etc.data.plane.w = (int32_t)w;
    etc.data.plane.h = (int32_t)h;
    rctx_ptr->disp_manager->entry_obj_etc(mat, etc);
}

void debug_put_sphere(const vec3& c, float_t r) {
    mat4 mat;
    mat4_translate(&c, &mat);
    mat4_scale_rot(&mat, r, &mat);

    rctx_ptr->disp_manager->entry_obj_by_object_info(mat, object_info(0x00, 0x02), &primitive_color);
}

void debug_put_sphere(const prj::Sphere3f& sphere) {
    debug_put_sphere(sphere.c, sphere.r);
}

void debug_put_sphere(float_t r) {
    debug_put_sphere(0.0f, 0.0f, 0.0f, r);
}

void debug_put_sphere(float_t x, float_t y, float_t z, float_t r) {
    debug_put_sphere(vec3(x, y, z), r);
}

void debug_put_triangle(const vec3& p0, const vec3& p1, const vec3& p2) {
    // Empty
}

void debug_put_triangle(const prj::Triangle3f& triangle) {
    debug_put_triangle(triangle.p0, triangle.p1, triangle.p2);
}

void debug_put_trnsl(float_t trnsl) {
    primitive_color.w = trnsl;
}

void dx_draw_line(vec3 p1, vec3 p2, color4u8 color) {
    debug_put_color(color);
    debug_put_line(p1, p2);
}

vec2 project_screen(vec3 vec, bool offset) {
    camera* cam = rctx_ptr->camera;

    mat4_transform_point(&cam->view, &vec, &vec);
    if (fabsf(vec.z) < 1.0e-10f)
        return 0.0f;

    vec2 sc_vec = cam->depth * *(vec2*)&vec.x * (1.0f / vec.z);

    ScreenParam& render_screen_param = get_render_screen_param();
    sc_vec.x = (float_t)render_screen_param.width * 0.5f - sc_vec.x;
    sc_vec.y = (float_t)render_screen_param.height * 0.5f + sc_vec.y;
    if (offset) {
        ScreenParam& screen_param = get_screen_param();
        sc_vec.x = (float_t)render_screen_param.xoffset + sc_vec.x;
        sc_vec.y = (float_t)(screen_param.height
            - render_screen_param.yoffset - render_screen_param.height) + sc_vec.y;
    }
    return sc_vec;
}

