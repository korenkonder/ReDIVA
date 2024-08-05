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
    template <class T>
    bool find(std::vector<T>& vec, const T& value) {
        auto begin = vec.begin();
        auto end = vec.end();
        for (auto i = begin; i != end; i++)
            if (*i == value)
                return true;
        return false;
    }

    template <class T>
    bool find(std::vector<T>& vec, const T&& value) {
        auto begin = vec.begin();
        auto end = vec.end();
        for (auto i = begin; i != end; i++)
            if (*i == value)
                return true;
        return false;
    }

    template <class T>
    inline void sort(std::vector<T>& vec) {
        std::sort(vec.begin(), vec.end());
    }

    template <class T>
    void unique(std::vector<T>& vec) {
        auto last = std::unique(vec.begin(), vec.end());
        vec.erase(last, vec.end());
    }

    template <class T>
    inline void sort_unique(std::vector<T>& vec) {
        sort(vec);
        unique(vec);
    }
}
