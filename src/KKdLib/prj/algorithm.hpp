/*
    by korenkonder
    GitHub/GitLab: korenkonder

    Taken from MSVC's VC/include/memory
*/

#pragma once

#include "../default.hpp"
#include <algorithm>
#include <vector>

namespace prj {
    template <class T, class U>
    inline bool find(T& in_c, const U& in_v) {
        auto elem = std::find(in_c.begin(), in_c.end(), in_v);
        return elem != in_c.end();
    }

    template <class T, class U>
    inline bool find(T& in_c, const U&& in_v) {
        auto elem = std::find(in_c.begin(), in_c.end(), in_v);
        return elem != in_c.end();
    }

    template <class T, class U>
    inline bool find_and_erase(T& in_c, const U& in_v) {
        auto elem = std::find(in_c.begin(), in_c.end(), in_v);
        if (elem != in_c.end()) {
            in_c.erase(elem);
            return true;
        }
        return false;
    }

    template <class T, class U>
    inline bool find_and_erase(T& in_c, const U&& in_v) {
        auto elem = std::find(in_c.begin(), in_c.end(), in_v);
        if (elem != in_c.end()) {
            in_c.erase(elem);
            return true;
        }
        return false;
    }

    template <class T>
    inline void sort(T& in_c) {
        std::sort(in_c.begin(), in_c.end());
    }

    template <class T>
    inline void unique_and_erase(T& in_c) {
        auto last = std::unique(in_c.begin(), in_c.end());
        in_c.erase(last, in_c.end());
    }

    template <class T>
    inline void sort_and_erase_non_unique(T& in_c) {
        sort(in_c);
        unique_and_erase(in_c);
    }
}
