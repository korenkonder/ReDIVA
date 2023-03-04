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
    class vector_pair_combine {
    public:
        using value_pair = std::pair<T, U>;
        std::vector<value_pair> data;
        std::vector<value_pair> new_data;

        inline typename auto find(T key) {
            auto k = data.begin();
            size_t l = data.size();
            size_t temp;
            while (l > 0) {
                if (k[temp = l / 2].first >= key)
                    l /= 2;
                else {
                    k += temp + 1;
                    l -= temp + 1;
                }
            }
            if (k == data.end() || key < k->first)
                return data.end();
            return k;
        }

        inline typename auto find(T key) const {
            auto k = data.begin();
            size_t l = data.size();
            size_t temp;
            while (l > 0) {
                if (k[temp = l / 2].first >= key)
                    l /= 2;
                else {
                    k += temp + 1;
                    l -= temp + 1;
                }
            }
            if (k == data.end() || key < k->first)
                return data.end();
            return k;
        }

        inline void combine() {
            if (new_data.size() <= 1)
                return;

            std::sort(data.begin(), data.end(),
                [](const value_pair& a, const value_pair& b) {
                    return a.first < b.first;
                });

            for (auto& i : new_data) {
                auto elem = find(i.first);
                if (elem != data.end())
                    elem->second = i.second;
                else
                    data.push_back(i);
            }

            new_data.clear();

            if (data.size() <= 1)
                return;

            std::sort(data.begin(), data.end(),
                [](const value_pair& a, const value_pair& b) {
                    return a.first < b.first;
                });

            auto begin = data.begin();
            auto end = data.end();
            for (auto i = begin, j = begin + 1; i != end && j != end; )
                if (i->first == j->first) {
                    std::move(j + 1, end, j);
                    end--;
                }
                else {
                    i++;
                    j++;
                }

            if (data.size() != end - begin)
                data.resize(end - begin);
        }

        inline typename auto begin() {
            return data.begin();
        }
        
        inline typename auto begin() const {
            return data.begin();
        }

        inline typename auto cbegin() const {
            return data.cbegin();
        }

        inline typename auto end() {
            return data.end();
        }
        
        inline typename auto end() const {
            return data.end();
        }

        inline typename auto cend() const {
            return data.cend();
        }

        inline typename auto rbegin() {
            return data.rbegin();
        }
        
        inline typename auto rbegin() const  {
            return data.rbegin();
        }

        inline typename auto crbegin() const {
            return data.crbegin();
        }

        inline typename auto rend() {
            return data.rend();
        }
        
        inline typename auto rend() const {
            return data.rend();
        }

        inline typename auto crend() const {
            return data.crend();
        }

        inline void push_back(const value_pair& value) {
            new_data.push_back(value);
        }

        inline void push_back(value_pair&& value) {
            new_data.push_back(value);
        }

        inline void reserve(size_t new_capacity) {
            new_data.reserve(new_capacity);
        }

        inline size_t size() const {
            return data.size();
        }

        value_pair& operator[](const size_t pos) noexcept {
            return data[pos];
        }

        const value_pair& operator[](const size_t pos) const noexcept {
            return data[pos];
        }
    };
}
