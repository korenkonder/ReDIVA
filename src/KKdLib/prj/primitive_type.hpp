/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include "../vec.hpp"

namespace prj {
    struct Capsule3f {
        vec3 c0;
        vec3 c1;
        float_t r;

        inline Capsule3f() : r() {

        }

        inline Capsule3f(const vec3& c0, const vec3& c1, float_t r) : c0(c0), c1(c1), r(r) {

        }
    };

    struct Cylinder3f {
        vec3 c0;
        vec3 c1;
        float_t r;

        inline Cylinder3f() : r() {

        }

        inline Cylinder3f(const vec3& c0, const vec3& c1, float_t r) : c0(c0), c1(c1), r(r) {

        }
    };

    struct Line3f {
        vec3 p0;
        vec3 p1;

        inline Line3f() {

        }

        inline Line3f(const vec3& p0, const vec3& p1) : p0(p0), p1(p1) {

        }
    };

    struct Plane3f {
        vec3 point;
        vec3 normal;
        float_t _l;
        float_t _m;
        float_t _n;
        float_t _p;

        inline Plane3f() : _l(), _m(), _n(), _p() {

        }

        inline Plane3f(const vec3& point, const vec3& normal)
            : point(point), normal(normal), _l(), _m(), _n(), _p() {

        }
    };

    struct Sphere3f {
        vec3 c;
        float_t r;

        inline Sphere3f() : r() {

        }

        inline Sphere3f(const vec3& c, const float_t r) : c(c), r(r) {

        }
    };

    struct Triangle3f {
        vec3 p0;
        vec3 p1;
        vec3 p2;

        inline Triangle3f() {

        }

        inline Triangle3f(const vec3& p0, const vec3& p1, const vec3& p2) : p0(p0), p1(p1), p2(p2) {

        }
    };
}
