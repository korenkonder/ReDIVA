/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../KKdLib/default.hpp"
#include "../KKdLib/prj/primitive_type.hpp"
#include "../KKdLib/mat.hpp"
#include "../KKdLib/vec.hpp"
#include "color.hpp"

extern void debug_put(const prj::Line3f& line);
extern void debug_put(const prj::Capsule3f& capsule);
extern void debug_put(const prj::Cylinder3f& cylinder);
extern void debug_put(const prj::Plane3f& plane);
extern void debug_put(const prj::Sphere3f& sphere);
extern void debug_put(const prj::Triangle3f& triangle);
extern void debug_put_capsule(const vec3& c0, const vec3& c1, float_t r);
extern void debug_put_capsule(const prj::Capsule3f& capsule);
extern void debug_put_capsule(float_t x, float_t r);
extern void debug_put_color(const vec4& color);
extern void debug_put_color(color4u8 color);
extern void debug_put_cylinder(const vec3& c0, const vec3& c1, float_t r);
extern void debug_put_cylinder(const prj::Cylinder3f& cylinder);
extern void debug_put_ellipse(const vec3& c0, const vec3& c1, float_t r);
extern void debug_put_line(const prj::Line3f& line);
extern void debug_put_line(const vec3& p0, const vec3& p1);
extern void debug_put_line(float_t p0x, float_t p0y, float_t p0z, float_t p1x, float_t p1y, float_t p1z);
extern void debug_put_line_axis(const mat4& mat);
extern void debug_put_line_axis(const mat4& mat, color4u8 color_x, color4u8 color_y, color4u8 color_z);
extern void debug_put_plane(const vec3& p, const vec3& n, const vec3& s = 1.0f);
extern void debug_put_plane(const prj::Plane3f& plane);
extern void debug_put_plane_zx(const vec3& p, float_t w, float_t h);
extern void debug_put_sphere(const vec3& c, float_t r);
extern void debug_put_sphere(const prj::Sphere3f& sphere);
extern void debug_put_sphere(float_t r);
extern void debug_put_sphere(float_t x, float_t y, float_t z, float_t r);
extern void debug_put_triangle(const vec3& p0, const vec3& p1, const vec3& p2);
extern void debug_put_triangle(const prj::Triangle3f& triangle);
extern void debug_put_trnsl(float_t trnsl);

extern void dx_draw_line(vec3 p1, vec3 p2, color4u8 color);

extern vec2 project_screen(vec3 vec, bool with_render_offset);
