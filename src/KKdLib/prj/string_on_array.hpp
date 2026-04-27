/*
    by korenkonder
    GitHub/GitLab: korenkonder
*/

#pragma once

#include "../default.hpp"
#include <string>
#include <string.h>
#include <wchar.h>

namespace prj {
    template <typename value_type>
    struct CharFinder;

    template <>
    struct CharFinder<char> {
        static const char* find_null_term(const char* ptr, size_t count) {
            return (const char*)memchr(ptr, 0, count);
        }
    };

    template <>
    struct CharFinder<wchar_t> {
        static const wchar_t* find_null_term(const wchar_t* ptr, size_t count) {
            return (const wchar_t*)wmemchr(ptr, 0, count);
        }
    };

    template <typename value_type>
    struct CharFinder {
        static const value_type* find_null_term(const value_type* ptr, size_t count) {
            while (count && *ptr == value_type()) {
                ptr++;
                count--;
            }
            return count ? 0 : ptr;
        }
    };

    template <class _Elem, class _Traits = std::char_traits<_Elem>, size_t unk = 0, bool fill_null = false>
    class CharTraitsAux {
    public:
        using value_type = _Elem;
        using traits_type = _Traits;
        using size_type = size_t;

        static const size_type npos = static_cast<size_type>(-1);

        static int compare(const value_type* left, size_type left_size, const value_type* right, size_type right_size) {
            size_type n = common_length(left_size, right_size);
            int res = traits_type::compare(left, right, n);
            if (res != 0)
                return res;
            else if (left_size < right_size)
                return -1;
            else if (left_size > right_size)
                return 1;
            return 0;
        }

        static value_type* copy_s(value_type* dst, size_type dst_size, const value_type* ptr, size_type count) {
            if (!dst || !ptr || !dst_size)
                return dst;

            size_type n = common_length(dst_size - 1, count);
            traits_type::copy(dst, ptr, n);
            return dst;
        }

        static size_type copy_pf(value_type* dst, const value_type* ptr, size_type count, size_type dst_size) {
            size_type n = common_length(count, dst_size - 1);
            traits_type::copy(dst, ptr, n);
            return count;
        }

        static size_type assign_pf(value_type* dst, value_type ch, size_type count, size_type dst_size) {
            size_type n = common_length(count, dst_size - 1);
            traits_type::assign(dst, n, ch);
            return count;
        }

        static size_type length(const value_type* ptr, size_type count) {
            const value_type* p = CharFinder<value_type>::find_null_term(ptr, count);
            return count && p ? p - ptr : count;
        }

        static size_type common_length(size_type left_size, size_type right_size) {
            return std::min(left_size, right_size);
        }

        static size_type terminate(value_type* dst, size_type count, size_type dst_size) {
            size_t n_null = fill_null ? dst_size - count : 1;
            memset(&dst[count], 0, count < dst_size ? n_null * sizeof(value_type) : 0);
            return count;
        }
    };

    template <class _Elem, class _Traits = std::char_traits<_Elem>, size_t len = 0, size_t unk = 0, bool fill_null = false>
    class PodStringOnArray {
    private:
        using traits_aux_type = CharTraitsAux<_Elem, _Traits, unk, fill_null>;
        using traits_type = _Traits;
        using value_type = _Elem;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = size_t;

    public:
        static const size_type npos = static_cast<size_type>(-1);

    private:
        value_type M_pool[len];

    public:
        size_type max_size() const {
            return len;
        }

        size_type capacity() const {
            return len;
        }

        size_type length() const {
            return traits_aux_type::length(M_pool, npos);
        }

        bool empty() const {
            return traits_aux_type::length(M_pool, npos) == 0;
        }

        const value_type* c_str() const {
            return M_pool;
        }

        const value_type* data() const {
            return M_pool;
        }

        size_type size() const {
            return traits_aux_type::length(M_pool, npos);
        }

        void clear() {
            if (len > 0)
                M_pool[0] = value_type();
        }

        PodStringOnArray& append(size_type count, value_type ch) {
            size_type cur = size();
            size_type n = traits_aux_type::assign_pf(M_pool + cur, ch, count, len - cur);
            traits_aux_type::terminate(M_pool, cur + n, len);
            return *this;
        }

        PodStringOnArray& append(const value_type* ptr, size_type count) {
            size_type cur = size();
            size_type n = traits_aux_type::copy_pf(M_pool + cur, ptr, traits_aux_type::length(ptr, count), len - cur);
            traits_aux_type::terminate(M_pool, cur + n, len);
            return *this;
        }

        PodStringOnArray& append(const value_type* ptr) {
            return append(ptr, npos);
        }

        template <bool rhs_fill_null = false>
        PodStringOnArray& append(const PodStringOnArray<_Elem, _Traits, len, unk, rhs_fill_null>& rhs) {
            return append(rhs.c_str(), rhs.size());
        }

        PodStringOnArray& assign(size_type count, value_type ch) {
            size_type n = traits_aux_type::assign_pf(M_pool, ch, count, len);
            traits_aux_type::terminate(M_pool, n, len);
            return *this;
        }

        __declspec(noinline) PodStringOnArray& assign(const value_type* ptr, size_type count) {
            if (c_str() == ptr)
                return *this;

            size_type n = traits_aux_type::copy_pf(M_pool, ptr, traits_aux_type::length(ptr, count), len);
            traits_aux_type::terminate(M_pool, n, len);
            return *this;
        }

        __declspec(noinline) PodStringOnArray& assign(const value_type* ptr) {
            if (c_str() == ptr)
                return *this;

            return assign(ptr, npos);
        }
        
        template <bool rhs_fill_null = false>
        __declspec(noinline) PodStringOnArray& assign(const PodStringOnArray<_Elem, _Traits, len, unk, rhs_fill_null>& rhs) {
            if (c_str() == rhs.c_str())
                return *this;

            return assign(rhs.c_str(), rhs.size());
        }

        const_reference operator[](size_type pos) const {
            return at(pos);
        }

        reference operator[](size_type pos) {
            return at(pos);
        }

        const_reference at(size_type pos) const {
            return M_pool[pos];
        }

        reference at(size_type pos) {
            return M_pool[pos];
        }

        int compare(const value_type* ptr, size_type count) const {
            return traits_aux_type::compare(c_str(), length(), ptr, count);
        }

        int compare(const value_type* ptr) const {
            return traits_aux_type::compare(c_str(), length(), ptr, traits_aux_type::length(ptr, npos));
        }

        int compare(const PodStringOnArray& rhs) const {
            return traits_aux_type::compare(c_str(), length(), rhs.c_str(), rhs.compare());
        }

        bool operator==(const PodStringOnArray& rhs) const {
            return compare(rhs) == 0;
        }

        bool operator<(const PodStringOnArray& rhs) const {
            return compare(rhs) < 0;
        }

        bool operator!=(const PodStringOnArray& rhs) const {
            return compare(rhs) != 0;
        }

        bool operator>(const PodStringOnArray& rhs) const {
            return compare(rhs) > 0;
        }

        bool operator<=(const PodStringOnArray& rhs) const {
            return compare(rhs) <= 0;
        }

        bool operator>=(const PodStringOnArray& rhs) const {
            return compare(rhs) >= 0;
        }
    };

    template <size_t len = 0, size_t unk = 0, bool fill_null = false>
    using string_on_array = PodStringOnArray<char, std::char_traits<char>, len, unk, fill_null>;

    template <size_t len = 0, size_t unk = 0, bool fill_null = false>
    using wstring_on_array = PodStringOnArray<wchar_t, std::char_traits<wchar_t>, len, unk, fill_null>;
}
